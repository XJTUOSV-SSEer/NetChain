#include "process_paysim.hpp"
#include <iostream>

int main(void) {
    std::string filename = "../paysim.csv";
    std::string target_file = "../dataset4.txt";
    size_t seg_size = 8;
    process_paysim::process_paysim_dataset(filename, target_file, seg_size);
}