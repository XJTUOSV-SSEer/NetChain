#include "../include/SMT.h"
#include <algorithm>

SMT::SMT(){

}


bool compare_by_compound_key(const SMTNode& a, const SMTNode& b){
    return a.compound_key < b.compound_key;
}


SMT::SMT(std::vector<struct SMTNode>& leaves){
    this->leaf_num = leaves.size();

    // 将leaves中所有叶结点加入tree数组，并根据混合键对元素进行排序
    // std::copy(leaves.begin(), leaves.end(), std::back_inserter(this->tree));
    this->tree = leaves;
    std::sort(this->tree.begin(), this->tree.end(), compare_by_compound_key);

    // 构造SMT，得到根结点
    int root_id = construct_tree(0, this->leaf_num-1);
    this->root_id = root_id;
}



int SMT::construct_tree(int lb, int ub){
    // 递归边界：当子树中只有一个叶结点时，直接返回
    if(lb==ub){
        return lb;
    }

    // 当子树中有至少两个叶结点时，可以构造高度至少为2的子树
    // 左子树
    int lchild = construct_tree(lb, int((lb+ub)/2));
    std::string lhash = tree[lchild].digest;
    // 右子树
    int rchild = construct_tree(int((lb+ub)/2)+1, ub);
    std::string rhash = tree[rchild].digest;

    // 当前子树的根结点
    struct SMTNode root = SMTNode(lchild, rchild, lhash, rhash);
    (this->tree).push_back(root);

    // 返回root在tree数组中的下标
    return (this->tree).size()-1;
}



int SMT::merkle_proof(std::set<int>& leaves, int id, SMTProof& proof){
    // 递归边界 - 叶结点
    if(this->tree[id].isLeaf){
        if(leaves.find(id)!= leaves.end()){
            // 将当前结点处理后加入proof
            SMTNode node = this->tree[id];
            node.digest = std::string(32, '\0');
            proof.subtree.push_back(node);
            proof.leaves.push_back(proof.subtree.size()-1);
            // 返回
            return proof.subtree.size()-1;
        }
        else{
            return -1;
        }
    }

    // 内部结点
    // 判断左、右子结点是否on path
    int lid = merkle_proof(leaves, this->tree[id].lchild, proof);
    int rid = merkle_proof(leaves, this->tree[id].rchild, proof);
    // on path，将当前结点处理后加入proof
    if(lid!=-1 || rid!=-1){
        SMTNode node = this->tree[id];
        node.digest = std::string(32, '\0');

        // 左子结点on path
        if(lid!=-1){
            node.lhash = std::string(32, '\0');
            node.lchild = lid;
        }
        else{
            node.lhash = this->tree[this->tree[id].lchild].digest;
            node.lchild = -1;
        }

        // 右子结点on path
        if(rid!=-1){
            node.rhash = std::string(32, '\0');
            node.rchild = rid;
        }
        else{
            node.rhash = this->tree[this->tree[id].rchild].digest;
            node.rchild = -1;
        }

        proof.subtree.push_back(node);
        // 返回
        return proof.subtree.size()-1;
    }
    else{
        return -1;
    }

}



SMTProof SMT::prove_Nonexistence(std::pair<std::string, std::string> com_key_q){
    // 记录与com_key_q相邻的叶结点在当前SMT.tree数组中的下标
    std::set<int> adj_leaves;

    /* ------------------ 找到与com_key_q相邻的叶结点 ----------------------------*/
    // 特殊情况：目标com_key_q比所有叶结点都大/小
    if(com_key_q < this->tree[0].compound_key){
        adj_leaves.insert(0);
    }
    else if(com_key_q > this->tree[this->leaf_num-1].compound_key){
        adj_leaves.insert(this->leaf_num-1);
    }
    // 一般情况。用二分的方法找到对应于com_key_q的两个相邻叶结点
    else{
        int lid = 0, rid = this->leaf_num-1;          // lid, rid分别为二分搜索的左右边界
        while(rid >= lid){
            int mid = int((lid+rid)/2);             // mid为当前二分搜索区间的中间点
            // 判断目标值是否落在[mid, mid+1]或[mid-1,mid]之间
            if(com_key_q < tree[mid].compound_key){
                if(com_key_q > tree[mid-1].compound_key){
                    adj_leaves.insert(mid-1);
                    adj_leaves.insert(mid);
                    break;
                }
                rid = mid-1;
            }
            else if(com_key_q > tree[mid].compound_key){
                if(com_key_q < tree[mid+1].compound_key){
                    adj_leaves.insert(mid);
                    adj_leaves.insert(mid+1);
                    break;
                }
            }
            lid = mid+1;
        }
    }


    /* ----------------------- 对adj_leaves中相邻的结点做merkle proof ---------------------*/
    SMTProof proof;
    int r = merkle_proof(adj_leaves, this->root_id, proof);
    proof.root_id = r;

    return proof;
}



bool SMT::verify_Nonexistence(SMTProof proof){
    /*------------------------ 判断是否为特殊情况，目标com_key_q比所有叶结点都大/小 ---------------*/



    /* -------------------- 一般情况，判断proof.leaves中两个叶结点是否相邻 ---------------------*/
    
}