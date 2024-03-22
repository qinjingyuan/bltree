#include<iostream>
#include<bitset>


int main(){
    std::bitset<512> b("1000111101");
    std::bitset<256> c("1000111101");

    // b = b | b;


    std::cout << b << "\n";
    std::cout << (b << 256) << "\n";
    b << 256;
    std::cout << b << "\n";
    return 0;
}

