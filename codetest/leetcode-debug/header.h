#include <algorithm>
#include <vector>
#include <string>
using namespace std;

class Solution {
public:
    bool checkValidString(string s) {
        int low = 0;
        int high= 0;

        for(int i=0;i<s.length();i++) {
            if (s[i]=='(') {
                low++;
                high++;
            }
            else if (s[i]==')') {
                high--;
                low--;
            }
            else if (s[i]=='*') {
                low--;
                high++; //左
            }
        }

        if (high<0)
            return false;

        return true;
    }
};


