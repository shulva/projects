#include "header.h"
#include <iostream>
#include <vector>
using namespace std;
int main() {
    Solution s;
    vector<vector<int>> temp{{9,9,4},{6,6,8},{2,1,1}};
    s.longestIncreasingPath(temp);
}
