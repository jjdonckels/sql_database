#include "table.h"

int Table::_serial = 1000;

Table::Table()
{
    _empty = true;
    _table_name = "";
}

Table::Table(const string& name, const vectorstr& field_names)
{
    _name = name;
    _table_name = name;
    _serial++;
    _last_rec_num = -1;
    _empty = true;

    // set up field names vector, map, 
    // and vector of multimaps for indices
    set_fields(field_names);

    // create new file to write into with appropriate name
    string file_name = name + ".bin";
    
    fstream f(file_name);
    if (f.is_open())
        f.close();

    open_fileW(f, file_name.c_str());

    // write names of fields to txt file
    write_info(field_names);
}

Table::Table(const string& name)
{
    _name = name;
    _table_name = name;
    _serial++;
    _last_rec_num = -1;
    _empty = true;

    // get field names from pre-existing txt file
    string file_name = name + "_fields.txt";
    ifstream infile(file_name);
    while (!infile.eof())
    {
        string temp_field;
        infile >> temp_field;
        _field_names_list.push_back(temp_field);
    }
    infile.close();

    set_fields(_field_names_list);

    // re-index to initialize _indices multimaps
    re_index();
}

void Table::add_rec_to_table(const FileRecord& r, long recno)
{
    // create vectorstr of fields from record to insert into table
        vectorstr vals = r.to_vec(_field_names_list.size());

        // insert vals from curr record into table multimap
        for (int k = 0; k < vals.size(); ++k)
        {
            (_indices[k])[vals.at(k)] += recno;
        }
        _recno_list.push_back(recno);

        _last_rec_num = recno;

        if (_empty)
            _empty = false;
}

void Table::insert_into(const vectorstr& values)
{
    FileRecord r;

    // put values into record
    r = FileRecord(values);

    string file_name = _name + ".bin";

    fstream f(file_name);

    if (!f.is_open())
        open_fileRW(f, file_name.c_str());

    // if (!_file.is_open())
    // {
    //     open_fileRW(_file, file_name.c_str());
    // }
    long recno = r.write(f);

    // update indices with new record data
    for (int i = 0; i < values.size(); ++i)
    {
        (_indices[i])[values.at(i)] += recno;
    }
    _recno_list.push_back(recno);

    // update last record number
    _last_rec_num = recno;

    _empty = false;
}

Table Table::select_all(vector<string> fields)
{
    // get vector of all record numbers by looping to last rec num
    vector<long> recno_vec;
    for (int i = 0; i <= _last_rec_num; ++i)
    {
        recno_vec.push_back(i);
    }

    Table t = table_from_recno_vec(recno_vec);

    _last_selected_recnos = recno_vec;

    if (fields.empty())
        t._field_names_list = _field_names_list;
    else 
        t._field_names_list = fields;
    t._last_selected_recnos = _last_selected_recnos;

    return t;
}

Table Table::select(const vectorstr& fields, 
            const string& field_name, const string& op, const string& field_val)
{
    // will hold record numbers of the records to be returned in a Table
    vectorlong recno_vec;

    if (op == "<" || op == "<=")
    {
        // get the index of the field to select from 
        long field_index = _field_names_map[field_name];

        // build a vector of all the recnos that meet the selection criteria
        mmap_sl::Iterator it = _indices[field_index].begin();
        for (; it != _indices[field_index].lower_bound(field_val); ++it)
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
    if (op == "=" || op == "<=" || op == ">=")
    {
        // get the index of the field to select from 
        long field_index = _field_names_map[field_name];
        // get a vector of all the recnos that meet the selection criteria
        vectorlong temp_vec = (_indices[field_index])[field_val];
        for (int i = 0; i < temp_vec.size(); ++i)
            recno_vec.push_back(temp_vec.at(i));        
    }
    if (op == ">" || op == ">=")
    {
        // get the index of the field to select from 
        long field_index = _field_names_map[field_name];

        // build a vector of all the recnos that meet the selection criteria
        mmap_sl::Iterator it = _indices[field_index].upper_bound(field_val);
        for (; it != _indices[field_index].end(); ++it)
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

    Table t = table_from_recno_vec(recno_vec);

    _last_selected_recnos = recno_vec;

    t._field_names_list = fields;
    t._last_selected_recnos = _last_selected_recnos;

    return t;
}



Table Table::select(const vectorstr& fields, Queue<Token *> qt)
{
    // RPN ALGORITHM

    // cout << "\n\nSTART RPN\n\n";

    Stack<Token *> token_stack;

    TokenType prev_token_type;

    bool first_token = true;

    while (!qt.empty())
    {
        // get next token ptr
        Token * curr_token = qt.pop();

        // put token ptr into correct spot
        if (curr_token->get_token_type() == TOKEN_STR)
        {
            // push token string into stack
            token_stack.push(curr_token);
        }
        else if (curr_token->get_token_type() == RELATIONAL)
        {
            // pop stack twice evaluate relational expression, 
            // and put ResultSet into stack afterwards
            // throw error if nothing to pop
            if (token_stack.empty())
            {
                // cout << "\n\nerror 1\n\n";
                throw runtime_error("Incomplete relational expression");
            }
            Token * field_val = token_stack.pop();
            // throw error if nothing to pop
            if (token_stack.empty())
            {            
                // cout << "\n\nerror 2\n\n";
                throw runtime_error("Incomplete relational expression");
            }
            Token * field_name = token_stack.pop();

            // now we've successfully popped the top two tokens 
            // from the stack to evaluate
            // we need to throw errors if they are not both token strings
            if (!(field_val->get_token_type() == TOKEN_STR && 
                    field_name->get_token_type() == TOKEN_STR))
            {
                // this branch means there are invalid arguments to the relational operator
                // this can mean an operator is missing or a field val / field name is missing
                if (field_val->get_token_type() == RESULT_SET || 
                        field_name->get_token_type() == RESULT_SET)
                {
                    throw runtime_error("Missing logical operator");
                }
                // cout << "\n\nerror 3\n\n";
                // cout << "\n\nfield name: " << field_name->get_token_str() << "\n\n";
                // cout << "\n\nfield val: " << field_val->get_token_str() << "\n\n";
                throw runtime_error("Incomplete relational expression");
            }



            token_stack.push(static_cast<Relational*>(curr_token)->eval
                (_indices[_field_names_map[field_name->get_token_str()]], 
                field_name, field_val));
        }
        else if (curr_token->get_token_type() == LOGICAL)
        {
            // pop stack twice to get the two result sets to operate on
            // apply the operator, and push the new ResultSet back into stack

            // throw errors if nothing to pop when getting result sets to evaluate
            if (token_stack.empty())
                throw runtime_error("Incomplete logical expression");
            Token* second = token_stack.pop();
            if (token_stack.empty())
                throw runtime_error("Incomplete logical expression");
            Token* first = token_stack.pop();

            token_stack.push(static_cast<Logical*>(curr_token)->eval
                (static_cast<ResultSet*>(first), static_cast<ResultSet*>(second)));
        }
        else 
        {
            cout << "\n\nfinal branch in RPN select(), shoudln't be reached\n\n";
            exit(123);
        }
        
        first_token = false;
        prev_token_type = curr_token->get_token_type();
    }

    Token * result = token_stack.pop();

    vectorlong recno_vec = static_cast<ResultSet*>(result)->recnos();

    Table t = table_from_recno_vec(recno_vec);

    _last_selected_recnos = recno_vec;

    t._field_names_list = fields;
    t._last_selected_recnos = _last_selected_recnos;

    // cout << "\n\nEND RPN\n\n";

    return t;
}

Table Table::select(const vectorstr& fields, const vectorstr& conditions)
{
    // SHUNGTING YARD ALGORITHM

    Queue<Token *> q;
    Stack<Token *> s;

    // convert infix vector into postfix queue
    for (int i = 0; i < conditions.size(); ++i)
    {
        // get next token from vector
        Token * curr_token = string_to_token(conditions.at(i));

        if (curr_token->get_token_type() == LPAREN)
            s.push(curr_token);
        else if (curr_token->get_token_type() == TOKEN_STR)
            q.push(curr_token);
        else if (curr_token->get_token_type() == RELATIONAL || 
                curr_token->get_token_type() == LOGICAL)
        {
            while (!s.empty() && 
                s.top()->get_token_type() != LPAREN && 
                    static_cast<Operator*>(s.top())->prec() <= 
                    static_cast<Operator*>(curr_token)->prec())
            {
                q.push(s.pop());
            }
            s.push(curr_token);
        }
        else if (curr_token->get_token_type() == RPAREN)
        {
            // loop through stack until finding matching lparen
            while(!s.empty() && 
                s.top()->get_token_type() != LPAREN)
            {
                q.push(s.pop());
            }

            // if stack is empty, lparen was never found
            // and we need to throw an error
            if (s.empty())
                throw runtime_error("Missing left paren");

            // pop stack one more time to remove lparen
            s.pop();
        }
    }

    // pop anything left in the stack to the end of the queue
    while (!s.empty())
    {
        // if there are any left parens left in the stack, we have mismatched 
        // parens and we're missing at least one right paren 
        // and we need to throw an error
        if (s.top()->get_token_type() == LPAREN)
            throw runtime_error("Missing right paren");

        // cout << "\n\nleft in stack: " << s.top()->get_token_str() << "\n\n";
        
        q.push(s.pop());
    }

    // cout << "\n\nSHUNTING YARD COMPLETE\n\n";

    return select(fields, q);
}

Token* Table::string_to_token(const string& s)
{
    if (s == "(")
        return new LParen();
    else if (s == ")")
        return new RParen();
    else if (s == "or")
        return new Logical("or");
    else if (s == "and")
        return new Logical("and");
    else if (s == "<")
        return new Relational("<");
    else if (s == "<=")
        return new Relational("<=");
    else if (s == "=")
        return new Relational("=");
    else if (s == ">=") 
        return new Relational(">=");
    else if (s == ">")
        return new Relational(">");
    else 
        return new TokenStr(s);
}

vectorlong Table::select_recnos()
{
    return _last_selected_recnos;
}

Table Table::table_from_recno_vec(const vector<long>& recno_vec)
{
    Table temp(_name + "_" + to_string(_serial), _field_names_list);

    // open my binary, insert records from recno_vec into temp, return temp
    string file_name = _name + ".bin";
    fstream f(file_name);
    if (f.is_open())
        f.close();

    // if (_file.is_open())
    //     _file.close();
    
    open_fileRW(f, file_name.c_str());

    FileRecord r;

    for (int i = 0; i < recno_vec.size(); ++i)
    {
        r.read(f, recno_vec.at(i));
        temp.insert_into(r.to_vec(_field_names_list.size()));
    }
    f.close();

    return temp;   
    
    temp._table_name = "_select_table_" + to_string(_serial);

    temp._recno_list = recno_vec;

    return temp;
}

void Table::set_fields(const vectorstr& names)
{
    bool fields_list_empty = _field_names_list.empty();
    // build field names vector and map
    for (int i = 0; i < names.size(); ++i)
    {   
        // add current field name to field_name_list if starting from scratch
        // if field names already populated, don't push back dups
        if (fields_list_empty)
            _field_names_list.push_back(names.at(i));

        // add field name to field_name_map, paried with proper index
        _field_names_map.insert(names.at(i), i);
    }

    // build vector of multimaps that will hold indices
    _indices.resize(names.size());
}

void Table::write_info(const vectorstr& names)
{
    // write field names to a txt file
    ofstream outfile;

    string file_name = _name + "_fields.txt";

    outfile.open(file_name);

    for (int i = 0; i < names.size(); ++i)
    {
        outfile << names.at(i) << endl;
    }

    outfile.close();
}

vectorstr& Table::get_fields()
{
    return _field_names_list;
}

void Table::re_index()
{
    // open pre-existing bin file
    string file_name = _name + ".bin";
    fstream f(file_name);
    if (f.is_open())
        f.close();
    open_fileRW(f, file_name.c_str());
    // read all records from bin file and insert into this table object
    int i = 0;
    FileRecord r;
    long bytes = r.read(f, i); //empty envelop to be filled by the FileRecord object

    while (bytes>0)
    {
        add_rec_to_table(r, i);
        i++;
        bytes = r.read(f, i);
    }

    f.close();
}

void Table::print_table(ostream& outs) const
{
    if (_empty)
    {
        outs << "Table " << _table_name<< " is empty";
        return;
    }
    int space = 25;

    // output header
    outs << "\nTable name: " << _table_name << ", records: ";
    
    outs << _recno_list.size() << endl;
    outs << setw(space) << "record";

    // loop through and print field names
    for (int i = 0; i < _field_names_list.size(); ++i)
    {
        outs << setw(space) << _field_names_list.at(i);
    }
    outs << "\n\n";

    // output record data from file
    FileRecord r;
    string file_name = _name + ".bin";
    fstream f(file_name);
    if (f.is_open())
    {
        f.close();
    }

    long bytes;

    for(int i = 0; i < _recno_list.size(); ++i)
    {
        open_fileRW(f, file_name.c_str());
        bytes = r.read(f, _recno_list.at(i));
        f.close();

        outs << setw(space) << i;
        for (int field_num = 0; field_num < _field_names_list.size(); ++field_num)
        {
            outs << setw(space);
            string field_name = _field_names_list.at(field_num);
            int rec_row = _field_names_map.at(field_name);
            outs << r._record[rec_row];
        }
        
        outs << endl;
    }
    
    outs << endl;
}

vectorstr Table::get_table_fields() const 
{
    return _field_names_list;
}