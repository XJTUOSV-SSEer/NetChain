#ifndef BLOCK_H
#define BLOCK_H


#include <vector>
#include <map>
#include <string>
#include "SMT.h"

class Block{
private:


public:
    // block header
    // 区块号
    int blk_id;
    // previous block hash
    std::string pre_hash;

    // H_SMT
    std::string h_smt;

    // block body
    // SMT
    SMT smt;

    // sorted list

    
};



#endif