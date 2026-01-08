#ifndef MBFT_H
#define MBFT_H
#include <vector>
#include <set>
#include <string>
#include "./MBF.h"


/* MBFT的结点 */
class MBFT_Node{
public:
    std::set<std::string> w_set;        // 储存u, type两个关键字
    MBF mbf;
    double l;
    double u;
    std::string digest;                 // 哈希摘要

    /*
        构造函数
    */
    MBFT_Node(std::set<std::string> w_set, double l, double u, std::string lhash, std::string rhash, size_t m_, size_t k_);


    /*
        将 double 转换为字节数组（即其内存表示），再封装为 std::string
    */
    static std::string double_to_string_bytes(double value);
};


class MBFT{
public:


};



#endif