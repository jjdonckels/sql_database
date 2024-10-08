#include "stokenize.h"
// #include <iostream>
// #include <iomanip>

using namespace std;

int STokenizer::_table[MAX_ROWS][MAX_COLUMNS];

STokenizer::STokenizer()
{
    // set state machine in table
    make_table(_table);
    char str[] = "";
    strcpy(_buffer, str); // set buffer to empty string i.e. clear buffer
    _pos = 0;
}

STokenizer::STokenizer(char str[])
{
    // set state machine in table
    make_table(_table);
    strcpy(_buffer, str); // set buffer to input str
    _pos = 0;
}

bool STokenizer::done()
{
    return _pos > strlen(_buffer);
}

bool STokenizer::more()
{
    return _pos <= strlen(_buffer);
}

//extract one token and put in t
STokenizer& operator >> (STokenizer& s, Tokenizer_Token& t)
{
    int state = 0; // reset state machine for each token
    state = s._table[0][s._buffer[s._pos]]; // get start state 
    string token_string = ""; // to be updated by get_token()

    // first char might be foreign char, so check ascii val to see if negative
    int first_char_val = s._buffer[s._pos]; // store ascii val of first char
    
    // if first char has negative ascii val and is invalid character, don't get token
    // i.e. make sure first char has valid ascii val in order to get a token from the buffer string
    if (first_char_val > 0 && s.get_token(state, token_string))
    {        
        // get_token() call in above if statement will update state
        // and token_string to proper values
        // match final success state with appropriate token type
        int token_type;
        switch(state)
        {
            // case numbers are associated with state machine and table
            case 1:
            case 3:
                token_type = TOKEN_NUMBER;
                break;
            case 4:
                token_type = TOKEN_ALPHA;
                break;
            case 5:
            case 9:
                token_type = TOKEN_OPERATOR;
                break;
            case 6:
                token_type = TOKEN_SPACE;
                break;
            case 7:
                token_type = TOKEN_PUNC;
                break;
            default:
                token_type = TOKEN_UNKNOWN;
        }
        // update token object with token that was extracted
        Tokenizer_Token new_token(token_string, token_type);
        t = new_token;
    }
    else 
    {
        ++s._pos; // increment pos if no token found to continue through buffer
    }

    return s;
}

void STokenizer::set_stk_string(char str[])
{
    // copy str into buffer
    strcpy(_buffer, str);

    // reset pos back to 0 to step through new buffer from beginning
    _pos = 0;
}

void STokenizer::make_table(int _table[][MAX_COLUMNS])
{
    // set everything to -1
    init_table(_table);

    // set NUMBERS (ints and doubles)
    mark_fail(_table, 0);            //Mark states 0 and 2 as fail states
    mark_success(_table, 1);         //Mark states 1 and 3 as success states
    mark_fail(_table, 2);
    mark_success(_table, 3);

    mark_cells(0, _table, DIGITS, 1);    //state [0] --- DIGITS ---> [1]
    mark_cells(0, _table, '.', '.', 2);  //state [0] --- '.' ------> [2] 
    mark_cells(1, _table, DIGITS, 1);    //state [1] --- DIGITS ---> [1]
    mark_cells(1, _table, '.', '.', 2);  //state [1] --- '.' ------> [2] 
    mark_cells(2, _table, DIGITS, 3);    //state [2] --- DIGITS ---> [3]
    mark_cells(3, _table, DIGITS, 3);    //state [3] --- DIGITS ---> [3]

    // set ALPHA (upper and lower case letters)
    mark_fail(_table, 0);           //Mark state 0 as fail state
    mark_success(_table, 4);        //Mark state 4 as success state

    mark_cells(0, _table, ALFA, 4);     //state [0] --- ALFA ---> [4]
    mark_cells(4, _table, ALFA, 4);     //state [4] --- ALFA --> [4]

    // set OPERATORS (defined as ><=+-%&| )
    mark_fail(_table, 0);           //Mark state 0 as fail state
    mark_success(_table, 5);        //Mark state 5 as success state

    mark_cells(0, _table, OPERATORS, 5);    //state [0] --- OPERATORS --> [5]

    // case for >=
    // make 9 success state for '>' or '<'
    mark_success(_table, 9);
    mark_cell(0, _table, '<', 9); // if '<' go to [9]
    mark_cell(0, _table, '>', 9); // if '>' go to [9]
    mark_cell(9, _table, '=', 5); // if '= after '<' or '>' go to [5]

    // set SPACE (white space characters)
    mark_fail(_table, 0);           //Mark state 0 as fail state
    mark_success(_table, 6);        //Mark state 6 as success state

    mark_cells(0, _table, SPACES, 6);   //state [0] --- SPACES ---> [6]
    mark_cells(6, _table, SPACES, 6);   //state [6] --- SPACES ---> [6]

    // set PUNC (defined as ?.,:;'`~ )
    mark_fail(_table, 0);           //Mark state 0 as fail state
    mark_success(_table, 7);        //Mark state 7 as success state

    mark_cells(0, _table, PUNC, 7);         //state [0] --- PUNC --> [7]

    //differentiate ! between operators and punctuation
    mark_cell(7, _table, '=', 5); // '!' followed by '=' is operator, not punc
}

bool STokenizer::get_token(int& start_state, string& token)
{
    // check if done by checking if _pos is on '\0'
    // this means entire buffer has already been processed and tokenized
    if (_buffer[_pos] == '\0')
    {
        ++_pos;
        return false;
    }

    // keep track of last success position in case token found
    int last_success_pos = -1;
    
    // keep track of last success state for token type 
    int last_success_state;
    
    // keep track of initial starting pos in case no token found
    // and need pos to remain unchanged
    int starting_pos = _pos;

    // true if first char needs to be processed, false otherwise
    bool first_char = true;

    // loop over chars in input
    // continue until end of input or fail state
    while (_buffer[_pos] != '\0' && start_state >= 0) 
    {
        // update state after reading current char (starts after reading 1st char)
        // 1st char is processed separately
        if (!first_char)
            start_state = _table[start_state][_buffer[_pos]];
        
        // check if new state is a success state
        if (_table[start_state]['\0'] == 1)
        {
            // this branch means current state is a success state
            last_success_pos = _pos; // update and remember last success pos

            last_success_state = start_state;
        }

        // first_char is set to false after being processed because
        // the original start_state is initially set for the first char 
        // outside of the get_token() function
        // after the first char the state is updated inside this get_token() 
        // function. This allows the function to determine whether to update 
        // state for any chars after the first one, or if the first one hasn't
        // been processed yet it won't double up the state updates and go
        // ahead by 1
        if (first_char) 
            first_char = false;

        // increment pos so next char is can be processed
        ++_pos;
    }

    // return false if no success states were reached,
    // meaning no tokens were found
    if (last_success_pos == -1)
    {
        // position remains unchanged if no tokens found 
        // so revert pos to starting pos, pos will then be incremented
        // outside of get_token() function
        _pos = starting_pos;

        return false;
    } 
    else 
    {
        // in this branch a success state was reached, so token needs to 
        // be updated, position will be after last successful state (set
        // to the next position to be processed in the next call of 
        // get_token), and state is updated to last successful state
        _pos = last_success_pos + 1;
        start_state = last_success_state;

        // take cstring and build string object
        // loop through buffer from starting pos to last success pos
        // and append chars from cstring buffer to string object token
        for (int i = starting_pos; i <= last_success_pos; i++)
        {
            token += _buffer[i];
        }

        return true;
    }
}