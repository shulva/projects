#include <vector>
using namespace std;

class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        vector<int> copy = nums;
        vector<int> ans;
        std::sort(nums.begin(), nums.end());

        int sum = 0;
        int i = 0;
        int j = nums.size()-1;

        while(1)
        {
            if(nums[i]+nums[j] == target)
            {
                ans=find(copy,nums[i],nums[j]);
                return ans;
            }
            else if (nums[i]+nums[j] < target)
                i++;
            else
                j--;
        }

    }

    vector<int> find (vector<int>& nums, int target1, int target2){
        vector<int> ans;
        for(int i=0;i<nums.size();i++)
        {
            if(nums[i]==target1){
                ans.push_back(i);
                continue;
            }
            if(nums[i]==target2){
                ans.push_back(i);
                continue;
            }
        }
        return ans;
    }
};

