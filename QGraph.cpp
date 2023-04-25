#include "QGraph.h"
#include "constant.h"
#include <fstream>
#include <iostream>
#include <queue>
#include <algorithm>
#include <random>

void QGraph::readQueryOnce(string filename)
{
	//第一次读取顶点数目，边的数目，顶点度数
	ifstream fin(filename,ios::in);
	if(!fin){
		cout<<"open file error!"<<endl;
		return;
	}
	string title;
	//fin>>title;
	getline(fin,title);
	char c;
	int u,lb,v;
	bool read_degree_flag=true;
	while(fin>>c){
		if(c=='v'){
			fin>>u>>lb;
			vert_num++;
			label.push_back(lb);
		}
		else if(c=='e'){
			if(read_degree_flag){
				edgelabel.assign(vert_num,vector<int>(vert_num,0));
				edge.assign(vert_num,vector<int>(vert_num,0));
				degree.assign(vert_num,0);
				indegree.assign(vert_num,0);
				outdegree.assign(vert_num,0);
				read_degree_flag=false;
			}
			fin>>u>>v>>lb;
			edge_num++;
			edge[u][v]=1;
			//edge label
			edgelabel[u][v]=lb;
			if(lb==1){
				edge_bfl_num++;
			}
			degree[u]++;
			degree[v]++;
			indegree[v]++;
			outdegree[u]++;
		}else{
			break;
		}
	}
	fin.close();
}

void QGraph::readQueryLoop(ifstream &fin,string &title,bool &isOver,int hasdegree){
	//第一次读取顶点数目，边的数目，顶点度数
	if(!fin||fin.eof()){
		//cout<<"open file error!"<<endl;
		isOver=true;
		return;
	}
	//string title;
	//fin>>title;
	getline(fin,title);
	char c;
	int u,lb,v,d;
	bool read_degree_flag=true;
	label.clear();
	while(fin>>c){
		if(c=='v'){
			if(hasdegree){
				fin>>u>>lb>>d;
			}else{
				fin>>u>>lb;
			}
			
			vert_num++;
			label.push_back(lb);
		}
		else if(c=='e'){
			if(read_degree_flag){
				edgelabel.assign(vert_num,vector<int>(vert_num,0));
				edge.assign(vert_num,vector<int>(vert_num,0));
				degree.assign(vert_num,0);
				indegree.assign(vert_num,0);
				outdegree.assign(vert_num,0);
				read_degree_flag=false;
			}
			fin>>u>>v>>lb;
			edge_num++;
			edge[u][v]=1;
			
			// cout<<"debug"<<endl;
			// if(lb==0){
			// 	lb=1;
			// }else{
			// 	lb=0;
			// }

			//edge label
			edgelabel[u][v]=lb;
			if(lb==1){
				edge_bfl_num++;
			}
			degree[u]++;
			degree[v]++;
			indegree[v]++;
			outdegree[u]++;
		}else{
			// 将文件描述符移动到行首
    		fin.seekg(-1, std::ios::cur);
			break;
		}
		if(fin.eof()){
			//isOver=true;
			break;
		}
	}
}


void QGraph::chooseStartVertex(const vector<vector<int>>& candi){
	startVert=0;
	//选出三个得分最小的
	//当前顶点的得分=数据图中带有此标签的顶点数目/查询图中当前顶点的度
	priority_queue<pair<double,int>> top3;
	for(int i=0;i<vert_num;i++){
		double sc=(double)candi[i].size()/degree[i];
		top3.push({sc,i});
		if(top3.size()>3){
			top3.pop();
		}
	}
	//3个中选候选集最小的
	startVert=top3.top().second;
	top3.pop();
	while(!top3.empty()){
		int id=top3.top().second;
		top3.pop();
		if(candi[id].size()<candi[startVert].size()){
			startVert=id;
		}
	}
	//cout<<"起始节点id是："<<startVert<<endl;
}
/*
void QGraph::generateQueryTree(){
	vector<int> visited(vert_num,0);
	visitTreeOrder.assign(vert_num,-1);
	//qGraphtoTreeMap.assign(vert_num,-1);

	queue<pair<int,int>> que;
	bool startIn=false;
	for(int i=0;i<vert_num;i++){
		int cur=i;
		if(!startIn){
			cur=startVert;
			startIn=true;
			i=-1;
		}else if(visited[i]==1){
			continue;
		}
		que.push({cur,-1});
		visited[cur]=1;
		int vertexId;//顶点原序号
		int parentId;//父节点在树中的id
		
		while(!que.empty()){
			vertexId = que.front().first;
			parentId = que.front().second;
			que.pop();

			queryTree.push_back(QueryNode());
			QueryNode &qNode 	= *(queryTree.rbegin());
			qNode.vertexId  	= vertexId;
			qNode.parent    	= parentId;
			qNode.id		 	= queryTree.size()-1;
			qNode.label 	  	= label[vertexId];
			qNode.childCount = 0;

			//qGraphtoTreeMap[vertexId] = qNode.id;
			
			visitTreeOrder[vertexId] = qNode.id;

			if(parentId != -1){
				queryTree[parentId].childList[queryTree[parentId].childCount] = qNode.id;
				queryTree[parentId].childCount++;
			}

			std::vector<std::pair<int, int> > neighbors;	//first:顶点度数  second:顶点序号
			for(int nid=0;nid<vert_num;nid++){
				if(edge[vertexId][nid]==0){
					continue;
				}
				neighbors.push_back({degree[nid],nid});
			}
			sort(neighbors.begin(), neighbors.end(),[](const pair<int,int>& A,const pair<int,int>& B){return A.first>B.first;});


			for(int nid = 0; nid < neighbors.size(); ++nid){
				int cNode = neighbors[nid].second;
				if(visited[cNode]==0){
					visited[cNode]=1;
					que.push({cNode, qNode.id});
				}
				else if(parentId != visitTreeOrder[cNode]){
					//cNode不是第一次访问，并且这次的父节点不是已经记录的父节点，说明这是由非树边
					qNode.ntEdges[qNode.ntEdgeCount] = cNode;
					qNode.ntEdgeCount++;
				}
			}
		}
	}
}
*/
/*
void QGraph::printQueryTree(){
	for(int i=0;i<vert_num;i++){
		cout<<queryTree[i].id<<"\t"<<queryTree[i].vertexId<<"\t"<<queryTree[i].parent<<"\t";
		cout<<"childNum:"<<queryTree[i].childCount<<"\t--";
		for(int j=0;j<queryTree[i].childCount;j++){
			cout<<queryTree[i].childList[j]<<" ";
		}
		cout<<endl;
		cout<<"ntchildNum:"<<queryTree[i].ntEdgeCount<<"\t--";
		for(int j=0;j<queryTree[i].ntEdgeCount;j++){
			cout<<queryTree[i].ntEdges[j]<<" ";
		}
		cout<<endl<<endl;
	}
}
*/
void QGraph::generateRI(){
	vector<int> visited(vert_num,0);
	visitOrder.clear();
	idToVisit.assign(vert_num,-1);
	auto cmp2 = [](pair<int, int>& a, pair<int, int>& b) {
        return a.first < b.first || (a.first == b.first && a.second < b.second);
    };
    priority_queue<pair<int, int>, vector<pair<int, int>>, decltype(cmp2)> maxHeap(cmp2);
	maxHeap.push({0,startVert});
	while(!maxHeap.empty()){
		auto data=maxHeap.top();
		maxHeap.pop();
		int cur=data.second;
		if(visited[cur]==1){
			continue;
		}
		visited[cur]=1;
		visitOrder.push_back(cur);
		idToVisit[cur]=visitOrder.size()-1;
		for(int i=0;i<vert_num;i++){
			if(visited[i]==1){
				continue;
			}
			if(edge[cur][i]!=0||edge[i][cur]!=0){
				maxHeap.push({degree[i],i});
			}
		}
	}
	
}

void QGraph::generateCECI(){
	vector<int> visited(vert_num,0);
	visitOrder.clear();
	idToVisit.assign(vert_num,-1);
	queue<pair<int,int>> que;
	que.push({startVert,-1});
	while(!que.empty()){
		int cur=que.front().first;
		int parent=que.front().second;
		que.pop();
		if(visited[cur]==1){
			continue;
		}
		visited[cur]=1;
		visitOrder.push_back(cur);
		idToVisit[cur]=visitOrder.size()-1;
		queryTree.push_back(QueryNode());
		QueryNode &qNode 	= *(queryTree.rbegin());
		qNode.vertexId  	= cur;
		qNode.parent    	= parent;
		qNode.id		 	= queryTree.size()-1;

		if(parent!=-1){
			queryTree[parent].childList.push_back(qNode.id);
		}

		std::vector<std::pair<int, int> > neighbors;	//first:顶点度数  second:顶点序号
		for(int nid=0;nid<vert_num;nid++){
			if(edge[cur][nid]==0&&edge[nid][cur]==0){
				continue;
			}
			neighbors.push_back({degree[nid],nid});
		}
		//sort(neighbors.begin(), neighbors.end(),[](const pair<int,int>& A,const pair<int,int>& B){return A.first>B.first;});

		for(int i=0;i<neighbors.size();i++){
			int p=neighbors[i].second;
			if(visited[p]==0){
				que.push({p,qNode.id});
				continue;
			}else if(parent!=idToVisit[p]){
				if(edge[p][cur]==1){
					qNode.ntEdges[0].push_back(p);
				}
				if(edge[cur][p]==1){
					qNode.ntEdges[1].push_back(p);
				}
			}
			
		}
	}
}

void QGraph::generateTopOrder(){
	queue<int> que;
	que.push(startVert);
	vector<int> degree_temp=indegree;
	vector<int> visited(vert_num,0);
	visitOrder.clear();
	topOrderEdge.clear();
	idToVisit.assign(vert_num,-1);
	while(!que.empty()){
		int cur=que.front();
		que.pop();
		visited[cur]=1;
		visitOrder.push_back(cur);
		idToVisit[cur]=visitOrder.size()-1;
		for(int i=0;i<vert_num;i++){
			if(visited[i]==1){
				continue;
			}
			if(edge[cur][i]!=0){ 
				degree_temp[i]--;
				topOrderEdge.push_back({cur,i});
			}
			if(degree_temp[i]==0){
				que.push(i);
			}
		}
	}
}
void QGraph::generateTopOrder(const vector<vector<int>>& candi){
	queue<int> que;
	que.push(startVert);
	vector<int> degree_temp=indegree;
	vector<int> visited(vert_num,0);
	idToVisit.assign(vert_num,-1);
	visitOrder.clear();
	topOrderEdge.clear();
	auto cmp = [](pair<int, int>& a, pair<int, int>& b) {
        return a.first > b.first || (a.first == b.first && a.second > b.second);
    };
    priority_queue<pair<int, int>, vector<pair<int, int>>, decltype(cmp)> minHeap(cmp);
	for(int i=1;i<vert_num;i++){
		if(degree_temp[i]==0){
			minHeap.push({INT32_MAX,i});
		}
	}
	while(!que.empty()){
		int cur=que.front();
		que.pop();
		visited[cur]=1;
		visitOrder.push_back(cur);
		idToVisit[cur]=visitOrder.size()-1;
		for(int i=0;i<vert_num;i++){
			if(visited[i]==1){
				continue;
			}
			if(edge[cur][i]!=0){ 
				degree_temp[i]--;
				topOrderEdge.push_back({cur,i});
				if(degree_temp[i]==0){
					minHeap.push({candi[i].size(),i});
				}
			}
		}
		if(que.empty()&&(!minHeap.empty())){
			auto &data=minHeap.top();
			que.push(data.second);
			minHeap.pop();
		}
	}
}

void QGraph::generateOrder(const vector<vector<int>>& candi){
	vector<int> visited(vert_num,0);
	idToVisit.assign(vert_num,-1);
	visitOrder.clear();
	auto cmp = [](pair<int, int>& a, pair<int, int>& b) {
        return a.first > b.first || (a.first == b.first && a.second > b.second);
    };
	priority_queue<pair<int, int>, vector<pair<int, int>>, decltype(cmp)> minHeap(cmp);
	minHeap.push({0,startVert});
	while(!minHeap.empty()){
		auto data=minHeap.top();
		minHeap.pop();
		int cur=data.second;
		if(visited[cur]==1){
			continue;
		}
		visited[cur]=1;
		visitOrder.push_back(cur);
		idToVisit[cur]=visitOrder.size()-1;
		for(int i=0;i<vert_num;i++){
			if(visited[i]==1){
				continue;
			}
			if(edge[cur][i]!=0||edge[i][cur]!=0){
				minHeap.push({candi[i].size(),i});
			}
		}
	}
}

void QGraph::generateOrder_2(const vector<vector<int>>& candi){
	generateOrder(candi);
	pre[0].assign(vert_num,vector<int>());
	pre[1].assign(vert_num,vector<int>());
	for(int i=1;i<vert_num;i++){
		int q=visitOrder[i];
		for(int j=0;j<i;j++){
			int p=visitOrder[j];
			if(edge[p][q]==1){
				pre[0][i].push_back(p);
			}
			if(edge[q][p]==1){
				pre[1][i].push_back(p);
			}
		}
	}
}

void QGraph::generateCut(const vector<vector<int>>& candi,const vector<int>& erased){
	vector<int> visited(vert_num,0);
	for(int i=0;i<vert_num;i++){
		if(erased[i]==1){
			visited[i]=2;
		}
	}
	visitOrder.clear();
	idToVisit.assign(vert_num,-1);
	queryTree.clear();
	queue<pair<int,int>> que;
	que.push({startVert,-1});
	while(!que.empty()){
		int cur=que.front().first;
		int parent=que.front().second;
		que.pop();
		if(visited[cur]==1){
			continue;
		}
		visited[cur]=1;
		visitOrder.push_back(cur);
		idToVisit[cur]=visitOrder.size()-1;
		queryTree.push_back(QueryNode());
		QueryNode &qNode 	= *(queryTree.rbegin());
		qNode.vertexId  	= cur;
		qNode.parent    	= parent;
		qNode.id		 	= queryTree.size()-1;

		if(parent!=-1){
			queryTree[parent].childList.push_back(qNode.id);
		}

		std::vector<std::pair<int, int> > neighbors;	//first:节点候选数  second:顶点序号
		for(int nid=0;nid<vert_num;nid++){
			if(visited[nid]==2){
				continue;
			}
			if(edge[cur][nid]==0&&edge[nid][cur]==0){
				continue;
			}
			neighbors.push_back({candi[nid].size(),nid});
		}
		sort(neighbors.begin(), neighbors.end());

		for(int i=0;i<neighbors.size();i++){
			int p=neighbors[i].second;
			if(visited[p]==0){
				que.push({p,qNode.id});
				continue;
			}else if(parent!=idToVisit[p]){
				if(edge[p][cur]==1){
					qNode.ntEdges[0].push_back(p);
				}
				if(edge[cur][p]==1){
					qNode.ntEdges[1].push_back(p);
				}
			}
			
		}
	}
}

void QGraph::generateMyOrder(const vector<vector<int>>& candi){
	vector<int> visited(vert_num,0);
	visitOrder.clear();
	idToVisit.assign(vert_num,-1);
	queue<pair<int,int>> que;
	que.push({startVert,-1});
	while(!que.empty()){
		int cur=que.front().first;
		int parent=que.front().second;
		que.pop();
		if(visited[cur]==1){
			continue;
		}
		visited[cur]=1;
		visitOrder.push_back(cur);
		idToVisit[cur]=visitOrder.size()-1;
		queryTree.push_back(QueryNode());
		QueryNode &qNode 	= *(queryTree.rbegin());
		qNode.vertexId  	= cur;
		qNode.parent    	= parent;
		qNode.id		 	= queryTree.size()-1;

		if(parent!=-1){
			queryTree[parent].childList.push_back(qNode.id);
		}

		std::vector<std::pair<int, int> > neighbors;	//first:节点候选数  second:顶点序号
		for(int nid=0;nid<vert_num;nid++){
			if(edge[cur][nid]==0&&edge[nid][cur]==0){
				continue;
			}
			neighbors.push_back({candi[nid].size(),nid});
		}
		sort(neighbors.begin(), neighbors.end());

		for(int i=0;i<neighbors.size();i++){
			int p=neighbors[i].second;
			if(visited[p]==0){
				que.push({p,qNode.id});
				continue;
			}else if(parent!=idToVisit[p]){
				if(edge[p][cur]==1){
					qNode.ntEdges[0].push_back(p);
				}
				if(edge[cur][p]==1){
					qNode.ntEdges[1].push_back(p);
				}
			}
			
		}
	}
}


struct KruEdge{
	int u;
	int v;
	int uSize;
	int vSize;
	int weight;
};
void compareTwo(int& a,int &b){
	if(a<=b){
		return;
	}else{
		int temp=a;
		a=b;
		b=temp;
	}
}
bool KruCmp(const KruEdge& a,const KruEdge& b){
	return a.uSize<b.uSize||(a.uSize==b.uSize&&a.vSize<b.vSize);
}
bool KruCmp_2(const KruEdge& a,const KruEdge& b){
	return a.weight<b.weight||(a.weight==b.weight&&a.uSize<b.uSize);
}
int find(int x,vector<int>& fa){
	if (fa[x] == x) return x;
    return fa[x] = find(fa[x],fa);
}
void QGraph::generateKruskal(int compareType,const vector<vector<int>>& candi,candiEdgeType& candiEdge){
	vector<KruEdge> edgeList;
	queue<pair<int,int>> treeEdge;
	for(int i=0;i<vert_num;i++){
		for(int j=i+1;j<vert_num;j++){
			if(edge[i][j]==0&&edge[j][i]==0){
				continue;
			}
			int weight=0;
			if(edge[i][j]==1){
				for(auto u:candi[i]){
					weight+=candiEdge[i][u][j].size();
				}
			}else{
				for(auto u:candi[j]){
					weight+=candiEdge[j][u][i].size();
				}
			}
			int u=i,v=j;
			int us=candi[i].size(),vs=candi[j].size();
			if(us>vs){
				swap(u,v);
				swap(us,vs);
			}
			edgeList.push_back({u,v,us,vs,weight});
		}
	}
	if(compareType==1){
		sort(edgeList.begin(),edgeList.end(),KruCmp);
	}else{
		sort(edgeList.begin(),edgeList.end(),KruCmp_2);
	}
	vector<int> fa(vert_num,0);	//并查集
	for(int i=0;i<vert_num;i++){
		fa[i]=i;
	}
	for(auto& data:edgeList){
		int u=data.u,v=data.v;
		int fu=find(u,fa),fv=find(v,fa);
		if(fu!=fv){
			fa[fv]=fu;
			treeEdge.push({u,v});
		}
	}

	//生成查询顺序
	startVert=treeEdge.front().first;
	vector<int> visited(vert_num,0);
	visitOrder.clear();
	idToVisit.assign(vert_num,-1);

	//处理第一个节点
	visited[startVert]=1;
	visitOrder.push_back(startVert);
	idToVisit[startVert]=visitOrder.size()-1;
	queryTree.push_back(QueryNode());
	QueryNode &qNode 	= *(queryTree.rbegin());
	qNode.vertexId  	= startVert;
	qNode.parent    	= -1;
	qNode.id		 	= queryTree.size()-1;

	while(!treeEdge.empty()){
		int u=treeEdge.front().first;
		int v=treeEdge.front().second;
		treeEdge.pop();
		if(visited[u]&&visited[v]){
			continue;
		}else if(visited[u]==0&&visited[v]==0){
			treeEdge.push({u,v});
			continue;
		}else if(visited[v]){
			swap(u,v);	//始终让u是父节点
		}
		int cur=v;
		int parent=idToVisit[u];

		visited[cur]=1;
		visitOrder.push_back(cur);
		idToVisit[cur]=visitOrder.size()-1;
		queryTree.push_back(QueryNode());
		QueryNode &qNode 	= *(queryTree.rbegin());
		qNode.vertexId  	= cur;
		qNode.parent    	= parent;
		qNode.id		 	= queryTree.size()-1;

		queryTree[parent].childList.push_back(qNode.id);

	}

	for(int i=1;i<vert_num;i++){
		int cur=visitOrder[i];
		int parent=queryTree[i].parent;
		for(int j=0;j<i;j++){
			int p=visitOrder[j];
			if(parent==idToVisit[p]){
				continue;
			}
			if(edge[p][cur]==1){
				queryTree[i].ntEdges[0].push_back(p);
			}
			if(edge[cur][p]==1){
				queryTree[i].ntEdges[1].push_back(p);
			}
		}
	}
}

void QGraph::generateKruskal_0(const vector<vector<int>>& candi){
	vector<KruEdge> edgeList;
	vector<pair<int,int>> treeEdge;
	for(int i=0;i<vert_num;i++){
		for(int j=i+1;j<vert_num;j++){
			if(edge[i][j]==0&&edge[j][i]==0){
				continue;
			}
			int u=i,v=j;
			int us=candi[i].size(),vs=candi[j].size();
			if(us>vs){
				swap(u,v);
				swap(us,vs);
			}
			edgeList.push_back({u,v,us,vs});
		}
	}
	sort(edgeList.begin(),edgeList.end(),KruCmp);
	vector<int> fa(vert_num,0);	//并查集
	for(int i=0;i<vert_num;i++){
		fa[i]=i;
	}
	for(auto& data:edgeList){
		int u=data.u,v=data.v;
		int fu=find(u,fa),fv=find(v,fa);
		if(fu!=fv){
			fa[fv]=fu;
			treeEdge.push_back({u,v});
		}
	}

	//生成查询顺序
	//startVert=treeEdge.front().first;
	startVert=0;
	vector<int> visited(vert_num,0);
	visitOrder.clear();
	idToVisit.assign(vert_num,-1);

	//处理第一个节点
	visited[startVert]=1;
	visitOrder.push_back(startVert);
	idToVisit[startVert]=visitOrder.size()-1;
	queryTree.push_back(QueryNode());
	QueryNode &qNode 	= *(queryTree.rbegin());
	qNode.vertexId  	= startVert;
	qNode.parent    	= -1;
	qNode.id		 	= queryTree.size()-1;

	vector<int> treeVisited(treeEdge.size(),0);
	for(int i=0;i<treeEdge.size();i++){
		if(treeVisited[i]==1){
			continue;
		}
		int u=treeEdge[i].first;
		int v=treeEdge[i].second;
		if(visited[u]&&visited[v]){
			treeVisited[i]=1;
			continue;
		}else if(visited[u]==0&&visited[v]==0){
			continue;
		}else if(visited[v]){
			swap(u,v);	//始终让u是父节点
		}
		treeVisited[i]=1;
		int cur=v;
		int parent=idToVisit[u];

		visited[cur]=1;
		visitOrder.push_back(cur);
		idToVisit[cur]=visitOrder.size()-1;
		queryTree.push_back(QueryNode());
		QueryNode &qNode 	= *(queryTree.rbegin());
		qNode.vertexId  	= cur;
		qNode.parent    	= parent;
		qNode.id		 	= queryTree.size()-1;

		queryTree[parent].childList.push_back(qNode.id);
		i=-1;
	}

	for(int i=1;i<vert_num;i++){
		int cur=visitOrder[i];
		int parent=queryTree[i].parent;
		for(int j=0;j<i;j++){
			int p=visitOrder[j];
			if(parent==idToVisit[p]){
				continue;
			}
			if(edge[p][cur]==1){
				queryTree[i].ntEdges[0].push_back(p);
			}
			if(edge[cur][p]==1){
				queryTree[i].ntEdges[1].push_back(p);
			}
		}
	}
}

using quickEdge=tuple<int,int,int>;
struct MyTupleComparator {
    bool operator()(const quickEdge& t1, const quickEdge& t2) const {
        return std::get<2>(t1) > std::get<2>(t2);
    }
};
void QGraph::generateQuickSI(const vector<vector<int>>& candi,candiEdgeType& candiEdge){
	//priority_queue<KruEdge,vector<KruEdge>,KruCompare> pri_que;
	std::priority_queue<quickEdge,std::vector<quickEdge>,MyTupleComparator> pri_que;
	vector<int> visited(vert_num,0);
	visited[startVert]=1;
	idToVisit[startVert]=visitOrder.size()-1;
	for(int i=0;i<vert_num;i++){
		int weight=0;
		if(edge[startVert][i]==1){
			for(auto u:candi[startVert]){
				weight+=candiEdge[startVert][u][i].size();
			}
		}else if(edge[i][startVert]==1){
			for(auto u:candi[i]){
				weight+=candiEdge[i][u][startVert].size();
			}
		}
		int u=i,v=startVert;
		int us=candi[u].size(),vs=candi[v].size();
		if(us>vs){
			swap(u,v);
			swap(us,vs);
		}
		pri_que.push({u,v,weight});
	}
	while(!pri_que.empty()){
		auto &data=pri_que.top();
		pri_que.pop();
		int u=get<0>(data),v=get<1>(data);
		if(visited[u]&&visited[v]){
			continue;
		}
		int idx=visited[u]==0?u:v;
		visited[idx]=1;
		idToVisit[idx]=visitOrder.size()-1;
		visited[idx]=1;
		for(int i=0;i<vert_num;i++){
			if(visited[i]==1){
				continue;
			}
			int weight=0;
			if(edge[idx][i]==1){
				for(auto u:candi[idx]){
					weight+=candiEdge[idx][u][i].size();
				}
			}else if(edge[i][idx]==1){
				for(auto u:candi[i]){
					weight+=candiEdge[i][u][idx].size();
				}
			}
			int u=i,v=idx;
			int us=candi[u].size(),vs=candi[v].size();
			if(us>vs){
				swap(u,v);
				swap(us,vs);
			}
			pri_que.push({u,v,weight});
		}
	}
}

void QGraph::tarjan(stack<int>& st,int u, int fa,vector<int>& dfn,vector<int>& low,vector<int>& visited,vector<vector<int>>& g,int& cnt) {
    dfn[u] = low[u] = ++cnt; // 对当前节点进行时间戳标记
    visited[u] = true; // 标记当前节点为已访问
	st.push(u);
    int child = 0; // 记录当前节点的儿子节点数
    for (int i = 0; i < g[u].size(); i++) {
        int v = g[u][i]; // 遍历u的邻居节点
        if (!visited[v]) {
            child++; // 记录当前节点的儿子节点数
            tarjan(st,v, u,dfn,low,visited,g,cnt); // 继续DFS遍历v节点
            low[u] = min(low[u], low[v]); // 更新当前节点的最早时间戳

            // 情况一：u是割点，如果满足以下两个条件之一，那么u是割点
            // （1）u是根节点，且u有两个或以上的儿子节点；
            // （2）u不是根节点，且存在儿子节点v，使得low[v] >= dfn[u]
            if (fa != -1 && low[v] >= dfn[u]) {
                isCutV[u]=1;
				stack<int> temp_st;
				while(st.top()!=u){
					temp_st.push(st.top());
					st.pop();
				}
				while(!temp_st.empty()){
					int vert=temp_st.top();
					if(low[vert]>=dfn[u]){
						cutChild[u].push_back(vert);
						cutErase[vert]=1;
					}
					//st.push(vert);
					temp_st.pop();
				}
            }
			// if ((fa == -1 && child > 1) || (fa != -1 && low[v] >= dfn[u])) {
            //     cout << "Node " << u << " is a cut point" << endl;
            // }

        } else if (v != fa) { // 如果v已经访问过，且不是u的父节点
            low[u] = min(low[u], dfn[v]); // 更新当前节点的最早时间戳
        }
    }
}

void QGraph::findCutVertex(){
	isCutV.assign(vert_num,0);
	cutChild.assign(vert_num,vector<int>());
	cutErase.assign(vert_num,0);
	vector<int> dfn(vert_num,0);
	vector<int> low(vert_num,0);
	vector<int> visited(vert_num,0);
	vector<vector<int>> g(vert_num,vector<int>());
	for(int i=0;i<vert_num;i++){
		for(int j=i+1;j<vert_num;j++){
			if(edge[i][j]==1||edge[j][i]==1){
				g[i].push_back(j);
				g[j].push_back(i);
			}
		}
	}
	int cnt=0;
	stack<int> st;
	tarjan(st,startVert,-1,dfn,low,visited,g,cnt);
	//cout<<"debug"<<endl;
	

/*
	for(int i=0;i<vert_num;i++){
		int p=visitOrder[i];
		if(!isCutV[p]){
			continue;
		}
		int val=dfn[p];
		for(int j=0;j<i;j++){
			int q=visitOrder[j];
			int val2=low[q];
			if(val==val2){
				isCutV[p]=0;
				break;
			}
		}
		if(!isCutV[p]){
			continue;
		}
		for(int j=i+1;j<vert_num;j++){
			int q=visitOrder[j];
			int val2=low[q];
			if(val==val2){
				cutChild[p].push_back(j);
			}
		}
	}
*/
}

void QGraph::printCut(){
	cout<<endl<<"-----------------------"<<endl;
	cout<<"cutVertex:"<<endl;
	for(int i=0;i<vert_num;i++){
		if(isCutV[i]){
			cout<<i<<" : ";
		}
		for(int j=0;j<cutChild[i].size();j++){
			cout<<cutChild[i][j]<<" ";
		}
		cout<<endl;
	}
	cout<<"-----------------------"<<endl;
}

void QGraph::printTree(){
	cout<<endl<<"-----------------------"<<endl;
	cout<<"Tree:"<<endl;
	int num=visitOrder.size();
	for(int i=0;i<num;i++){
		cout<<i<<"\t"<<visitOrder[i]<<"\t";
		cout<<"child:\t";
		for(int j=0;j<queryTree[i].childList.size();j++){
			cout<<queryTree[i].childList[j]<<" ";
		}
		cout<<endl<<"ntchild:"<<endl;;
		for(int j=0;j<queryTree[i].ntEdges[0].size();j++){
			cout<<queryTree[i].ntEdges[0][j]<<" ";
		}
		cout<<endl;
		for(int j=0;j<queryTree[i].ntEdges[1].size();j++){
			cout<<queryTree[i].ntEdges[1][j]<<" ";
		}
		cout<<endl;
	}
	cout<<"-----------------------"<<endl;
}

