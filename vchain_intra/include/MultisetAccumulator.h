#ifndef MULTISETACCUMULATOR_H
#define MULTISETACCUMULATOR_H

#include <pbc/pbc.h>
#include<string>
#include <set>



/*
    储存不相交证明。
*/
struct Nonmembership_Proof{
    // a
    std::string a;
    // d=g^b mod N
    std::string d;
};


/*
    RSA累加器
*/
class MultisetAccumulator{
public:
    MultisetAccumulator();

    /*
        对于给定的关键字集合，生成ACC

        param:
            w_set: 关键字的明文集合

        return:
            acc的十进制字符串形式
    */
    std::string get_acc(std::set<std::string> w_set);


    /*
        给定素数集合，计算对应的ACC

        param:
            x_set - 储存若干素数
    */
    std::string get_acc_prime(std::set<std::string> x_set);

    /*
        对于给定的x_p，生成ACC

        param:
            x_p: 集合对应的product

        return:
            acc的十进制字符串形式
    */
    std::string get_acc(std::string x_p);


    /*
        对于给定的关键字集合，计算对应素数的乘积

        param:
            w_set: 关键字的明文集合

        return:
            对应素数的乘积
    */
    std::string get_product(std::set<std::string> w_set);


    /*
        证明某关键字w不存在于某个集合(不存在证明)。输入为w的明文。

        param:
            product:集合对应素数的乘积，十进制字符串形式
            x:关键字的明文
    */
    struct Nonmembership_Proof proove_nonmembership(std::string product,std::string w);



    /*
        证明某关键字w不存在于某个集合(不存在证明)。输入为w对应的素数

        param:
            product:集合对应素数的乘积，十进制字符串形式
            x:关键字对应的素数
    */
    struct Nonmembership_Proof proove_nonmembership_prime(std::string product,std::string x);


    /*
        给定从服务器获取的acc，不相交证明pi，验证关键字w不属于集合

        param:
            acc:
            pi:
            w:
        return:
            若w不属于集合，返回true。否则返回false
    */
    bool verify_nonmembership(std::string acc,struct Nonmembership_Proof pi,std::string w);



    /*
        给定从服务器获取的acc，不相交证明pi，验证关键字w不属于集合。输入为w对应的素数

        param:
            acc:
            pi:
            x_s: w对应的素数
        return:
            若w不属于集合，返回true。否则返回false
    */
    bool verify_nonmembership_prime(std::string acc,struct Nonmembership_Proof pi,std::string x_s);



    /*
        使用欧拉降幂公式计算 a^x mod N

        param:
            a,x:十进制字符串
        
        return:
            结果，一个十进制字符串
    */
    std::string mod_exp_Euler(std::string a,std::string x);





private:
    // 安全的素数P
    std::string P;

    // 安全的素数Q
    std::string Q;

    // 模数N=PQ
    std::string N;

    // 生成元g
    unsigned long g;
};


#endif