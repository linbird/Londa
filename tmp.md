在内存的每个区域zone都关联一组LRU链表，这些链表记录了物理页面的状态，物理页面的回收工作就是以这些链表为依据。在zone结构中，存在struct lruvec lruvec;字段，lruvec保存了保存了这些链表的链表头，看下该结构
```c
struct lruvec {
    struct list_head lists[NR_LRU_LISTS];
    struct zone_reclaim_stat reclaim_stat;
#ifdef CONFIG_MEMCG
/*其关联的区域zone*/
    struct zone *zone;
#endif
};

enum lru_list {
    LRU_INACTIVE_ANON = LRU_BASE,
    LRU_ACTIVE_ANON = LRU_BASE + LRU_ACTIVE,
    LRU_INACTIVE_FILE = LRU_BASE + LRU_FILE,
    LRU_ACTIVE_FILE = LRU_BASE + LRU_FILE + LRU_ACTIVE,
    LRU_UNEVICTABLE,//不可换出
    NR_LRU_LISTS
};
```
