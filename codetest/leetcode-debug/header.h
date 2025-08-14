#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <list>
#include <stack>
#include <queue>
#include <iterator>

using namespace std;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

class Union {
public:

    vector<int> root;

    void init(){
        root.resize(1001);
        for (int i = 1; i < 1001; ++i) {
            root[i] = i;
        }
    }

    int find(int num){//find root
        if(num==root[num])
            return num;
        else{
            root[num] = find(root[num]);
            return root[num];
        }
    }

    bool is_Same(int u,int v){ // is_same_root
        u = find(u) ;
        v = find(v) ;
        return u==v;
    }

    void join(int u,int v){ // v->u
        int root_u = find(u);
        int root_v = find(v);
        if(root_u==root_v) return;
        root[root_v] = root_u;
    }

};

struct Dnode{
    int key,value;
    Dnode* prev;
    Dnode* next;
    Dnode():key(0),value(0),prev(nullptr),next(nullptr){}
    Dnode(int k,int v):key(k),value(v),prev(nullptr),next(nullptr){}
};

class Solution {
    public:

    int lengthOfLIS(vector<int>& nums) {

    }
};


