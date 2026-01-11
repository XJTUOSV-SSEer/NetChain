#ifndef MBF_H
#define MBF_H

#include "MurmurHash3.h"
#include <vector>
#include <string>
#include <cmath>
#include <array>
#include <set>



class MBF{
public:
    // double false_positive_rate_;  // Target false positive rate (ε)
    // size_t expected_elements_;    // Expected number of elements (n)
    size_t m_;                    // 对应的朴素布隆过滤器的长度（位数）
    size_t k_;                    // 哈希函数的数量
    size_t len_h_;                // 动态哈希的选取位数
    std::vector<bool> bits_;      // MBF，长度为2*m_

    MBF(size_t m_, size_t k_, size_t len_h_);


    MBF();

    /*
        将一个关键字集合插入MBF
    */
    void add(const std::set<std::string>& w_set);


    /*
        判断一个关键字是否属于MBF
    */
    bool check(const std::string& w);


    /*
        生成MBF对应的hint。具体地，将MBF中非全0的slot加入hint。
    */
    std::vector<bool> gen_hint();



    /*
        给定两个MBF与相应的hint，合并得到新的MBF。
        要求2个给定的MBF长度相等
    */
   void merge(MBF& mbf1, MBF& mbf2, std::vector<bool>& hint);



   /*
        将布隆过滤器数组表示为二进制01串形式，然后将01串写入字节数组并封装为string
        用于计算哈希摘要
    */
    std::string bin_form();



    /*
        对data调用murmurhash3，返回哈希值（一个std::array，包含hashA和hashB两个随机哈希）
    */
    static std::array<uint64_t, 2> hash(const uint8_t *data,std::size_t len);

    /*
        计算(hashA + n * hashB)，等价于一个哈希函数族
    */
    static size_t nthHash(size_t n, size_t hashA, size_t hashB);

    /*
        给定假阳性率和需要插入的元素个数，计算朴素布隆过滤器的最佳长度和哈希函数的数量
        input:
            false_positive_rate_ - 假阳性率
            expected_elements_ - 插入元素个数
            m_ - BF长度
            k_ - 哈希函数数量
    */
    static void cal_pram(double false_positive_rate_, size_t expected_elements_, size_t& m_, size_t& k_);
};





#endif