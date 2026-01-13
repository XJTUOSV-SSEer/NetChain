#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H
#include <vector>
#include <stdint.h>
#include <array>

class BloomFilter{
    public:
        // BF长度
        size_t m_;
        // 哈希函数族的大小
        size_t k_;
        // BF数组
        std::vector<bool> bits_;

        /*
            对data调用murmurhash3，返回哈希值（一个std::array，包含hashA和hashB两个随机哈希）
        */
        static std::array<uint64_t, 2> hash(const uint8_t *data,std::size_t len);

        /*
            计算(hashA + n * hashB)，等价于一个哈希函数族
        */
        static size_t nthHash(size_t n, size_t hashA, size_t hashB);


        /* 
            构造函数
            m_：BF数组的size
            k_：哈希函数族的大小
        */
        BloomFilter(size_t m_, size_t k_);

        BloomFilter();

        /*
            data:要插入的数据
            len: data的字节数
        */
        void add(const unsigned char* data, size_t len);


        /*
            data: 一个字符串
        */
        void add(std::string data);

        /*
            verify
            len:data的字节数
        */
        bool check(const unsigned char* data, size_t len);


        /*
            verify（以字符串的形式）
        */
        bool check(std::string data);


        /*
            将布隆过滤器数组表示为二进制01串形式，然后将01串写入字节数组并封装为string
            用于计算哈希摘要
        */
        std::string bin_form();
       
        

};
 
#endif