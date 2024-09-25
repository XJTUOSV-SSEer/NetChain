#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>
#include <set>
#include <map>
#include <algorithm>
#include "MultisetAccumulator.h"
#include "BigInteger.h"
#include "Crypto_Primitives.h"

/*
    区块中的交易对象
*/
struct transaction{
    std::string u;
    std::string v;
    std::string type;
    int w;

    transaction(std::string u, std::string v, std::string type, int w):u(u), v(v), type(type), w(w){}
};



// MHT的结点
struct MHTNode{
    // 是否为叶结点
    bool isLeaf;
    // 子结点在tree数组中的下标
    int lchild;
    int rchild;

    // 集合。若为叶结点，包含对应的交易中的混合键；若为非叶结点，包含子结点中混合键的并集
    std::set<std::pair<std::string, std::string>> com_key_set;
    // 当结点为叶结点时，填充混合键对应的value=<v,w>
    std::pair<std::string, int> value;

    // com_key_set集合对应的acc值
    std::string acc;

    // com_key_set集合对应的素数乘积
    std::string product;

    // 结点的digest=H(H(lhash||rhash)||Acc)
    std::string digest;


    /*
        默认构造函数
    */
    MHTNode(){

    }

    /*
        构造函数，构造一个叶结点
        param:
            tx - 一条交易
            prime_map - 储存每个混合键对应的素数。用于打表快速计算素数
    */
    MHTNode(transaction& tx, std::map<std::pair<std::string, std::string>, std::string>& prime_map, 
            MultisetAccumulator& msa){
        this->isLeaf = true;
        this->lchild = -1;  this->rchild = -1;
        std::pair<std::string, std::string> com_key = std::make_pair(tx.u, tx.type);
        this->com_key_set.insert(com_key);
        this->value = std::make_pair(tx.v, tx.w);
        
        // 计算com_key_set的acc
        std::string p;          // 混合键对应的素数
        // 首先判断混合键的素数是否已存在于prime_map
        if(prime_map.find(com_key) != prime_map.end()){
            p = prime_map[com_key];
        }
        else{
            // 计算混合键对应的素数
            BigInteger bi;
            BigInteger::generate_prime(bi, com_key.first + com_key.second);
            p = bi.to_string();
            prime_map[com_key] = p;
        }

        // 计算acc
        std::set<std::string> p_set;    // 素数集合
        p_set.insert(p);
        this->acc = msa.get_acc_prime(p_set);
        
        // 素数乘积
        this->product = p;

        // 计算digest
        std::string msg = std::string(32, '\0') + std::string(32, '\0');
        std::string tmp = Crypto_Primitives::SHA256_digest(msg);
        this->digest = Crypto_Primitives::SHA256_digest(tmp + this->acc);
    }



    /*
        构造函数，构造非叶结点
        param:
            lhash, rhash - 左、右子结点的digest
            lchild, rchild - 左右子结点在tree数组中的下标
            com_key_set_l, com_key_set_r - 左右子结点的混合键集合
            prime_map - 储存每个混合键对应的素数。用于打表快速计算素数
            msa
    */
    MHTNode(std::string lhash, std::string rhash, 
            int lchild, int rchild,
            std::set<std::pair<std::string, std::string>>& com_key_set_l, 
            std::set<std::pair<std::string, std::string>>& com_key_set_r, 
            std::map<std::pair<std::string, std::string>, std::string>& prime_map, 
            MultisetAccumulator& msa){
        
        this->isLeaf = false;
        this->lchild = lchild;      this->rchild = rchild;
        
        // 合并com_key_set_l和com_key_set_r到com_key_set中
        std::set_union(com_key_set_l.begin(), com_key_set_l.end(), com_key_set_r.begin(), com_key_set_r.end(),
                        std::inserter(this->com_key_set, this->com_key_set.begin()));

        // 计算com_key_set对应的素数乘积
        std::set<std::string> x_set;                // 储存集合中元素对应的素数
        BigInteger x_p(1);                         // 集合中元素对应素数的乘积
        for(std::pair<std::string, std::string> com_key: this->com_key_set){
            std::string p;      // 素数
            // 判断当前混合键是否在prime_map中
            if(prime_map.find(com_key) != prime_map.end()){
                p = prime_map[com_key];
            }
            else{
                // 计算混合键对应的素数
                BigInteger bi;
                BigInteger::generate_prime(bi, com_key.first + com_key.second);
                p = bi.to_string();
                prime_map[com_key] = p;
            }

            x_set.insert(p);
            // 计算乘积
            BigInteger bi_p(p);
            BigInteger::mul(x_p, x_p, bi_p);
        }
        this->product = x_p.to_string();

        // 计算com_key_set对应的acc
        this->acc = msa.get_acc_prime(x_set);

        // 计算digest
        std::string tmp = Crypto_Primitives::SHA256_digest(lhash+rhash);
        this->digest = Crypto_Primitives::SHA256_digest(tmp + this->acc);
    }
};


// MHT的证明
struct MHTProof{
    std::vector<MHTNode> subtree;
    int root_id;

    // 储存subtree中不匹配的结点对应的non-membership proof
    std::map<int, Nonmembership_Proof> proof;
};


// SP给client返回的数据
struct Response{
    // 每个区块对应的证明子树
    std::map<int, MHTProof> VO;
};




/*
    用于对原始图数据进行shuffle，使得局部有序
*/
struct segment{
    // <u,v> pair
    std::vector<std::pair<int, int>> v;
};





// 跳表中一跳对应的node
struct SkipListNode{
    // 这一跳为空，不填充
    bool is_empty;
    // 一跳中的区块数
    int skip;
    // 一跳中区块的所有混合键
    std::set<std::pair<std::string, std::string>> com_key_set;
    std::string acc;

    // 对应于com_key_set的素数乘积
    std::string product;
};


#endif