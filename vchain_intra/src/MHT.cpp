#include "../include/MHT.h"
#include <vector>
#include <queue>
#include <set>
#include <algorithm>



double MHT::get_similarity(std::set<std::pair<std::string, std::string>>& a, 
                    std::set<std::pair<std::string, std::string>>& b){
    // 对两个集合求交集和并集
    std::set<std::pair<std::string, std::string>> s_union;
    std::set<std::pair<std::string, std::string>> s_inter;

    std::set_union(a.begin(), a.end(), b.begin(), b.end(), std::inserter(s_union, s_union.begin()));
    std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::inserter(s_inter, s_inter.begin()));

    return s_inter.size() / s_union.size();
}


MHT::MHT(std::vector<transaction> txs, std::map<std::pair<std::string, std::string>, std::string>& prime_map, 
            MultisetAccumulator& msa){

    // 一层的结点在tree数组中的下标
    std::vector<int> nodes;

    // 将所有交易构造为叶结点，加入nodes
    for(transaction tx: txs){
        this->tree.push_back(MHTNode(tx, prime_map, msa));
        nodes.push_back(this->tree.size()-1);
    }

    // 自底向上建立MHT，层序建立
    // 当nodes中元素数量为1时，MHT构造完成，剩余的一个元素为树根
    while(nodes.size() > 1){
        // 储存当前层的父结点在tree数组中的下标
        std::vector<int> newnodes;

        while(nodes.size() > 1){
            // nodes中第一个结点
            int id_l = nodes[0];

            // 遍历nodes中其他结点，寻找与第一个结点相似度最大的结点作为id_r
            double max_similarity = 0;
            int id_r = -1;       // 与id_l相似度最大的结点在tree中的下标
            int id_target = -1;       // 与id_l相似度最大的结点在nodes中的下标
            for(int i=1; i<nodes.size(); i++){
                int cid = nodes[i];
                // 计算两个结点的相似度
                double similarity = get_similarity(this->tree[id_l].com_key_set, this->tree[cid].com_key_set);
                if(similarity >= max_similarity){
                    max_similarity = similarity;
                    id_r = cid;
                    id_target = i;
                }
            }

            // 将id_l, id_r从nodes中删除。注意，先删id_r，否则索引会变
            nodes.erase(nodes.begin()+id_target);
            nodes.erase(nodes.begin());


            // 将id_l和id_r作为子结点，构造父结点并加入tree数组
            this->tree.push_back(MHTNode(
                this->tree[id_l].digest, this->tree[id_r].digest,
                id_l, id_r,
                this->tree[id_l].com_key_set, this->tree[id_r].com_key_set,
                prime_map, msa
            ));

            // 将新的结点加入newnodes
            newnodes.push_back(this->tree.size()-1);
        }

        // 更新nodes为新的一层结点
        nodes.insert(nodes.end(), newnodes.begin(), newnodes.end());
    }

    // 得到根结点 
    this->root_id = nodes[0];
}


MHT::MHT(){

}




int MHT::search(MHTProof& mht_proof, int current_id, std::pair<std::string, std::string> com_key_q, 
                MultisetAccumulator& msa, std::string p){
    std::vector<MHTNode>& subtree = mht_proof.subtree;
    std::map<int, Nonmembership_Proof>& proof = mht_proof.proof;


    // 判断结点是否包含混合键
    MHTNode& node = this->tree[current_id];

    // 包含com_key_q
    if(node.com_key_set.find(com_key_q) != node.com_key_set.end()){
        // 是叶结点
        if(node.isLeaf){
            MHTNode newnode;
            newnode.isLeaf = true;
            newnode.lchild = -1;    newnode.rchild = -1;
            newnode.value = node.value;
            newnode.acc = node.acc;
            subtree.push_back(newnode);
            return subtree.size()-1;
        }

        // 是内部结点
        else{
            MHTNode newnode;
            newnode.isLeaf = false;
            newnode.acc = node.acc;
            // 判断左右子结点
            newnode.lchild = search(mht_proof, node.lchild, com_key_q, msa, p);
            newnode.rchild = search(mht_proof, node.rchild, com_key_q, msa, p);

            subtree.push_back(newnode);
            return subtree.size()-1;
        }
    }

    // 不包含com_key_q
    else{
        MHTNode newnode;
        newnode.acc = node.acc;
        newnode.digest = node.digest;
        subtree.push_back(newnode);
        
        // 生成Nonmembership proof
        Nonmembership_Proof nonmember_proof = msa.proove_nonmembership_prime(node.product, p);

        proof[subtree.size()-1] = nonmember_proof;

        return subtree.size()-1;
    }
}