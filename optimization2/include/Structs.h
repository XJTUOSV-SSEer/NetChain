// 一些结构体
#ifndef STRUCT_H
#define STRUCT_H


#include <vector>
#include <string>
#include <set>
#include "Crypto_Primitives.h"
#include <map>



// SMT的结点
struct SMTNode{
    // 标识是否为叶结点
    bool isLeaf;
    // 混合键 <u, type>
    std::pair<std::string, std::string> compound_key;
    // 该混合键对应sorted list的长度
    int l;
    // 该混合键对应sorted list的头哈希
    std::string h1;
    // 左子结点在tree数组中的下标。若不存在，置为-1
    int lchild;
    std::string lhash;
    // 右子结点在tree数组中的下标。若不存在，置为-1
    int rchild;
    std::string rhash;
    // 结点的哈希, 叶结点的哈希为H(u||type||l||h1)，非叶结点的哈希为H(lhash||rhash)
    std::string digest;

    // 构造函数，构造一个叶结点
    SMTNode(std::string u, std::string type, int l, std::string h1){
        this->isLeaf = true;
        this->compound_key = std::make_pair(u, type);
        this->l = l;
        this->h1 = h1;

        // H(u||type||l||h1)
        std::string msg = u+type+ std::to_string(l) + h1;
        this->digest = Crypto_Primitives::SHA256_digest(msg);

        this->lchild = -1;
        this->rchild = -1;
    }

    // 构造函数，构造一个非叶结点
    SMTNode(int lchild, int rchild, std::string lhash, std::string rhash){
        this->isLeaf = false;

        this->lchild = lchild;
        this->rchild = rchild;
        
        // H(lhash||rhash)
        this->digest = Crypto_Primitives::SHA256_digest(lhash+rhash);
    }
};



/*
    MPT树的结点
*/
struct MPTNode{
    // 标识是否为叶结点
    bool isLeaf;
    // 当是叶结点时，储存的该混合键最新记录所在区块号
    int latest_blk_id;

    // 储存指向各个子结点的指针(即子结点在tree数组中的下标)。在本文中，有13个子结点代表"0"-"9", type。整个type作为单独的
    // 路径元素
    std::vector<int> ptr_vec;
    // 储存各个子结点的哈希
    std::vector<std::string> hash_vec;

    MPTNode(){
        this->isLeaf = false;
        // 将两个数组初始化为size为13
        this->ptr_vec = std::vector<int>(13);
        this->hash_vec = std::vector<std::string>(13);
        for(int i = 0; i<this->ptr_vec.size(); i++){
            this->ptr_vec[i] = -1;
        }
        for(int i=0; i<this->hash_vec.size(); i++){
            this->hash_vec[i] = std::string(32, '\0');
        }
    }
};


/*
    每个混合键对应sorted List中的结点
*/
struct ListNode{
    // 点v
    std::string v;
    // 权重w
    int w;
    // 哈希指针ptr
    std::string ptr;

    ListNode(std::string v, int w){
        this->v = v;
        this->w = w;
        // 哈希指针默认设置为空, 即32字节全0字符串
        this->ptr = std::string(32, '\0');
    }
};



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




/*
    SMT的存在/不存在证明
*/
struct SMTProof{
    // 储存merkle证明的子树
    std::vector<SMTNode> subtree;
    // 叶结点在子树中的下标。当做存在证明时，只有一个元素代表匹配的叶结点；做不存在证明时，有1或2个元素代表相邻叶结点
    std::vector<int> leaves;
    // 根结点在子树中的下标
    int root_id;
};



/*
    MPT中元素的存在证明
*/
struct MPTProof{
    // 储存merkle证明的子树
    std::vector<MPTNode> subtree;
    // 根结点在子树中的下标
    int root_id;
    // 被验证的混合键对应叶结点在子树中的下标
    int leaf_id;
};



/*
    打包储存查询结果与验证对象
*/
struct Response{
    // 每个区块的查询结果R
    std::map<int, std::vector<ListNode>> R;
    // 每个区块中com_key_q的存在/不存在证明
    std::map<int, SMTProof> VO;
};





/*
    用于对原始图数据进行shuffle，使得局部有序
*/
struct segment{
    // <u,v> pair
    std::vector<std::pair<int, int>> v;
};



#endif