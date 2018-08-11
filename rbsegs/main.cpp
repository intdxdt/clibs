#include <iostream>
#include <vector>
using namespace std;

int main() {
    std::vector<std::vector<int>> crossings;
    crossings.emplace_back(std::vector<int>{2,3});
    cout << crossings.size() << '\n';
    return 0;
}