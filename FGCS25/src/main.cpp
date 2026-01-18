#include "../include/experiment.h"

int main(void) {
    std::vector<int> tw_size = {5000, 10000, 15000, 20000, 25000};
    experiment::test_query("../../dataset/dataset4.txt", 200, tw_size, "C1231006815", "PAYMENT");
}