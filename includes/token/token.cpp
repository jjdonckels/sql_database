#include "token.h"

Token::Token()
{

}

string Token::get_token_str() const
{
    return _token;
}

void Token::set_token_str(const string& s)
{
    _token = s;
}

void Token::print_token(ostream& outs) const
{
    outs << "Token str: " << _token << endl;
    outs << "Token type: " << get_type_string() << endl;
}

TokenType Token::get_token_type() const
{
    return _type;
}

void Token::set_token_type(TokenType t)
{
    _type = t;
}

string Token::get_type_string() const
{
    switch(_type)
    {
    case TOKEN:
        return "TOKEN";
    case LPAREN:
        return "LPAREN";
    case RPAREN:
        return "RPAREN";
    case TOKEN_STR:
        return "TOKEN_STR";
    case OPERATOR:
        return "OPERATOR";
    case RELATIONAL:
        return "RELATIONAL";
    case LOGICAL:
        return "LOGICAL";
    case RESULT_SET:
        return "RESULT_SET";
    default:
        return "ERROR IN TOKEN::GET_TYPE_STRING, SHOULDN'T BE REACHED";
    }
}