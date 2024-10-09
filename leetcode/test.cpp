//
// Created by shulva on 24-9-5.
//
#include <iostream>
#include <string>
using namespace std;

class Solution {
    int ascii [128];
public:
    int lengthOfLongestSubstring(string s) {
        for(int i = 0;i<128;i++)
            ascii[i] = 0 ;

        int head = 1;
        int max = 0;

        for(int i=0;i<s.size();i++) {

            if(ascii[s[i]]!=0){
                if(head<ascii[s[i]]+1)
                    head = ascii[s[i]] + 1;
            }

            ascii[s[i]] = i+1; //数组下标记录字符信息，内容记录位置信息。数组内容非零说明前面已经有重复的了

            if(i+1-head+1>max)
                max = i+1-head+1;

        }
        return max;
    }
};

int main()
{
    Solution s;
    string test;
    cin>>test;
    s.lengthOfLongestSubstring(test);
}