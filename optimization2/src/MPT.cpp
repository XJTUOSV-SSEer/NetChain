#include "../include/MPT.h"
#include <iostream>
#include <stack>


int MPT::get_order(std::string s){
    // 假设输入"0"-"9"，返回0-9
    if(s.length()==1){
        char ch = s[0];
        return int(ch-48);
    }



    // 若输入type类型，如 "friend"，返回10；"family"，返回11；"colleague"，返回12
    else if(s=="friend"){
        return 10;
    }
    else if(s=="family"){
        return 11;
    }
    else if(s=="colleague"){
        return 12;
    }

    return -1;
}




std::string MPT::update(std::pair<std::string, std::string> com_key, int blk_id){
    std::string u = com_key.first;
    std::string type = com_key.second;

    // 根据u和type确定混合键在MPT中的路径
    std::vector<int> path;          // 计算混合键在MPT中的路径
    for(char ch: u){
        path.push_back(get_order(std::string(1, ch)));
    }
    path.push_back(get_order(type));


    // 按照路径寻找叶结点，若路径上某结点不存在，生成对应的新的结点加入MPT
    // 将路径上经过的所有结点在数组中的下标压入栈中
    std::stack<int> st;
    int current_id = this->root_id;           // 当前结点在tree数组中的下标
    for(int i=0; i<path.size(); i++){
        // 压栈
        st.push(current_id);

        // order为子结点中的顺序
        int order = path[i];
        // 若路径上该结点为空，新建结点
        if(this->tree[current_id].ptr_vec[order] == -1){
            MPTNode node;

            // 将新结点加入tree
            this->tree.push_back(node);
            // 更新指针
            this->tree[current_id].ptr_vec[order] = this->tree.size()-1;
        }

        // 找到子结点
        current_id = this->tree[current_id].ptr_vec[order];
    }

    // 更新叶结点中的latest_blk_id
    this->tree[current_id].isLeaf = true;
    this->tree[current_id].latest_blk_id = blk_id;
    st.push(current_id);


    // 回代，更新路径上结点的哈希值
    // 使用栈完成
    MPTNode& node = this->tree[st.top()];
    std::string child_hash = node.get_hash();
    st.pop();
    for(int i = path.size()-1; i>=0; i--){
        int order = path[i];
        MPTNode& node = this->tree[st.top()];
        node.hash_vec[order] = child_hash;
        child_hash = node.get_hash();
        st.pop();
    }

    // 返回最新的MPT的根哈希
    return child_hash;
}



int MPT::search(std::pair<std::string, std::string> com_key){
    std::string u = com_key.first;
    std::string type = com_key.second;

    // 根据u和type确定混合键在MPT中的路径
    std::vector<int> path;          // 混合键在MPT中的路径
    for(char ch: u){
        path.push_back(get_order(std::string(1, ch)));
    }
    path.push_back(get_order(type));

    // 按照路径寻找叶结点。若某结点不存在，返回-1
    int current_id = this->root_id;           // 当前结点在tree数组中的下标
    for(int order: path){
        if(this->tree[current_id].ptr_vec[order] == -1){
            // std::cout << "Compound key is not in MPT" <<std::endl;
            return -1;
        }
        
        // 找到子结点
        current_id = this->tree[current_id].ptr_vec[order];
    }

    // 返回叶结点中的latest_blk_id
    return this->tree[current_id].latest_blk_id;
}



MPTProof MPT::proveExistence(std::pair<std::string, std::string> com_key){
    std::string u = com_key.first;
    std::string type = com_key.second;
    MPTProof proof;
    proof.root_id = 0;

    // 根据u和type确定混合键在MPT中的路径
    std::vector<int> path;          // 混合键在MPT中的路径
    for(char ch: u){
        path.push_back(get_order(std::string(1, ch)));
    }
    path.push_back(get_order(type));

    // 按照路径寻找叶结点，一边找一边将路径上的结点加入proof。若某结点不存在，报错并返回-1
    int current_id = this->root_id;           // 当前结点在tree数组中的下标
    for(int order: path){
        if(this->tree[current_id].ptr_vec[order] == -1){
            std::cout << "Compound key is not in MPT" <<std::endl;
            return MPTProof();
        }

        // 复制当前结点并处理后加入proof
        MPTNode node;
        node.ptr_vec[order] = proof.subtree.size()+1;
        node.hash_vec = this->tree[current_id].hash_vec;
        node.hash_vec[order] = std::string(32, '\0');
        
        proof.subtree.push_back(node);
        
        // 找到子结点
        current_id = this->tree[current_id].ptr_vec[order];
    }

    // 将叶结点加入proof
    MPTNode node = this->tree[current_id];
    proof.subtree.push_back(node);
    proof.leaf_id = proof.subtree.size()-1;

    return proof;
}



bool MPT::verifyExistence(std::pair<std::string, std::string> com_key, int latest_blk, MPTProof proof, std::string h_mpt){
    // 几个检查点：
    // proof中叶结点的latest_blk_id等于服务器宣称的latest_blk
    // proof中从root到leaf的路径与com_key正确的路径相同
    // 根哈希相同

    if(proof.subtree[proof.leaf_id].latest_blk_id != latest_blk){
        std::cout << "latest block id is wrong" <<std::endl;
        return false;
    }

    // 混合键的路径
    std::string u = com_key.first;
    std::string type = com_key.second;
    std::vector<int> path;          // 混合键在MPT中的路径
    for(char ch: u){
        path.push_back(get_order(std::string(1, ch)));
    }
    path.push_back(get_order(type));


    // 使用栈计算proof中子树的根哈希
    std::stack<int> st;
    int current_id = proof.root_id;           // 当前结点在tree数组中的下标
    for(int order:path){
        st.push(current_id);
        if(proof.subtree[current_id].ptr_vec[order] == -1){
            std::cout << "the proof not belong to compound key" <<std::endl;
            return false;
        }
        
        // 找到子结点
        current_id = proof.subtree[current_id].ptr_vec[order];
    }
    st.push(current_id);

    // 使用栈计算子树的根哈希
    MPTNode& node = proof.subtree[st.top()];
    std::string child_hash = node.get_hash();
    st.pop();
    for(int i = path.size()-1; i>=0; i--){
        int order = path[i];
        MPTNode& node = proof.subtree[st.top()];
        node.hash_vec[order] = child_hash;
        child_hash = node.get_hash();
        st.pop();
    }

    // 判断根哈希是否相等
    if(child_hash != h_mpt){
        std::cout << "root hash of proof is wrong" <<std::endl;
        return false;
    }

    return true;
}




MPT::MPT(){
    this->tree.push_back(MPTNode());
    this->root_id = 0;
}