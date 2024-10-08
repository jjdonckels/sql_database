#ifndef MULTIMAP_H
#define MULTIMAP_H

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "bplustree.h"

using namespace std;

template <typename K, typename V>
struct MPair{
    K key;
    vector<V> value_list;

    //--------------------------------------------------------------------------------
    /*      MPair CTORs:
     *  must have these CTORs:
     *  - default CTOR / CTOR with a key and no value: this will create an empty vector
     *  - CTOR with a key AND a value: pushes the value into the value_list
     *  _ CTOR with a key and a vector of values (to replace value_list)
     */
    //--------------------------------------------------------------------------------
    MPair(const K& k=K());
    MPair(const K& k, const V& v);
    MPair(const K& k, const vector<V>& vlist);
    //--------------------------------------------------------------------------------

    //You'll need to overlod << for your vector:
    friend ostream& operator <<(ostream& outs, const MPair<K, V>& print_me)
    {
        int space = 20;
        outs << setw(space) << print_me.key << " : " << print_me.value_list;
        return outs;
    }
    
    friend bool operator ==(const MPair<K, V>& lhs, const MPair<K, V>& rhs)
    {
        return lhs.key == rhs.key;
    }

    friend bool operator < (const MPair<K, V>& lhs, const MPair<K, V>& rhs)
    {
        return lhs.key < rhs.key;
    }

    friend bool operator <= (const MPair<K, V>& lhs, const MPair<K, V>& rhs)
    {
        return lhs.key <= rhs.key;
    }

    friend bool operator > (const MPair<K, V>& lhs, const MPair<K, V>& rhs)
    {
        return lhs.key > rhs.key;
    }

    friend MPair<K, V> operator + (const MPair<K, V>& lhs, const MPair<K, V>& rhs)
    {
        return lhs;
    }
};

template <typename K, typename V>
MPair<K, V>::MPair(const K& k)
{
    key = k;
}

template <typename K, typename V>
MPair<K, V>::MPair(const K& k, const V& v)
{
    key = k; 
    value_list += v;
}

template <typename K, typename V>
MPair<K, V>::MPair(const K& k, const vector<V>& vlist)
{
    key = k;
    value_list = vlist;
}

template <typename K, typename V>
class MMap
{
public:
    typedef BPlusTree<MPair<K, V> > map_base;

    class Iterator{
    public:
        friend class MMap;
        Iterator();
        Iterator(typename map_base::Iterator it);
        Iterator operator ++(int unused);
        Iterator operator ++();
        MPair<K, V> operator *();
        friend bool operator ==(const Iterator& lhs, const Iterator& rhs)
        {
            return lhs._it == rhs._it;
        }
        friend bool operator !=(const Iterator& lhs, const Iterator& rhs)
        {
            return lhs._it != rhs._it;
        }

    private:
        typename map_base::Iterator _it;
    };

    MMap();

    // print MMap as a table with Key on left
    void print_lookup(ostream& outs = cout);

//  Iterators
    Iterator begin();
    Iterator end();

//  Capacity
    int size() const;
    bool empty() const;

//  Element Access
    const vector<V>& operator[](const K& key) const;
    vector<V>& operator[](const K& key);
    vector<V>& at(const K& key);

//  Modifiers
    void insert(const K& k, const V& v);
    void erase(const K& key);
    void clear();

//  Operations:
    bool contains(const K& key) const ;
    vector<V> &get(const K& key);

    Iterator find(const K& key);
    int count(const K& key);

    Iterator lower_bound(const K& key)
    {
        return Iterator(mmap.lower_bound(key));
    }

    Iterator upper_bound(const K& key)
    {
        return Iterator(mmap.upper_bound(key));
    }

    bool is_valid();

    friend ostream& operator<<(ostream& outs, const MMap<K, V>& print_me)
    {
        outs<<print_me.mmap<<endl;
        return outs;
    }

private:
    BPlusTree<MPair<K, V> > mmap;
};

template <typename K, typename V>
MMap<K, V>::Iterator::Iterator()
{
    _it = nullptr;
}

template <typename K, typename V>
MMap<K, V>::Iterator::Iterator(typename map_base::Iterator it)
{
    _it = it;
}

template <typename K, typename V>
typename MMap<K, V>::Iterator MMap<K, V>::Iterator::operator ++(int unused)
{
    Iterator temp = *this;
    ++_it;
    return temp;
}

template <typename K, typename V>
typename MMap<K, V>::Iterator MMap<K, V>::Iterator::operator ++()
{
    ++_it;
    return *this;
}

template <typename K, typename V>
MPair<K, V> MMap<K, V>::Iterator::operator *()
{
    return *_it;
}

template <typename K, typename V>
MMap<K, V>::MMap()
{
    // mmap.clear_tree();
}

template <typename K, typename V>
void MMap<K, V>::print_lookup(ostream& outs)
{
    if (mmap.empty())
    {
        outs << "Multimap is empty";
        return;
    }
    for (Iterator it = begin(); it != end(); ++it)
    {
        outs << *it << endl;
    }
}

template <typename K, typename V>
typename MMap<K, V>::Iterator MMap<K, V>::begin()
{
    return Iterator(mmap.begin());
}

template <typename K, typename V>
typename MMap<K, V>::Iterator MMap<K, V>::end()
{
    return Iterator(mmap.end());
}

template <typename K, typename V>
int MMap<K, V>::size() const 
{
    return mmap.size();
}

template <typename K, typename V>
bool MMap<K, V>::empty() const 
{
    return mmap.empty();
}

template <typename K, typename V>
const vector<V>& MMap<K, V>::operator[](const K& key) const 
{
    return mmap.get(key).value_list;
}

template <typename K, typename V>
vector<V>& MMap<K, V>::operator[](const K& key)
{
    return mmap.get(key).value_list;
}

template <typename K, typename V>
vector<V>& MMap<K, V>::at(const K& key)
{
    return mmap.get(key).value_list;
}

template <typename K, typename V>
void MMap<K, V>::insert(const K& k, const V& v)
{
    mmap.insert(MPair<K, V>(k, v));
}

template <typename K, typename V>
void MMap<K, V>::erase(const K& key)
{
    // never used anywhere
}

template <typename K, typename V>
void MMap<K, V>::clear()
{
    mmap.clear_tree();
}

template <typename K, typename V>
bool MMap<K, V>::contains(const K& key) const 
{
    return mmap.contains(key);
}

template <typename K, typename V>
vector<V>& MMap<K, V>::get(const K& key)
{
    return mmap.get(key).value_list;
}

template <typename K, typename V>
typename MMap<K, V>::Iterator MMap<K, V>::find(const K& key)
{
    return Iterator(mmap.find(key));
}

template <typename K, typename V>
int MMap<K, V>::count(const K& key)
{
    return 0;
}

template <typename K, typename V>
bool MMap<K, V>::is_valid()
{
    return mmap.is_valid();
}


#endif