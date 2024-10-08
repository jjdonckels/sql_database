#include "ftokenize.h"

// PART OF STUBS, NOT IN IMPLEMENTATION

// FTokenizer::FTokenizer(string fname)
// {
//     // open file
//     _f.open(fname);
//     // construct STokenizer member object
//     _stk = STokenizer();

//     // returns true if more blocks need to be read, false if last block
//     // has been put into _stk
//     _more = get_new_block();
// }

// Tokenizer_Token FTokenizer::next_token()
// {
//     Tokenizer_Token t;
//     _stk >> t; // next token is updated into t
//     return t;
// }

// bool FTokenizer::more()
// {
//     if (_more) 
//     {
//         // this branch means there's more blocks to read from file
//         return true;
//     }
//     else 
//     {
//         // this branch means no more blocks
//         // so check if last block in stk has more
//         return _stk.more();
//     }
// }

// int FTokenizer::pos()
// {
//     return _pos;
// }

// int FTokenizer::block_pos()
// {
//     return _blockPos;
// }

// FTokenizer& operator >> (FTokenizer& f, Tokenizer_Token& t)
// {
//     //check if stk reached end of block
//     if (f._stk.done())
//     {
//         // get new block for _stk
//         f._more = f.get_new_block();
//     }
//     t = f.next_token();
//     return f;
// }

// bool FTokenizer::get_new_block()
// {
//     char block[MAX_BLOCK]; // declare block array which will become _buffer for stk
//     _f.read(block, MAX_BLOCK); // read into block array
    
//     if (_f.gcount() < MAX_BLOCK) // branch for read function reaching end of file
//     {
//         _f.close(); // close file after reaching end

//         // add null character to end of string that was read in
//         block[_f.gcount()] = '\0';

//         _stk.set_string(block); // put block into _stk

//         // cout << "---- NEW BLOCK ------------------[" << strlen(block) << "] bytes\n";

//         return false;
//     }
    
//     // add null character to end of string that was read in
//     block[_f.gcount()] = '\0';

//     _stk.set_string(block); // put block into _stk

//     // cout << "---- NEW BLOCK ------------------[" << strlen(block) << "] bytes\n";
    
//     return true; // end of file not reached while reading next block, still more left
// }