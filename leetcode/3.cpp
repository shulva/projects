//
// Created by shulva on 24-9-6.
//
#include <string>
using namespace std;

class Solution {
    int ascii [128];
public:
    int lengthOfLongestSubstring(string s) {
        for(int i = 0;i<128;i++)
            ascii[i] = 0 ;

        int head = 0;
        int max = 0;

        for(int i=0;i<s.size();i++) {

            if(ascii[s[i]]!=0){
                head = ascii[s[i]] + 1;
            }

            ascii[s[i]] = i; //数组下标记录字符信息，内容记录位置信息。数组内容非零说明前面已经有重复的了

            if(i-head>max)
                max = i-head;

        }
        return max;
    }
};