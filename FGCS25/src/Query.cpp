#include "../include/Query.h"


std::vector<Block> Query::Search(std::string u_q, std::string type_q, size_t lb, size_t ub, std::vector<Block>& chain){
    std::string kw_q = u_q + type_q;        // 查询复合键
}