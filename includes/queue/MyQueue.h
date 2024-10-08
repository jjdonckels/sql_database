#ifndef MYQUEUE_H
#define MYQUEUE_H
#include "../node/node.h"
#include "../linked_list_functions/linkedlistfunctionsTemplated.h"
#include <assert.h>

template <typename T>
class Queue
{
public:

    class Iterator{
    public:
        //give access to list to access _ptr
        friend class Queue;
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

        //member operator:++it or ++it = new_value
        Iterator& operator++()
        {
            _ptr = _ptr->_next;
            return *this;
        }

        //friend operator: it++
        friend Iterator operator++(Iterator& it, int unused)
        {
            Iterator temp = it;
            it._ptr = it._ptr->_next;
            return temp;
        }

    private:
        node<T>* _ptr; // pointer being encapsulated
    };

    Queue()
    {
        _front = NULL;
        _rear = NULL;
        _size = 0;
    }

    Queue(const Queue<T>& copyMe)
    {
        _front = NULL;
        _rear = NULL;
        _size = 0;
        _rear = _copy_list(_front, copyMe._front);
        _size = copyMe._size;
    }

    ~Queue()
    {
        _clear_list(_front);
    }

    Queue<T>& operator=(const Queue<T>& RHS)
    {
        // clear any pre-existing list
        _clear_list(_front);
        
        _rear = _copy_list(_front, RHS._front);
        _size = RHS._size;
        return *this;
    }

    bool empty()
    {
        return _size == 0;
    }

    T front()
    {
        return _front->_item;
    }

    T back()
    {
        return _rear->_item;
    }

    void push(T item)
    {        
        //case for empty list
        if (_front == NULL)
        {
            _insert_head(_front, item);
            _rear = _front;
        }
        else 
        {
            // case for at least one node
            _rear = _insert_after(_front, _rear, item);
        }
        ++_size;
    }

    T pop()
    {
        assert(!empty() && "cannot pop empty queue");
        --_size;
        return _delete_node(_front, _front);
    }

    //Iterator to the head node
    Iterator begin() const
    {
        Iterator it(_front);
        return it;
    }

    //Iterator to NULL
    Iterator end() const
    {
        Iterator it(NULL);
        return it;
    }

    void print_pointers()
    {
        node<T>* temp = _front;
        cout << "Queue:Head->";
        while (temp != NULL)
        {
            cout << "[" << temp->_item << "]->";
            temp = temp->_next;
        }
        cout << "|||\n";
    }

    int size() const { return _size; }

private:
    node<T>* _front;
    node<T>* _rear;
    int _size;
};

#endif