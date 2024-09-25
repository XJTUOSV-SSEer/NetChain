#include "../include/Crypto_Primitives.h"
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <iostream>
#include <cstring>

int Crypto_Primitives::sym_encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
                                   unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;

    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new()))
        std::cout << "create cipher ctx failed" << std::endl;

    /*
     * Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        std::cout << "encrypt init failed" << std::endl;

    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        std::cout << "encrypt update failed" << std::endl;
    ciphertext_len = len;

    /*
     * Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        std::cout << "encrypt final failed" << std::endl;
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int Crypto_Primitives::sym_decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
                                   unsigned char *iv, unsigned char *plaintext) {
    EVP_CIPHER_CTX *ctx;

    int len;

    int plaintext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        std::cout<<"create cipher ctx failed"<<std::endl;

    /*
     * Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        std::cout<<"decrypt init failed"<<std::endl;

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary.
     */
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        std::cout<<"decrypt update failed"<<std::endl;
    plaintext_len = len;

    /*
     * Finalise the decryption. Further plaintext bytes may be written at
     * this stage. 
     * 对于输入恰好是16字节的整数的情况，无需进行这一步
     */
    if(ciphertext_len%16!=0 && 1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        std::cout<<"decrypt final failed"<<std::endl;
    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}


std::string Crypto_Primitives::get_rand(int num){
    char* buf=new char[num];
    int rc = RAND_bytes((unsigned char*)buf, num);
    if(rc != 1) 
        std::cout<<"generate RAND_bytes failed"<<std::endl;
    return std::string(buf,num);
}


void Crypto_Primitives::get_prf(unsigned char* k,unsigned char* data,int len, unsigned char* v){
    // 使用一个固定的16字节IV
    std::string s1="XJTU-OSV-PRF--IV";
    // s1.resize(16);

    // 调用加密算法
    char c[16];
    string2char(s1,c);
    sym_encrypt(data,len,k,(unsigned char*)c,v);
}


void Crypto_Primitives::string_xor(char* a,char* b,int len,char* res){
    for(int i=0;i<len;i++){
        res[i]=(a[i]) xor (b[i]);
    }
}



void Crypto_Primitives::string2char(std::string str,char* c){
    int len=str.length();
    for(int i=0;i<len;i++){
        c[i]=str[i];
    }
}



void Crypto_Primitives::SHA512_digest(const unsigned char *message, size_t message_len, 
unsigned char* digest, unsigned int *digest_len)
{
    EVP_MD_CTX *mdctx;

    if ((mdctx = EVP_MD_CTX_new()) == NULL)
        std::cout<<"create ctx error"<<std::endl;

    if (1 != EVP_DigestInit_ex(mdctx, EVP_sha512(), NULL))
        std::cout<<"digest init error"  <<std::endl;

    if (1 != EVP_DigestUpdate(mdctx, message, message_len))
        std::cout<< "digest update error" <<std::endl;

    unsigned char* s;

    if ((s = (unsigned char *)OPENSSL_malloc(EVP_MD_size(EVP_sha512()))) == NULL)
        std::cout<< "memory alloc error" <<std::endl;

    if (1 != EVP_DigestFinal_ex(mdctx, s, digest_len))
        std::cout<< "digest final error" <<std::endl;
    
    // 复制
    memcpy(digest,s,*digest_len);
    // 释放内存
    OPENSSL_free(s);

    EVP_MD_CTX_free(mdctx);
}




void Crypto_Primitives::SHA256_digest(const unsigned char *message, size_t message_len, 
unsigned char* digest, unsigned int *digest_len)
{
    EVP_MD_CTX *mdctx;

    if ((mdctx = EVP_MD_CTX_new()) == NULL)
        std::cout<<"create ctx error"<<std::endl;

    if (1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL))
        std::cout<<"digest init error"  <<std::endl;

    if (1 != EVP_DigestUpdate(mdctx, message, message_len))
        std::cout<< "digest update error" <<std::endl;

    unsigned char* s;

    if ((s = (unsigned char *)OPENSSL_malloc(EVP_MD_size(EVP_sha256()))) == NULL)
        std::cout<< "memory alloc error" <<std::endl;

    if (1 != EVP_DigestFinal_ex(mdctx, s, digest_len))
        std::cout<< "digest final error" <<std::endl;
    
    // 复制
    memcpy(digest,s,*digest_len);
    // 释放内存
    OPENSSL_free(s);

    EVP_MD_CTX_free(mdctx);
}



std::string Crypto_Primitives::SHA256_digest(std::string message){
    char* msg_bytes = new char[message.length()];
    Crypto_Primitives::string2char(message, msg_bytes);
    char* digest_bytes = new char[32];
    int tmp;

    SHA256_digest((unsigned char*)msg_bytes, message.length(), (unsigned char*)digest_bytes, (unsigned int*)&tmp);

    // 将digest从字节形式转换为string
    std::string digest = std::string(digest_bytes, 32);

    // 释放内存
    delete[] msg_bytes;
    delete[] digest_bytes;

    return digest;
}