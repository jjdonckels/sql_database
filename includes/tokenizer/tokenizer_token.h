#ifndef TOKENIZER_TOKEN_H
#define TOKENIZER_TOKEN_H

#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <cstdlib>

#include "constants.h"

using namespace std;

class Tokenizer_Token
{
public:
    Tokenizer_Token();
    Tokenizer_Token(string str, int type);
    friend ostream& operator << (ostream& outs, const Tokenizer_Token& t)
    {
        // format and print token string
        outs << "|" << t._token << "|";
        return outs;
    }
    int type() const;
    string type_string() const;
    string token_str() const;
private:
    string _token;
    int _type;
};


#endif 