#include <iostream>
#include <string>
#include <unordered_map>
#include <cstring>
#include <vector>
#include <stack>
using namespace std;

//#define MAX_LINE_LEN 40 //单个数字长度
#define MAX_EDGE_NUM 300000 //最大转账数
#define MAX_POINT_NUM MAX_EDGE_NUM << 1
int edge[MAX_EDGE_NUM][3];
int tot_edge_num = 0;
int tot_point_num = 0;

unordered_map<int, int> hash_id; //hash map
int decode_id[MAX_POINT_NUM];

void read_data() {
    string buf;
    freopen("test_data.txt", "r", stdin);
    while (getline(cin, buf)){
        string::size_type sz = -1;
        string temp = buf;
        for(int i=0; i<3; ++i){
            temp = temp.substr(sz+1);
            edge[tot_edge_num][i] = stoi(temp, &sz);
        }
        tot_edge_num++;
    }
    fclose(stdin);
}

//散列化id，因为最多 32w 条转账数据，所以可以将32位的id（10 位数） 映射到 32*2=64w 的空间内。
void hash_id_f() {
    for(int i=0; i < tot_edge_num; ++i) {
        for(int j=0;j<2;++j) {
            unordered_map<int,int>::const_iterator got = hash_id.find (edge[i][j]);
            if(got == hash_id.end()) {
                hash_id[edge[i][j]] = tot_point_num;
                decode_id[tot_point_num] = edge[i][j];
                edge[i][j] = tot_point_num++;
            }
            else {
                edge[i][j] = hash_id[edge[i][j]];
            }
        }
    }
}

struct e {
    int next; //下一条边
    int to; //下一个点
    int v; //边上的值
}m[MAX_EDGE_NUM];
int head[MAX_POINT_NUM];
int d[MAX_POINT_NUM];
int cnt = 0;

void make_edge(int u, int to, int v) {
    m[cnt].to = to;
    m[cnt].next = head[u];
    head[u] = cnt++;
    d[to]++;
}

//使用邻接矩阵建图
void make_map() {
    memset(head,-1,sizeof(head));
    for(int i=0; i < tot_edge_num; i++) {
        make_edge(edge[i][0], edge[i][1], edge[i][2]);
    }
}

int vis_g[MAX_POINT_NUM];
int vis[MAX_POINT_NUM];
vector< vector<int> > ans;
stack<int> path;
int dfs(int u, int target) {
//    cout << "dfs: " << u <<  ' ' << target << ' ' << tot_point_num << endl;
    vis[u] = 1;
    path.push(u);
    for(int i=head[u]; i != -1; i=m[i].next){
        int to = m[i].to;
        if(vis_g[to])
            continue;
        if(vis[to]) {
            if(to == target && path.size() >= 3) {
                stack<int> temp_p = path;
                vector<int> temp_v;
                while(!temp_p.empty()) {
                    temp_v.push_back(temp_p.top());
                    temp_p.pop();
                }
                ans.push_back(temp_v);

//                for(int j=0;j<temp_v.size();++j) {
//                    cout << temp_v[j] << ' ';
//                }
//                cout << endl;

            }
        }
        else if(path.size() < 7) {
            dfs(to, target);
        }
    }
    path.pop();
    vis[u] = 0;
}

void print_ans() {
    int tot_ans = ans.size();
    cout << tot_ans << endl;
    for(int i=0;i<tot_ans;++i) {
        int len = ans[i].size();
        ans[i][0] = decode_id[ans[i][0]];
        int pos = 0;
        int mn = ans[i][0];
        for(int j=1;j<len;++j) {
            ans[i][j] = decode_id[ans[i][j]];
            if(ans[i][j] < mn) {
                mn = ans[i][j];
                pos = j;
            }
        }
        cout << ans[i][pos];
        for(int j=(pos-1+len)%len;j!=pos;j=(j-1+len)%len) {
            cout << ',' << ans[i][j];
        }
        cout << endl;
    }
}

int main() {

    cout << "begin read data" << endl;
    read_data();
    cout << "begin hash id" << endl;
    hash_id_f();
    cout << "begin make map" << endl;
    make_map();
    cout << "begin dfs" << endl;
    for(int i=0;i<tot_point_num;++i) {
        if(d[i] == 0)
            continue;
        dfs(i, i);
        vis_g[i] = 1;
        for(int i=head[i]; i != -1; i=m[i].next) {
            d[m[i].to]--;
        }
    }
    cout << "print ans" << endl;
    print_ans();

    return 0;
}
