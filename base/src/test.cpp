#include <iostream>
#include "../include/Crypto_Primitives.h"


int main(){
    std::string s = Crypto_Primitives::SHA256_digest("zhg");
    for(int i=0; i<s.length(); i++){
        std::cout << int(s[i])<<std::endl;
    }
}