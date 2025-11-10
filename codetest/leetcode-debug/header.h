#include <algorithm>
#include <vector>
#include <string>
using namespace std;

class Solution {
public:
    vector<vector<int>> permute(vector<int>& nums) {
        vector<vector<int>> res;
        vector<int> ans;
        vector<bool> valid(false,nums.size());

    }

    void back_trace(vector<int>& nums, vector<int>& ans,vector<vector<int>> &res,vector<bool> valid) {
        if (ans.size() == nums.size()) {
            res.push_back(ans);
        }

        for (int i = 0; i < nums.size(); i++) {
            if (valid[i]) {
                continue;
            }

            valid[i] = true;
            ans.push_back(nums[i]);
            back_trace(nums, ans, res, valid);
            valid[i] = false;
            ans.pop_back();
        }
    }
};