#include "../include/MBFT.h"
#include "../include/MBF.h"
#include "../include/Crypto_Primitives.h"


MBFT_Node::MBFT_Node(std::set<std::string> w_set, double l, double u, std::string lhash, std::string rhash, size_t m_, size_t k_):
w_set(w_set), l(l), u(u), mbf(m_, k_)
{
    // 计算digest=H(lhash||rhash||mbf||l||u)
    std::string mbf_bytes = mbf.bin_form();
    std::string l_bytes = double_to_string_bytes(l);
    std::string r_bytes = double_to_string_bytes(u);
    this->digest = Crypto_Primitives::SHA256_digest(lhash+rhash+mbf_bytes+l_bytes+r_bytes);
}



std::string MBFT_Node::double_to_string_bytes(double value){
    std::string result(8, '\0'); // 构造 8 字节字符串，初始化为 0
    std::memcpy(&result[0], &value, sizeof(double));
    return result;
}