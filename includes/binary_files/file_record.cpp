#include "file_record.h"

FileRecord::FileRecord()
{
    // _record[0] = '\0';
    // recno = -1;
}

FileRecord::FileRecord(char str[])
{
    // strncpy(_record, str, MAX);
}

FileRecord::FileRecord(string s)
{
    // strncpy(_record, s.c_str(), MAX);
}

FileRecord::FileRecord(const vectorstr& v)
{
    // loop through vector indices and store each entry in new row
    for (int i = 0; i < v.size(); ++i)
    {
        strncpy(_record[i], v.at(i).c_str(), MAX);
    }
    size = v.size();
    // fill all unused entries in 2d array with null characters
    // so no garbage leaks
    for (int i = size; i < MAX + 1; ++i)
    {
        for (int k = 0; k < MAX + 1; ++k)
            _record[i][k] = '\0';
    }
}

long FileRecord::write(fstream& outs)
{
    // write to the end of the file
    outs.seekg(0, outs.end);

    // retrieve current position of the file pointer
    long pos = outs.tellp();

    // write the record contents into the file
    for(int i = 0; i < (/*sizeof(_record) / */sizeof(_record[0])); ++i)
    {
        outs.write(_record[i], sizeof(_record[0]));
    }

    // return record number
    return pos / sizeof(_record);
}

long FileRecord::read(fstream& ins, long recno)
{
    // get position in file to read from
    long pos = recno * sizeof(_record);

    ins.seekg(pos, ios_base::beg);

    // read file contents into record
    for (int i = 0; i < (/*sizeof(_record) / */sizeof(_record[0])) - 1; ++i)
    {
        ins.read(_record[i], sizeof(_record[0]));
    }

    return ins.gcount();   
}

vectorstr FileRecord::to_vec(int num_fields) const
{
    vectorstr temp;

    // input record rows into a vector and return it
    for (int i = 0; i < num_fields; ++i)
    {
        string str = _record[i];

        temp.push_back(str);
    }

    return temp;
}