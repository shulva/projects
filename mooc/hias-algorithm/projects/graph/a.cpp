//
// Created by 13290 on 2025/2/20.
//
#include <vector>
using namespace std;
class Solution {
public:

    vector<vector<int>> graph;

    bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {

        if(prerequisites.size()==0)
            return true;

        graph.resize(numCourses);

        for(int i =0;i<prerequisites.size();i++)
            graph[prerequisites[i][1]].push_back(prerequisites[i][0]);

        vector<int> in_degree;
        in_degree.resize(numCourses,-1);

        for(int i =0;i<prerequisites.size();i++)
        {
            if(in_degree[prerequisites[i][0]]==-1)
                in_degree[prerequisites[i][0]]=0;

            if(in_degree[prerequisites[i][1]]==-1)
                in_degree[prerequisites[i][1]]=0;

            in_degree[prerequisites[i][0]]++;
        }

        int true_num = 0;
        for(int i =0;i<prerequisites.size();i++)
            if(in_degree[i]!=-1)
                true_num++;

        queue<int> q;
        vector<int> ans;

        for(int i=0;i<in_degree.size();i++)
            if(in_degree[i]==0)
                q.push(i);

        while(!q.empty())
        {
            int vertex = q.front();
            q.pop();
            ans.push_back(vertex);

            for(int i=0;i<graph[vertex].size();i++)
                if(--in_degree[graph[vertex][i]]==0)
                    q.push(graph[vertex][i]);
        }

        if(ans.size()==true_num)
            return true;

        return false;

    }
};

int main()
{
    vector<vector<int>>& prerequisites = [[1,4],[2,4],[3,1],[3,2]];
    Solution s;
    s.canFinish(prerequisites);
}