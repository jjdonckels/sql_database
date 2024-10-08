#ifndef TOKEN_H
#define TOKEN_H

#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>

#include "../table/typedefs.h"

using namespace std;

enum TokenType
{
    TOKEN,
    LPAREN,
    RPAREN,
    TOKEN_STR,
    OPERATOR,
    RELATIONAL,
    LOGICAL,
    RESULT_SET,
};

class Token
{
public:
    Token();

    string get_token_str() const;
    
    void set_token_str(const string& s);

    virtual void print_token(ostream& outs) const;

    TokenType get_token_type() const;

    void set_token_type(TokenType t);

    string get_type_string() const;

    friend ostream& operator << (ostream&outs, const Token& t)
    {
        t.print_token(outs);
        return outs;
    }

private:
    string _token;
    TokenType _type;
};

#endif