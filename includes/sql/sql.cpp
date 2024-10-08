#include "sql.h"

SQL::SQL()
{
    cout << "\n\nSQL: Currently tracking these tables:\n";
    cout << "-------------------------------------\n";
    ifstream infile("managed_tables.txt");

    if (infile.fail())
    {
        cout << "no tables currently managed\n\n";
    }
    else 
    {
        // loop through file, printing each table name 
        // and populating table map
        while (!infile.eof())
        {
            // read table name from file
            string table_name;
            infile >> table_name;

            if (table_name.length() <= 0)
                break;

            // print table name to console
            // cout << "table name: |" << table_name << "|" << endl;
            cout << table_name << endl;
            // cout << "\n\ntable ascii: ";
            // int a = table_name[0];
            // cout << a << "\n\n";
            // cout << "\n\ntable name size: " << table_name.length() << "\n\n";

            // cout << "\n\ncheckpoint\n\n";

            // populate table map
            Table t(table_name);
            // cout << "\n\ncheckpoint a\n\n";
            table_names_list.push_back(table_name);
            // cout << "\n\ncheckpoint b\n\n";
            _table_map[table_name] = t;
            // cout << "\n\ncheckpoint c\n\n";
            table_fields_map[table_name] = t.get_table_fields();
            // cout << "\n\ncheckpoint d\n\n";
        }
        infile.close();

        // cout << "\n\ncheckpoint 2\n\n";
    }

    // cout << "\n\nEND OF SQL CTOR\n\n";
}

void SQL::batch()
{
    const vectorstr sample_commands = {
        // 1. valid command:
        "select lname, fname, major from student where ((lname=Yang or major=CS) and age<23 )or lname=Jackson",
        // 2. Expected comma:
        "select lname fname, major from student where ((lname=Yang or major=CS) and age<23 )or lname=Jackson",
        // 3. Expected: field name
        "select lname, , major from student where ((lname=Yang or major=CS) and age<23 )or lname=Jackson",
        // 4. Expected from:
        "select lname, fname, major  student where ((lname=Yang or major=CS) and age<23 )or lname=Jackson",
        // 5. Expected table name:
        "select lname, fname, major from  where ((lname=Yang or major=CS) and age<23 )or lname=Jackson",
        // 6. Expected condition:
        "select lname, fname, major from student where ",
        // 7. Missing left paren:
        "select lname, fname, major from student where (lname=Yang or major=CS) and age<23 )or lname=Jackson",
        // 8. Missing right paren:
        "select lname, fname, major from student where ((lname=Yang or major=CS and age<23 )or lname=Jackson",
        // 9. :
        "select lname, fname, major from student where ((lname= or major=CS) and age<23 )or lname=Jackson",
        // 10. :
        "select lname, fname, major from student where ((lname=Yang or major=CS) and age<23 )or lname=Jackson",
        // 11. :
        "select lname, fname, major from student where ((lname=Yang  major=CS) and age<23 )or lname=Jackson",
        // 12. :
        "select lname, fname, major from student where ((lname=Yang or ) and age<23 )or lname=Jackson",
        // 13. :
        "select lname, fname, major from student where ((lname=Yang or major=CS) and age<23 )or ",
        // 14. :
        "select lname, fname, major from student where ((lname=Yang or major=CS) and age<23 )or lname=Jackson"
    };

    for (int i = 0; i < sample_commands.size(); ++i)
    {
        cout << "\n\n" << (i + 1) << ") " << sample_commands.at(i) << "\n\n";
        try_command(sample_commands.at(i));
        cout << "\n\n";
    }
}

void SQL::try_command(string s, ostream& outs)
{
    Table t;
    try
    {
        t = command(s);
    }
    catch (const exception& e)
    {
        outs << "Error: " << e.what() << endl;
        return;
    }

    outs << t << endl;
}

Table SQL::command(string s)
{
    char temp[MAX_BUFFER];
    strcpy(temp, s.c_str());
    parser.set_string(temp);

    // try 
    // {
        mmap_ss ptree = parser.parse_tree();
    // }
    // catch (const exception& e)
    // {

    // }

    // if (parser.get_fail())
    // {
        if (ptree.contains("where") && !ptree.contains("condition"))
            throw runtime_error("Expected condition");
    // }

    if (parser.get_fail())
        throw runtime_error("invalid command, unable to parse");

    Table t;

    Table* curr_table = nullptr;

    if (ptree.empty())
    {
        throw runtime_error("invalid command, could not be processed");
        cout << "\n\nptree is empty, invalid parser command\n\n";
        return Table();
    }

    // assume ptree is valid
    if (ptree["command"][0] == "make")
    {
        string table_name = ptree["table_name"][0];
        vectorstr field_names = ptree["col"];

        // branch to stop duplicate tables from being created
        if (_table_map.contains(table_name))
            return _table_map[table_name];

        Table t(table_name, field_names);

        _table_map[table_name] = t;

        table_names_list.push_back(table_name);
        // table_visibility_map[table_name] = true;
        // table_index_map[table_name] = table_names_list.size() - 1;
        table_fields_map[table_name] = field_names;

        ofstream outfile("managed_tables.txt");
        for (int i = 0; i < table_names_list.size(); ++i)
            outfile << table_names_list.at(i) << endl;

        outfile.close();

        return t;
    }
    else if (ptree["command"][0] == "insert")
    {
        string table_name = ptree["table_name"][0];
        vectorstr field_values = ptree["values"];

        if (_table_map.contains(table_name))
        {
            t = _table_map[table_name];
            curr_table = &_table_map[table_name];
            // cout << "\n\ntable from map " << table_name << "\n\n";
            // cout << "\n\n" << t << "\n\n";
        }
        else 
        {
            throw runtime_error("Table " + table_name + " does not exist");
            t = Table(table_name);
            cout << "\n\nALTERNATE BRANCH\n\n";
        }
        // t.insert_into(field_values);
        // _table_map[table_name].insert_into(field_values);
        curr_table->insert_into(field_values);

        // cout << "\n\n" << _table_map[table_name] << "\n\n";

        // return t;
        // return _table_map[table_name];
        return *curr_table;
    }
    else if (ptree["command"][0] == "select")
    {
        bool contains_where = ptree.contains("where");
        bool all_fields = ptree["fields"][0] == "*";
        string table_name = ptree["table_name"][0];

        if (_table_map.contains(table_name))
        {
            t = _table_map[table_name];
            curr_table = &_table_map[table_name];
        }
        else 
        {
            throw runtime_error("Table " + table_name + " does not exist");
            t = Table(table_name);
        }

        vectorstr fields_to_output;

        // validate fields 
        // if a field was specified that doesn't exist, bypass it
        if (!all_fields)
        {
            vectorstr input_fields = ptree["fields"];

            // loop through input fields and bypass any that don't exist
            // by building a new vector with only the valid fields
            for (int i = 0; i < input_fields.size(); ++i)
            {
                bool found = false;
                for (int j = 0; j < table_fields_map[table_name].size(); ++j)
                {
                    if (input_fields.at(i) == 
                        table_fields_map[table_name].at(j))
                        found = true;
                }

                if (found)
                    fields_to_output.push_back(input_fields.at(i));
            }
        }
        else 
        {
            fields_to_output = table_fields_map[table_name];
        }


        if (!contains_where)
        {
            // branch for select all
            cout << "there is no condition" << endl;
            
            if (all_fields)
            {
                // t = t.select_all();
                // t = _table_map[table_name].select_all();
                t = curr_table->select_all();
            }
            else 
            {
                // pass validated vector of desired fields to output into 
                // select_all to get a table with the desired fields
                // t = t.select_all(fields_to_output);
                // t = _table_map[table_name].select_all(fields_to_output);
                t = curr_table->select_all(fields_to_output);
            }
        }
        else 
        {
            // branch with conditions
            if (all_fields)
            {
                // pass entire vector of table fields to select function
                // to get a table with all fields that satisfies the 
                // appropriate conditions
                // t = t.select(table_fields_map[table_name], ptree["condition"]);
                // t = _table_map[table_name].select(table_fields_map[table_name], ptree["condition"]);
                t = curr_table->select(table_fields_map[table_name], ptree["condition"]);
            }
            else 
            {
                // pass validated vector of fields to output
                // into select function 
                // to get a table with desired fields that satisifes 
                // the given conditions
                // t = t.select(fields_to_output, ptree["condition"]);
                // t = _table_map[table_name].select(fields_to_output, ptree["condition"]);
                t = curr_table->select(fields_to_output, ptree["condition"]);
            }
        }

        // updated vector of recnos 
        // last_selected_recnos = t.select_recnos();
        // last_selected_recnos = _table_map[table_name].select_recnos();
        last_selected_recnos = curr_table->select_recnos();
        return t;
        // return *curr_table;

    }
    cout << "\n\nno branch entered in SQL::command() function, "
        << "should never be reached\n\n";

    return t;
}

vectorlong SQL::select_recnos()
{
    return last_selected_recnos;
}