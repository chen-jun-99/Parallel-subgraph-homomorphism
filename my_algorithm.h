#include "Graph.h"
#include "GQ.h"
#include "QueryQue.h"
#include "QGraph.h"
#include <algorithm>
#include <chrono>
#include <queue>
using namespace std;


struct Result{
    string dataGraName;         //
    string queryGraName;        //
    string queryTitle;          //
    int dataGraVNum;            //
    int dataGraENum;            //
    int queryGraVNum;           //
    int queryGraENum;           //
    int queryGraENum_bfl;       //
    int threadNum;              //
    double dataGraIndexTime;    //
    double findCandiTime;       //
    double FBSTime;             //
    double deleteCandiTime;     //
    double bflTime;             //
    double matchTime;           //
    long long matchNum;         //

    double totalTime;
    int candiSize[2];

    vector<double> fbsThreadTime;
    vector<double> threadTime;
    int fbsTerm;
};
//bool DegreeFilter=true;
//bool NeighborFilter=true;

struct BflQueryType{
    int u,v,q;
	int edge_label;
    BflQueryType():u(0),v(0),q(0),edge_label(0){}
    BflQueryType(int _u,int _v,int _q,int _edge_label):u(_u),v(_v),q(_q),edge_label(_edge_label){}
};

void worker(int tid,Graph &dataG,GQ &gq,QueryQue<BflQueryType,vector<int>> &qu);

//度过滤
bool DF(Graph& dataG,QGraph& queryG,int di,int qi);

//邻域标签过滤
void NF();

//按照标签找出查询图中每个节点在数据图中的候选节点
void initGQ(Graph& dataG,QGraph& queryG,GQ& gq);


//查询图边p->q，u是p的候选，看是否有边u->v与其对应
bool uHasEdge(GQ &gq,int p,int q,int u,int edge_label,Graph& dataG);
//查询图边p->q，v是q的候选，看是否有边u->v与其对应
bool vHasEdge(GQ &gq,int p,int q,int v,int edge_label,Graph& dataG);

bool forwardPrune(GQ &gq,QGraph &queryG,Graph& dataG);

//并行化节点剪枝，按节点划分
bool parl_forwardPrune(GQ &gq,QGraph &queryG,Graph& dataG);
bool parl_forwardPruneDag(GQ &gq,QGraph &queryG,Graph& dataG);

bool backwardPrune(GQ &gq,QGraph &queryG,Graph& dataG);

bool parl_backwardPrune(GQ &gq,QGraph &queryG,Graph& dataG);
bool parl_backwardPruneDag(GQ &gq,QGraph &queryG,Graph& dataG);
struct FBSType{
    int p;
    int q;
    int label;
    int type;
};
//查询图边p->q，u是p的候选，看是否有边u->v与其对应
bool uHasEdge_parl(GQ &gq,int p,int q,int u,int edge_label,Graph& dataG,int tid);
//查询图边p->q，v是q的候选，看是否有边u->v与其对应
bool vHasEdge_parl(GQ &gq,int p,int q,int v,int edge_label,Graph& dataG,int tid);

bool uHasEdge_parl_mseq(GQ &gq,int p,int q,int u,int edge_label,Graph& dataG,int tid);
bool vHasEdge_parl_mseq(GQ &gq,int p,int q,int v,int edge_label,Graph& dataG,int tid);

void worker_for_pfbs(int tid,Graph &dataG,GQ &gq,QueryQue<FBSType,int> &que,bool &change);
void parl_fbs(GQ &gq,QGraph &queryG,Graph& dataG);

//节点剪枝
void FBSimBas(GQ &gq,QGraph &queryG,Graph& dataG);

void FBSimDag(GQ &gq,QGraph &queryG,Graph& dataG);

void hybridFBS(GQ &gq,QGraph &queryG,Graph& dataG);
void hybridFBS_threadpool(GQ &gq,QGraph &queryG,Graph& dataG);

//原本的思路是先查询所有节点对间的边
//现在需改为先对节点剪枝再找边
void parl_bfl(Graph& dataG,GQ& gq,QGraph& queryG);
//对候选节点分割进行可达边查找
void parl_bfl_2(Graph& dataG,GQ& gq,QGraph& queryG);

void ceci(GQ& gq,QGraph& queryG);

extern timeType startT,endT;
void timePrint(const string& str,timeType st=startT,timeType end=endT);
double getTime(timeType st=startT,timeType end=endT);
string getName(string str);
string getNameBeforePoint(string str);
void outToFile(ofstream &fout,Result &result);
string eraseTab(string title);
string eraseSpace(string str);

void subgraph_match(Graph& dataG,QGraph& queryG,Result &result);

//直接计算匹配数
void calculateMatch(Graph& dataG,QGraph& queryG,GQ &gq,Result &result);

double my_ceci(GQ& gq,QGraph& queryG);