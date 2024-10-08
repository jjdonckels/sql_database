#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cassert>
#include "btree_array_funcs.h"

using namespace std;

template <class T>
class BPlusTree
{
public:
    class Iterator{
    public:
        friend class BPlusTree;
        Iterator(BPlusTree<T>* _it=nullptr, int _key_ptr = 0):
                                                it(_it), key_ptr(_key_ptr){}

        T operator *();

        Iterator operator++(int un_used);
        Iterator operator++();
        friend bool operator ==(const Iterator& lhs, const Iterator& rhs)
        {
            return lhs.it == rhs.it && lhs.key_ptr == rhs.key_ptr;
        }
        friend bool operator !=(const Iterator& lhs, const Iterator& rhs)
        {
            return lhs.it != rhs.it || lhs.key_ptr != rhs.key_ptr;
        }
        void print_Iterator(){
            if (it){
                cout<<"iterator: ";
                print_array(it->data, it->data_count, key_ptr);
            }
            else{
                cout<<"iterator: NULL, key_ptr: "<<key_ptr<<endl;
            }
        }
        bool is_null(){return !it;}
        void info(){
            cout<<endl<<"Iterator info:"<<endl;
            cout<<"key_ptr: "<<key_ptr<<endl;
            cout<<"it: "<<*it<<endl;
        }

    private:
        BPlusTree<T>* it;
        int key_ptr;
    };

    BPlusTree(bool dups = false);
    BPlusTree(T *a, int size, bool dups = false);

    //big three:
    BPlusTree(const BPlusTree<T>& other);
    ~BPlusTree();
    BPlusTree<T>& operator =(const BPlusTree<T>& RHS);

    void copy_tree(const BPlusTree<T>& other);  //copy other into this object
    void copy_tree(const BPlusTree<T>& other, BPlusTree<T>*& last_node);

    void insert(const T& entry);   //insert entry into the tree
    void remove(const T& entry);   //remove entry from the tree
    void clear_tree();             //clear this object (delete all nodes etc.)

    bool contains(const T& entry) const; //true if entry can be found
    T& get(const T& entry);              //return a reference to entry
    const T& get(const T& entry)const;   //return a reference to entry
    T& get_existing(const T& entry);     //return a reference to entry
    Iterator find(const T& key);         //return an iterator to this key.
                                         //     NULL if not there.
    Iterator lower_bound(const T& key);  //return first that goes NOT BEFORE
                                         // key entry or next if does not
                                         // exist: >= entry
    Iterator upper_bound(const T& key);  //return first that goes AFTER key
                                         //exist or not, the next entry  >entry

    int size() const;                    //count the number of elements

    bool empty() const;                  //true if the tree is empty

    void print_tree(int level = 0,
                    ostream &outs=cout) const;
    friend ostream& operator<<(ostream& outs,
                               const BPlusTree<T>& print_me){
        print_me.print_tree(0, outs);
        return outs;
    }

    bool is_valid();
    string in_order();
    string pre_order();
    string post_order();

    Iterator begin();
    Iterator end();
    ostream& list_keys(Iterator from = NULL, Iterator to = NULL){
        if (from == NULL) from = begin();
        if (to == NULL) to = end();
        for (Iterator it = from; it != to; it++)
            cout<<*it<<" ";
        return cout;

    }

private:
    static const int MINIMUM = 1;
    static const int MAXIMUM = 2 * MINIMUM;

    bool dups_ok;                        //true if duplicate keys are allowed
    int data_count;                      //number of data elements
    T data[MAXIMUM + 1];                 //holds the keys
    int child_count;                     //number of children
    BPlusTree* subset[MAXIMUM + 2];      //subtrees
    BPlusTree* next;
    bool is_leaf() const
    {return child_count==0;}             //true if this is a leaf node


    T* find_ptr(const T& entry);         //return a pointer to this key.
                                         // NULL if not there.

    //insert element functions
    void loose_insert(const T& entry);   //allows MAXIMUM+1 data elements in
                                         //   the root
    void fix_excess(int i);              //fix excess in child i

    //remove element functions:
    void loose_remove(const T& entry);  //allows MINIMUM-1 data elements
                                        //  in the root

    BPlusTree<T>* fix_shortage(int i);  //fix shortage in child i
    // and return the smallest key in this subtree
    BPlusTree<T>* get_smallest_node();
    void get_smallest(T& entry);      //entry := leftmost leaf
    void get_biggest(T& entry);       //entry := rightmost leaf
    void remove_biggest(T& entry);    //remove the biggest child of tree->entry
    void transfer_left(int i);        //transfer one element LEFT from child i
    void transfer_right(int i);       //transfer one element RIGHT from child i
    BPlusTree<T> *merge_with_next_subset(int i); //merge subset i with  i+1

};



template <class T>
T BPlusTree<T>::Iterator::operator *()
{
    return it->data[key_ptr];
}

template <class T>
typename BPlusTree<T>::Iterator BPlusTree<T>::Iterator::operator++(int un_used)
{
    Iterator temp = *this;

    ++key_ptr; // move to next entry in data array

    // move to next node if key_ptr goes out of bounds
    if (key_ptr >= it->data_count)
    {
        key_ptr = 0;
        it = it->next;
    }

    return temp;
}

template <class T>
typename BPlusTree<T>::Iterator BPlusTree<T>::Iterator::operator++()
{
    ++key_ptr; // move to next entry in data array

    // move to next node if key_ptr goes out of bounds 
    if (key_ptr >= it->data_count)
    {
        key_ptr = 0;
        it = it->next;
    }

    return *this;
}




template <class T>
BPlusTree<T>::BPlusTree(bool dups)
{
    dups_ok = dups;
    data_count = 0;
    child_count = 0;

    // set data array to all default values and clear any garbage values
    for (int i = 0; i < MAXIMUM + 1; ++i)
    {
        data[i] = T();
    }

    // set all subset ptrs to null
    for (int i = 0; i < MAXIMUM + 2; ++i)
    {
        subset[i] = nullptr;
    }

    // set next ptr to null
    next = nullptr;
}

template <class T>
BPlusTree<T>::BPlusTree(T* a, int size, bool dups)
{
    dups_ok = dups;
    data_count = 0;
    child_count = 0;

    // set data array to all default values and clear any garbage values
    for (int i = 0; i < MAXIMUM + 1; ++i)
    {
        data[i] = T();
    }

    // set all subset ptrs to null
    for (int i = 0; i < MAXIMUM + 2; ++i)
    {
        subset[i] = nullptr;
    }

    // set next ptr to null
    next = nullptr;

    // insert entries from a into data
    for (int i = 0; i < size; ++i)
    {
        // cout << "\n\ninsert " << a[i] << "\n\n";
        T to_insert = a[i];
        insert(to_insert);
    }
}

template <class T>
BPlusTree<T>::BPlusTree(const BPlusTree<T>& other)
{
    copy_tree(other);
}

template <class T>
BPlusTree<T>::~BPlusTree()
{
    clear_tree();
}

template <class T>
BPlusTree<T>& BPlusTree<T>::operator = (const BPlusTree<T>& RHS)
{
    // case for empty RHS
    if (RHS.empty())
    {
        data_count = 0; 
        child_count = 0;
        dups_ok = RHS.dups_ok;
        next = nullptr;
        return *this;
    }
    // check self-reference
    if (this == &RHS)
    {
        return *this;
    }

    // clear *this
    clear_tree();

    copy_tree(RHS);
    return *this;
}

template <class T>
void BPlusTree<T>::copy_tree(const BPlusTree<T>& other)
{
    BPlusTree<T>* last_node = nullptr;
    copy_tree(other, last_node);
}

template <class T>
void BPlusTree<T>::copy_tree(const BPlusTree<T>& other, BPlusTree<T>*& last_node)
{
    // copy root data
    dups_ok = other.dups_ok;
    copy_array(data, other.data, data_count, other.data_count);
    child_count = other.child_count;

    if (is_leaf())
        last_node = this;
    else 
        next = nullptr;

    // copy subtrees
    for (int i = 0; i < other.child_count; ++i)
    {
        subset[i] = new BPlusTree<T>(dups_ok);
        if (last_node && other.subset[i]->is_leaf())
        {
            last_node->next = subset[i];
            last_node = subset[i];
        }
        subset[i]->copy_tree(*other.subset[i], last_node);
    }
}

template <class T>
void BPlusTree<T>::insert(const T& entry)
{
    loose_insert(entry);

    // if excess at the root, grow tree
    if (data_count > MAXIMUM)
    {
        // create temp root to copy everything into
        BPlusTree<T>* temp_root = new BPlusTree<T>(dups_ok);

        // copy current root data and subsets into temp root
        copy_array(temp_root->data, data, temp_root->data_count, data_count);
        copy_array(temp_root->subset, subset, temp_root->child_count, child_count);

        // clear original root
        data_count = 0;
        
        // point subset to temp root
        child_count = 1;
        subset[0] = temp_root;

        // fix excess on subset[0]
        fix_excess(0);
    }
}

template <class T>
void BPlusTree<T>::remove(const T& entry)
{
    if (empty())
        return;

    loose_remove(entry);

    // shrink tree if necessary
    if (child_count == 1 && data_count == 0)
    {
        BPlusTree<T>* temp_tree = subset[0];

        // copy everything from subset[0] into root
        copy_array(data, temp_tree->data, data_count, temp_tree->data_count);
        copy_array(subset, temp_tree->subset, child_count, temp_tree->child_count);

        // update next
        next = temp_tree->next;

        // delete temp_tree
        // set temp_tree data and child count to 0 so when deleted the
        // entire copy of the tree won't be wiped out
        temp_tree->data_count = 0;
        temp_tree->child_count = 0;
        delete temp_tree;
        temp_tree = nullptr;
    }
}

template <class T>
void BPlusTree<T>::clear_tree()
{
    // delete children 
    for (int i = 0; i < child_count; ++i)
    {
        subset[i]->clear_tree();
        delete subset[i];
        subset[i] = nullptr;
    }
    data_count = 0;
    child_count = 0;
    next = nullptr;
}

template <class T>
bool BPlusTree<T>::contains(const T& entry) const 
{
    int index = first_ge(data, data_count, entry);
    bool found = index < data_count && data[index] == entry;
    if (found && is_leaf())
        return true;
    else if (!found && is_leaf())
        return false;
    else if (found && !is_leaf())
        return true;
    else if (!found && !is_leaf())
        return subset[index]->contains(entry);
    else 
    {
        cout << "\n\nfinal branch in BPlusTree contains, shouldn't be reached\n\n";
        return false;
    }
}

template <class T>
T& BPlusTree<T>::get(const T& entry)
{
    //If entry is not in the tree, add it to the tree
    const bool debug = false;
    if (!contains(entry))
        insert(entry);

    return get_existing(entry);
}

template <class T>
const T& BPlusTree<T>::get(const T& entry) const 
{
    assert(contains(entry) && "failed const T& get(), entry should exist in BPlusTree");

    int index = first_ge(data, data_count, entry);
    bool found = index < data_count && data[index] == entry;
    if (found && is_leaf())
        return data[index];
    else if (found && !is_leaf())
        return subset[index + 1]->get(entry);
    else if (!found && !is_leaf())
        return subset[index]->get(entry);
    else 
    {
        cout << "\n\nfinal branch in BPlusTree const T& get, shouldn't be reached\n\n";
        return entry;
    }
}

template <class T> 
T& BPlusTree<T>::get_existing(const T& entry)
{
    const bool debug = false;
    int i = first_ge(data, data_count, entry);
    bool found = (i<data_count && data[i]==entry);
    if (is_leaf())
    {
        if (found)
        {
            return data[i];
        }
        else
        {
            if (debug) cout<<"get_existing was called with nonexistent entry"<<endl;
            assert(found);
        }
    }
    if (found) //inner node
        return subset[i+1]->get(entry);
    else //not found yet...
        return subset[i]->get(entry);
}

template <class T>
typename BPlusTree<T>::Iterator BPlusTree<T>::find(const T& key)
{
    int index = first_ge(data, data_count, key);
    bool found = index < data_count && data[index] == key;
    if (found && is_leaf())
        return Iterator(this, index);
    else if (!found && is_leaf())
        return Iterator();
    else if (found && !is_leaf())
        return subset[index + 1]->find(key);
    else if (!found && !is_leaf())
        return subset[index]->find(key);
    else 
    {
        cout << "\n\nfinal branch in BPlusTree find, shouldn't be reached\n\n";
        return Iterator();
    }
}

template <class T>
typename BPlusTree<T>::Iterator BPlusTree<T>::lower_bound(const T& key) 
{
    int index = first_ge(data, data_count, key);
    bool found = index < data_count && data[index] == key;
    if (found && is_leaf())
    {
        return Iterator(this, index);
    }
    else if (found && !is_leaf())
        return subset[index + 1]->lower_bound(key);
    else if (!found && !is_leaf())
        return subset[index]->lower_bound(key);
    else // !found && is_leaf()
    {
        // branch for key greater than max bplustree value
        if (index >= data_count && !next)
        {
            return Iterator();
        }
        if (index >= data_count && next)
        {

            Iterator temp(this, data_count - 1);
            ++temp;
            return temp;

        }
        return Iterator(this, index);
    }
}

template <class T>
typename BPlusTree<T>::Iterator BPlusTree<T>::upper_bound(const T& key) 
{
    Iterator lb = lower_bound(key);
    if (!lb.is_null() && *lb == key)
        ++lb;
    return lb;
}

template <class T>
int BPlusTree<T>::size() const 
{
    int size = 0;
    size += data_count;
    for (int i = 0; i < child_count; ++i)
    {
        size += subset[i]->size();
    }
    return size;
}

template <class T>
bool BPlusTree<T>::empty() const 
{
    return data_count == 0 && child_count == 0;
}

template <class T>
void BPlusTree<T>::print_tree(int level, ostream& outs) const 
{
    if (level == 0)
    cout << "\n\ndc: " << data_count << " | cc: " << child_count << "\n\n";
    int space = 4;
    bool debug = true;
    if (debug) space = 20;
    if (data_count < 1) // case for empty bt
        return;

    //1. print the last child (if any)
    if (child_count > 0 && child_count == data_count + 1 && subset[child_count - 1])
    {
        subset[child_count - 1]->print_tree(level + 1, outs);
    }

    //2. print current data[i] then current subset[i] from back to front
    if (is_leaf() && next)
        outs << setw(space * level) << "" << "^\n";
    outs << setw(space * level) << "" << "\u23B4\n";
    for (int i = data_count - 1; i >= 0; --i)
    {
        outs << setw(space * level) << "" << data[i];
        if (debug)
        {
            outs << " | d: " << data_count;
            outs << ", c: " << child_count;
        }
        cout << endl;
        // cout << subset[i];
        if (i == 0)
            outs << setw(space * level) << "" << "\u23B5\n";
        if (child_count == data_count + 1 && subset[i])
            subset[i]->print_tree(level + 1, outs);
    }
}

template <class T>
bool BPlusTree<T>::is_valid()
{
    // stub; assume this is a valid bpt
    return true;
}

template <class T>
string BPlusTree<T>::in_order()
{
    string str = "";
    for (int i = 0; i < data_count; ++i)
    {
        if (subset[i])
            str += subset[i]->in_order();
        str += to_string(data[i]) + "|";
    }
    if (subset[data_count])
        str += subset[data_count]->in_order();
    return str;
}

template <class T>
string BPlusTree<T>::pre_order()
{
    string str = "";
    for (int i = 0; i < data_count; ++i)
    {
        str += to_string(data[i]) + "|";
        if (subset[i])
            str += subset[i]->pre_order();

    }
    if (subset[data_count])
        str += subset[data_count]->pre_order();
    return str;
}

template <class T>
string BPlusTree<T>::post_order()
{
    string str = "";

    if (subset[0])
        str += subset[0]->post_order();
    for (int i = 0; i < data_count; ++i)
    {
        if (subset[i + 1])
            str += subset[i + 1]->post_order();

        str += to_string(data[i]) + "|";
    }

    return str;
}

template <class T>
typename BPlusTree<T>::Iterator BPlusTree<T>::begin()
{
    if (is_leaf())
        return Iterator(this, 0);
    else 
        return subset[0]->begin();
}

template <class T>
typename BPlusTree<T>::Iterator BPlusTree<T>::end() 
{
    return Iterator(nullptr, 0);
}

template <class T>
T* BPlusTree<T>::find_ptr(const T& entry) 
{
    if (!contains(entry))
        return nullptr;
    
    Iterator target = find(entry);
    return &(target.it->data[target.key_ptr]);
}

template <class T>
void BPlusTree<T>::loose_insert(const T& entry)
{
    int index = first_ge(data, data_count, entry);
    if (!dups_ok && index < data_count && data[index] == entry) // duplicate
    {
        data[index] = entry; // overwrite dups
        return;
    }
    else if (is_leaf()) // reached lowest level, insert into array
    {
        insert_item(data, index, data_count, entry);
    }
    else // go one branch deeper into tree and repeat
        subset[index]->loose_insert(entry);

    // fix excess if needed
    if (subset[index] && subset[index]->data_count > MAXIMUM)
        fix_excess(index);  
}

template <class T>
void BPlusTree<T>::fix_excess(int i)
{
    // create new child 
    insert_item(subset, i + 1, child_count, new BPlusTree<T>(dups_ok));

    // split current child and move over to new child
    // split data in child
    split(subset[i]->data, subset[i]->data_count,
            subset[i + 1]->data, subset[i + 1]->data_count);

    // split subset in child
    split(subset[i]->subset, subset[i]->child_count, 
            subset[i + 1]->subset, subset[i + 1]->child_count);

    // detach last value from child data and insert to parent data
    T val;
    detach_item(subset[i]->data, subset[i]->data_count, val);
    insert_item(data, i, data_count, val);

    // update pointers if child with fixed excess is leaf
    if (subset[i]->is_leaf())
    {
        // prepend data to new node
        T to_prepend = data[i];
        insert_item(subset[i + 1]->data, 0, subset[i + 1]->data_count, to_prepend);

        // point new node at old node's next
        subset[i + 1]->next = subset[i]->next;

        // point old node to new node
        subset[i]->next = subset[i + 1];
    }
}

template <class T>
void BPlusTree<T>::loose_remove(const T& entry)
{
    int index = first_ge(data, data_count, entry);
    if (is_leaf())
    {
        if (index < data_count && data[index] == entry)
        {
            // delete entry if present in leaf
            T temp;
            delete_item(data, index, data_count, temp);
        }
        // not on tree if didn't enter above if statement, just return
    }
    else if (!is_leaf() && index < data_count && data[index] == entry)
    {
        // branch for hitting breadcrumbs
        // continue loose_remove until leaf in subset[index + 1]
        subset[index + 1]->loose_remove(entry);

        if (subset[index + 1]->data_count < MINIMUM)
            fix_shortage(index + 1);

        T smallest;
        if (subset[index + 1])
        // check if subset[index + 1] still exists bc it might have been deleted from merging
            subset[index + 1]->get_smallest(smallest);
        else 
        {
            subset[index]->get_smallest(smallest);
        }
        data[index] = smallest;
    }
    else if (!is_leaf())
    {
        // go down straight because no breadcrumbs
        subset[index]->loose_remove(entry);

        if (subset[index]->data_count < MINIMUM)
            fix_shortage(index);
    }
}

template <class T>
BPlusTree<T>* BPlusTree<T>::fix_shortage(int i)
{
    // not yet functional
    cout << "\n\nfix shortage called\n\n";
    if (i < child_count - 1 && subset[i + 1]->data_count > MINIMUM)
    {
        cout << "\n\ntransfer left\n\n";
        transfer_left(i);
    }
    else if (i >= 1 && i < child_count && subset[i - 1]->data_count > MINIMUM)
    {
        cout << "\n\ntransfer right\n\n";
        transfer_right(i);
    }
    else 
    {
        merge_with_next_subset(i);

        // DEBUG 
        // cout << "\n\nfinished merge\n\n";
        // cout << *this << "\n\n";
    }

    cout << "\n\n" << *this << "\n\n";
    return nullptr;
}

template <class T>
BPlusTree<T>* BPlusTree<T>::get_smallest_node()
{
    return nullptr;
}

template <class T>
void BPlusTree<T>::get_smallest(T& entry)
{
    if (is_leaf())
        entry = data[0];
    else 
        subset[0]->get_smallest(entry);
}

template <class T>
void BPlusTree<T>::get_biggest(T& entry)
{

}

template <class T>
void BPlusTree<T>::remove_biggest(T& entry)
{

}

template <class T>
void BPlusTree<T>::transfer_left(int i)
{
    // borrow from child i + 1
    T borrowed;
    delete_item(subset[i + 1]->data, 0, subset[i + 1]->data_count, borrowed);

    // transfer borrowed value to shortage child i
    attach_item(subset[i]->data, subset[i]->data_count, borrowed);

    // update breadcrumb in data
    data[i] = subset[i + 1]->data[0];
}

template <class T>
void BPlusTree<T>::transfer_right(int i)
{
    // borrow from child i - 1
    T borrowed;
    delete_item(subset[i - 1]->data, subset[i - 1]->data_count - 1, 
                subset[i - 1]->data_count, borrowed);

    // transfer borrowed value to shortage child i
    insert_item(subset[i]->data, 0, subset[i]->data_count, borrowed);

    // update breadcrumb in 
    data[i - 1] = subset[i]->data[0];
}

template <class T>
BPlusTree<T>* BPlusTree<T>::merge_with_next_subset(int i)
{
    // check to merge with left sibling first
    if (i >= 1 && i < child_count)
    {
        // branch to merge with left

        // DEBUG 
        subset[i]->next = nullptr;
        subset[i - 1]->next = subset[i]->next;
        cout << "\n\nmerge with left\n\n";

        // merge subset data
        merge(subset[i - 1]->data, subset[i - 1]->data_count, 
                subset[i]->data, subset[i]->data_count);

        // merge subset children data
        if (!subset[i]->is_leaf())
        merge(subset[i - 1]->subset[subset[i - 1]->child_count - 1]->data, subset[i - 1]->subset[subset[i - 1]->child_count - 1]->data_count, 
                subset[i]->subset[0]->data, subset[i]->subset[0]->data_count);

        // DEBUG 
        // cout << "\n\ncheckpoint 1\n\n";

        // delete data i - 1, which is breadcrumb of child that had shortage
        T temp;
        delete_item(data, i - 1, data_count, temp);

        // DEBUG 
        // cout << "\n\ncheckpoint 2\n\n";

        // point next to next node if not null
        // subset[i - 1]->next = subset[i]->next;

        // cout << "\n\nsubset[i]->next: " << subset[i]->next << "\n\n";

        // delete node that had the shortage
        delete subset[i];
        subset[i] = nullptr;

        // delete shortage node from subset array
        BPlusTree<T>* trash;
        delete_item(subset, i, child_count, trash);
    }
    else if (i < child_count - 1)
    {
        subset[i + 1]->next = nullptr;
        subset[i]->next = subset[i + 1]->next;
        // branch to merge with right
        cout << "\n\nmerge with right\n\n";

        // merge subset data
        merge(subset[i]->data, subset[i]->data_count, 
                subset[i + 1]->data, subset[i + 1]->data_count);

        // merge subset children
        merge(subset[i]->subset, subset[i]->child_count, 
                subset[i + 1]->subset, subset[i + 1]->child_count);

        // delete data i, which is breadcrumb of node that got merged
        T temp;
        delete_item(data, i, data_count, temp);

        // point next to next node if not null
        // subset[i]->next = subset[i + 1]->next;

        // cout << "\n\nsubset[i + 1]->next: " << subset[i + 1]->next << "\n\n";

        // delete node that got merged
        delete subset[i + 1];
        subset[i + 1] = nullptr;

        // delete node that got merged from subset array 
        BPlusTree<T>* trash;
        delete_item(subset, i + 1, child_count, trash);
    }

    return nullptr;
}


#endif