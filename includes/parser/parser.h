#ifndef PARSER_H
#define PARSER_H

#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <typeinfo>

#include <cctype>


#include "../table/typedefs.h"
#include "../bplustree/btree_array_funcs.h"
#include "../bplustree/multimap.h"
#include "../bplustree/map.h"
#include "../stl_utils/vector_utilities.h"

#include "../queue/MyQueue.h"
#include "../tokenizer/stokenize.h"

using namespace std;



class Parser
{
public:

    Parser();
    Parser(char* s);

    void set_string(char* s);
    int get_column(const string& s);

    bool get_fail() const;

    mmap_ss parse_tree();

private:

    void make_parse_table(int _table[][MAX_COLUMNS]);
    void build_keyword_list();
    bool get_parse_tree();
    void process_string();

    // changes any upper-case characters in s to lower-case
    string string_to_lower(const string& s);

    char _buffer[MAX_BUFFER];
    mmap_ss _ptree;
    map_sl _keywords_map;
    static int _adjacency_table[MAX_ROWS][MAX_COLUMNS];
    bool _fail_flag;
    Queue<Tokenizer_Token> _token_q;

    enum KEYS
    {
        ZERO,
        SELECT, 
        FROM, 
        STAR, 
        WHERE, 
        INSERT, 
        INTO,
        MAKE, 
        DROP, 
        TABLE, 
        FIELDS, 
        VALUES,
        COMMA,
        SYMBOL
    };
};


#endif 