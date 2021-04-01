#include <memory>

using std::shared_ptr;

#ifndef __RB_H_
#define __RB_H_
namespace RB{

enum COLOR{
	BLACK	= 0,
	RED	= 1	
};

enum FIND{
    RECURSION	= 0,
    LOOP		= 1
};

//构造，查找，计数，插入，删除，迭代器
template<typename DataType, typename ValueType = int>
class RBNode{
    private:
        const ValueType value;
        DataType Data;
	enum COLOR color;
	shared_ptr<RBNode> left;
        shared_ptr<RBNode> right;
        shared_ptr<RBNode> prev;
        shared_ptr<RBNode> next;
    public:
        RBNode(ValueType _value, DataType _data)
            : value(_value), Data(_data), color(COLOR::RED)
            , left(nullptr), right(nullptr)
            , prev(nullptr), next(nullptr)
        {}
};

// node->left->value < node->val <= node->right->value
template<typename DataType, typename ValueType = int>
class RBTree{
    private:
        shared_ptr<RBTree> root;
    public:
        RBTree(std::front_insert_iterator)
        const shared_ptr<RB::RBNode<DataType, ValueType>> find(ValueType target);
        const shared_ptr<RB::RBNode<DataType, ValueType>> insert(const ValueType&& value, DataType data);
        const shared_ptr<RB::RBNode<DataType, ValueType>> insert(const ValueType& value, DataType data);
        const shared_ptr<RB::RBNode<DataType, ValueType>> insert(const ValueType value, DataType data);
        shared_ptr<RB::RBNode<DataType, ValueType>> erase(const ValueType value);
};
#endif
}
