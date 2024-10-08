#ifndef BTREE_ARRAY_FUNCS_H
#define BTREE_ARRAY_FUNCS_H

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <list>

using namespace std;

template <class T>
T maximal(const T& a, const T& b);                      //return the larger of the two items

template <class T>
int index_of_maximal(T data[ ], int n);                 //return index of the largest item in data

template <class T>
void ordered_insert(T data[ ], int& n, T entry);        //insert entry into the sorted array
                                                        //data with length n

template <class T>
int first_ge(const T data[ ], int n, const T& entry);   //return the first element in data that is
                                                        //not less than entry

template <class T>
void attach_item(T data[ ], int& n, const T& entry);    //append entry to the right of data

template <class T>
void insert_item(T data[ ], int i, int& n, T entry);    //insert entry at index i in data

template <class T>
void detach_item(T data[ ], int& n, T& entry);          //remove the last element in data and place
                                                        //it in entry

template <class T>
void delete_item(T data[ ], int i, int& n, T& entry);   //delete item at index i and place it in entry

template <class T>
void merge(T data1[ ], int& n1, T data2[ ], int& n2);   //append data2 to the right of data1

template <class T>
void split(T data1[ ], int& n1, T data2[ ], int& n2);   //move n/2 elements from the right of data1
                                                        //and move to data2

template <class T>
void copy_array(T dest[], const T src[],
                int& dest_size, int src_size);              //copy src[] into dest[]

template <class T>
void print_array(const T data[], int n, int pos = -1);  //print array data

template <class T>
bool is_gt(const T data[], int n, const T& item);       //item > all data[i]

template <class T>
bool is_le(const T data[], int n, const T& item);       //item <= all data[i]

//-------------- Vector Extra operators: ---------------------

template <class T>
ostream& operator<<(ostream& outs, const vector<T>& list); //print vector list

template <class T>
vector<T>& operator+=(vector<T>& list, const T& addme); //list.push_back addme

template <typename T>
vector<T> vec_intersect(const vector<T>& first, const vector<T>& second);

template <typename T>
vector<T> vec_union(const vector<T>& first, const vector<T>& second);


//-----------------DEFINITIONS-------------------------
template <class T>
T maximal(const T& a, const T& b)
{
    if (a > b)
        return a;
    return b;
}

template <class T>
int index_of_maximal(T data [], int n)
{
    if (n == 0)
        return -1;
    if (n == 1)
        return 0;

    T max = data[0];
    int max_index = 0;
    
    for (int i = 1; i < n; ++i)
    {
        if (data[i] > max)
        {
            max = data[i];
            max_index = i;
        }
    }

    return max_index;
}

template <class T>
void ordered_insert(T data[], int& n, T entry)
{
    int curr_index = 0;
    while (curr_index < n && data[curr_index] < entry)
        ++curr_index;

    // index is now at the right spot to be inserted
    insert_item(data, curr_index, n, entry);
}

template <class T>
int first_ge(const T data[], int n, const T& entry)
{
    if (n == 0)
        return 0;
    
    int curr_index = 0;

    while (curr_index < n && data[curr_index] < entry)
        ++curr_index;

    return curr_index;
}

template <class T>
void attach_item(T data[], int& n, const T& entry)
{
    data[n++] = entry;
}

template <class T>
void insert_item(T data[], int i, int& n, T entry)
{
    ++n; // increment size

    // shift everything after i to the right
    for (int curr_index = n - 1; curr_index > i; --curr_index)
    {
        swap(data[curr_index], data[curr_index - 1]);
    }

    data[i] = entry;
}

template <class T>
void detach_item(T data[], int& n, T& entry)
{
    entry = data[--n];
}

template <class T>
void delete_item(T data[], int i, int& n, T& entry)
{
    entry = data[i];

    // shift everything to the right of i down 1
    for (int curr_index = i; curr_index < n - 1; ++curr_index)
        swap(data[curr_index], data[curr_index + 1]);
    
    --n;
}

template <class T>
void merge(T data1[], int& n1, T data2[], int& n2)
{
    for (int i = 0; i < n2; ++i)
    {
        attach_item(data1, n1, data2[i]);
    }
    n2 = 0;
}

template <class T>
void split(T data1[], int& n1, T data2[], int& n2)
{
    int num_to_move = n1 / 2;

    for (int i = 0; i < num_to_move; ++i)
    {
        T val;
        detach_item(data1, n1, val);
        insert_item(data2, 0, n2, val);
    }
}

template <class T>
void copy_array(T dest[], const T src[], int& dest_size, int src_size)
{
    dest_size = 0;
    for (int i = 0; i < src_size; ++i)
    {
        attach_item(dest, dest_size, src[i]);
    }
}

template <class T>
void print_array(const T data[], int n, int pos)
{
    int i = 0;
    if (pos >= 0)
        i = pos;
    for (; i < n; ++i)
    {
        cout << data[i] << " ";
    }
    cout << "\nsize: " << n;
}

template <class T>
bool is_gt(const T data[], int n, const T& item)
{
    for (int i = 0; i < n; ++i)
    {
        if (item <= data[i])
            return false;
    }
    return true;
}

template <class T>
bool is_le(const T data[], int n, const T& item)
{
    for (int i = 0; i < n; ++i)
    {
        if (item > data[i])
            return false;
    }
    return true;
}

template <typename T>
ostream& operator<<(ostream& outs, const vector<T>& list)
{
    if (!list.empty())
        outs << list.at(0);
    for (int i = 1; i < list.size(); ++i)
    {
        outs << " " << list.at(i);
    }
    return outs;
}

template <typename T>
vector<T>& operator+=(vector<T>& list, const T& addme)
{
    list.push_back(addme);
    return list;
}

template <typename T>
vector<T> vec_intersect(const vector<T>& first, const vector<T>& second)
{
    vector<T> intersection;

    for (int i = 0; i < first.size(); ++i)
    {
        bool found = false;
        for (int j = 0; j < second.size(); ++j)
        {
            if (first.at(i) == second.at(j))
                found = true;
        }
        if (found)
            intersection.push_back(first.at(i));
    }

    return intersection;
}

template <typename T>
vector<T> vec_union(const vector<T>& first, const vector<T>& second)
{
    vector<T> u = first;

    for (int i = 0; i < second.size(); ++i)
    {
        bool found = false;
        for (int j = 0; j < first.size(); j++)
        {
            if (second.at(i) == first.at(j))
                found = true;
        }

        if (!found)
            u.push_back(second.at(i));
    }

    return u;
}


#endif