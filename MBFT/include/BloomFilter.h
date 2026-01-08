#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H
#include <vector>
#include <stdint.h>
#include <array>

class BloomFilter{
    public:
        // 哈希函数族的大小
        int m_numHashes;
        // BF数组
        std::vector<bool> m_bits;

        /*
            对data调用murmurhash3，返回哈希值（一个std::array）
        */
        std::array<uint64_t, 2> hash(const uint8_t *data,std::size_t len);

        /*
            计算(hashA + n * hashB) % filterSize，等价于一个哈希函数族
        */
        int nthHash(int n, int hashA, int hashB, int filterSize);


        /* 
            构造函数
            size：BF数组的size
            numHashes：哈希函数族的大小
        */
        BloomFilter(int size, int numHashes);

        /*
            data:要插入的数据
            len: data的字节数
        */
        void add(const unsigned char* data, int len);


        /*
            data: 一个字符串
        */
        void add(std::string data);

        /*
            verify
            len:data的字节数
        */
        bool check(const unsigned char* data, int len);


        /*
            verify（以字符串的形式）
        */
        bool check(std::string data);


        /*
            将布隆过滤器数组表示为二进制01串形式，然后将01串写入字节数组并封装为string
            用于计算哈希摘要
        */
        std::string bin_form(std::string& s);
       
        

};
 
#endif