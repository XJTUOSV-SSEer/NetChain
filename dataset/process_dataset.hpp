#ifndef PROCESS_PAYSIM_HPP
#define PROCESS_PAYSIM_HPP

#include <vector>
#include <queue>
#include <map>
#include "../base/include/Structs.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <functional>
#include <unordered_map>
#include <cctype>
#include <iterator>
#include <algorithm>    // for std::shuffle
#include <random>       // for std::default_random_engine
#include <iomanip>






class process_dataset{
public:
    /*
        将原始的id映射为[0, 2^31-1]范围内的随机整数，作为新的统一id，并转换为长度10字节的字符串明文
    */
    static std::string hash_string_to_24bit(const std::string& s){
        std::hash<std::string> hasher;
        size_t hash_val = static_cast<uint32_t>(hasher(s)) & ((1U << 31) - 1);  // 限制在 [0, 2^31 - 1]
        std::ostringstream oss;
        oss << std::setw(10) << std::setfill('0') << hash_val;
        return oss.str();
    }


    /*
        将paysim数据集中的账户交易信息转换为交易，并储存在target_file中
        input:
            seg_size - 对相同复合键聚合后的segment size
    */
    static void process_paysim_dataset(std::string filename, std::string target_file, size_t seg_size) {
        std::vector<transaction> transactions;
        std::ifstream file(filename);

        if (!file.is_open()) {
            std::cerr << "无法打开文件: " << filename << std::endl;
            return;
        }

        std::string line;
        // 跳过表头
        std::getline(file, line);

        std::set<std::string> account_set;         // 储存出现的账户名

        while (std::getline(file, line)) {
            if (line.empty()) continue;

            std::stringstream ss(line);
            std::string field;
            std::vector<std::string> fields;

            // 按逗号分割字段（简单 CSV 解析，假设无引号或换行）
            while (std::getline(ss, field, ',')) {
                fields.push_back(field);
            }

            // PaySim 应有 11 列
            if (fields.size() < 11) {
                std::cerr << "警告：跳过格式错误的行: " << line << std::endl;
                continue;
            }

            std::string type = fields[1];          // type
            double amount = 0;
            try {
                amount = std::stod(fields[2]);  // amount
            } catch (const std::exception& e) {
                std::cerr << "无效金额: " << fields[2] << std::endl;
                continue;
            }
            std::string nameOrig = fields[3];      // nameOrig
            std::string nameDest = fields[6];      // nameDest
            transactions.push_back(transaction(nameOrig, nameDest, type, (int)amount));
            account_set.insert(nameOrig);
            account_set.insert(nameDest);
        }

        file.close();
        std::cout << "num of accounts: " << account_set.size() << std::endl;

        // 为复合键<C1231006815, PAYMENT>再生成900条随机交易
        std::vector<std::string> account_list(account_set.begin(), account_set.end());
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis1(0, static_cast<int>(account_list.size()) - 1);
        std::uniform_int_distribution<int> dis2(5000, 500000);
        for(size_t i = 0; i<900; i++) {
            std::string nameDest = account_list[dis1(gen)];     // 随机选取一个接收账户
            int amount = dis2(gen);
            // 构造一条随机交易
            std::string nameOrig = "C1231006815";
            std::string type = "PAYMENT";
            transactions.push_back(transaction(nameOrig, nameDest, type, (int)amount));
        }

        std::cout << transactions.size() << std::endl;

        // 根据复合键对交易分segment，然后shuffle
        std::map<std::string, std::queue<transaction>> multimap;       // 储存复合键->对应交易
        for(transaction& tx : transactions){
            std::string kw = tx.u + tx.type;
            if(multimap.find(kw)==multimap.end()){
                multimap[kw] = std::queue<transaction>();
            }
            multimap[kw].push(tx);
        }
        std::vector<std::vector<transaction>> segments;
        for(std::map<std::string, std::queue<transaction>>::iterator it = multimap.begin(); it!=multimap.end(); it++){
            std::string kw = it -> first;
            std::queue<transaction> txs = it->second;
            while(!txs.empty()) {
                std::vector<transaction> segment;
                for(size_t i=0; i<seg_size; i++) {
                    segment.push_back(txs.front());
                    txs.pop();
                    // 若当前txs内元素不足seg_size且集合已空，提前退出
                    if(txs.empty()){
                        break;
                    }
                }
                // 将segment加入segments
                segments.push_back(segment);
            }
        }

        // 对segments进行shuffle
        std::default_random_engine rng(65537);
        std::shuffle(segments.begin(), segments.end(), rng);


        // 将交易写入文件        
        std::ofstream outFile(target_file);     // 打开输出文件流
        if (!outFile) {
            std::cout << "Error opening file for writing" << std::endl;
            return;
        }
        for(std::vector<transaction>& segment : segments) {
            for(transaction& tx : segment) {
                outFile << hash_string_to_24bit(tx.u) << ' '<< hash_string_to_24bit(tx.v) << ' ' << tx.type << ' ' << tx.w << '\n';
            }
        }
        outFile.close();
    }


    

    /*
        处理email，wiki, gplus数据集，权重为均匀分布
        边的类型可能为friend, family, colleague,
    */
    static void process_other_dataset(std::string filename, std::string target_file, size_t seg_size) {
        std::vector<transaction> transactions;
        std::ifstream file(filename);

        if (!file.is_open()) {
            std::cerr << "无法打开文件: " << filename << std::endl;
            return;
        }

        std::string line;
        std::vector<std::string> type_list = {"friend", "family", "colleague"};
        int ub = 10000;
        std::uniform_int_distribution<> dist1(0, ub);
        std::uniform_int_distribution<> dist2(0, type_list.size() - 1);
        
        std::set<std::string> user_set;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;

            std::istringstream ss(line);
            std::string u, v;
            ss >> u >> v;    
            user_set.insert(u);
            user_set.insert(v);
            std::random_device rd;      
            std::mt19937 gen(rd());
            std::string type = type_list[dist2(gen)];          // type                      
            int amount = dist1(gen);
            transactions.push_back(transaction(u, v, type, amount));
        }
        file.close();
        std::cout << "num of users: " << user_set.size();

        // 根据复合键对交易分segment，然后shuffle
        std::map<std::string, std::queue<transaction>> multimap;       // 储存复合键->对应交易
        for(transaction& tx : transactions){
            std::string kw = tx.u + tx.type;
            if(multimap.find(kw)==multimap.end()){
                multimap[kw] = std::queue<transaction>();
            }
            multimap[kw].push(tx);
        }
        std::vector<std::vector<transaction>> segments;
        for(std::map<std::string, std::queue<transaction>>::iterator it = multimap.begin(); it!=multimap.end(); it++){
            std::string kw = it -> first;
            std::queue<transaction> txs = it->second;
            while(!txs.empty()) {
                std::vector<transaction> segment;
                for(size_t i=0; i<seg_size; i++) {
                    segment.push_back(txs.front());
                    txs.pop();
                    // 若当前txs内元素不足seg_size且集合已空，提前退出
                    if(txs.empty()){
                        break;
                    }
                }
                // 将segment加入segments
                segments.push_back(segment);
            }
        }
        // 对segments进行shuffle
        std::default_random_engine rng(65537);
        std::shuffle(segments.begin(), segments.end(), rng);


        // 将交易写入文件        
        std::ofstream outFile(target_file);     // 打开输出文件流
        if (!outFile) {
            std::cout << "Error opening file for writing" << std::endl;
            return;
        }
        for(std::vector<transaction>& segment : segments) {
            for(transaction& tx : segment) {
                // 将id统一编码
                outFile << hash_string_to_24bit(tx.u) << ' '<< hash_string_to_24bit(tx.v) << ' ' << tx.type << ' ' << tx.w << '\n';
            }
        }
        outFile.close();
    }


    /*
        对GOWALLA数据集进行处理，得到对应的图数据集。
        具体地，用户-用户若是好友，则边权设置为共同签到地点数+1, type = friend；
        若不是好友但去过同一地点，则边权设置为共同签到地点, type = co-interest；
        用户-地点边的边权设置为签到的次数, type = check-in
        input:
            filename_user - 用户-用户好友情况数据集
            filename_loc - 用户-地点签到情况数据集
    */
    static void process_gowalla_dataset(std::string filename_user, std::string filename_loc, std::string target_file, size_t seg_size) {
        // 内联函数，验证ID为数字（安全过滤）
        auto isNumber = [](const std::string& s) {
            return !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
        };
        // 用于存储 co-interest 边（无向，去重）
        struct PairCompare {
            bool operator()(const std::pair<std::string, std::string>& a, const std::pair<std::string, std::string>& b) const {
                if (a.first != b.first) return a.first < b.first;
                return a.second < b.second;
            }
        };

        std::cout << "1" << std::endl;

        // Step 1: 读取签到记录，构建 user -> (location -> count)
        std::unordered_map<std::string, std::unordered_map<std::string, int>> userLocCount; // "Uxxx" -> ("Pyyy" -> count)
        std::ifstream checkinFile(filename_loc);
        std::string line;
        while (getline(checkinFile, line)) {
            if (line.empty()) continue;
            std::istringstream iss(line);
            std::string userIdStr, timestamp, lat, lon, locIdStr;
            if (!(iss >> userIdStr >> timestamp >> lat >> lon >> locIdStr)) continue;

            
            if (!isNumber(userIdStr) || !isNumber(locIdStr)) continue;

            std::string uid = "U" + userIdStr;
            std::string pid = "P" + locIdStr;
            userLocCount[uid][pid]++;
        }
        checkinFile.close();
        std::cout << "num of users: " << userLocCount.size() << std::endl;

        // 构建 location -> set<user> 的倒排索引（用于快速计算共同签到）
        std::unordered_map<std::string, std::unordered_set<std::string>> locToUsers;
        for (const auto& userEntry : userLocCount) {
            const std::string& uid = userEntry.first;
            for (const auto& locEntry : userEntry.second) {
                const std::string& pid = locEntry.first;
                locToUsers[pid].insert(uid);
            }
        }

        std::cout << "num of locations: " << locToUsers.size() << std::endl;

        // Step 2: 读取显式 friend 边（保留方向）
        std::set<std::pair<std::string, std::string>> friendEdges; // 保留原始方向：(Ua, Ub)
        std::ifstream edgeFile(filename_user);
        std::string uStr, vStr;
        while (edgeFile >> uStr >> vStr) {
            if (!isNumber(uStr) || !isNumber(vStr)) continue;
            std::string u = "U" + uStr;
            std::string v = "U" + vStr;
            friendEdges.insert({u, v});
        }
        edgeFile.close();
        
        // Step 3: 生成交易
        std::vector<transaction> transactions;
        // 3.1 输出 user-location 边 (checkin)
        for (const auto& userEntry : userLocCount) {
            const std::string& uid = userEntry.first;
            for (const auto& locEntry : userEntry.second) {
                const  std::string& pid = locEntry.first;
                int count = locEntry.second;
                transactions.push_back(transaction(uid, pid, "checkin", count));

                // outFile << uid << " " << pid << " " << "checkin " << count << "\n";
            }
        }
        // 3.2 输出 friend 边（带权重 = common + 1）
        for (const auto& edge : friendEdges) {
            const std::string& u = edge.first;
            const std::string& v = edge.second;

            // 计算共同签到地点数
            int common = 0;
            if (userLocCount.count(u)) {
                for (const auto& locCount : userLocCount.at(u)) {
                    const std::string& loc = locCount.first;
                    if (userLocCount.count(v) && userLocCount.at(v).count(loc)) {
                        common++;
                    }
                }
            }
            int weight = common + 1;
            transactions.push_back(transaction(u, v, "friend", weight));
            transactions.push_back(transaction(v, u, "friend", weight));
            // outFile << u << " " << v << " " << "friend " << weight << "\n";
            // outFile << v << " " << u << " " << "friend " << weight << "\n";
        }

        std::cout << "step 3.2" << std::endl;

        // 3.3 构建 co-interest 边（非 friend 但有共同地点）
        std::set<std::pair<std::string, std::string>, PairCompare> coInterestEdges; // 无向，自动去重
        // 遍历每个地点的用户列表
        for (const auto& locEntry : locToUsers) {
            const auto& users = locEntry.second;
            if (users.size() < 2) continue;

            // 两两组合
            std::vector<std::string> userList(users.begin(), users.end());
            for (size_t i = 0; i < userList.size(); ++i) {
                for (size_t j = i + 1; j < userList.size(); ++j) {
                    std::string u = userList[i];
                    std::string v = userList[j];
                    // 确保字典序（小在前）
                    if (u > v) swap(u, v);
                    // 跳过已存在的 friend 边（任意方向）
                    if (friendEdges.count({u, v}) || friendEdges.count({v, u})) {
                        continue;
                    }
                    coInterestEdges.insert({u, v});
                }
            }
        }

        // 为每个 co-interest 边计算共同地点数
        for (const auto& edge : coInterestEdges) {
            const std::string& u = edge.first;
            const std::string& v = edge.second;

            int common = 0;
            if (userLocCount.count(u)) {
                for (const auto& locCount : userLocCount.at(u)) {
                    const std::string& loc = locCount.first;
                    if (userLocCount.count(v) && userLocCount.at(v).count(loc)) {
                        common++;
                    }
                }
            }
            // co-interest 权重 = 共同地点数（不加1）
            if (common > 0) {
                transactions.push_back(transaction(u, v, "cointerest", common));
                transactions.push_back(transaction(v, u, "cointerest", common));
                // outFile << u << " " << v << " " << "cointerest " << common << "\n";
                // outFile << v << " " << u << " " << "cointerest " << common << "\n";
            }
        }

        std::cout << "step 3.3" << std::endl;

        // 根据复合键对交易分segment，然后shuffle
        std::map<std::string, std::queue<transaction>> multimap;       // 储存复合键->对应交易
        for(transaction& tx : transactions){
            std::string kw = tx.u + tx.type;
            if(multimap.find(kw)==multimap.end()){
                multimap[kw] = std::queue<transaction>();
            }
            multimap[kw].push(tx);
        }
        std::vector<std::vector<transaction>> segments;
        for(std::map<std::string, std::queue<transaction>>::iterator it = multimap.begin(); it!=multimap.end(); it++){
            std::string kw = it -> first;
            std::queue<transaction> txs = it->second;
            while(!txs.empty()) {
                std::vector<transaction> segment;
                for(size_t i=0; i<seg_size; i++) {
                    segment.push_back(txs.front());
                    txs.pop();
                    // 若当前txs内元素不足seg_size且集合已空，提前退出
                    if(txs.empty()){
                        break;
                    }
                }
                // 将segment加入segments
                segments.push_back(segment);
            }
        }
        // 对segments进行shuffle
        std::default_random_engine rng(65537);
        std::shuffle(segments.begin(), segments.end(), rng);

        // 将交易写入文件        
        std::ofstream outFile(target_file);     // 打开输出文件流
        if (!outFile) {
            std::cout << "Error opening file for writing" << std::endl;
            return;
        }
        for(std::vector<transaction>& segment : segments) {
            for(transaction& tx : segment) {
                outFile << hash_string_to_24bit(tx.u) << ' '<< hash_string_to_24bit(tx.v) << ' ' << tx.type << ' ' << tx.w << '\n';
            }
        }
        outFile.close();
    }



    /*
        对LASTFM数据集进行处理，得到对应的图数据集。
        图中有用户、艺术家、tag三种顶点。
        四种边：
        若用户为好友，则存在一条边，边权为共同收听艺术家的数量+1，type=friend；
        用户与听的艺术家之间存在一条边，边权为收听次数, type = listen；
        艺术家与被赋予的标签之间存在一条边，边权为被赋予该标签的次数, type = hastag
        用户U与标签T间的边权为U对所有被打上标签T的艺术家的收听次数之和，type = interest
        input:
            filename_user - 用户-用户好友情况数据集
            filename_loc - 用户-地点签到情况数据集
    */
    static void process_lastfm_dataset(std::string file_user_artists, std::string file_user_friends,
                            std::string file_user_artist_tags, std::string target_file, size_t seg_size) {
        // Step 1: 读取 user_artists.dat → 构建 listen 边 & 用户-艺术家映射
        std::unordered_map<std::string, std::unordered_map<std::string, int>> userArtists; // Uid -> (Aid ->  收听次数)
        std::unordered_map<std::string, std::unordered_set<std::string>> userArtistSet;   // Uid -> {Aid} （用于 interest 计算）

        std::ifstream fin_ua(file_user_artists);
        if (!fin_ua.is_open()) {
            std::cerr << "ERROR: Cannot open user_artists.dat: " << std::endl;
            return;
        }
        std::string line;
        while (getline(fin_ua, line)) {
            if (line.empty() || line[0] == 'u')     continue;       // 跳过首行
            replace(line.begin(), line.end(), '\t', ' ');
            std::istringstream iss(line);
            std::string uid, aid;
            int weight;
            if (!(iss >> uid >> aid >> weight)) continue;
            std::string u = "U" + uid;
            std::string a = "A" + aid;
            userArtists[u][a] = weight;
            userArtistSet[u].insert(a);
        }
        fin_ua.close();
        std::cout << "num of users: " << userArtists.size() << std::endl;

        // Step 2: 读取 artist_tags.dat → 构建 has_tag 边
        std::unordered_map<std::string, std::unordered_map<std::string, int>> artistTags; // Aid -> (Tid -> count)
        std::ifstream fin_at(file_user_artist_tags);
        if (!fin_at.is_open()) {
            std::cerr << "ERROR: Cannot open artist_tags.dat: " << std::endl;
            return;
        }
        while (getline(fin_at, line)) {
            if (line.empty() || line[0] == 'u')     continue;
            replace(line.begin(), line.end(), '\t', ' ');
            std::istringstream iss(line);
            std::string uid, aid, tid, ts;
            iss >> uid >> aid >> tid >> ts;
            std::string a = "A" + aid;
            std::string t = "T" + tid;
            artistTags[a][t]++;
        }
        fin_at.close();

        std::cout << "num of artists: " << artistTags.size() << std::endl;

        // 构建反向映射：tag -> set<artist>
        std::unordered_map<std::string, std::unordered_set<std::string>> tagArtists;
        for (const auto& aEntry : artistTags) {
            const std::string& a = aEntry.first;
            for (const auto& tEntry : aEntry.second) {
                const std::string& t = tEntry.first;
                tagArtists[t].insert(a);
            }
        }

        std::cout << "num of tags: " << tagArtists.size() << std::endl;

        // Step 3: 读取 user_friends.dat → 构建 friend 边（后续赋权）
        std::vector<std::pair<std::string, std::string>> rawFriendEdges; // 保留原始方向
        std::ifstream fin_uf(file_user_friends);
        if (!fin_uf.is_open()) {
            std::cerr << "ERROR: Cannot open user_friends.dat: " << std::endl;
            return;
        }
        while (getline(fin_uf, line)) {
            if (line.empty() || line[0] == 'u') continue;
            replace(line.begin(), line.end(), '\t', ' ');
            std::istringstream iss(line);
            std::string uid, fid;
            if (!(iss >> uid >> fid)) continue;
            std::string u = "U" + uid;
            std::string f = "U" + fid;
            rawFriendEdges.emplace_back(u, f);
        }
        fin_uf.close();

        // Step 4: 生成所有交易
        std::vector<transaction> transactions;

        // 4.1 listen 边: U -> A
        for (const auto& uEntry : userArtists) {
            const std::string& u = uEntry.first;
            for (const auto& aEntry : uEntry.second) {
                const std::string& a = aEntry.first;
                int w = aEntry.second;
                transactions.emplace_back(u, a, "listen", w);
            }
        }
        // 4.2 has_tag 边: A -> T
        for (const auto& aEntry : artistTags) {
            const std::string& a = aEntry.first;
            for (const auto& tEntry : aEntry.second) {
                const std::string& t = tEntry.first;
                int w = tEntry.second;
                transactions.emplace_back(a, t, "hastag", w);
            }
        }
        // 4.3 friend 边: U -> U (权重 = 共同艺术家数量)
        for (const auto& edge : rawFriendEdges) {
            const std::string& u = edge.first;
            const std::string& v = edge.second;
            int common = 0;
            if (userArtistSet.count(u) && userArtistSet.count(v)) {
                const auto& artistsU = userArtistSet.at(u);
                const auto& artistsV = userArtistSet.at(v);
                for (const std::string& a : artistsU) {
                    if (artistsV.count(a)) {
                        common++;
                    }
                }
            }
            // 即使 common=0，也保留边（权重=1）
            transactions.emplace_back(u, v, "friend", common + 1);
        }
        // 4.4 interest 边: U -> T (权重 = 用户对所有被打上此标签的艺术家的收听次数之和)
        for (const auto& uEntry : userArtists) {
            const std::string& u = uEntry.first;
            const auto& artistListenMap = uEntry.second; // Aid -> listen_count
            std::unordered_map<std::string, int> tagTotalWeight; // Tz -> total listen count
            // 遍历该用户收听的每个艺术家
            for (const auto& aEntry : artistListenMap) {
                const std::string& a = aEntry.first;
                int listenCount = aEntry.second;
                // 如果该艺术家有标签
                if (artistTags.count(a)) {
                    for (const auto& tEntry : artistTags.at(a)) {
                        const std::string& t = tEntry.first;
                        // 累加：该用户对这个艺术家的收听次数
                        tagTotalWeight[t] += listenCount;
                    }
                }
            }
            // 输出 U -> T 边
            for (const auto& tw : tagTotalWeight) {
                const std::string& t = tw.first;
                int total = tw.second;
                transactions.emplace_back(u, t, "interest", total);
            }
        }

        // 根据复合键对交易分segment，然后shuffle
        std::map<std::string, std::queue<transaction>> multimap;       // 储存复合键->对应交易
        for(transaction& tx : transactions){
            std::string kw = tx.u + tx.type;
            if(multimap.find(kw)==multimap.end()){
                multimap[kw] = std::queue<transaction>();
            }
            multimap[kw].push(tx);
        }
        std::vector<std::vector<transaction>> segments;
        for(std::map<std::string, std::queue<transaction>>::iterator it = multimap.begin(); it!=multimap.end(); it++){
            std::string kw = it -> first;
            std::queue<transaction> txs = it->second;
            while(!txs.empty()) {
                std::vector<transaction> segment;
                for(size_t i=0; i<seg_size; i++) {
                    segment.push_back(txs.front());
                    txs.pop();
                    // 若当前txs内元素不足seg_size且集合已空，提前退出
                    if(txs.empty()){
                        break;
                    }
                }
                // 将segment加入segments
                segments.push_back(segment);
            }
        }
        // 对segments进行shuffle
        std::default_random_engine rng(65537);
        std::shuffle(segments.begin(), segments.end(), rng);

        // 将交易写入文件        
        std::ofstream outFile(target_file);     // 打开输出文件流
        if (!outFile) {
            std::cout << "Error opening file for writing" << std::endl;
            return;
        }
        for(std::vector<transaction>& segment : segments) {
            for(transaction& tx : segment) {
                outFile << hash_string_to_24bit(tx.u) << ' '<< hash_string_to_24bit(tx.v) << ' ' << tx.type << ' ' << tx.w << '\n';
            }
        }
        outFile.close();
    }




    /*
        对paysim数据集进行统计分析，寻找对应交易数最多的复合键
    */
    static void stat_dataset(std::string filename) {
        std::ifstream file(filename);  // 打开文件
        std::string line;
        std::map<std::string, int> m;

        if (!file.is_open()) {
            std::cout << "can not open file" << std::endl;
            return;
        }

        // 逐行读取文件
        int txs_cnt = 0;
        while (std::getline(file, line)) {
            // 跳过以 # 开头的行
            if (line.empty() || line[0] == '#') {
                continue;
            }

            txs_cnt += 1;
            
            std::istringstream iss(line);
            std::string u, v, type;
            int w;
            
            // 读取每一行中的两个整数
            iss >> u >> v >> type >> w;
            std::string key = u + type;
            if(m.find(key)==m.end()){
                m[key] = 0;
            }
            m[key] += 1;
        }
        
        file.close();  // 关闭文件

        std::cout << "count of transactions: " << txs_cnt << std::endl;

        // 将统计信息写入文件
        std::ofstream outputfile("../../dataset/stat.txt");

        for(std::map<std::string, int>::iterator it = m.begin(); it!=m.end(); it++){
            outputfile << it->first << " " << it->second <<std::endl;
        }

        // 寻找对应最多交易的复合键
        int max_amount = 0;
        std::string max_key;
        for(std::map<std::string, int>::iterator it = m.begin(); it!=m.end(); it++) {
            if(it->second > max_amount){
                max_key = it->first;
                max_amount = it->second;
            }
        }
        std::cout << max_key << " " << max_amount << std::endl;

        return;
    }
};






#endif