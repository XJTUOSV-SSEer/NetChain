#include "../include/experiment.h"


#include <vector>
#include <queue>
#include <map>
#include "../include/Structs.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>    // for std::shuffle
#include <random>       // for std::default_random_engine
#include <chrono>
#include "../include/Block.h"
#include "../include/Query.h"
#include "../include/MPT.h"




void experiment::filterate(std::string filename){
    std::ifstream file(filename);  // 打开文件
    std::string line;
    std::map<std::string, std::set<std::string>> m;

    if (!file.is_open()) {
        std::cout << "can not open file" << std::endl;
        return;
    }

    // 逐行读取文件
    int cnt = 0;            // 行数
    while (std::getline(file, line)) {
        // 跳过以 # 开头的行
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::istringstream iss(line);
        std::string a, b;
        
        // 读取每一行中的两个整数
        iss >> a >> b;
        if(m.find(a)==m.end()){
            m[a] = std::set<std::string>();
        }
        m[a].insert(b);

        cnt++;
    }
    
    file.close();  // 关闭文件

    std::cout << "line number:" << cnt <<std::endl;


    // 将统计信息写入原数据集文件
    std::ofstream outputfile(filename);

    for(std::map<std::string, std::set<std::string>>::iterator it = m.begin(); it!=m.end(); it++){
        for(std::string str: it->second){
            outputfile << it->first << " " << str <<std::endl;
        }
    }

    return;
}



void experiment::show_dataset(std::string filename, int num){
    std::ifstream file(filename);  // 打开文件
    std::string line;
    std::map<std::string, std::set<std::string>> m;

    if (!file.is_open()) {
        std::cout << "can not open file" << std::endl;
        return;
    }

    // 逐行读取文件
    int cnt = 0;            // 行数
    while (std::getline(file, line)) {
        // 跳过以 # 开头的行
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::istringstream iss(line);
        std::string a, b;
        
        // 读取每一行中的两个整数
        iss >> a >> b;
        if(m.find(a)==m.end()){
            m[a] = std::set<std::string>();
        }
        m[a].insert(b);

        cnt++;
    }
    
    file.close();  // 关闭文件

    std::cout << "line number:" << cnt <<std::endl;


    // 将统计信息写入文件
    std::ofstream outputfile("../../dataset/stat.txt");

    for(std::map<std::string, std::set<std::string>>::iterator it = m.begin(); it!=m.end(); it++){
        outputfile << it->first << " " << it->second.size() <<std::endl;
    }

    return;
}



void experiment::adjust_dataset(std::string filename, int seg_size, int ub, std::string target_file){
    std::ifstream file(filename);  // 打开文件
    std::string line;
    // 键为u，值为相应的v的集合
    std::map<std::string, std::queue<std::string>> multimap;

    if (!file.is_open()) {
        std::cout << "can not open file" << std::endl;
        return;
    }

    // 逐行读取文件
    while (std::getline(file, line)) {
        // 跳过以 # 开头的行
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::istringstream iss(line);
        std::string a, b;        
        // 读取每一行中的两个整数
        iss >> a >> b;

        if(multimap.find(a)==multimap.end()){
            multimap[a] = std::queue<std::string>();
        }
        
        multimap[a].push(b);
    }

    // 将multimap中每个u对应的v进行分段
    std::vector<segment> segments;
    for(std::map<std::string, std::queue<std::string>>::iterator it = multimap.begin(); it!=multimap.end(); it++){
        std::string u = it -> first;
        std::queue<std::string> v_set = it->second;

        
        while(!v_set.empty()){
            segment s;
            for(int i=0; i<seg_size; i++){
                s.v.push_back(std::make_pair(u, v_set.front()));
                v_set.pop();

                // 若当前s内元素不足seg_size且集合已空，提前退出
                if(v_set.empty()){
                    break;
                }
            }
            // 将一个segment加入segments
            segments.push_back(s);
        }
    }

    // 对以上segment进行shuffle
    std::default_random_engine rng(65537);
    std::shuffle(segments.begin(), segments.end(), rng);

    file.close();  // 关闭文件
    // 打开输出文件流
    std::ofstream outFile(target_file);

    // 将shuffle后的结果依次写入文件，且加入随机的权重
    for(int i=0; i< segments.size(); i++){
        segment& seg = segments[i];
        for(int j=0; j<seg.v.size(); j++){
            std::pair<std::string, std::string>& p = seg.v[j];
            // 随机的w
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(0, ub);
            int w = dist(gen);

            // 将u,v,w写入文件
            
            if (!outFile) {
                std::cout << "Error opening file for writing" << std::endl;
                return;
            }
            // 写入
            outFile << p.first << ' '<< p.second << ' ' << w<< '\n';
        }
    }
    
    
    outFile.close();
}





experiment::experiment(std::string filename){
    std::ifstream file(filename);  // 打开文件
    std::string line;

    if (!file.is_open()) {
        std::cout << "can not open file" << std::endl;
        return;
    }

    // 逐行读取文件
    while (std::getline(file, line)) {
        // 跳过以 # 开头的行
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::istringstream iss(line);
        std::string u,v;
        int w;
        
        // 读取每一行中的3个整数
        iss >> u >> v >> w;
        transaction tx(u, v, "friend", w);
        this->transactions.push_back(tx);
    }
    
    file.close();  // 关闭文件
}



void experiment::test_mining(int txs_in_one_block){
    auto start = std::chrono::high_resolution_clock::now();
    MPT mpt;
    std::vector<Block> blockchain = Block::construct_chain(transactions, txs_in_one_block, mpt);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    // 输出执行时间
    std::cout << "Duration: " << duration.count() << " seconds" << std::endl;

    // 区块中ADS的平均大小/KB
    int total_size = 0;
    for(int i=100; i<=300; i++){
        total_size = total_size + test_ADS_size(blockchain[i]);
    }
    std::cout << "ADS SIZE:" << (double)total_size/(double)(200*1024)<<std::endl;
}



void experiment::test_query(int txs_in_one_block, std::vector<int> tw_size, std::vector<int> K_list, 
                            std::string u_q){
    MPT mpt;
    std::vector<Block> blockchain = Block::construct_chain(transactions, txs_in_one_block, mpt);
    std::string type_q = "friend";
    std::chrono::_V2::system_clock::time_point start, end;
    std::chrono::duration<double> duration;

    // 对不同组合的参数进行测试
    
    for(int tw: tw_size){
        int lb = 0;
        int ub = tw-1;
        Response response;
        for(int K: K_list) {
            // Search
            start = std::chrono::high_resolution_clock::now();

            for(int x=0;x<50;x++){
                response = Query::Search(u_q, type_q, K, lb, ub, blockchain, mpt);
            }
        
            end = std::chrono::high_resolution_clock::now();
            duration = end - start;
            // 查询时间
            std::cout << "Search Duration: " << duration.count()/50 << " seconds" << std::endl;


            // VO Size, KB
            double vo_size = double(test_VO_size(response)) / double(1024);
            std::cout << "VO Size: " << vo_size << std::endl;


            // Verify
            start = std::chrono::high_resolution_clock::now();

            for(int x=0; x<50; x++){
                Query::Verify(u_q, type_q, response, K, lb, ub, blockchain);
            }

            end = std::chrono::high_resolution_clock::now();
            duration = end - start;
            // 验证时间
            std::cout << "Verify Duration: " << duration.count()/50 << " seconds" << std::endl;
        }
    }

}



int experiment::test_VO_size(Response& response){
    // 每个区块的查询结果R
    std::map<int, std::vector<ListNode>>& R = response.R;
    // 每个区块中com_key_q的存在/不存在证明
    std::map<int, SMTProof>& VO = response.VO;
    // 当com_key_q的最新记录在[lb,ub]内时或小于lb时，根据MPT为com_key_q生成的新鲜性证明
    MPTProof& VO2 = response.VO2;

    // 总的存储空间
    int total_size = 0;

    // 计算VO SIZE
    for(std::pair<int, SMTProof> pf: VO){
        std::vector<SMTNode>& subtree = pf.second.subtree;
        
        // 对subtree中所有SMT结点，计算存储空间。
        // 若是叶结点，包含com_key, l, h1, id_pre ---> 
        // 若是非叶结点，包含lchild, rchild, lhash, rhash --->
        for(SMTNode node: subtree){
            if(node.isLeaf){
                total_size = total_size + 40 + node.compound_key.first.length() + node.compound_key.second.length();
            }
            else{
                if(node.lchild == -1){
                    total_size += 32;
                }
                else{
                    total_size += 4;
                }
                if(node.rchild == -1){
                    total_size += 32;
                }
                else{
                    total_size += 4;
                }
            }
        }

    }


    // 计算R size
    for(std::pair<int, std::vector<ListNode>> pf: R){
        for(ListNode node: pf.second){
            total_size = total_size + node.v.length() + 4 + 32;
        }
    }


    // 计算MPT Proof size
    // proof中有若干MPT NODE，其中一个为叶结点，size为4 bytes；其余为内部结点，包含1个int变量和12个digest（32字节）
    if(VO2.subtree.size() > 0){
        total_size = total_size + 4*1 + (VO2.subtree.size()-1) * (4 + 12*32);
    }
    

    return total_size;
}


int experiment::test_ADS_size(Block blk){
    std::vector<struct SMTNode>& tree = blk.smt.tree;
    std::map<std::pair<std::string, std::string>, std::vector<struct ListNode>>& Lists = blk.Lists;
    int total_size = 0;

    // 计算SMT的大小
    for(SMTNode node: tree){
        if(node.isLeaf){
            total_size = total_size + node.compound_key.first.length() + node.compound_key.second.length() + 32 +4;
        }
        else{
            total_size += 64;
        }
    }


    // 计算Lists的大小
    for(auto pair: Lists){
        std::vector<struct ListNode>& vec = pair.second;
        for(auto node: vec){
            total_size = total_size + node.v.length() + 4 + 32;
        }
    }

    return total_size;
}