#include <iostream>
#include <string>
#include <unordered_map>
#include <cstring>
#include <vector>
#include <algorithm>
#include <set>
#include <ctime>
using namespace std;

#define MAX_EDGE_NUM 300000 //最大转账数
#define MAX_POINT_NUM 600000
//#define debug

void read_data();
void hash_id_f();
void sort_ans();
void print_ans();

typedef struct MAP_s {
    int next; //下一条边
    int to; //下一个点
}MAP_t;

#ifdef debug
const char filename_input[100] = "data/1004812/test_data.txt";
const char filename_output[100] = "data/1004812/my_result.txt";
#else
const char filename_input[100] = "/data/test_data.txt";
const char filename_output[100] = "/projects/student/result.txt";
#endif

int edge[MAX_EDGE_NUM][2];
unordered_map<int, int> hash_id; //id 的散列值
int decode_id[MAX_POINT_NUM];
int tot_edge_num = 0;
int tot_point_num = 0;

int in_d[MAX_POINT_NUM];
struct tot_d_s{
    int d;
    int point_id;

    bool operator <(const struct tot_d_s &x) const{
        return d > x.d;
    }
};
struct tot_d_s tot_d_t[MAX_POINT_NUM];
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
        out_d[edge[i][0]]++;
        in_d[edge[i][1]]++;
        make_edge(edge[i][0], edge[i][1], m, head);
    }

    memset(head_bwd,-1,sizeof(head));
    cnt = 0;
    for(int i=0; i < tot_edge_num; i++) {
        make_edge(edge[i][1], edge[i][0], m_bwd, head_bwd);
    }
}

bool vis_g[MAX_POINT_NUM];
bool vis[MAX_POINT_NUM];

vector< vector<int> > ans;
vector<int> path;
unordered_map<int, vector< vector<int> > > forward_path; //正向深搜长度为5的序列，尾节点被去掉
unordered_map<int, vector< vector<int> > > backward_path[3]; //反向深搜长度为2,3,4的序列，头结点被去掉

void forward_dfs(int u, int target, int dep, int max_dep) {
    vis[u] = true;
    path.push_back(u);
    for(int i=head[u]; i != -1; i=m[i].next){
        int to = m[i].to;
        if(vis_g[to])
            continue;
        if(vis[to]) {
            if(to == target && path.size() >= 3) {
                ans.push_back(path);
            }
        }
        else {
            if (dep < max_dep && out_d[to] > 0) {
                forward_dfs(to, target, dep + 1, max_dep);
            }
            else {
                forward_path[to].push_back(path);
            }
        }
    }
    path.pop_back();
    vis[u] = false;
}

void backward_dfs(int u, int dep, int max_dep) {
    vis[u] = true;
    for(int i=head_bwd[u]; i != -1; i=m_bwd[i].next) {
        int to = m_bwd[i].to;
        if(vis_g[to])
            continue;
        if(!vis[to])  {
            path.push_back(to);
            backward_path[max_dep-1][to].push_back(path);
            if (dep < max_dep && out_d[to] > 0) {
                backward_dfs(to, dep + 1, max_dep);
            }
            path.pop_back();
        }
    }
    vis[u] = false;
}
set<int> charge_replica;
void merge_path() {
    for(auto iter : forward_path) { //遍历当前起点不同尾点长度为 5 的正向路径
        int len_fwd_path = iter.second.size();
        for(int cnt_fwd_path=0;cnt_fwd_path<len_fwd_path;++cnt_fwd_path) { //遍历指定起点尾点的正向长度为 5 的路径
            for(auto & cnt_bwd : backward_path) { //backward_path长度分别为2,3,4的路径集合
                if(cnt_bwd.find(iter.first) != cnt_bwd.end()) { //bwd 和 fwd 尾点相同
                    int len_bwd_path = cnt_bwd[iter.first].size();
                    int len_fwd_point = iter.second[cnt_fwd_path].size();
                    for(int cnt_bwd_path=0;cnt_bwd_path<len_bwd_path;++cnt_bwd_path){ //遍历指定反向路径
                        int len_bwd_point = cnt_bwd[iter.first][cnt_bwd_path].size();
                        charge_replica.clear();
                        for (int cnt_fwd_point = 1; cnt_fwd_point < len_fwd_point; ++cnt_fwd_point) { //遍历指定正向路径的元素（除头点）
                            charge_replica.insert(iter.second[cnt_fwd_path][cnt_fwd_point]);
                        }
                        for (int cnt_bwd_point = 0; cnt_bwd_point < len_bwd_point-1; ++cnt_bwd_point) { //遍历指定反向路径的元素（除尾点）
                            charge_replica.insert(cnt_bwd[iter.first][cnt_bwd_path][cnt_bwd_point]);
                        }

                        if(charge_replica.size() == len_fwd_point + len_bwd_point - 2) { //合并路径放入答案
                            vector<int> tmp_path = iter.second[cnt_fwd_path]; //正向加入正向路径结点
                            for (int cnt_bwd_point = len_bwd_point-1; cnt_bwd_point >= 0; --cnt_bwd_point) { //反向加入反向路径结点
                                tmp_path.push_back(cnt_bwd[iter.first][cnt_bwd_path][cnt_bwd_point]);
                            }
                            ans.push_back(tmp_path);
                        }
                    }
                }
            }
        }
    }
    forward_path.clear();
    for(auto & i : backward_path) {
        i.clear();
    }
}

int main() {

#ifdef debug
    clock_t s_t = clock();
#endif

    read_data();

#ifdef debug
    clock_t r_t = clock();
    cout << "read data time: " << (r_t - s_t)/CLOCKS_PER_SEC << endl;
#endif

    hash_id_f();

#ifdef debug
    clock_t h_t = clock();
    cout << "hash id time: " << (h_t - r_t)/CLOCKS_PER_SEC << endl;
#endif

    make_map();

#ifdef debug
    clock_t m_t = clock();
    cout << "make map time: " << (m_t - h_t)/CLOCKS_PER_SEC << endl;
    cout << tot_point_num <<endl<<endl;
#endif

    for(int i=0;i<tot_point_num;++i) {
        tot_d_t[i].point_id = i;
        tot_d_t[i].d = in_d[i] + out_d[i];
    }
    sort(tot_d_t, tot_d_t+tot_point_num);

    for(int i=0;i<tot_point_num;++i) {
        int point_id = tot_d_t[i].point_id;
#ifdef debug
        if(i % 100 == 0)
            cout << i << endl;
#endif
        if(in_d[point_id] == 0)
            continue;
        forward_dfs(point_id, point_id, 1, 4); //找出长度为 3,4的环并记录长度为 5 的非环路径
        backward_dfs(point_id, 1, 3); //反向记录长度为 2, 3, 4的非环路径
        merge_path();
        vis_g[point_id] = true;
        for(int j=head[point_id]; j != -1; j=m[j].next) {
            in_d[m[j].to]--;
        }
    }

#ifdef debug
    clock_t d_t = clock();
    cout << "dfs time: " << (d_t - m_t)/CLOCKS_PER_SEC << endl;
#endif

    sort_ans();

#ifdef debug
    clock_t sort_t = clock();
    cout << "sort time: " << (sort_t - d_t)/CLOCKS_PER_SEC << endl;
#endif

    print_ans();

#ifdef debug
    clock_t p_t = clock();
    cout << "print time: " << (p_t - sort_t)/CLOCKS_PER_SEC << endl;
    cout << "all time: " << (p_t - s_t)/CLOCKS_PER_SEC << endl;
#endif

    return 0;
}

void read_data() {
    string buf;
    FILE* file=fopen(filename_input,"r");
    int u,v,c;
    while (fscanf(file,"%d,%d,%d",&u,&v,&c) != EOF){
        edge[tot_edge_num][0] = u;
        edge[tot_edge_num][1] = v;
        tot_edge_num++;
    }
    fclose(file);
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

void sort_ans() {
    int tot_ans = ans.size();
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

        vector<int> tmp;
        tmp.push_back(ans[i][pos]);
        for(int j=(pos+1)%len;j!=pos;j=(j+1)%len) {
            tmp.push_back(ans[i][j]);
        }
        ans[i] = tmp;
    }
    sort(ans.begin(), ans.end(), cmp);
}

void print_ans() {
    FILE* file=fopen(filename_output,"w");

    int tot_ans = ans.size();
    fprintf(file,"%d\n",tot_ans);
    for(int i=0;i<tot_ans;++i) {
        int len = ans[i].size();
        fprintf(file,"%d",ans[i][0]);
        for(int j=1;j<len;++j) {
            fprintf(file,",%d",ans[i][j]);
        }
        fprintf(file,"\n");
    }

    fclose(file);
}