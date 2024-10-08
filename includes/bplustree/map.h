#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "bplustree.h"


using namespace std;

template <typename K, typename V>
struct Pair{
    K key;
    V value;

    Pair(const K& k=K(), const V& v=V())
    {
        key = k;
        value = v;
    }

    friend ostream& operator <<(ostream& outs, const Pair<K, V>& print_me)
    {
        outs << print_me.key << ":" << print_me.value;
        return outs;
    }

    friend bool operator ==(const Pair<K, V>& lhs, const Pair<K, V>& rhs)
    {
        return lhs.key == rhs.key;
    }

    friend bool operator < (const Pair<K, V>& lhs, const Pair<K, V>& rhs)
    {
        return lhs.key < rhs.key;
    }

    friend bool operator > (const Pair<K, V>& lhs, const Pair<K, V>& rhs)
    {
        return lhs.key > rhs.key;
    }

    friend bool operator <= (const Pair<K, V>& lhs, const Pair<K, V>& rhs)
    {
        return lhs.key <= rhs.key;
    }

    friend Pair<K, V> operator + (const Pair<K, V>& lhs, const Pair<K, V>& rhs)
    {
        return lhs;
    }
};


template <typename K, typename V>
class Map
{
public:
    typedef BPlusTree<Pair<K, V> > map_base;
    class Iterator{
    public:
        friend class Map;
        Iterator();
        Iterator(typename map_base::Iterator it);
        Iterator operator ++(int unused);
        Iterator operator ++();
        Pair<K, V> operator *();
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

    Map();

    // big three
    // Map(const Map<K, V>& other);
    // ~Map();
    // Map<K, V>& operator =(const Map<K, V>& rhs);

//  Iterators
    Iterator begin();
    Iterator end();

//  Capacity
    int size() const;
    bool empty() const;

//  Element Access
    V& operator[](const K& key);
    V& at(const K& key);
    const V& at(const K& key) const;

//  Modifiers
    void insert(const K& k, const V& v);
    void erase(const K& key);
    void clear();
    V get(const K& key);

//  Operations:
    Iterator find(const K& key);
    bool contains(const Pair<K, V>& target) const;

    int count(const K& key)
    {
        // not used anywhere
        return 0;
    }

    Iterator lower_bound(const K& key)
    {
        return Iterator(map.lower_bound(key));
    }

    Iterator upper_bound(const K& key)
    {
        return Iterator(map.upper_bound(key));
    }

    // not used anywhere
    bool is_valid(){return map.is_valid();}

    friend ostream& operator<<(ostream& outs, const Map<K, V>& print_me)
    {
        outs<<print_me.map<<endl;
        return outs;
    }
private:
    int key_count;
    BPlusTree<Pair<K, V> > map;
};

template <typename K, typename V>
Map<K, V>::Iterator::Iterator()
{
    _it = nullptr;
}

template <typename K, typename V>
Map<K, V>::Iterator::Iterator(typename map_base::Iterator it)
{
    _it = it;
}

template <typename K, typename V>
typename Map<K, V>::Iterator  Map<K, V>::Iterator::operator ++(int unused)
{
    Iterator temp = *this;
    ++_it;
    return temp;
}

template <typename K, typename V>
typename Map<K, V>::Iterator  Map<K, V>::Iterator::operator ++()
{
    ++_it;
    return *this;
}

template <typename K, typename V>
Pair<K, V>  Map<K, V>::Iterator::operator *()
{
    return *_it;
}

template <typename K, typename V>
Map<K, V>::Map()
{
    key_count = 0;
}

// template <typename K, typename V>
// Map<K, V>::Map(const Map<K, V>& other)
// {
//     key_count = other.key_count;
//     map = other.map;
// }

// template <typename K, typename V>
// Map<K, V>::~Map()
// {
//     map.clear_tree();
// }

// template <typename K, typename V>
// Map<K, V>& Map<K, V>::operator=(const Map<K, V>& rhs)
// {
//     // clear any existing map stuff
//     key_count = rhs.key_count;
//     map.clear_tree();


// }

template <typename K, typename V>
typename Map<K, V>::Iterator  Map<K, V>::begin() 
{
    return Iterator(map.begin());
}

template <typename K, typename V>
typename Map<K, V>::Iterator  Map<K, V>::end()
{
    return Iterator(map.end());
}

template <typename K, typename V>
int Map<K, V>::size() const 
{
    return map.size();
}

template <typename K, typename V>
bool Map<K, V>::empty() const 
{
    return map.empty();
}

template <typename K, typename V>
V& Map<K, V>::operator[](const K& key)
{
    if (!contains(key))
        ++key_count;
    return map.get(key).value;
}

template <typename K, typename V>
V& Map<K, V>::at(const K& key)
{
    if (!contains(key))
        ++key_count;
    return map.get(key).value;
}

template <typename K, typename V>
const V& Map<K, V>::at(const K& key) const 
{
    return map.get(key).value;
}

template <typename K, typename V>
void Map<K, V>::insert(const K& k, const V& v)
{
    map.insert(Pair<K, V>(k, v));
    ++key_count;
}

template <typename K, typename V>
void Map<K, V>::erase(const K& key)
{
    // never used anywhere
}

template <typename K, typename V>
void Map<K, V>::clear() 
{
    map.clear_tree();
}

template <typename K, typename V>
V Map<K, V>::get(const K& key)
{
    return map.get(key).value;
}

template <typename K, typename V>
typename Map<K, V>::Iterator Map<K, V>::find(const K& key)
{
    return Iterator(map.find(key));
}

template <typename K, typename V>
bool Map<K, V>::contains(const Pair<K, V>& target) const 
{
    return map.contains(target);
}


#endif