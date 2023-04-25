#ifndef _Q_GRAPH_H__
#define _Q_GRAPH_H__

#include <vector>
#include <string>
#include <fstream>
#include <stack>
#include <unordered_map>
using namespace std;

#define MAX_QUERY_NODE 64
struct QueryNode{
	public:
		int id;			//顶点在查询树中的序号
		int label;
		int vertexId;   //顶点原序号

		int parent;     //父节点在树中的id
		//int childCount;
		//int ntEdgeCount;				//非树边数量
		vector<int> childList;	    //存储孩子节点的访问序号
		vector<int> ntEdges[2];	//存储非树边,直接存储顶点 0--别人指它  1--它指别人

};

using candiEdgeType=unordered_map<int,unordered_map<int,unordered_map<int,vector<int>>>>;
class QGraph{

public:
    int vert_num,edge_num,edge_bfl_num;

    vector<int> label;//顶点标签

    //仅对查询图有效
    vector<vector<int>> edgelabel;

    vector<vector<int>> edge;

    vector<int> degree;
    vector<int> indegree;
    vector<int> outdegree;

    //求割点
    vector<int> isCutV;
    vector<vector<int>> cutChild;
    vector<int> cutErase;

    //枚举(ceci)相关↓↓↓

    int startVert;
    vector<QueryNode> queryTree;
    vector<int> visitTreeOrder;

    vector<vector<int>> pre[2];

    //vector<int> qGraphtoTreeMap;

    //枚举(ceci)相关↑↑↑

    vector<int> visitOrder;
    vector<int> idToVisit;
    vector<pair<int,int>> topOrderEdge;

    
public:

    QGraph(){
        startVert=0;
        vert_num=0;
        edge_num=0;
        edge_bfl_num=0;
    }
    void readQueryOnce(string filename);
    void readQueryLoop(ifstream &fin,string &title,bool &isOver,int hasdegree);


    //枚举(ceci)相关↓↓↓

    //选择查询图中开始匹配的节点序号
    void chooseStartVertex(const vector<vector<int>>& candi);

    //bfs生成查询树
    void generateQueryTree();
    void printQueryTree();

    void generateTopOrder();
    void generateTopOrder(const vector<vector<int>>& candi);

    void generateOrder(const vector<vector<int>>& candi);

    void generateOrder_2(const vector<vector<int>>& candi);

    void generateMyOrder(const vector<vector<int>>& candi);

    void generateKruskal(int compareType,const vector<vector<int>>& candi,candiEdgeType& candiEdge);
    void generateKruskal_0(const vector<vector<int>>& candi);

    void generateRI();

    void generateCECI();

    void generateCut(const vector<vector<int>>& candi,const vector<int>& erased);

    void generateQuickSI(const vector<vector<int>>& candi,candiEdgeType& candiEdge);
    //枚举(ceci)相关↑↑↑

    void findCutVertex();
    void tarjan(stack<int>& st,int u, int fa,vector<int>& dfn,vector<int>& low,vector<int>& visited,vector<vector<int>>& gu,int& cnt);
    void printCut();

    void printTree();
};




#endif