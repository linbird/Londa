SYSCALL_DEFINE4(epoll_ctl, int, epfd, int, op, int, fd, struct epoll_event __user *, event) {
	int did_lock_epmutex = 0;
    struct epoll_event epds;
    copy_from_user(&epds, event, sizeof(struct epoll_event));//将epoll_event结构从用户空间copy到内核空间.
    struct file *file = fget(epfd); //取得 epfd 对应的文件
    struct file *tfile = fget(fd); //取得目标文件
	if (!tfile->f_op || !tfile->f_op->poll) {
        goto error_tgt_fput;
    }// 检查目标文件是否支持poll操作
	if (file == tfile || !is_file_epoll(file)) {
        goto error_tgt_fput;
    }// 检查参数的合理性（传入参数是否符合预期）
	struct eventpoll *ep = file->private_data; // 取得内部结构eventpoll（参见上文链式查询）
    if (op == EPOLL_CTL_ADD || op == EPOLL_CTL_DEL) { // 获取全局锁epmutex
        mutex_lock(&epmutex);
        did_lock_epmutex = 1;
    }
	if (op == EPOLL_CTL_ADD) {
        if (is_file_epoll(tfile)) {
            error = -ELOOP;
            if (ep_loop_check(ep, tfile) != 0) {  // 目标文件也是epoll 检测是否有循环包含的问题
                goto error_tgt_fput;
            }
        } else  {
            list_add(&tfile->f_tfile_llink, &tfile_check_list); // 将目标文件添加到epoll全局的tfile_check_list 中
        }
    }
	mutex_lock_nested(&ep->mtx, 0); // 接下来会修改eventpoll，加锁进入临界区
    struct epitem *epi = ep_find(ep, tfile, fd); // 以tfile 和fd 为key 在rbtree 中查找文件对应的epitem
    switch (op) {
    	case EPOLL_CTL_ADD:{
			if (!epi) { // 没找到红黑树节点, 添加额外添加ERR HUP 事件
            	epds.events |= POLLERR | POLLHUP;
            	error = ep_insert(ep, &epds, tfile, fd);
        	}
        	clear_tfile_check_list(); // 清空文件检查列表
        	break;
        }case EPOLL_CTL_DEL:{
            if (epi) {
                error = ep_remove(ep, epi);
            }
            break;
        }case EPOLL_CTL_MOD:{
            if (epi) {
                epds.events |= POLLERR | POLLHUP;
                error = ep_modify(ep, epi, &epds);
            }
            break;
        }
    }
	mutex_unlock(&ep->mtx);
    error_tgt_fput:
        if (did_lock_epmutex) {
            mutex_unlock(&epmutex);
        }
}

// ep_insert():向epollfd里面添加一个待监听的fd
static int ep_insert(struct eventpoll *ep, struct epoll_event *event, struct file *tfile, int fd) {
	long user_watches = atomic_long_read(&ep->user->epoll_watches);
    if (unlikely(user_watches >= max_user_watches)) {
        return -ENOSPC;
    }// ---------------NOTE: 此处有最大监听数量的限制-----------------
	struct epitem *epi = kmem_cache_alloc(epi_cache, GFP_KERNEL));//创建epitem空间
    // epi其他数据成员的初始化……………………
    ep_set_ffd(&epi->ffd, tfile, fd);// 初始化红黑树中的key
    epi->event = *event; // 直接复制用户结构
    struct ep_pqueue epq;// 初始化临时的 epq
    epq.epi = epi;
    init_poll_funcptr(&epq.pt, ep_ptable_queue_proc);
    epq.pt._key = event->events;  // 设置事件掩码
	int revents = tfile->f_op->poll(tfile, &epq.pt); //内部会调用上面传入的ep_ptable_queue_proc,在文件tfile对应的wait queue head 上注册回调函数, 并返回当前文件的状态
    spin_lock(&tfile->f_lock); // 自旋锁加锁
    list_add_tail(&epi->fllink, &tfile->f_ep_links);// 添加当前的epitem 到文件的f_ep_links 链表
    spin_unlock(&tfile->f_lock);
    ep_rbtree_insert(ep, epi);// 插入epi 到rbtree
}

static int ep_poll_callback(wait_queue_t *wait, unsigned mode, int sync, void *key){
    unsigned long flags;
    struct epitem *epi = ep_item_from_wait(wait);
    struct eventpoll *ep = epi->ep;
    // 这个lock比较关键，操作“就绪链表”相关的，均需要这个lock，以防丢失事件。
    spin_lock_irqsave(&ep->lock, flags);
    // 如果发生的事件我们并不关注，则不处理直接返回即可。
    if (key && !((unsigned long) key & epi->event.events))
        goto out_unlock;

    // 实际将发生事件的epitem加入到“就绪链表”中。
    if (!ep_is_linked(&epi->rdllink)) {
        list_add_tail(&epi->rdllink, &ep->rdllist);
    }
    // 既然“就绪链表”中有了新成员，则唤醒阻塞在epoll_wait系统调用的task去处理。注意，如果本来epi已经在“就绪队列”了，这里依然会唤醒并处理的。
    if (waitqueue_active(&ep->wq)) {
        wake_up_locked(&ep->wq);
    }

out_unlock:
    spin_unlock_irqrestore(&ep->lock, flags);
    ...
}

static int ep_poll(struct eventpoll *ep, struct epoll_event __user *events, int maxevents, long timeout){
    unsigned long flags;
    wait_queue_t wait;
    if (!ep_events_available(ep)) {// 当前没有事件才睡眠
        init_waitqueue_entry(&wait, current);
        __add_wait_queue_exclusive(&ep->wq, &wait);
        for (;;) {
            set_current_state(TASK_INTERRUPTIBLE);
            ...// 例行的schedule timeout
        }
        __remove_wait_queue(&ep->wq, &wait);
        set_current_state(TASK_RUNNING);
    }
    ep_send_events(ep, events, maxevents);// 往用户态上报事件，即那些epoll_wait返回后能获取的事件。
}

ep_scan_ready_list(){
    ready_list_for_each() {// 遍历“就绪链表”
        list_del_init(&epi->rdllink);// 将epi从“就绪链表”删除
        revents = ep_item_poll(epi, &pt);// 实际获取具体的事件，睡眠entry的回调函数只是通知有“事件”，具体需要每一个文件句柄的特定poll回调来获取。
        if (revents) {
            if (__put_user(revents, &uevent->events) ||__put_user(epi->event.data, &uevent->data)) {// 如果没有完成，则将epi重新加回“就绪链表”等待下次。
                list_add(&epi->rdllink, head);
                return eventcnt ? eventcnt : -EFAULT;
            }
            if (!(epi->event.events & EPOLLET)) {// 如果是LT模式，则无论如何都会将epi重新加回到“就绪链表”，等待下次重新再poll以确认是否仍然有未处理的事件。这也符合“水平触发”的逻辑，即“只要你不处理，我就会一直通知你”。
                list_add_tail(&epi->rdllink, &ep->rdllist);
            }
        }
    }
    if (!list_empty(&ep->rdllist)) {// 如果“就绪链表”上仍有未处理的epi，且有进程阻塞在epoll句柄的睡眠队列，则唤醒它！(这将是LT惊群的根源)
        if (waitqueue_active(&ep->wq))
            wake_up_locked(&ep->wq);
    }
}

// 以下是简单的创建过程，非可执行代码
SYSCALL_DEFINE1(epoll_create, int, size);//展开SYSCALL_DEFINE1宏，调用到sys_epoll_create1(0);
SYSCALL_DEFINE1(epoll_create1, int, flags){//最终执行的epoll_create1()
    struct eventpoll *ep = NULL;//主描述符
	int error = ep_alloc(&ep); //分配一个struct eventpoll
	error = anon_inode_getfd("[eventpoll]", &eventpoll_fops, ep, O_RDWR | (flags & O_CLOEXEC));//创建一个匿名fd
	//eventpoll_fops指向该虚拟文件支持的operations函数表，epoll只实现了poll和release(即close)操作，其它文件系统操作由VFS处理。
    //ep作为一个私有数据保存在struct file的private指针里，目的在于实现fd->file->eventpoll的查找过程
    return error;
}
// eventpoll初始化函数
static int __init eventpoll_init(void){}
