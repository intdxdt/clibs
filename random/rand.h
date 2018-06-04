#include <iostream>
#include <cmath>
#include <chrono>
#include <ctime>
#include <functional>
#include <iostream>
#include <random>


#ifndef RANDOM_RANDOM_H
#define RANDOM_RANDOM_H

struct URandom {
    std::default_random_engine gen;
    std::uniform_real_distribution<double> dist;

    explicit URandom(double i = 0, double j = 1.0) {
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        gen = std::default_random_engine(seed);
        dist = std::uniform_real_distribution<double>(i, j);
    }

    URandom(std::default_random_engine generator,
                std::uniform_real_distribution<double> distribution) :
            gen(generator), dist(distribution) {}

    double random() {
        return dist(gen);
    }
};

//auto gen(double i=0, double j=1.0) {
//    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
//    std::default_random_engine generator(seed);
//    std::uniform_real_distribution<double> distribution(i, j);
//   return   RandGen{generator, distribution};
//}

#endif //RANDOM_RANDOM_H
