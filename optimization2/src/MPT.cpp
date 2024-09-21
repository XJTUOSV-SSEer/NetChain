#include "../include/MPT.h"
#include <iostream>


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




void MPT::update(std::pair<std::string, std::string> com_key, int blk_id){
    std::string u = com_key.first;
    std::string type = com_key.second;

    // 根据u和type确定混合键在MPT中的路径
    std::vector<int> path;          // 计算混合键在MPT中的路径
    for(char ch: u){
        path.push_back(get_order(std::string(1, ch)));
    }
    path.push_back(get_order(type));


    // 按照路径寻找叶结点，若路径上某结点不存在，生成对应的新的结点加入MPT
    int current_id = this->root_id;           // 当前结点在tree数组中的下标
    for(int i=0; i<path.size(); i++){
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

    // 按照路径寻找叶结点。若某结点不存在，报错并返回-1
    int current_id = this->root_id;           // 当前结点在tree数组中的下标
    for(int order: path){
        if(this->tree[current_id].ptr_vec[order] == -1){
            std::cout << "Compound key is not in MPT" <<std::endl;
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

    // 根据u和type确定混合键在MPT中的路径
    std::vector<int> path;          // 混合键在MPT中的路径
    for(char ch: u){
        path.push_back(get_order(std::string(1, ch)));
    }
    path.push_back(get_order(type));

    // 按照路径寻找叶结点，且一边找一边将路径上的结点加入proof。若某结点不存在，报错并返回-1
    int current_id = this->root_id;           // 当前结点在tree数组中的下标
    for(int order: path){
        if(this->tree[current_id].ptr_vec[order] == -1){
            std::cout << "Compound key is not in MPT" <<std::endl;
            return MPTProof();
        }
        
        // 找到子结点
        current_id = this->tree[current_id].ptr_vec[order];
    }
}







MPT::MPT(){
    this->tree.push_back(MPTNode());
    this->root_id = 0;
}