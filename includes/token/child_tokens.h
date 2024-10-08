#ifndef CHILD_TOKENS_H
#define CHILD_TOKENS_H

#include "token.h"

using namespace std;

class TokenStr:public Token
{
public:
    TokenStr(const string& s)
    {
        set_token_str(s);
        set_token_type(TOKEN_STR);
    }

    void print_token(ostream& outs) const 
    {
        outs << "Token str: " << get_token_str() << endl;
        outs << "Token type: " << get_type_string() << endl;
    }
};

class LParen:public Token
{
public:
    LParen()
    {
        set_token_str("(");
        set_token_type(LPAREN);
    }

    void print_token(ostream& outs) const 
    {
        outs << "Token str: " << get_token_str() << endl;
        outs << "Token type: " << get_type_string() << endl;
    }
};

class RParen:public Token 
{
public:
    RParen()
    {
        set_token_str(")");
        set_token_type(RPAREN);
    }

    void print_token(ostream& outs) const 
    {
        outs << "Token str: " << get_token_str() << endl;
        outs << "Token type: " << get_type_string() << endl;
    }
};

class ResultSet:public Token 
{
public:
    ResultSet(const vectorlong& rnums)
    {
        _recnos = rnums;
        set_token_str("RESULT_SET");
        set_token_type(RESULT_SET);
    }

    vectorlong recnos()
    {
        return _recnos;
    }

    ResultSet* and_with(const vectorlong& recs)
    {
        _recnos = vec_intersect(_recnos, recs);
        return this;
    }

    ResultSet* or_with(const vectorlong& recs)
    {
        _recnos = vec_union(_recnos, recs);
        return this;
    }

    void print_token(ostream& outs) const 
    {
        outs << "Token str: " << get_token_str() << endl;
        outs << "Token type: " << get_type_string() << endl;
        outs << "Recnos: " << _recnos << endl;
    }

private:
    vectorlong _recnos;
};

class Operator:public Token 
{
public:
    Operator() 
    {
        _op = "undefined operator, default Operator CTOR called";
        _prec = -1;
        set_token_str(_op);
        set_token_type(OPERATOR);
    }

    void set_op(const string& s)
    {
        _op = s;
    }

    void set_prec(int n)
    {
        _prec = n;
    }

    int prec() const 
    {
        return _prec;
    }

    void print_token(ostream& outs) const 
    {
        outs << "Token str: " << get_token_str() << endl;
        outs << "Operator str(same as tokenstr): " << _op << endl;
        outs << "Token type: " << get_type_string() << endl;
        outs << "Precedence: " << _prec << endl;
    }

private:
    string _op;
    int _prec;
};

class Relational:public Operator 
{
public:
    Relational(const string& s)
    {
        _op = s;
        set_token_str(_op);
        set_op(_op);
        set_token_type(RELATIONAL);

        // set precedence 
        if (_op == "=")
            set_prec(2);
        else // < <= > >= all same precedence, higher than = 
            set_prec(1);
    }

    ResultSet* eval(mmap_sl& m, Token* field, Token* value)
    {
        vectorlong recno_vec;

        if (_op == "<" || _op == "<=")
        {
            mmap_sl::Iterator it = m.begin();

            for (; it != m.lower_bound(value->get_token_str()); ++it)
            {
                // get vector of recnos at current iterator in mmap
                vectorlong temp_list = (*it).value_list;
                // loop through all recnos and push into recno_vec
                for (int i = 0; i < temp_list.size(); ++i)
                {
                    recno_vec.push_back(temp_list.at(i));
                }
            }
        }

        if (_op == "=" || _op == "<=" || _op == ">=")
        {
            vectorlong temp_vec = m[value->get_token_str()];
            for (int i = 0; i < temp_vec.size(); ++i)
                recno_vec.push_back(temp_vec.at(i));        
        }

        if (_op == ">" || _op == ">=")
        {
            mmap_sl::Iterator it = m.upper_bound(value->get_token_str());
            for (; it != m.end(); ++it)
            {
                // get vector of recnos at current iterator in mmap
                vectorlong temp_list = (*it).value_list;
                // loop through all recnos and push into recno_vec
                for (int i = 0; i < temp_list.size(); ++i)
                {
                    recno_vec.push_back(temp_list.at(i));
                }
            }
        }

        return new ResultSet(recno_vec);
    }

    void print_token(ostream& outs) const 
    {
        outs << "Token str: " << get_token_str() << endl;
        outs << "Operator str(same as tokenstr): " << _op << endl;
        outs << "Token type: " << get_type_string() << endl;
        outs << "Precedence: " << prec() << endl;
    }

private:
    string _op;
    int _type; // NOT CURRENTLY USED
};

class Logical:public Operator 
{
public:
    Logical() 
    {

    }

    Logical(const string& s)
    {
        _op = s;
        set_token_str(_op);
        set_op(_op);
        set_token_type(LOGICAL);

        // set precedence
        if (_op == "and")
            set_prec(3);
        else // or has lower precedence than and
            set_prec(4);
    }

    ResultSet* eval(ResultSet* left, ResultSet* right)
    {
        if (_op == "and")
        {
            return left->and_with(right->recnos());
        }
        // _op == "or"
        return left->or_with(right->recnos());
    }

    void print_token(ostream& outs) const 
    {
        outs << "Token str: " << get_token_str() << endl;
        outs << "Operator str(same as tokenstr): " << _op << endl;
        outs << "Token type: " << get_type_string() << endl;
        outs << "Precedence: " << prec() << endl;
    }

private:
    string _op;
    int _type; // NOT CURRENTLY USED
};

#endif 