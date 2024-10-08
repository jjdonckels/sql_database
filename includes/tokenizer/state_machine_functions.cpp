#include "state_machine_functions.h"
// #include <iostream>
// #include <iomanip>

using namespace std;

void init_table(int _table[][MAX_COLUMNS])
{
    // set everything to -1
    for (int i = 0; i < MAX_ROWS; ++i)
    {
        for (int j = 0; j < MAX_COLUMNS; ++j)
        {
            _table[i][j] = -1;
        }
    }
}

void mark_success(int _table[][MAX_COLUMNS], int state)
{
    _table[state]['\0'] = 1;
}

void mark_fail(int _table[][MAX_COLUMNS], int state)
{
    _table[state]['\0'] = 0;
}

bool is_success(int _table[][MAX_COLUMNS], int state)
{
    if (_table[state]['\0'] == 1)
    {
        return true;
    }
    else 
    {
        return false;
    }
}

void mark_cells(int row, int _table[][MAX_COLUMNS], int from, int to, int state)
{
    // in the specified row, loop through the specified range of columns 
    // and set to the desired state
    for (int col = from; col <= to; col++)
    {
        _table[row][col] = state;
    }
}

void mark_cells(int row, int _table[][MAX_COLUMNS], const char columns[], int state)
{
    // for a specified row (state), set all the columns for chars found in columns[]
    // to the desired state
    int pos = 0; // walker through columns[] 
    while (columns[pos] != '\0')
    {
        // loop through columns[] until the end
        // for each character, set that value in the table to the desired state
        _table[row][columns[pos]] = state;
        pos++;
    }
}

void mark_cell(int row, int _table[][MAX_COLUMNS], int column, int state)
{
    _table[row][column] = state;
}

void print_table(int _table[][MAX_COLUMNS])
{
    for (int i = 0; i < MAX_ROWS; i++)
    {
        for (int j = 0; j < MAX_COLUMNS; j++)
        {
            cout << setw(3) << _table[i][j];
        }
        cout << endl;
    }
}

void print_table(int _table[][MAX_COLUMNS], int from, int to)
{
    for (int i = 0; i < MAX_ROWS; i++)
    {
        for (int j = from; j <= to; j++)
        {
            cout << setw(3) << _table[i][j];
        }
        cout << endl;
    }
}

void show_string(char s[], int _pos)
{
    cout << s << "    pos: " << _pos << endl;
    for (int i = 0; i < _pos; i++)
    {
        cout << " ";
    }
    cout << "^\n";
}