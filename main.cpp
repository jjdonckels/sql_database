//#include <iostream>
//
//int main() {
//    std::cout << "Test file import" << std::endl;
//    return 0;
//}

#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>

#include "includes/sql/sql.h"


using namespace std;

const vectorstr command_list = {
        "make table employee fields  last, first, dep, salary, year",
        "insert into employee values Blow, Joe, CS, 100000, 2018",
        "insert into employee values Blow, JoAnn, Physics, 200000, 2016",
        "insert into employee values Johnson, Jack, HR, 150000, 2014",
        "insert into employee values Johnson, \"Jimmy\" ,Chemistry,140000, 2018",

        "make table student fields fname, lname, major, age, company",
        "insert into student values Flo, Yao, CS, 20, Google",
        "insert into student values Bo, Yang, CS, 28, Microsoft",
        "insert into student values \"Sammuel L.\", Jackson, CS, 40, Uber",
        "insert into student values \"Flo\", \"Jackson\", Math, 21, Google",
        "insert into student values \"Greg\", \"Pearson\", Physics, 20, Amazon"
};

int main(int argv, char** argc) {
//    cout << "\nrun main\n"
//         << endl;

    SQL sql;
    Table t;

    // populate tables if not already populated
    ifstream infile("managed_tables.txt");
    if (infile.fail())
    {
        // populate tables using command_list above
        for (int i = 0; i < command_list.size(); i++)
        {
            sql.command(command_list.at(i));
        }
    }

//    sql.batch();

    string input;

    cout << "\n\nenter a query or type \"exit\" to end the program:\n\n";

    getline(cin, input);

    while (input != "exit")
    {
        sql.try_command(input);

        cout << "\n\nenter a query or type \"exit\" to end the program:\n\n";

        getline(cin, input);
    }





    cout << "\n\n\n======== END MAIN =============" << endl;
    return 0;
}
