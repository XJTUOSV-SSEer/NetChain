#include "../include/SMT.h"
#include <algorithm>
#include <iostream>
#include <regex>
#include <queue>

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



bool SMT::verify_Nonexistence(SMTProof& proof, std::string h_smt, std::pair<std::string, std::string> com_key_q){
    /*------------------------ 判断是否为特殊情况(目标com_key_q比所有叶结点都大/小) ---------------*/
    // 判断proof中的叶结点是否为left most或right most
    if(proof.leaves.size()==1){
        // 叶结点在proof.subtree中的下标
        int id = proof.leaves[0];

        // 当查询的混合键小于叶结点的混合键时，叶结点是否为left most
        if(proof.subtree[id].compound_key > com_key_q){
            // 找到叶结点的路径
            std::string path = "*";
            if(get_path(proof.subtree, proof.root_id, id, path)){
                // 判断路径是否为 "*[0...0]"形式，且id对应的结点确实为叶结点
                std::regex pattern(R"(^\*0*$)");
                if(std::regex_match(path, pattern) && proof.subtree[id].lchild == -1 && proof.subtree[id].rchild == -1){

                }
                else{
                    std::cout << "leaf is not left-most node"<<std::endl;
                    return false;
                }
            }
        }
        // 当查询的混合键大于叶结点的混合键时，叶结点是否为right most
        else if(proof.subtree[id].compound_key < com_key_q){
            // 找到叶结点的路径
            std::string path = "*";
            if(get_path(proof.subtree, proof.root_id, id, path)){
                // 判断路径是否为 "*[1...1]"形式，且id对应的结点确实为叶结点
                std::regex pattern(R"(^\*1*$)");
                if(std::regex_match(path, pattern) && proof.subtree[id].lchild == -1 && proof.subtree[id].rchild == -1){

                }
                else{
                    std::cout << "leaf is not right-most node"<<std::endl;
                    return false;
                }
            }
            else{
                std::cout << "can not find leaf"<<std::endl;
                return false;
            }
        }
    }


    /* -------------------- 一般情况，判断proof.leaves中两个叶结点是否相邻 ---------------------*/
    else{
        // 首先判断两个叶结点是否一个比com_key_q大，一个比com_key_q小
        int id0 = proof.leaves[0]; int id1 = proof.leaves[1];
        if(proof.subtree[id0].compound_key < com_key_q && proof.subtree[id1].compound_key > com_key_q){
        }
        else{
            std::cout << "leaves cannot cover com_key_q" <<std::endl;
            return false;
        }

        // 判断叶结点是否相邻
        // 由于树的层数可能不均匀且返回的是子树，因此不能使用层序遍历(子树中内部结点可能只有一个子结点)
        // 因此，仍使用path来判断。找到left leaf的path，从后向前找到第1个"0"，删除这个片段后加入"10*"，判断得到的是否为
        // right leaf的path
        int left_id = proof.leaves[0];
        int right_id = proof.leaves[1];
        std::string left_path = "*";
        if(get_path(proof.subtree, proof.root_id, left_id, left_path)){
            // 从后向前找到第1个"0"，并删除这一片段
            while(left_path != "*"){
                char last_char = left_path.back();
                if(last_char == '1'){
                    left_path.pop_back();
                }
                else if(last_char == '0'){
                    left_path.pop_back();
                    break;
                }
            }

            // 找到right leaf的path
            std::string right_path="*";
            if(get_path(proof.subtree, proof.root_id, right_id, right_path)){
                // 判断left_path处理后是否为right_path的前缀
                if(right_path.compare(0, left_path.size(), left_path)){
                    // 之后判断right_path的后续部分是否为 "10*"
                    std::string s = right_path.substr(left_path.size());
                    std::regex pattern(R"(^1(0*)$)");
                    if(!std::regex_match(s, pattern) && proof.subtree[right_id].lchild == -1 &&
                            proof.subtree[right_id].rchild == -1){
                        std::cout << "leaves are not adjacent" <<std::endl;
                        return false;
                    }
                }
                else{
                    std::cout << "leaves are not adjacent" <<std::endl;
                    return false;
                }
            }
            else{
                std::cout << "can not find right leaf"<<std::endl;
                return false;
            }
        }
        else{
            std::cout << "can not find left leaf"<<std::endl;
            return false;
        }
    }
    

    /* --------------------- 验证merkle proof -----------------------------------*/
    if(cal_root_hash(proof.subtree, proof.root_id) != h_smt){
        std::cout << "incorrect root hash" <<std::endl;
        return false;
    }

    // SMT不存在性验证通过
    return true;
}




bool SMT::get_path(std::vector<SMTNode>& tree, int id, int target_id, std::string& current_path){
    // 深度优先搜索(DFS)
    // 递归边界 - 找到目标，返回
    if(id == target_id){
        return true; 
    }

    // 判断左右子结点
    if(tree[id].lchild != -1){
        current_path.push_back('0');
        // 左子结点on path of root->target
        bool l_on_path = get_path(tree, tree[id].lchild, target_id, current_path);
        if(l_on_path){
            return true;
        }        
        else{
            // 撤销对current_path的更改
            current_path.pop_back();
        }
    }

    if(tree[id].rchild != -1){
        current_path.push_back('1');
        // 右子结点on path of root->target
        bool r_on_path = get_path(tree, tree[id].rchild, target_id, current_path);
        if(r_on_path){
            return true;
        }        
        else{
            // 撤销对current_path的更改
            current_path.pop_back();
        }
    }
    
    return false;
}




std::string SMT::cal_root_hash(std::vector<SMTNode>& subtree, int id){
    SMTNode& node = subtree[id];
    // 递归边界
    if(node.isLeaf){
        std::string u = node.compound_key.first;
        std::string type = node.compound_key.second;
        std::string msg = u+type+ std::to_string(node.l) + node.h1;
        node.digest = Crypto_Primitives::SHA256_digest(msg);
        return node.digest;
    }
    
    // 内部结点
    std::string lhash, rhash;
    if(node.lchild != -1){
        lhash = cal_root_hash(subtree, node.lchild);
    }
    else{
        lhash = node.lhash;
    }
    if(node.rchild != -1){
        rhash = cal_root_hash(subtree, node.rchild);
    }
    else{
        rhash = node.rhash;
    }

    node.digest = Crypto_Primitives::SHA256_digest(lhash+rhash);
    return node.digest;
}




SMTProof SMT::prove_Existence(int target_id){
    SMTProof proof;
    // 直接调用merkle_prove方法
    std::set<int> leaves;
    leaves.insert(target_id);
    int r= merkle_proof(leaves, this->root_id, proof);
    proof.root_id = r;

    return proof;
}



bool SMT::verify_Existence(SMTProof& proof, std::string h_smt, std::pair<std::string, std::string> com_key_q){
    // 判断proof.leaves中的叶结点是否等于com_key_q
    if(proof.subtree[proof.leaves[0]].compound_key != com_key_q){
        std::cout << "incorrect leaf node" <<std::endl;
        return false;
    }

    // merkle proof
    if(cal_root_hash(proof.subtree, proof.root_id) != h_smt){
        std::cout << "incorrect root hash" <<std::endl;
        return false;
    }

    // SMT存在性验证通过
    return true;
}


































// 队列储存遍历中的结点在subtree数组中的下标
// std::queue<int> q;
// q.push(proof.root_id);
// // 上一次遍历的结点在subtree数组中的下标
// int pre_id = -1;
// // 标识两个叶结点是否相邻
// bool flag = false;
// while(!q.empty()){
//     // 当前结点
//     int id = q.front();
//     // 判断是否命中leaves中的两个叶结点
//     if(pre_id == proof.leaves[0] && id == proof.leaves[1]){
//         flag = true;
//         break;
//     }
//     // 加入左右子结点
//     if(proof.subtree[id].lchild != -1){
//         q.push(proof.subtree[id].lchild);
//     }
//     if(proof.subtree[id].rchild != -1){
//         q.push(proof.subtree[id].rchild);
//     }

//     // 更新pre_id
//     pre_id = id;
//     q.pop();
// }

// if(!flag){
//     std::cout << "leaves are not adjacent" <<std::endl;
//     return false;
// }