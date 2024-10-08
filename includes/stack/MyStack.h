#ifndef MYSTACK_H
#define MYSTACK_H
#include "../node/node.h"
#include "../linked_list_functions/linkedlistfunctionsTemplated.h"
#include <assert.h>

template <typename T>
class Stack
{
public:
    
    class Iterator
    {
    public:
        //give access to list to access _ptr
        friend class Stack;

        //default ctor
        Iterator()
        {
            _ptr = NULL;
        }

        //Point Iterator to where p is pointing to
        Iterator(node<T>* p)
        {
            _ptr = p;
        }

        //dereference operator
        T operator *()
        {
            return _ptr->_item;
        }

        //true if _ptr is NULL
        bool is_null()
        {
            return _ptr == NULL;
        }

        //true if left != right
        friend bool operator !=(const Iterator& left, const Iterator& right)
        {
            return left._ptr != right._ptr;
        }

        //true if left == right
        friend bool operator ==(const Iterator& left, const Iterator& right)
        {
            return left._ptr == right._ptr;
        }

        //member operator: ++it or ++it = new_value
        Iterator& operator++()
        {
            _ptr = _ptr->_next;
            return *this;
        }

        //friend operator: it++
        friend Iterator operator++(Iterator& it, int unused)
        {
            assert(it._ptr!=NULL);
            Iterator temp = it;
            it._ptr = it._ptr->_next;
            return temp;
        }

    private:
        node<T>* _ptr;    //pointer being encapsulated
    };

    Stack()
    {
        _top = NULL;
        _size = 0;
    }

    Stack(const Stack<T>& copyMe)
    {
        _top = NULL;
        _size = copyMe._size;
        _top = _copy_list(copyMe._top);
    }

    ~Stack()
    {
        _clear_list(_top);
    }

    Stack<T>& operator=(const Stack<T>& RHS)
    {
        // clear any pre-existing list
        _clear_list(_top);

        _size = RHS._size;
        _top = _copy_list(RHS._top);
        return *this;
    }

    T top()
    {
        return _top->_item;
    }

    bool empty()
    {
        return _size == 0;
    }

    void push(T item)
    {
        _insert_head(_top, item);
        ++_size;
    }

    T pop()
    {
        --_size;
        return _delete_node(_top, _top);
    }

    template<typename TT>
    friend ostream& operator<<(ostream& outs, const Stack<TT>& printMe)
    {
        outs << "Stack:Head->";
        node<TT>* temp = printMe._top;
        while (temp != NULL)
        {
            outs << "[" << temp->_item << "]->";
            temp = temp->_next;
        }
        outs << "|||\n";
        return outs;
    }

    //Iterator to the head node
    Iterator begin() const
    {
        Iterator it(_top);
        return it;
    }
    //Iterator to NULL
    Iterator end() const
    {
        Iterator it(NULL);
        return it;
    }
    
    int size() const { return _size; }

private:
    node<T>* _top;
    int _size;
};

#endif