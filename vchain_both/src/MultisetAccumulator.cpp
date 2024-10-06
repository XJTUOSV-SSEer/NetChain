#include<iostream>
#include "../include/MultisetAccumulator.h"
#include "../include/BigInteger.h"

#define P_BITS_LEN 128      // 素数P的位数
#define Q_BITS_LEN 128      // 素数Q的位数


MultisetAccumulator::MultisetAccumulator(){
    // 随机生成参数P,Q,N
    // 生成P,Q
    // BigInteger p,q;
    // BigInteger::generate_prime(p,P_BITS_LEN,1);
    // BigInteger::generate_prime(q,Q_BITS_LEN,1);
    // P=p.to_string();
    // Q=q.to_string();
    // 生成N=PQ
    // BigInteger n;
    // BigInteger::mul(n,p,q);
    // N=n.to_string();

    // 使用固定参数
    P="283086456303674575668400057000482309863";
    Q="326896810465200637570669519551882712907";
    N="92540059651567610024194486489383252364422717166295481558146780071654243501741";

    // g=65537
    g=65537;
    // g=13;


    // std::cout<<P<<std::endl<<Q<<std::endl<<N<<std::endl;
}




std::string MultisetAccumulator::get_product(std::set<std::string> w_set){
    // 素数的乘积
    BigInteger product(1);
    for(std::set<std::string>::iterator it=w_set.begin();it!=w_set.end();it++){
        std::string w=(*it);

        // 素数
        BigInteger p;
        BigInteger::generate_prime(p,w);
        // std::cout<<p.to_string()<<" "<<BigInteger::is_prime(p)<<std::endl;
        

        // 累乘
        BigInteger::mul(product,product,p);
    }

    return product.to_string();
}



std::string MultisetAccumulator::get_acc(std::set<std::string> w_set){
    // 素数的乘积
    BigInteger product(1);

    // phi(N)=(P-1)(Q-1)
    BigInteger phi;
    BigInteger one(1);
    BigInteger p(this->P);
    BigInteger p_minors_one;
    BigInteger::sub(p_minors_one,p,one);
    BigInteger q(this->Q);
    BigInteger q_minors_one;
    BigInteger::sub(q_minors_one,q,one);
    BigInteger::mul(phi,p_minors_one,q_minors_one);


    // 为每个关键字生成一个对应的素数
    for(std::set<std::string>::iterator it=w_set.begin();it!=w_set.end();it++){
        std::string w=(*it);

        // 素数
        BigInteger p;
        BigInteger::generate_prime(p,w);
        // std::cout<<BigInteger::is_prime(p)<<std::endl;

        // 累乘（使用欧拉降幂公式提速）
        BigInteger::mod_mul(product,p,product,phi);
    }

    // 计算acc=g^product mod N
    BigInteger acc;
    BigInteger generator(this->g);
    BigInteger n(this->N);
    BigInteger::mod_exp(acc,generator,product,n);

    return acc.to_string();
}



std::string MultisetAccumulator::get_acc_prime(std::set<std::string> x_set){
    // 素数的乘积
    BigInteger product(1);

    // phi(N)=(P-1)(Q-1)
    BigInteger phi;
    BigInteger one(1);
    BigInteger p(this->P);
    BigInteger p_minors_one;
    BigInteger::sub(p_minors_one,p,one);
    BigInteger q(this->Q);
    BigInteger q_minors_one;
    BigInteger::sub(q_minors_one,q,one);
    BigInteger::mul(phi,p_minors_one,q_minors_one);

    for(std::set<std::string>::iterator it=x_set.begin();it!=x_set.end();it++){
        std::string x_str=(*it);

        // 素数
        BigInteger p(x_str);
        // std::cout<<BigInteger::is_prime(p)<<std::endl;

        // 累乘（使用欧拉降幂公式提速）
        BigInteger::mod_mul(product,p,product,phi);
    }

    // 计算acc=g^product mod N
    BigInteger acc;
    BigInteger generator(this->g);
    BigInteger n(this->N);
    BigInteger::mod_exp(acc,generator,product,n);

    return acc.to_string();
}




struct Nonmembership_Proof MultisetAccumulator::proove_nonmembership(std::string product,std::string w){
    // 对关键字w求对应的素数x
    BigInteger x;
    BigInteger::generate_prime(x,w);
    // std::cout<<"x:"<<x.to_string()<<" "<<BigInteger::is_prime(x)<<std::endl;

    // 根据拓展欧几里得定理计算a*product+b*x=1的解a,b
    BigInteger a,b;
    BigInteger x_p(product);
    BigInteger::xgcd(x_p,x,a,b);

    // std::cout<<"a:"<<a.to_string()<<" b:"<<b.to_string()<<std::endl;

    // 计算证明pi
    struct Nonmembership_Proof pi;
    pi.a=a.to_string();

    BigInteger generator(this->g);
    BigInteger n(this->N);

    // d=g^b mod N，使用欧拉降幂公式加速
    BigInteger d;
    // BigInteger::mod_exp(d,generator,b,n);
    // pi.d=d.to_string();
    pi.d=mod_exp_Euler(generator.to_string(),b.to_string());    

    return pi;
}



struct Nonmembership_Proof MultisetAccumulator::proove_nonmembership_prime(std::string product,std::string x_s){
    BigInteger x(x_s);
    BigInteger x_p(product);
    struct Nonmembership_Proof pi;

    // 判断，若 x_p%x == 0 返回
    BigInteger tmp;
    BigInteger::mod(tmp, x_p, x);
    if(tmp.is_zero()){
        pi.a = "0";
        pi.d = "0";
        return pi;
    }

    // 根据拓展欧几里得定理计算a*product+b*x=1的解a,b
    BigInteger a,b;    
    BigInteger::xgcd(x_p,x,a,b);

    // std::cout<<"a:"<<a.to_string()<<" b:"<<b.to_string()<<std::endl;

    // 计算证明pi    
    pi.a=a.to_string();

    BigInteger generator(this->g);
    BigInteger n(this->N);

    // d=g^b mod N，使用欧拉降幂公式加速
    BigInteger d;
    // BigInteger::mod_exp(d,generator,b,n);
    // pi.d=d.to_string();
    pi.d=mod_exp_Euler(generator.to_string(),b.to_string());    

    return pi;
}



bool MultisetAccumulator::verify_nonmembership(std::string acc,struct Nonmembership_Proof pi,std::string w){
    BigInteger bi_N(this->N);
    BigInteger bi_g(this->g);

    // 计算w对应的素数x
    BigInteger x;
    BigInteger::generate_prime(x,w);

    // 计算acc^a
    std::string str1=mod_exp_Euler(acc,pi.a);

    // 计算d^x
    std::string str2=mod_exp_Euler(pi.d,x.to_string());

    // 判断 acc^a times d^x mod N==g?
    BigInteger n1(str1);
    BigInteger n2(str2);
    BigInteger left;
    BigInteger::mod_mul(left,n1,n2,bi_N);

    return BigInteger::cmp(left,bi_g)==0;

}




bool MultisetAccumulator::verify_nonmembership_prime(std::string acc,struct Nonmembership_Proof pi,std::string x_s){
    BigInteger bi_N(this->N);
    BigInteger bi_g(this->g);

    // 计算w对应的素数x
    BigInteger x(x_s);

    // 计算acc^a
    std::string str1=mod_exp_Euler(acc,pi.a);

    // 计算d^x
    std::string str2=mod_exp_Euler(pi.d,x.to_string());

    // 判断 acc^a times d^x mod N==g?
    BigInteger n1(str1);
    BigInteger n2(str2);
    BigInteger left;
    BigInteger::mod_mul(left,n1,n2,bi_N);

    return BigInteger::cmp(left,bi_g)==0;
}



std::string MultisetAccumulator::mod_exp_Euler(std::string a,std::string x){
    // 计算phi(N)
    // phi(N)=(P-1)(Q-1)
    BigInteger phi;
    BigInteger one(1);
    BigInteger p(this->P);
    BigInteger p_minors_one;
    BigInteger::sub(p_minors_one,p,one);
    BigInteger q(this->Q);
    BigInteger q_minors_one;
    BigInteger::sub(q_minors_one,q,one);
    BigInteger::mul(phi,p_minors_one,q_minors_one);

    BigInteger bi_a(a);
    BigInteger bi_x(x);
    BigInteger bi_N(this->N);

    // 判断指数x是否为负数，是的话求a的逆元，x乘-1
    BigInteger zero(0);
    if(BigInteger::cmp(bi_x,zero)==-1){
        BigInteger::mod_inverse(bi_a,bi_a,bi_N);
        BigInteger tmp("-1");
        BigInteger::mul(bi_x,bi_x,tmp);
    }

    // 使用欧拉降幂公式计算幂
    BigInteger::mod(bi_x,bi_x,phi);
    BigInteger ret;
    BigInteger::mod_exp(ret,bi_a,bi_x,bi_N);

    return ret.to_string();
}



std::string MultisetAccumulator::get_acc(std::string x_p){
    // 计算acc=g^product mod N
    return mod_exp_Euler(std::to_string(this->g),x_p);
}