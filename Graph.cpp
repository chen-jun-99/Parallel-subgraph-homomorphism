#include "Graph.h"
#include <fstream>

void Graph::readGraph(string filename,int hasdegree)
{
	edge_num = 0;
	vert_num = 0;

	//第一次读取顶点数目，边的数目，顶点度数
	ifstream fin(filename,ios::in);
	if(!fin){
		cout<<"open file error!"<<endl;
		return;
	}
	string title;
	fin>>title;
	// if(title!="dag"){
	// 	cout<<"file type error!"<<endl;
	// 	return;
	// }
	char c;
	int u,l,v,d;
	bool degreeflag=true;
	while(fin>>c){
		if(c=='v'){
			if(hasdegree){
				fin>>u>>l>>d;
			}else{
				fin>>u>>l;
			}
			vert_num++;
		}
		else if(c=='e'){
			if(degreeflag){
				in.initDegree(vert_num);
				out.initDegree(vert_num);
				label.assign(vert_num,0);
				degreeflag=false;
			}
			if(hasdegree){
				fin>>u>>v;
			}else{
				fin>>u>>v>>l;
			}
			edge_num++;
			out.degree[u]++;
			in.degree[v]++;

		}
	}
	fin.close();
	
	in.initCSR(vert_num, edge_num);
	out.initCSR(vert_num, edge_num);

	// in.degree.assign(vert_num,0);
	// out.degree.assign(vert_num,0);

	//第二次读顶点标签，读取边到csr
	fin.open(filename,ios::in);
	if(!fin){
		cout<<"open file error!"<<endl;
		return;
	}
	title;
	fin>>title;
	while(fin>>c){
		if(c=='v'){
			if(hasdegree){
				fin>>u>>l>>d;
			}else{
				fin>>u>>l;
			}
			label[u]=l;
		}
		else if(c=='e'){
			if(hasdegree){
				fin>>u>>v;
			}else{
				fin>>u>>v>>l;
			}
			out.insert(u, v);
			in.insert(v, u);

		}
	}
	fin.close();
}


//bfl
int Graph::h_in() {
	static int c = 0, r = rand();
	if (c >= (int)nodes.size() / D) {
		c = 0;
		r = rand();
	}
	c++;
	return r;
}

int Graph::h_out() {
	static int c = 0, r = rand();
	if (c >= (int)nodes.size() / D) {
		c = 0;
		r = rand();
	}
	c++;
	return r;
}

void Graph::dfs_in(int u) {
	nodes[u].vis = vis_cur;

	if (in.degree[u] == 0) {
		nodes[u].h_in = h_in() % (K * 32);
	}
	else {
		for (int i = 0; i < K; i++) {
			nodes[u].L_in[i] = 0;
		}

		for (int i = in.beg_pos[u]; i < in.beg_pos[u + 1]; i++) {
			int v = in.csr[i];
			if (nodes[v].vis != vis_cur) {
				dfs_in(v);
			}
			if (in.degree[v] == 0) {
				int hu = nodes[v].h_in;
				nodes[u].L_in[(hu >> 5) % K] |= 1 << (hu & 31);
			}
			else {
				for (int j = 0; j < K; j++) {
					nodes[u].L_in[j] |= nodes[v].L_in[j];
				}
			}
		}

		int hu = h_in();
		nodes[u].L_in[(hu >> 5) % K] |= 1 << (hu & 31);
	}
}

void Graph::dfs_out(int u) {
	nodes[u].vis = vis_cur;

	nodes[u].L_interval.first = cur++;

	if (out.degree[u] == 0) {
		nodes[u].h_out = h_out() % (K * 32);
	}
	else {
		for (int i = 0; i < K; i++) {
			nodes[u].L_out[i] = 0;
		}

		for (int i = out.beg_pos[u]; i < out.beg_pos[u + 1]; i++) {
			int v = out.csr[i];
			if (nodes[v].vis != vis_cur) {
				dfs_out(v);
			}
			if (out.degree[v] == 0) {
				int hu = nodes[v].h_out;
				nodes[u].L_out[(hu >> 5) % K] |= 1 << (hu & 31);
			}
			else {
				for (int j = 0; j < K; j++) {
					nodes[u].L_out[j] |= nodes[v].L_out[j];
				}
			}
		}

		int hu = h_out();
		nodes[u].L_out[(hu >> 5) % K] |= 1 << (hu & 31);
	}

	nodes[u].L_interval.second = cur;
}
void Graph::constructIndex() {
	nodes.assign(vert_num, node());
	vis_cur++;
	for (int u = 0; u < vert_num; u++) {
		//如果顶点u的出度为0，即自底向上遍历
		if (out.degree[u] == 0) {
			dfs_in(u);
		}
	}
	vis_cur++;
	cur = 0;
	for (int u = 0; u < vert_num; u++) {
		//如果顶点u的入度为0，即自顶向下遍历
		if (in.degree[u] == 0) {
			dfs_out(u);
		}
	}
}


bool Graph::reach(int u, int v,int tid) {

	if (nodes[u].L_interval.second < nodes[v].L_interval.second) {
		return false;
	}
	else if (nodes[u].L_interval.first <= nodes[v].L_interval.first) {
		return true;
	}

	if (in.degree[v] == 0) {
		return false;
	}
	if (out.degree[u] == 0) {
		return false;
	}
	if (out.degree[v] == 0) {
		if ((nodes[u].L_out[nodes[v].h_out >> 5] & (1 << (nodes[v].h_out & 31))) == 0) {
			return false;
		}
	}
	else {
		for (int i = 0; i < K; i++) {
			if ((nodes[u].L_out[i] & nodes[v].L_out[i]) != nodes[v].L_out[i]) {
				return false;
			}
		}
	}
	if (in.degree[u] == 0) {
		if ((nodes[v].L_in[nodes[u].h_in >> 5] & (1 << (nodes[u].h_in & 31))) == 0) {
			return false;
		}
	}
	else {
		for (int i = 0; i < K; i++) {
			if ((nodes[u].L_in[i] & nodes[v].L_in[i]) != nodes[u].L_in[i]) {
				return false;
			}
		}
	}

	for (int i = out.beg_pos[u]; i < out.beg_pos[u + 1]; i++) {
		int next = out.csr[i];
		if (nodes[next].thread_vis[tid] != thread_cur[tid]) {
			nodes[next].thread_vis[tid] = thread_cur[tid];
			if (reach(next, v,tid)) {
				return true;
			}
		}
		// if (nodes[next].vis != vis_cur) {
		// 	nodes[next].vis = vis_cur;
		// 	if (reach(next, v,tid)) {
		// 		return true;
		// 	}
		// }
	}

	return false;
}

bool Graph::canReach(int u, int v,int tid) {
	if(u==v){
		return false;
	}
	//vis_cur++;
	thread_cur[tid]++;
	return reach(u, v,tid);
}

void Graph::print() {
	cout << "graph info:" << endl;
	cout << "vert_num:" << vert_num << endl;
	for(int i=0;i<vert_num;i++){
		cout<<i<<"\t"<<label[i]<<endl;
	}
	cout << "edge_num:" << edge_num << endl;
	for(int i=0;i<vert_num;i++){
		cout<<i<<":\t";
		for(int j=out.beg_pos[i];j<out.beg_pos[i+1];j++){
			cout<<out.csr[j]<<"\t";
		}
		cout<<endl;
	}
	cout << endl;
}

bool Graph::hasEdge(int u,int v){
	// if(u==v){
	// 	return true;
	// }
	for(int i=out.beg_pos[u];i<out.beg_pos[u+1];i++){
		if(out.csr[i]==v){
			return true;
		}
	}
	return false;
}