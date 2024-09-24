#ifndef BIGINTEGER_H
#define BIGINTEGER_H

#include <string>
#include <openssl/bn.h>

/*
    大整数类，基于openssl的BIGNUM
*/
class BigInteger
{
public:
    /*
        创建空的BIGNUM对象
    */
    BigInteger();

    /*
        使用字符串创建BIGNUM，字符串为数值十进制形式的字面量

        param:
            s:一个字符串，如"21837182"，"-1231298319"
    */
    BigInteger(std::string s);

    /*
        使用较小的正整数创建BIGNUM

        param:
            x：一个正整数(unsigned long)
    */
    BigInteger(BN_ULONG x);


    /*
        将二进制的小端序字节数组转换为BIGNUM

        param:
            s：字节数组
            len：字节数组的长度
    */
    BigInteger(unsigned char* s,int len);


    /*
        使用字符串更新BIGNUM，使之修改为新的值
    */
    void reset(std::string s);

    /*
        使用较小的正整数更新BIGNUM，使之修改为新的值
    */
    void reset(BN_ULONG x);


    /*
        析构函数，销毁BIGNUM对象
    */
    ~BigInteger();

    // 二元运算，后续考虑使用运算符重载进行优化

    /*
        估计整数转换为十进制表示形式的位数，如十进制数"128378"的位数为6，十进制数"-12345"的位数为5
        估计方法：转换为对应的10进制字符串，然后计算字符串长度

        return:
            位数
    */
    int estimate_dec_len();

    /*
        将数值转换为十进制字符串
    */
    std::string to_string();


    /*
        判断数值是否为0
    */
    bool is_zero();


    /*
        比较2个数

        return:
            a < b，返回-1
            a = b，返回0
            a > b，返回1
    */
    static int cmp(BigInteger& a,BigInteger& b);
    





    /*
        加运算，r=a+b

        param:
            r:需要预先构造，作为参数传入。r的值可以为a或b，此时运算为a=a+b或b=a+b
    */
    static void add(BigInteger &r, const BigInteger &a, const BigInteger &b);

    /*
        减运算,r=a-b

        param:
            r:需要预先构造，作为参数传入。r的值可以为a或b，此时运算为a=a-b或b=a-b
    */
    static void sub(BigInteger &r, const BigInteger &a, const BigInteger &b);

    /*
        乘运算,r=a*b

        param:
            r:需要预先构造，作为参数传入。r的值可以为a或b，此时运算为a=a*b或b=a*b
    */
    static void mul(BigInteger &r, const BigInteger &a, const BigInteger &b);

    /*
        除运算,r=a/b,rem=a%b

        param:
            r:需要预先构造，作为参数传入。r的值可以为a或b，此时运算为a=a/b或b=a/b

    */
    static void div(BigInteger &r, BigInteger &rem, const BigInteger &a, const BigInteger &b);

    /*
        平方,r=a^2

        param:
            r:需要预先构造，作为参数传入。
    */
    static void sqr(BigInteger &r, const BigInteger &a);

    /*
        幂运算，r=a^p

        param:
            r:需要预先构造，作为参数传入。
    */
    static void exp(BigInteger &r, const BigInteger &a, const BigInteger &p);

    /*
        取模,rem=a%m
    */
    static void mod(BigInteger &rem, const BigInteger &a, const BigInteger &m);

    /*
        模加，r=(a+b) mod m

    */
    static void mod_add(BigInteger& r,BigInteger& a,BigInteger& b,const BigInteger& m);


    /*
        模减, r=(a-b) mod m
    */
    static void mod_sub(BigInteger& r,BigInteger& a,BigInteger& b,const BigInteger& m);


    /*
        模乘,r=(a*b) mod m
    */
    static void mod_mul(BigInteger& r,BigInteger& a,BigInteger& b,const BigInteger& m);

    /*
        模平方, r=(a^2) mod m
    */
    static void mod_sqr(BigInteger& r,BigInteger& a,const BigInteger& m);


    /*
        模幂运算 r=(a^p) mod m。

        注意，若指数p为负数，那么计算法则为：先求a的逆元a_inv，然后计算(a_inv)^(-p) mod m
    */
    static void mod_exp(BigInteger& r,BigInteger& a,BigInteger& p,const BigInteger& m);


    /*
        计算最大公因数
    */
    static void gcd(BigInteger& r,BigInteger& a,BigInteger& b);


    /*
        给定a,b 使用拓展欧几里得定理计算方程 ax+by=gcd(a,b)
    */
    static void xgcd(BigInteger& a,BigInteger& b,BigInteger& x,BigInteger& y);


    /*
        求逆元 (a*r) mod m==1
    */
    static void mod_inverse(BigInteger& r,BigInteger& a,const BigInteger& m);
    

    /*
        生成随机的素数

        param:
            ret：生成的素数
            bits：素数位数
            safe：若为1，则生成的是安全的素数，即(p-1)/2也是素数
    */
    static void generate_prime(BigInteger& ret,int bits,int safe);


    /*
        判断p是否为质数
        
        return:
            若是质数，返回true；否则返回false
    */
    static bool is_prime(const BigInteger& p);


    /*
        将字符串s转换为一个素数
        具体操作为先v=Hash(s)。若v不是素数，那么递增v直到得到一个素数

        param:
            s：字符串
    */
    static void generate_prime(BigInteger& ret,std::string s);

private:
    BIGNUM *bi;
};

#endif