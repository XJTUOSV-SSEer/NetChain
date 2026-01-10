#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include "Crypto_Primitives.h"

/*
    区块中的交易对象
*/
struct transaction{
    std::string u;
    std::string v;
    std::string type;
    double w;

    transaction(std::string u, std::string v, std::string type, double w):u(u), v(v), type(type), w(w){}

    bool operator<(const transaction& other) const {
        return w < other.w;  // 升序依据
    }
};




/*
    一个MAX查询
*/
struct query_condition {

};


/*
    单个MBFT中查询结果的证明
*/
struct MBFTProof{

};





    


// // MHT的证明
// struct MHTProof{
//     std::vector<MHTNode> subtree;
//     int root_id;

//     // 储存subtree中不匹配的结点对应的non-membership proof
//     std::map<int, Nonmembership_Proof> proof;
// };


// // SP给client返回的数据
// struct Response{
//     // 每个区块对应的证明子树
//     std::map<int, MHTProof> VO;
// };




/*
    用于对原始图数据进行shuffle，使得局部有序
*/
struct segment{
    // <u,v> pair
    std::vector<std::pair<std::string, std::string>> v;
};




#endif