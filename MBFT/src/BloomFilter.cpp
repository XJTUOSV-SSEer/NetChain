#include "../include/BloomFilter.h"
#include "../include/MurmurHash3.h"

// credited to http://blog.michaelschmatz.com/2016/04/11/how-to-write-a-bloom-filter-cpp/
// https://github.com/aappleby/smhasher/blob/master/src/MurmurHash3.h

// https://hur.st/bloomfilter/?n=100000&p=1.0E-6&m=&k=13
// https://drewdevault.com/2016/04/12/How-to-write-a-better-bloom-filter-in-C.html

BloomFilter::BloomFilter(int size, int numHashes)
{
    this->m_bits.resize(size);
    this->m_numHashes = numHashes;
}

void BloomFilter::add(const unsigned char* data, int len)
{
    auto hashValues = hash((uint8_t*)data, (std::size_t)len);

    for (int n = 0; n < m_numHashes; n++)
    {
        m_bits[nthHash(n, (int)hashValues[0], (int)hashValues[1], m_bits.size())] = true;
    }
}


void BloomFilter::add(std::string data)
{
    this->add((unsigned char*)data.c_str(), data.length());
}

bool BloomFilter::check(const unsigned char* data, int len)
{
    std::array<uint64_t, 2> hashValues = hash((uint8_t*)data, (std::size_t)len);

    for (int n = 0; n < m_numHashes; n++)
    {
        // 只要有一个位置为0，就是阴性
        if (!m_bits[nthHash(n, (int)hashValues[0], (int)hashValues[1], m_bits.size())])
        {
            return false;
        }
    }

    return true;
}


bool BloomFilter::check(std::string data){
    return this->check((unsigned char*)data.c_str(), data.length());
}

std::array<uint64_t, 2> BloomFilter::hash(const uint8_t *data, std::size_t len)
{
    std::array<uint64_t, 2> hashValue;
    MurmurHash3_x64_128(data, len, 0, hashValue.data());
    return hashValue;
}

inline int BloomFilter::nthHash(int n, int hashA, int hashB, int filterSize)
{
    return (hashA + n * hashB) % filterSize;
}



std::string BloomFilter::bin_form(std::string& s)
{
    size_t n_bits = m_bits.size();
    size_t n_bytes = (n_bits + 7) / 8; // 向上取整除以 8
    std::vector<unsigned char> result(n_bytes, 0);

    for (size_t i = 0; i < n_bits; ++i) {
        if (m_bits[i]) {
            size_t byte_idx = i / 8;
            size_t bit_idx  = i % 8;
            result[byte_idx] |= (1U << bit_idx); // LSB 为第 0 位（小端序：bit0=LSB）
        }
    }

    // 封装为string容器
    s.assign(reinterpret_cast<const char*>(result.data()), result.size());
    return s;
}