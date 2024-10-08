#ifndef SQL_H
#define SQL_H

#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <typeinfo>
#include <algorithm>

#include "../table/table.h"
#include "../parser/parser.h"

using namespace std;

class SQL
{
    
public:

    SQL();

    void batch();

    void try_command(string s, ostream& outs = cout);

    Table command(string s);
    vectorlong select_recnos();

private:
    Parser parser;

    // vector<Table> table_list; // holds all tables in database

    Map<string, Table> _table_map;

    vectorstr table_names_list; // holds all table names in database

    // accounts for dropped tables by hiding them
    Map<string, bool> table_visibility_map; 

    // maps table names to indices for table_names_list vector
    // Map<string, int> table_index_map;

    vectorlong last_selected_recnos;
    
    // used for select * to get all fields for given table
    Map<string, vectorstr> table_fields_map; 
};

#endif