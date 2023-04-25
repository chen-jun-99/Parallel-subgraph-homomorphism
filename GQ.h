#ifndef _GQ_H__
#define _GQ_H__

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <shared_mutex>
#include <map>
#include <thread>
#include <queue>
#include <chrono>
#include "CSR.h"
#include "QGraph.h"
#include "QueryQue.h"
#include "constant.h"
//#include "tbb/concurrent_unordered_map.h"

using namespace std;

typedef std::chrono::time_point<std::chrono::steady_clock> timeType;
typedef std::pair<int, std::vector<int> > Data;//first:父节点的一个候选节点 second:与之对应的当前节点的候选
//typedef std::vector< Data >::iterator edgeCandIterator;
class DataCompare
{
	public:
		// Comparison function for sorting
		bool operator()(const Data& lhs, const Data& rhs) const
		{
			return keyLess(lhs.first, rhs.first);
		}

		// COmparison fuunctions for lookup
		bool operator()(const Data& lhs, const Data::first_type& k) const
		{
			return keyLess(lhs.first, k);
		}

		bool operator()(const Data::first_type& k, const Data& rhs) const
		{
			return keyLess(k, rhs.first);
		}
	private:
		bool keyLess(const Data::first_type& k1, const Data::first_type& k2) const
		{
			return k1 < k2;
		}
};
struct CRI{
	unordered_map<int,unsigned long long>  cardinality;	//节点基数，初始为1，0表示该候选节点无效
	//std::vector<int> node_cands;	//当前顶点在数据图中的候选节点，值为-1表示忽略
    int id; //candi中已经存储了候选节点序号，故只需记录对应在查询图中的id

    //存储树边，结构待考虑 to_be_done
	std::vector<Data> edge_cands;
	std::map<int, std::vector<Data> >nte_cands;

};

class thread_status{
	public:
		uint32_t mybeg, myend;
		bool go;			//默认为true
		vector<int> embedding;		//embedding[i]为查询树中序号为i的节点在当前进程中的一个对应候选
		uint8_t curr_query_node;

	public:
		thread_status(int qsize)
		{
			mybeg = 0;
			myend = 0;
			go = true;
			curr_query_node = 0;
            embedding.assign(qsize,0);
		}
};

struct EdgeType{
    int u,v,p,q;
};

typedef std::shared_mutex MyMutex;
//typedef std::mutex MyMutex;
class GQ{

public:
    int vert_num,edge_num;
    int q_vert_num;
    //to_be_done 二维数组，扩容时会不会开销太大，考虑改为指针
    //存储的是顶点在数据图中的编号
    vector<vector<int>> candi;  
    //割点直接记录答案
    unordered_map<int,unordered_map<int,int>> cut_candi_weight;

    CSR in, out;
    //to_be_done 边的存储方式，不使用csr，!!!注意映射!!!
    //<查询图中顶点，<父节点候选，<查询图中顶点，查询图中顶点对此父节点的候选> > >
    candiEdgeType candiEdge;
    candiEdgeType re_candiEdge;
    unordered_map<int,int> ump; //记录原节点编号与新编号间的映射关系
    vector<EdgeType> edge[32];

    MyMutex pruned_mtx;
    unordered_map<int,bool> isPruned;//顶点是否被剪枝
    vector<unordered_map<int,bool>> visPruned;
    //vector<int> isPruned;
    //unordered_map<int,int> candi_to_q;//数据图顶点对应的查询图顶点

    //mseq[u][v]=1--有边（和边标签匹配）    ==-1--不可达    ==0--待判断
    //MyMutex mseq_mtx[MAX_QUERY_NUM];
    MyMutex mseq_mtx;
    //vector<unordered_map<int,int>> mseq;
    unordered_map<int,unordered_map<int,int>> mseq;

    //多线程相关
    //mutable std::shared_mutex m_mutex;


    //枚举(ceci)相关↓↓↓

    vector<CRI> myCR;

    unsigned long long threshold;
    unsigned long long totalwork;

    int startVert;
    vector<int> visitOrder;
    vector<int> idToVisit;
    vector<int> preset;

    MyMutex idx_mtx;
    int units_idx;
    vector<vector<int> > work_units;

    vector<thread_status> myTCB;

    //由于好像queryTree[i].id=i，故删除了这两项
    // vector<int> queryMatchingSequence;
    // vector<int> queryMatchingSequence_inv;

    vector<long long> numberofRecursiveCalls;//记录每个线程调用递归搜索函数的次数
    vector<long long> numberofEmbeddings;//记录每个线程找到的匹配数
    int thread_stop;
    long long totalEmbeddings;
    MyMutex emb_mtx;
    //枚举(ceci)相关↑↑↑

    //mine↓↓↓
    unordered_map<int,unordered_map<int,long long>> weight;

    //mine↑↑↑

    //work stealing
    vector<int> thread_free;
    MyMutex free_mtx;
    struct work_stealed{
        vector<int> matched;
        vector<int> nextCandi;
    };
    queue<work_stealed> que_stealed;
    MyMutex que_mtx;

    vector<double> fbs_thread_time;
    vector<double> threadTime;

    vector<vector<int>> workQue;
    vector<pair<int,int>> work_idx; 

    int fbsTerm;

    timeType match_st;
    timeType match_et;


public:


    GQ(int querySize){
        vert_num=0;
        edge_num=0;
        q_vert_num=querySize;
        candi.assign(querySize,vector<int>());
        // queryMatchingSequence.assign(q_vert_num,0);
        // queryMatchingSequence_inv.assign(q_vert_num,0);
        numberofRecursiveCalls.assign(MATCHING_THREAD_NUM,0);
        numberofEmbeddings.assign(MATCHING_THREAD_NUM,0);
        thread_stop=0;
        totalEmbeddings=0;

        visPruned.assign(querySize,unordered_map<int,bool>());

        units_idx=0;

        //mseq_mtx.assign(querySize,std::mutex());
    }

    // //给图的每个顶点重新编号，改变vert_num
    // void addVert(int u){
    //     ump[u]=vert_num++;
    // }

    void addEdge(int tid,int u,int v,int p,int q){
        //edge[tid].push_back(make_pair(ump[u],ump[v]));
        edge[tid].push_back({u,v,p,q});
    }


    void constructGraph();

    void print();

    //去掉剪枝的图，重新构造图
    void construct_Graph2();


    //枚举(ceci)相关↓↓↓

    void initCR(QGraph& queryG);

    //确定每个节点的树边和非树边候选，计算节点基数
    void explore(QGraph& queryG);

    ////极端簇分解
    void prepare();
    void prepare_work(std::vector<std::vector<int>>& work_units, int pi, std::vector<int>& pre, std::vector<std::pair<long long, int> >& works, unsigned long long int card_old);

    //并行化匹配
    void parl_matching(QGraph& queryG);
    void parl_matching_cutV(QGraph& queryG);
    void subgraphSearch(int tid,QGraph& queryG);
    void my_subgraphSearch(int tid,QGraph& queryG,int weight);
    void my_subgraphSearch_steal(int thid,QGraph& queryG,vector<int> inters);

    void workDistribute();
    void match_cutV(QGraph& queryG);

    void printInfo(long long &match_num);

    int get_mseq(int p,int u,int v);
    void set_mseq(int p,int u,int v,int val);

    bool get_pruned(int p,int u);
    void set_pruned(int p,int u,bool val=true);

    int get_idx();

    void addEmbedding(int num);
    long long getEmbedding();
    //枚举(ceci)相关↑↑↑

    int getFreeThread();
    void setFreeThread(int idx,int val);

    bool queEmpty();
    void quePut(vector<int> matched,vector<int> nextCandi);
    void queGet(vector<int> &matched,vector<int> &nextCandi);
};

#endif