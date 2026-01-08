#include "../include/MBF.h"
#include "../include/MurmurHash3.h"



void MBF::cal_pram(double false_positive_rate_, size_t expected_elements_, size_t& m_, size_t& k_){
    // 根据公式计算
    double ln2 = std::log(2.0);
    m_ = static_cast<size_t>(- (static_cast<double>(expected_elements_) * std::log(false_positive_rate_)) / (ln2 * ln2));
    k_ = static_cast<size_t>((static_cast<double>(m_) / expected_elements_) * ln2);
}


std::array<uint64_t, 2> MBF::hash(const uint8_t *data,std::size_t len)
{
    std::array<uint64_t, 2> hashValue;
    MurmurHash3_x64_128(data, len, 0, hashValue.data());
    return hashValue;
}

size_t MBF::nthHash(size_t n, size_t hashA, size_t hashB)
{
    return (hashA + n * hashB);
}


MBF::MBF(size_t m_, size_t k_) : m_(m_), k_(k_) {
    this->bits_.resize(2*m_, false);

    // 计算动态哈希位数，即log(m_)
    this->len_h_ = 0;
    size_t tmp = m_;
    while(tmp > 1){
        tmp >>= 1;
        this->len_h_ ++;
    }
}



void MBF::add(const std::set<std::string>& w_set){
    // 首先构造对应的朴素布隆过滤器。其中，记录每一位是否被多个哈希函数值命中，且各个哈希函数值的第len_h_+1位。
    std::vector<bool> bf;           // 朴素布隆过滤器
    bf.resize(m_, false);
    std::vector<int> record_;       // 记录命中一个位置的多个哈希函数第len_h_+1位的情况：若有0有1，记为3；
                                    // 若只有0，记为1；只有1，记为2
    record_.resize(m_, 0);
    for(const std::string& s : w_set){
        // 计算一个关键字s对应的k_个位置
        std::array<uint64_t, 2> hashValues = hash((uint8_t*)s.c_str(), s.length());
        for(size_t n=0; n<this->k_; n++){
            size_t h_ = nthHash(n, hashValues[0], hashValues[1]);    // s对应的第n个哈希值
            size_t pos = h_ % this->m_;                              // s对应的第n个位置
            bf[pos] = true;
            // 计算哈希值的第len_h_+1位：右移len_h_位，然后和1U做按位与运算。
            bool flag = (h_ >> this->len_h_) & 1U;
            if(!flag) {
                if(record_[pos] == 0){
                    record_[pos] = 1;
                }
                else if(record_[pos] == 2){
                    record_[pos] = 3;
                }
            }
            else{
                if(record_[pos] == 0){
                    record_[pos] = 2;
                }
                else if(record_[pos] == 1){
                    record_[pos] = 3;
                }
            }
        }
    }

    // 根据得到的朴素布隆过滤器和record_数组来构造MBF
    for(size_t i = 0; i < this->m_; i++){
        if(bf[i]){
            // opcode = 01
            if(record_[i] == 1){
                this->bits_[2*i] = false;
                this->bits_[2*i+1] = true;
            }
            // opcode = 10
            else if(record_[i] == 2){
                this->bits_[2*i] = true;
                this->bits_[2*i+1] = false;
            }
            // opcode = 11
            else if(record_[i] == 3){
                this->bits_[2*i] = true;
                this->bits_[2*i+1] = true;
            }
        }
    }
}



bool MBF::check(const std::string& w){
    std::array<uint64_t, 2> hashValues = hash((uint8_t*)w.c_str(), w.length());
    // 检查k_个位置
    for(size_t i = 0; i < this->k_; i++){
        size_t pos = nthHash(i, hashValues[0], hashValues[1]) % this->m_;
        if(!this->bits_[pos*2] && !this->bits_[pos*2+1]){   // 1个opcode中需要至少有1位是1
            return false;
        }    
    }
    return true;
}


std::vector<bool> MBF::gen_hint(){
    std::vector<bool> hint;
    for(size_t i = 0; i < this->m_; i++){
        if(this->bits_[i*2] ||this->bits_[i*2+1]){
            hint.push_back(this->bits_[i*2]);
            hint.push_back(this->bits_[i*2+1]);
        }
    }
    return hint;
}



void MBF::merge(MBF& mbf1, MBF& mbf2, std::vector<bool>& hint){
    // 设置合并后的MBF的参数
    this->m_ = mbf1.m_ * 2;
    this->k_ = mbf1.k_;
    this->len_h_ = mbf1.len_h_ + 1;
    this->bits_.resize(this->m_*2, false);

    // 将两个MBF按位异或
    std::vector<bool> tmp;
    tmp.resize(mbf1.bits_.size(), false);
    for(size_t i=0; i<mbf1.bits_.size(); i++){
        tmp[i] = mbf1.bits_[i] || mbf2.bits_[i];
    }

    // 按slot将其转换为朴素布隆过滤器
    std::vector<bool> bf;
    bf.resize(this->m_, false);
    for(size_t i=0; i<mbf1.m_; i++){
        if(!tmp[i*2] && !tmp[i*2+1]){
            continue;
        }
        // opcode = 01
        if(tmp[i*2]==false && tmp[i*2+1]==true){
            bf[i] = true;
        }
        // opcode = 10
        else if(tmp[i*2]==true && tmp[i*2+1]==false){
            bf[i+mbf1.m_] = true;
        }

        // opcode = 11
        else if(tmp[i*2]==true && tmp[i*2+1]==true){
            bf[i] = true;
            bf[i+mbf1.m_] = true;
        }
    }

    // 根据hint和朴素布隆过滤器重建MBF
    size_t j = 0;           // 指针，指示hint中已读取的slot
    for(size_t i=0; i<this->m_; i++){
        if(bf[i]){
            this->bits_[i*2] = hint[j*2];
            this->bits_[i*2+1] = hint[j*2+1];
            j++;
        }
    }
}



std::string MBF::bin_form()
{
    size_t n_bits = bits_.size();
    size_t n_bytes = (n_bits + 7) / 8; // 向上取整除以 8
    std::vector<unsigned char> result(n_bytes, 0);

    for (size_t i = 0; i < n_bits; ++i) {
        if (bits_[i]) {
            size_t byte_idx = i / 8;
            size_t bit_idx  = i % 8;
            result[byte_idx] |= (1U << bit_idx); // LSB 为第 0 位（小端序：bit0=LSB）
        }
    }

    // 封装为string容器
    std::string s;
    s.assign(reinterpret_cast<const char*>(result.data()), result.size());
    return s;
}
