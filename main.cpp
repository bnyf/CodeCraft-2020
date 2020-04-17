#include <iostream>
#include <string>
#include <unordered_map>
#include <cstring>
#include <vector>
#include <algorithm>
#include <set>
#include <ctime>
#include <map>
#include <queue>
using namespace std;

#define MAX_EDGE_NUM 300000 //最大转账数
#define MAX_POINT_NUM 300000
//#define debug

void read_data();
void print_ans();

typedef struct MAP_s {
    int next; //下一条边
    int to; //下一个点
}MAP_t;

#ifdef debug
string input_file = "data/";
string output_file = "data/";
#else
string input_file = "/data/test_data.txt";
string output_file = "/projects/student/result.txt";
#endif

struct EDGE {
    int u;
    int v;
    bool operator <(const struct EDGE &x) const{
        if(u != x.u)
            return u > x.u;
        else
            return v > x.v;
    }
};
struct EDGE edge[MAX_EDGE_NUM];

map<int, int> hash_id; //id 的散列值
int decode_id[MAX_POINT_NUM];
int tot_edge_num = 0;
int tot_point_num = 0;

int in_d[MAX_POINT_NUM];
int out_d[MAX_POINT_NUM];

int cnt = 0;

MAP_t m[MAX_EDGE_NUM];
int head[MAX_POINT_NUM];
MAP_t m_bwd[MAX_EDGE_NUM]; // backward map
int head_bwd[MAX_POINT_NUM];

void make_edge(int u, int to, MAP_t *m_tmp, int *head_tmp) {
    m_tmp[cnt].to = to;
    m_tmp[cnt].next = head_tmp[u];
    head_tmp[u] = cnt++;
}

//使用邻接矩阵建图
void make_map() {
    memset(head,-1,sizeof(head));
    for(int i=0; i < tot_edge_num; i++) {
        edge[i].u = hash_id[edge[i].u];
        edge[i].v = hash_id[edge[i].v];
        in_d[edge[i].v]++;
        out_d[edge[i].u]++;
        make_edge(edge[i].u, edge[i].v, m, head);
    }

    memset(head_bwd,-1,sizeof(head));
    cnt = 0;
    for(int i=0; i < tot_edge_num; i++) {
        make_edge(edge[i].v, edge[i].u, m_bwd, head_bwd);
    }
}

bool vis[MAX_POINT_NUM];

vector< vector<int> > ans;
vector<int> path;
unordered_map<int, vector< int > > backward_path; 

void forward_dfs(int u, int target, int dep, int max_dep) {
    vis[u] = true;
    path.push_back(u);
    if(dep == max_dep && backward_path.find(u) != backward_path.end()) {
        int len = backward_path[u].size();
        for(int j=0;j<len;++j){
            int flag = true;
            for(int k=1;k<dep-1;++k) {
                if(path[k] == backward_path[u][j]) {
                    flag = false;
                    break;
                }
            }
            if(flag) {
                path.push_back(backward_path[u][j]);
                ans.push_back(path);
                path.pop_back();
            }
        }
    }
    for(int i=head[u]; i != -1; i=m[i].next){
        int to = m[i].to;
        if(to < target || out_d[to] == 0)
            continue;
        if(vis[to]) {
            if(to == target && dep >= 3) {
                ans.push_back(path);
            }
        }
        else if (dep < max_dep) {
            forward_dfs(to, target, dep + 1, max_dep);
        }
    }
    path.pop_back();
    vis[u] = false;
}

void backward_dfs(int u, int target, int dep, int max_dep) {
    vis[u] = true;
    for(int i=head_bwd[u]; i != -1; i=m_bwd[i].next) {
        int to = m_bwd[i].to;
        if(to < target)
            continue;
        if(!vis[to])  {
            if(dep == max_dep) {
                backward_path[to].push_back(u);
            }
            if (dep < max_dep) {
                backward_dfs(to, target, dep + 1, max_dep);
            }
        }
    }
    vis[u] = false;
}

bool cmp(const vector<int> &a, vector<int> &b) {
    int len1 = a.size();
    int len2 = b.size();
    if(len1 == len2) {
        for(int i=0;i<len1;++i) {
            if(a[i] != b[i])
                return a[i] < b[i];
        }
    }

    return len1 < len2;
}

queue<int> q_in;
queue<int> q_out;
void topo() {
    for(int i=0;i<tot_point_num;++i) {
        if(in_d[i] == 0) {
            q_in.push(i);
        }
        if(out_d[i] == 0) {
            q_out.push(i);
        }
    }
    while(!q_in.empty()) {
        int cur = q_in.front();
        q_in.pop();
        for(int i=head[cur]; i != -1; i=m[i].next) {
            int to = m[i].to;
            in_d[to]--;
            if(in_d[to] == 0) {
                q_in.push(to);
            }
        }
        head[cur] = -1;
    }
    while(!q_out.empty()) {
        int cur = q_out.front();
        q_out.pop();
        for(int i=head_bwd[cur]; i != -1; i=m_bwd[i].next) {
            int to = m_bwd[i].to;
            out_d[to]--;
            if(out_d[to] == 0) {
                q_out.push(to);
                head[to] = -1;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    path.reserve(7);
#ifdef debug
    input_file += argv[1];
    output_file += argv[1];
    input_file +="/test_data.txt";
    output_file += "/my_result.txt";
    clock_t s_t = clock();
#endif
    read_data();
    make_map();
#ifdef debug
    clock_t m_t = clock();
    // cout << tot_point_num << endl;
#endif
    topo();


    for(int i=0;i<tot_point_num;++i) {
// #ifdef debug
        // if(i % 1000 == 0)
        //     cout << i << endl;
// #endif
        if(in_d[i] != 0 || out_d[i] == 0) {
            backward_dfs(i, i, 1, 2);
            forward_dfs(i, i, 1, 6);
            backward_path.clear();
        }
    }


#ifdef debug
    clock_t d_t = clock();
#endif
    print_ans();
#ifdef debug
    clock_t p_t = clock();
    cout << (d_t - m_t)/CLOCKS_PER_SEC << ' ' << (p_t - s_t)/CLOCKS_PER_SEC << endl;
#endif

    return 0;
}

void read_data() {
    string buf;
    FILE* file=fopen(input_file.c_str(),"r");
    int u,v,c;
    while (fscanf(file,"%d,%d,%d",&u,&v,&c) != EOF){
        edge[tot_edge_num].u = u;
        edge[tot_edge_num].v = v;
        hash_id[u] = 1;
        hash_id[v] = 1;
        tot_edge_num++;
    }
    fclose(file);
    for(auto &h: hash_id) {
        h.second = tot_point_num;
        decode_id[tot_point_num++] = h.first;
    }
}

void print_ans() {
    sort(ans.begin(),ans.end(),cmp);

    FILE* file=fopen(output_file.c_str(),"w");
    int tot_ans = ans.size();
    fprintf(file,"%d\n",tot_ans);
    for(int i=0;i<tot_ans;++i) {
        int len = ans[i].size();
        fprintf(file,"%d",decode_id[ans[i][0]]);
        for(int j=1;j<len;++j) {
            fprintf(file,",%d",decode_id[ans[i][j]]);
        }
        fprintf(file,"\n");
    }

    fclose(file);
}