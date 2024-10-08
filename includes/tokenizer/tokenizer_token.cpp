#include "tokenizer_token.h"

using namespace std;

Tokenizer_Token::Tokenizer_Token()
{
    _token = ""; // default to empty string
    _type = -1; // default to UNKNOWN token type
}

// PRECONDITION: token data already known
Tokenizer_Token::Tokenizer_Token(string str, int type)
{
    _token = str; // init _token string 
    _type = type; // init _token type
}

int Tokenizer_Token::type() const
{
    return _type; // type refers to NUMBER, ALPHA, PUNC, etc.
}

string Tokenizer_Token::type_string() const
{
    // go through different types and return appropriate type string
    switch(_type)
    {
        case TOKEN_NUMBER:
            return "NUMBER";
        case TOKEN_ALPHA:
            return "ALPHA";
        case TOKEN_SPACE:
            return "SPACE";
        case TOKEN_OPERATOR:
            return "OPERATOR";
        case 9:
            return "OPERATOR";
        case TOKEN_PUNC:
            return "PUNC";
        default:
            return "UNKNOWN";
    }
}

string Tokenizer_Token::token_str() const
{
    return _token;
}