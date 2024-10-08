#include "parser.h"

using namespace std;

int Parser::_adjacency_table[MAX_ROWS][MAX_COLUMNS];

Parser::Parser()
{
    // set up adjacency table 
    make_parse_table(_adjacency_table);

    // build keyword list
    build_keyword_list();
}

Parser::Parser(char * s)
{
    // copy string to internal buffer
    strcpy(_buffer, s);

    // set up adjacency table 
    make_parse_table(_adjacency_table);

    // build keyword list
    build_keyword_list();

    // set string to be tokenized and put into token queue to be parsed
    set_string(_buffer);
}

void Parser::make_parse_table(int _table[][MAX_COLUMNS])
{
    // set everything to -1
    init_table(_table);

    // set states for select command
    mark_cell(0, _table, SELECT, 1); // select keyword
    mark_cell(1, _table, SYMBOL, 2); // symbols after select keyword
    mark_cell(2, _table, COMMA, 21); // comma after symbol 
    mark_cell(21, _table, SYMBOL, 2); // symbol after comma
    mark_cell(1, _table, STAR, 3); // * after select keyword
    // mark_cell(2, _table, SYMBOL, 2); // additional symbols stay in same state
    mark_cell(2, _table, FROM, 4); // from keyword goes to state 4
    mark_cell(3, _table, FROM, 4); // from keyword goes to state 4
    mark_cell(4, _table, SYMBOL, 5); // single word for table name after from keyword
    mark_cell(5, _table, WHERE, 6); // where keyword goes to state 6
    mark_cell(6, _table, SYMBOL, 7); // symbols after where keyword
    mark_cell(7, _table, SYMBOL, 7); // additional symbols stay in same state

    mark_fail(_table, 21); 
    mark_fail(_table, 0); 
    mark_fail(_table, 1);
    mark_fail(_table, 2); 
    mark_fail(_table, 3);
    mark_fail(_table, 4);
    mark_success(_table, 5); // success state for select_all with no conditions
    mark_fail(_table, 6); 
    mark_success(_table, 7); // success state with conditions


    // set states for insert command 
    mark_cell(0, _table, INSERT, 8); // insert keyword
    mark_cell(8, _table, INTO, 9); // into keyword
    mark_cell(9, _table, SYMBOL, 10); // table name after into keyword
    mark_cell(10, _table, VALUES, 11); // values keyword
    mark_cell(11, _table, SYMBOL, 17); // field values after values keyword
    mark_cell(17, _table, COMMA, 22); // comma after field value
    mark_cell(22, _table, SYMBOL, 17); // field value after comma
    // mark_cell(17, _table, SYMBOL, 17); // additional field values

    mark_fail(_table, 22); 
    mark_fail(_table, 8); 
    mark_fail(_table, 9); 
    mark_fail(_table, 10); 
    mark_fail(_table, 11); 
    mark_success(_table, 17); // success state after reaching field values


    // set states for make table command
    mark_cell(0, _table, MAKE, 12); // make keyword
    mark_cell(12, _table, TABLE, 13); // table keyword
    mark_cell(13, _table, SYMBOL, 14); // table name after table keyword
    mark_cell(14, _table, FIELDS, 15); // fields keyword
    mark_cell(15, _table, SYMBOL, 16); // field names after fields keyword
    mark_cell(16, _table, COMMA, 23); // comma after field name
    mark_cell(23, _table, SYMBOL, 16); // field name after comma
    // mark_cell(16, _table, SYMBOL, 16); // additional field names

    mark_fail(_table, 23); 
    mark_fail(_table, 12); 
    mark_fail(_table, 13); 
    mark_fail(_table, 14); 
    mark_fail(_table, 15); 
    mark_success(_table, 16); // success state after reaching field names


    // set states for drop table command 
    mark_cell(0, _table, DROP, 18); // drop keyword
    mark_cell(18, _table, TABLE, 19); // table keyword after drop keyword
    mark_cell(19, _table, SYMBOL, 20); // table_name after table keyword

    mark_fail(_table, 18); 
    mark_fail(_table, 19); 
    mark_success(_table, 20); // success state after specifying which table to drop
}

void Parser::set_string(char* s)
{
    // clear token queue if not empty
    while (!_token_q.empty())
        _token_q.pop();
    strcpy(_buffer, s);
    if (!_ptree.empty())
        _ptree.clear();
}

void Parser::process_string()
{
    // tokenize input buffer into input queue
    STokenizer stk(_buffer);

    Tokenizer_Token t;

    stk >> t;
    while (stk.more())
    {
        // push tokens into queue
        _token_q.push(t);
        
        // get next token
        stk >> t;
    }
    // now we should have all the tokens in _token_q
    // and we're ready to parse 

    // get_parse_tree returns true if reached success state
    // and false otherwise, so fail flag is opposite of 
    // the return value of get_parse_tree
    _fail_flag = !get_parse_tree();
}

bool Parser::get_parse_tree()
{
    int last_success_state = 0;
    int state = 0; 
    int prev_state = -1;
    bool fail = false;
    bool reached_success_state = false;


    bool last_token_quote = false;

    // go through queue, updating state machine with each token
    // until queue is empty
    while (!_token_q.empty())
    {
        Tokenizer_Token t = _token_q.pop();
        
        // if (t.token_str() == ",")
        // {
        //     // do nothing, dummy branch to get past comma tokens
        // }
        if (t.type() == TOKEN_SPACE)
        {
            // do nothing, whitespace isn't processed
        }
        else if (t.token_str() == "\"")
        {
            last_token_quote = true;
        }
        else
        // branch for symbols, commas, and keywords (basically everything but quotes)
        {
            string str = t.token_str();

            if (last_token_quote)
            {
                // pop queue and add to current token until another quote is reached
                while (!_token_q.empty() && _token_q.front().token_str() != "\"")
                {                    
                    t = _token_q.pop();
                    str += t.token_str();
                }
                // exiting the above while loop means an end quote 
                // is at the front of the queue 

                // pop the end quote to continue as normal
                _token_q.pop();

                // reset bool that checks if we hit a begin quote
                last_token_quote = false;
            }
            
            // update prev state before updating current state
            prev_state = state;

            // update state accordingly
            state = _adjacency_table[state][get_column(string_to_lower(str))];

            // check if comma is missing in insert or make commands
            // this occurs when the state is a symbol and fails after that
            // because after symbols the comma state should be next
            if (state == -1 && (prev_state == 17 || prev_state == 16))
            {
                // cout << "\n\nerror found in if statement\n\n";
                throw runtime_error("Expected comma");
            }

            // error handling for select (given fields and not *)
            if (state == -1 && prev_state == 2)
            {
                // we need to figure out if a comma is missing or 
                // the from keyword is missing, as both are possibilities that 
                // made a fail state be reached from state 2

                // if we process the rest of the command string and see the from keyword, 
                // we know a comma was missing, but if we never find a from keyword, we know
                // the missing from keyword is why this particular command failed

                // so we will go through the rest of the token queue, and if one of the tokens
                // is the from keyword, we know we missed a comma, but if we empty the queue
                // without seeing the from keyword, we know the from keyword is what was missing
                // and caused us to fail

                while (!_token_q.empty())
                {
                    string temp = _token_q.pop().token_str();
                    if (temp == "from")
                        throw runtime_error("Expected comma");
                }
                // we've made it through every token in the queue and didn't 
                // find the from keyword, so we know that's what's missing
                // and can throw the appropriate error message
                throw runtime_error("Expected from");
            }

            // check for missing field
            // this occurs in select commands when a field is not 
            // inputted between two commas
            if (state == -1 && prev_state == 21)
            {
                throw runtime_error("Expected field name");
            }

            // check for missing from keyword 
            // this happens when select * is not followed by 
            // the from keyword
            if (state == -1 && prev_state == 3)
            {
                throw runtime_error("Expected from");
            }

            // check for missing table name in select commands
            if (state == -1 && prev_state == 4)
            {
                throw runtime_error("Expected table name");
            }




            // check abort state (-1) in adjacency table that shouldn't be reached
            if (_adjacency_table[state][0] == -1)
            {
                cout << "\n\nreached invalid state\n\n";
                return false;
            }

            switch (state)
            {
                case 1: // select keyword
                    _ptree["command"] += string_to_lower(str);
                    break;
                case 2: // field name
                    _ptree["fields"] += str;
                    break;
                case 21: // comma after fields in select branch
                    if (str != ",")
                    {
                        // cout << "\n\nerror in case 21\n\n";
                        throw runtime_error("Expected comma");
                    }
                    str = "";
                    break;
                case 3: // * - all fields
                    _ptree["fields"] += str;
                    break;
                case 4: // from - useless dummy branch
                    break;
                case 5: // table name
                    _ptree["table_name"] += str;
                    break;
                case 6: // where 
                    _ptree["where"] += string("yes");
                    break;
                case 7: // conditions
                    _ptree["condition"] += str;
                    break;
                case 8: // insert 
                    _ptree["command"] += string_to_lower(str);
                    break;
                case 9: // into - dummy branch
                    break;
                case 10: // table name
                    _ptree["table_name"] += str;
                    break;
                case 11: // values keyword - dummy branch
                    break;
                case 17: // field values
                    _ptree["values"] += str;
                    break;
                case 22: // comma after values in insert branch
                    if (str != ",")
                    {
                        // cout << "\n\nerror in case 22\n\n";
                        throw runtime_error("Expected comma");
                    }
                    str = "";
                    break;
                case 12: // make
                    _ptree["command"] += string_to_lower(str);
                    break;
                case 13: // table keyword - dummy branch
                    break;
                case 14: // table name 
                    _ptree["table_name"] += str;
                    break;
                case 15: // fields - dummy branch
                    break;
                case 16: // field names
                    _ptree["col"] += str;
                    break;
                case 23: // comma after field name in make table branch
                    if (str != ",")
                    {
                        // cout << "\n\nerror in case 23\n\n";
                        throw runtime_error("Expected comma");
                    }
                    str = "";
                    break;
                case 18: // drop keyword
                    _ptree["command"] += string_to_lower(str);
                    break;
                case 19: // table keyword after drop keyword - dummy branch
                    break;
                case 20: // table name to drop
                    _ptree["table_name"] += str;
                    break;
                default:
                    const bool debug = false;
                    // if (str == ",")
                    // {
                    //     // ignore commas and continue tokenizing
                    // }
                    // else
                    // {
                    if (debug)
                    {
                        cout << "\n\ndefualt branch in get_parse_tree() switch "
                        << "statement, shouldn't be reached | invalid state: " 
                        << state << "\nstring: " << str << "\n";
                        cout << "previous state: " << prev_state << "\n\n";
                    }
                        return false;
                    // }
            }

            // check success status
            if (_adjacency_table[state][0] == 1) 
            {
                // if we make it in here, our current state is a success state
                last_success_state = state;
                reached_success_state = true;
            }
            else
            {
                reached_success_state = false;
            }
        }
    }
    // the queue should be empty when we reach this spot
    // _ptree should be built as well

    return reached_success_state;
}

int Parser::get_column(const string& s)
{
    if (_keywords_map.contains(s))
        return _keywords_map[s];
    
    return SYMBOL;
}

string Parser::string_to_lower(const string& s)
{
    string str = "";
    for (int i = 0; i < s.length(); ++i)
    {
        str += tolower(s.at(i));
    }
    return str;
}

mmap_ss Parser::parse_tree()
{
    const bool debug = false;
    process_string();
    if (_fail_flag && debug)
        cout << "\n\nparser query failed, fail flag set to true\n\n";
    return _ptree;
}

void Parser::build_keyword_list()
{
    // map strings to corresponding enums
    _keywords_map["select"] = SELECT;
    _keywords_map["from"] = FROM;
    _keywords_map["*"] = STAR;
    _keywords_map["where"] = WHERE;
    _keywords_map["insert"] = INSERT;
    _keywords_map["into"] = INTO;
    _keywords_map["make"] = MAKE;
    _keywords_map["drop"] = DROP;
    _keywords_map["table"] = TABLE;
    _keywords_map["fields"] = FIELDS;
    _keywords_map["values"] = VALUES;
    _keywords_map[","] = COMMA;

    // SYMBOL is not a keyword so it will not be in this map of keywords
}

bool Parser::get_fail() const 
{
    return _fail_flag;
}