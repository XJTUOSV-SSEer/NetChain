#include "../include/BigInteger.h"
#include "../include/Crypto_Primitives.h"
#include <iostream>
#include <cstdio>
#include <cstring>

BigInteger::BigInteger()
{
    this->bi = BN_new();
}

BigInteger::BigInteger(std::string s)
{
    this->bi = BN_new();
    int flag = BN_dec2bn(&(this->bi), s.c_str());
    if (flag == 0)
    {
        std::cout << "convert error" << std::endl;
    }
}

BigInteger::BigInteger(BN_ULONG x)
{
    this->bi = BN_new();
    int flag = BN_set_word(this->bi, x);
    if (flag == 0)
    {
        std::cout << "convert error" << std::endl;
    }
}

BigInteger::BigInteger(unsigned char* s,int len){
    this->bi = BN_new();
    BIGNUM* flag=BN_lebin2bn(s,len,this->bi);
    if(flag==NULL){
        std::cout << "convert error" << std::endl;
    }
}


void BigInteger::reset(std::string s)
{
    int flag = BN_dec2bn(&(this->bi), s.c_str());
    if (flag == 0)
    {
        std::cout << "convert error" << std::endl;
    }
}

void BigInteger::reset(BN_ULONG x)
{
    int flag = BN_set_word(this->bi, x);
    if (flag == 0)
    {
        std::cout << "reset error" << std::endl;
    }
}

bool BigInteger::is_zero()
{
    int flag = BN_is_zero(bi);
    if (flag == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int BigInteger::cmp(BigInteger &a, BigInteger &b)
{
    return BN_cmp(a.bi, b.bi);
}

int BigInteger::estimate_dec_len()
{
    // 获取对应的十进制字符串
    char* dec_bytes = BN_bn2dec(this->bi);
    std::string s = std::string(dec_bytes);
    OPENSSL_free(dec_bytes);
    // 判断是否为负数
    if (s[0] != '-')
    {
        return s.length();
    }
    else
    {
        return s.length() - 1;
    }
}

std::string BigInteger::to_string()
{
    char* dec_bytes = BN_bn2dec(this->bi);
    std::string s = std::string(dec_bytes);
    OPENSSL_free(dec_bytes);
    return s;
}

BigInteger::~BigInteger()
{
    // BN_clear(this->bi);
    BN_free(this->bi);
}

void BigInteger::add(BigInteger &r, const BigInteger &a, const BigInteger &b)
{
    int flag = BN_add(r.bi, a.bi, b.bi);
    if (flag == 0)
    {
        std::cout << "add failed" << std::endl;
    }
}

void BigInteger::sub(BigInteger &r, const BigInteger &a, const BigInteger &b)
{
    int flag = BN_sub(r.bi, a.bi, b.bi);
    if (flag == 0)
    {
        std::cout << "sub failed" << std::endl;
    }
}

void BigInteger::mul(BigInteger &r, const BigInteger &a, const BigInteger &b)
{
    // 创建CTX变量
    BN_CTX *ctx;
    ctx = BN_CTX_new();
    if (!ctx)
    {
        std::cout << "create ctx failed" << std::endl;
    }

    int flag = BN_mul(r.bi, a.bi, b.bi, ctx);
    if (flag == 0)
    {
        std::cout << "sub failed" << std::endl;
    }

    BN_CTX_free(ctx);
}

void BigInteger::div(BigInteger &r, BigInteger &rem, const BigInteger &a, const BigInteger &b)
{
    // 创建CTX变量
    BN_CTX *ctx;
    ctx = BN_CTX_new();
    if (!ctx)
    {
        std::cout << "create ctx failed" << std::endl;
    }

    int flag = BN_div(r.bi, rem.bi, a.bi, b.bi, ctx);
    if (flag == 0)
    {
        std::cout << "sub failed" << std::endl;
    }

    BN_CTX_free(ctx);
}

void BigInteger::sqr(BigInteger &r, const BigInteger &a)
{
    // 创建CTX变量
    BN_CTX *ctx;
    ctx = BN_CTX_new();
    if (!ctx)
    {
        std::cout << "create ctx failed" << std::endl;
    }

    int flag = BN_sqr(r.bi, a.bi, ctx);
    if (flag == 0)
    {
        std::cout << "sub failed" << std::endl;
    }

    BN_CTX_free(ctx);
}

void BigInteger::mod(BigInteger &rem, const BigInteger &a, const BigInteger &m)
{
    // 创建CTX变量
    BN_CTX *ctx;
    ctx = BN_CTX_new();
    if (!ctx)
    {
        std::cout << "create ctx failed" << std::endl;
    }

    int flag = BN_mod(rem.bi, a.bi, m.bi, ctx);
    if (flag == 0)
    {
        std::cout << "sub failed" << std::endl;
    }

    BN_CTX_free(ctx);
}

void BigInteger::exp(BigInteger &r, const BigInteger &a, const BigInteger &p)
{
    // 创建CTX变量
    BN_CTX *ctx;
    ctx = BN_CTX_new();
    if (!ctx)
    {
        std::cout << "create ctx failed" << std::endl;
    }

    int flag = BN_exp(r.bi, a.bi, p.bi, ctx);
    if (flag == 0)
    {
        std::cout << "sub failed" << std::endl;
    }

    BN_CTX_free(ctx);
}

void BigInteger::mod_add(BigInteger &r, BigInteger &a, BigInteger &b, const BigInteger &m)
{
    // 创建CTX变量
    BN_CTX *ctx;
    ctx = BN_CTX_new();
    if (!ctx)
    {
        std::cout << "create ctx failed" << std::endl;
    }

    int flag = BN_mod_add(r.bi, a.bi, b.bi, m.bi, ctx);
    if (flag == 0)
    {
        std::cout << "sub failed" << std::endl;
    }

    BN_CTX_free(ctx);
}

void BigInteger::mod_sub(BigInteger &r, BigInteger &a, BigInteger &b, const BigInteger &m)
{
    // 创建CTX变量
    BN_CTX *ctx;
    ctx = BN_CTX_new();
    if (!ctx)
    {
        std::cout << "create ctx failed" << std::endl;
    }

    int flag = BN_mod_sub(r.bi, a.bi, b.bi, m.bi, ctx);
    if (flag == 0)
    {
        std::cout << "sub failed" << std::endl;
    }

    BN_CTX_free(ctx);
}

void BigInteger::mod_mul(BigInteger &r, BigInteger &a, BigInteger &b, const BigInteger &m)
{
    // 创建CTX变量
    BN_CTX *ctx;
    ctx = BN_CTX_new();
    if (!ctx)
    {
        std::cout << "create ctx failed" << std::endl;
    }

    int flag = BN_mod_mul(r.bi, a.bi, b.bi, m.bi, ctx);
    if (flag == 0)
    {
        std::cout << "sub failed" << std::endl;
    }

    BN_CTX_free(ctx);
}

void BigInteger::mod_sqr(BigInteger &r, BigInteger &a, const BigInteger &m)
{
    // 创建CTX变量
    BN_CTX *ctx;
    ctx = BN_CTX_new();
    if (!ctx)
    {
        std::cout << "create ctx failed" << std::endl;
    }

    int flag = BN_mod_sqr(r.bi, a.bi, m.bi, ctx);
    if (flag == 0)
    {
        std::cout << "sub failed" << std::endl;
    }

    BN_CTX_free(ctx);
}

void BigInteger::mod_exp(BigInteger &r, BigInteger &a, BigInteger &p, const BigInteger &m)
{
    // 创建CTX变量
    BN_CTX *ctx;
    ctx = BN_CTX_new();
    if (!ctx)
    {
        std::cout << "create ctx failed" << std::endl;
    }

    BigInteger zero(0);

    // 指数大于等于0
    if (cmp(p, zero) != -1)
    {
        int flag = BN_mod_exp(r.bi, a.bi, p.bi, m.bi, ctx);
        if (flag == 0)
        {
            std::cout << "sub failed" << std::endl;
        }
    }
    else{
        // 求a的逆元
        BigInteger a_inv;
        mod_inverse(a_inv,a,m);

        // p取反
        BigInteger p_new;
        mul(p_new,p,BigInteger("-1"));
        
        int flag=BN_mod_exp(r.bi, a_inv.bi, p_new.bi, m.bi, ctx);
        if (flag == 0)
        {
            std::cout << "sub failed" << std::endl;
        }
    }

    BN_CTX_free(ctx);
}

void BigInteger::mod_inverse(BigInteger &r, BigInteger &a, const BigInteger &m)
{
    // 创建CTX变量
    BN_CTX *ctx;
    ctx = BN_CTX_new();
    if (!ctx)
    {
        std::cout << "create ctx failed" << std::endl;
    }

    BIGNUM *flag = BN_mod_inverse(r.bi, a.bi, m.bi, ctx);
    if (flag == NULL)
    {
        std::cout << "cal mod inverse failed" << std::endl;
    }

    BN_CTX_free(ctx);
}

void BigInteger::gcd(BigInteger &r, BigInteger &a, BigInteger &b)
{
    // 创建CTX变量
    BN_CTX *ctx;
    ctx = BN_CTX_new();
    if (!ctx)
    {
        std::cout << "create ctx failed" << std::endl;
    }

    int flag = BN_gcd(r.bi, a.bi, b.bi, ctx);
    if (flag == 0)
    {
        std::cout << "cal mod inverse failed" << std::endl;
    }

    BN_CTX_free(ctx);
}

void BigInteger::xgcd(BigInteger &a, BigInteger &b, BigInteger &x, BigInteger &y)
{
    // 创建CTX变量
    BN_CTX *ctx;
    ctx = BN_CTX_new();
    if (!ctx)
    {
        std::cout << "create ctx failed" << std::endl;
    }

    // 算法
    BigInteger x0(1);
    BigInteger x1(0);
    BigInteger y0(0);
    BigInteger y1(1);

    while (!b.is_zero())
    {
        // q储存a除以b的整数部分，rem储存 a mod b
        BigInteger q;
        BigInteger rem;
        div(q, rem, a, b);

        // 将a更新为b
        a.reset(b.to_string());

        // b更新为rem
        b.reset(rem.to_string());

        // x0=x1,x1=x0-q*x1
        BigInteger q_mul_x1;
        mul(q_mul_x1, q, x1);
        BigInteger tmp;
        sub(tmp, x0, q_mul_x1);
        x0.reset(x1.to_string());
        x1.reset(tmp.to_string());

        // y0=y1,y1=y0-q*y1
        BigInteger q_mul_y1;
        mul(q_mul_y1, q, y1);
        sub(tmp, y0, q_mul_y1);
        y0.reset(y1.to_string());
        y1.reset(tmp.to_string());
    }

    x.reset(x0.to_string());
    y.reset(y0.to_string());

    BN_CTX_free(ctx);
}

void BigInteger::generate_prime(BigInteger &ret, int bits, int safe)
{
    BN_generate_prime_ex(ret.bi, bits, safe, NULL, NULL, NULL);
}

bool BigInteger::is_prime(const BigInteger &p)
{
    // 创建CTX变量
    BN_CTX *ctx;
    ctx = BN_CTX_new();
    if (!ctx)
    {
        std::cout << "create ctx failed" << std::endl;
    }

    int flag = BN_is_prime_ex(p.bi, BN_prime_checks, ctx, NULL);

    BN_CTX_free(ctx);

    if (flag == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}



void BigInteger::generate_prime(BigInteger& ret,std::string s){
    int len=s.length();
    char* c=new char[len];

    Crypto_Primitives::string2char(s,c);

    // 调用SHA256求哈希
    char digest[32];
    int digest_len;
    Crypto_Primitives::SHA256_digest((unsigned char*)c,len,(unsigned char*)digest,(unsigned int*)&digest_len);

    // 将digest转换为BigInteger对象。将digest前4字节取出，并累增进行素性测试
    BigInteger num((unsigned char*)digest, 4);
    BigInteger zero(0);
    if(BigInteger::cmp(num,zero)==-1){
        BigInteger tmp("-1");
        BigInteger::mul(num,num,tmp);
    }

    // 素性测试
    BigInteger one(1);
    while(!is_prime(num)){
        BigInteger::add(num,num,one);
    }

    std::string str=num.to_string();
    ret.reset(str);

    delete[] c;
}