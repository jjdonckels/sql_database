#ifndef FILE_RECORD_H
#define FILE_RECORD_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iomanip>

#include "../table/typedefs.h"
using namespace std;

class FileRecord
{
public:
    FileRecord();

    FileRecord(char str[]);

    FileRecord(string s);

    FileRecord(const vectorstr& v);

    long write(fstream& outs);
    long read(fstream& ins, long recno);

    vectorstr to_vec(int num_fields) const;

    friend ostream& operator<<(ostream&outs, const FileRecord& r)
    {
        int space = 25;
        for (int i = 0; i < MAX + 1; ++i)
        {
            if (strlen(r._record[i]) > 0)
                outs << setw(space) << r._record[i];
        }
        return outs;
    }


private:
    static const int MAX = 100;
    int recno;
    int size;
public:
    char _record[MAX + 1][MAX + 1];
    

};

#endif