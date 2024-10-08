#ifndef NODE_H
#define NODE_H

#include <iostream>

using namespace std;

template <class T>
struct node
{
    T _item;
    node<T>* _next;

    node(const T& item = T(), node<T>* next = NULL): _item(item), _next(next){}
    friend ostream& operator << (ostream& outs, const node<T>& print_me)
    {
        outs<<"["<<print_me._item<<"]->";
        return outs;
    }
};

#endif