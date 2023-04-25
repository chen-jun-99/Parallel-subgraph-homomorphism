#ifndef _GRAPH_H__
#define _GRAPH_H__

#include "CSR.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>

#ifndef K
#define K 5
#endif
#ifndef D
#define D (320 * K)
#endif

#define MAX_THREAD_NUM 32
struct node {
    int vis = 0;
    int thread_vis[MAX_THREAD_NUM]={0};
    union {
        int L_in[K];
#if K > 8
        unsigned int h_in;
#else
        unsigned char h_in;//[0,32*K)
#endif
    };
    union {
        int L_out[K];
#if K > 8
        unsigned int h_out;
#else
        unsigned char h_out;
#endif
    };
    pair<int, int> L_interval;
};


class Graph {
public:
    CSR in, out;
    int vert_num, edge_num;
    //bfl相关
    int vis_cur, cur;
    int thread_cur[MAX_THREAD_NUM]={0};
    vector<node> nodes;
    
    vector<int> label;//顶点标签

    //仅对查询图有效
    vector<vector<int>> edgelabel;
public:

    Graph() {
        vert_num = 0;
        edge_num = 0;
        vis_cur = 0;
        cur = 0;
    }

    //输出图的信息、
    void print();

    void readGraph(string filename,int hasdegree);

    
    //插入一条有向边u->v，主要记录原节点和新节点的映射，节点的入度和出度
    void insert(int u, int v, vector<int>& indegree, vector<int>& outdegree);

    //插入u->v，v不属于当前分区
    void inserton(int u);

    //查询图中是否有边u->v
    bool hasEdge(int u,int v);


    //bfl相关
    int h_in();
    int h_out();
    void dfs_in(int u);
    void dfs_out(int u);
    void constructIndex();
 
    bool reach(int u, int v,int tid);//查询可达请调用canReach
    bool canReach(int u, int v,int tid);//查询u是否能到达v；u,v为原始大图中的序号
    
};

#endif