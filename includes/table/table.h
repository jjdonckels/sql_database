#ifndef TABLE_H
#define TABLE_H
#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <typeinfo>

#include "typedefs.h"
#include "../binary_files/file_record.h"
#include "../binary_files/utilities.h"
#include "../bplustree/multimap.h"
#include "../bplustree/map.h"
#include "../token/token.h"
#include "../token/child_tokens.h"
#include "../queue/MyQueue.h"
#include "../stack/MyStack.h"

using namespace std;

class Table
{
public:
    Table();
    Table(const string& name, const vectorstr& field_names);
    Table(const string& name);

    void insert_into(const vectorstr& values);
    Table select_all(vector<string> fields = {});
    Table select(const vectorstr& fields, 
            const string& field_name, const string& op, const string& field_val);
    Table select(const vectorstr& fields, Queue<Token *> qt);
    Table select(const vectorstr& fields, const vectorstr& conditions);
    vectorlong select_recnos();
    Table table_from_recno_vec(const vector<long>& recno_vec);
    Token* string_to_token(const string& s);

    vectorstr get_table_fields() const;

    void print_table(ostream& outs = cout) const;

    friend ostream& operator << (ostream& outs, const Table& t)
    {
        t.print_table(outs);
        return outs;
    }
    
private:

    void set_fields(const vectorstr& names);
    void write_info(const vectorstr& names);
    vectorstr& get_fields();
    void re_index();

    void add_rec_to_table(const FileRecord& r, long recno);

    string _name;
    string _table_name;
    vector<mmap_sl> _indices;
    vectorstr _field_names_list;
    vectorlong _recno_list;
    map_sl _field_names_map;
    bool _empty;
    long _last_rec_num;

    vectorlong _last_selected_recnos;

    // fstream _file;

    static int _serial;
};


#endif