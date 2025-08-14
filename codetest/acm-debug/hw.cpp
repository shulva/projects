#include <iostream>
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <queue>
#include <list>
#include <bitset>
#include <stack>
using namespace std;

int main(){

    FILE* file = freopen("../input.txt", "r", stdin);

    string temp;
    int num;
    getline(cin,temp);
    num = stoi(temp);

    int index1 = 0;

    unordered_map<string,int> station_to_id;
    vector<string> id_to_station;
    vector<vector<string>> line(num);

    for (int i = 0; i < num; ++i) {
        string station;
        getline(cin,station);
        stringstream ss(station);
        string x;
        while(ss>>x){

            if(station_to_id.find(x)==station_to_id.end()){
                station_to_id.emplace(x,index1);
                id_to_station.push_back(x);
                index1++;
            }

            line[i].push_back(x);
        }
    }

    string a1,a2;
    cin>>a1>>a2;

    fclose(stdin);

    int start,end;

    start = station_to_id[a1];
    end = station_to_id[a2];

    vector<vector<pair<int,int>>> graph(station_to_id.size());
    for (auto l:line) {
        for (int i = 0; i < l.size()-1; ++i) {
            int u = station_to_id[l[i]];
            int v = station_to_id[l[i+1]];

            graph[u].emplace_back(v,0);
            graph[v].emplace_back(u,0);
        }
    }

    for (int i = 0; i < line.size(); ++i) {
        for (int j = i+1; j < line.size(); ++j) {
            for (auto s:line[i]) {
                if(find(line[j].begin(),line[j].end(),s)!=line[j].end()){
                    int u = station_to_id[s];
                    for (auto change:line[j]) {
                        if(station_to_id[change]==station_to_id[s]) continue;

                        int v = station_to_id[change];
                        graph[u].emplace_back(v,1);
                    }
                }
            }
        }
    }

    queue<pair<int,int>> q;
    q.push(make_pair(start,0));
    vector<int> visited(station_to_id.size(),-1);
    vector<int> parent(station_to_id.size(),-1);

    while(!q.empty()){
        auto [current,count] = q.front();
        q.pop();

        if(current == end) break;

        for (auto [neighbor,cost]:graph[current]) {
            if(visited[neighbor]==-1||visited[neighbor]>count+cost){
                visited[neighbor] = count+cost;
                parent[neighbor]  = current;
                q.emplace(neighbor,count+cost);
            }
        }
    }

    if(visited[end]==-1){
        cout<<"NA";
        return 0;
    }

    stack<int> path;
    int current = end;

    while(current!=-1){
        if(current==start){
            path.push(current);
            break;
        }

        path.push(current);
        current = parent[current];
    }

    cout<<id_to_station[path.top()];
    path.pop();

    while(!path.empty()){
        cout<<"-"<<id_to_station[path.top()];
        path.pop();
    }

    cout<<endl;
    cout<<2+visited[end]<<endl;

    return 0;
}