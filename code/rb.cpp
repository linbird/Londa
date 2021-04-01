#include "rb.h"

using namespace RB;

template<typename DataType, typename ValueType>
const shared_ptr<RB::RBNode<DataType, ValueType>> 
find(ValueType target, RB::FIND method){
	if(method != RB::FIND::RECURSION)	return loop_find(root, target);
	else					return recursion_find(root, target);
};

template<typename DataType, typename ValueType>
const shared_ptr<RB::RBNode<DataType, ValueType>> 
RB::RBTree::recursion_find(shared_ptr<RB::RBNode<DataType, ValueType>> node, ValueType target){
	if(node == nullptr)	return nullptr;
	else{
		if(node->value == target)	return node;
		else if(node->value > target)	return recursion_find(node->left, target);
		else				return recursion_find(node->right, target);
	}
}

template<typename DataType, typename ValueType>
const shared_ptr<RBNode<DataType, ValueType>> 
RBTree::loop_find(shared_ptr<RBNode<DataType, ValueType>> node, ValueType target){
	while(node != nullptr){
		if(node->value == target)	break;
		else if(node->value > target)	node = node->left;
		else				node = node->right;	
	}
	return node;
}

template<typename DataType, typename ValueType>
const shared_ptr<RBNode<DataType, ValueType>> 
insert(const ValueType&& value, DataType data){
	RBNode<DataType, ValueType> node = make_shared<RBNode<DataType, ValueType>>(value, data);
}

// find具体方法:查找指定值的节点，如果需要插入则建立节点，否则只返回查找结果
// 1. 找到节点:新建一个右子节点
// 2. 没找到节点:在最后搜索的节点的左边或者右边建立节点
// 由1&2 -> find方法返回最后被搜索的节点
const shared_ptr<RBNode> 
do_insert(const shared_ptr<RBNode> insert_point, const ValueType&& value, DataType data){}
