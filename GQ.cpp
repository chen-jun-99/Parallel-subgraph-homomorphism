#include "GQ.h"
#include <algorithm>

// void GQ::constructGraph(){

// 	in.initDegree(vert_num);
// 	out.initDegree(vert_num);

// 	for(int i=0;i<THREADNUM;i++){
// 		edge_num+=edge[i].size();
// 		for(const auto &e:edge[i]){
// 			out.degree[e.first]++;
// 			in.degree[e.second]++;
//     	}
// 	}
    
	
// 	in.initCSR(vert_num, edge_num);
// 	out.initCSR(vert_num, edge_num);

// 	in.degree.assign(vert_num,0);
// 	out.degree.assign(vert_num,0);

// 	for(int i=0;i<THREADNUM;i++){
// 		for(const auto &e:edge[i]){
// 			out.insert(e.first, e.second);
// 			in.insert(e.second, e.first);
//     	}
// 	}
// }
void GQ::constructGraph(){
	for(int i=0;i<BFLTHREADNUM;i++){
		edge_num+=edge[i].size();
		for(const auto& e:edge[i]){
			int u=e.u,v=e.v,q=e.q,p=e.p;
			candiEdge[p][u][q].push_back(v);
			re_candiEdge[q][v][p].push_back(u);
		}
	}
}

void GQ::print()
{
	cout << "graph info:" << endl;
	cout << "vert_num:" << vert_num << endl;
	cout << "edge_num:" << edge_num << endl;
	for(int i=0;i<candi.size();i++){
		cout<<i<<":\t"<<candi[i].size()<<endl;
	}
	cout << endl;
}

void GQ::construct_Graph2(){
	vert_num=0;
	for(int i=0;i<candi.size();i++){
		vector<int> temp;
		for(int j=0;j<candi[i].size();j++){
			if(get_pruned(i,candi[i][j])){
				continue;
			}
			ump[candi[i][j]]=vert_num++;
			temp.push_back(candi[i][j]);
		}
		//vert_num+=temp.size();
		candi[i].swap(temp);
	}

}

void GQ::initCR(QGraph& queryG){

	startVert=queryG.startVert;
	visitOrder=queryG.visitOrder;
	idToVisit=queryG.idToVisit;

#ifdef MATCH_FENJIE
	myCR.assign(candi.size(),CRI());	
	for(int i=queryG.visitOrder.size()-1;i>=0;i--){
		int q=queryG.visitOrder[i];
		auto& node=queryG.queryTree[i];
		int parent=node.parent;
		int pid=visitOrder[parent];
		for(int k=0;k<candi[pid].size();k++){
			int u=candi[pid][k];
			if(myCR[pid].cardinality[u]==0){
				myCR[pid].cardinality[u]=1;
			}
			unsigned long long score=0;
			for(auto it=candiEdge[pid][u][q].begin();it!=candiEdge[pid][u][q].end();it++){
				int v=*it;
				if(myCR[q].cardinality[v]==0){
					myCR[q].cardinality[v]=1;
				}
				score+=myCR[q].cardinality[v];
			}
			if(score==0){
				score=1;
			}
			myCR[pid].cardinality[u]*=score;
			if(myCR[pid].cardinality[u]<0){
				cout<<"溢出"<<endl;
			}
		}
	}
#endif
}
/*
void GQ::explore(QGraph& queryG){

	unordered_map<int,bool> visit_status;
	unordered_map<int,bool> accept_status;
	for(int cur_node=1;cur_node<q_vert_num;cur_node++){
		int parent=queryG.queryTree[cur_node].parent;
		myCR[cur_node].id=queryG.queryTree[cur_node].vertexId;
		if(parent==-1){
			continue;
		}
		for(int mybeg=0;mybeg<candi[myCR[parent].id].size();mybeg++){
			int pNode=candi[myCR[parent].id][mybeg];//父节点对应的一个候选节点
			if(pNode==-1){
				//按理不会执行
				cout<<"父节点不存在"<<endl;
				continue;
			}

			vector<int> collector;
			collector.reserve(50);

			auto& neighbors=candiEdge[myCR[parent].id][pNode][queryG.queryTree[cur_node].vertexId];
			if(neighbors.empty()){
				//按理不会执行
				cout<<"neighbors为空"<<endl;
				continue;
			}
			
			for(auto it=neighbors.begin();it!=neighbors.end();it++){
				if( visit_status[*it] != true){
					visit_status[*it] = true;
					//accept_status[*it] = false;

					//myCR[cur_node].id=queryG.queryTree[cur_node].vertexId;
					accept_status[*it]=true;
				}else if(accept_status[*it]==false){
					//该节点访问过并且状态为false
					continue;
				}

				collector.push_back(*it);
			}

			if(!collector.empty()){
				myCR[cur_node].edge_cands.push_back({pNode, collector});
				collector.clear();
				continue;
			}else{
				//collector为空，需删除父节点
				//按道理不会执行
				cout<<"collector不空"<<endl;
				exit(1);
			}
		}
	}

	//生成非树边，同上
	unordered_map<int,bool> visit_status_2;
	unordered_map<int,bool> accept_status_2;
	for(int cur_node=0;cur_node<q_vert_num;cur_node++){
		for(int i=0;i<queryG.queryTree[cur_node].ntEdgeCount;i++){
			int target_id=queryG.visitTreeOrder[queryG.queryTree[cur_node].ntEdges[i]];
			if(target_id>cur_node){
				//之后的点不考虑
				continue;
			}
			std::vector<Data> xnte_cands;
			xnte_cands.reserve(10000);

			for(int mybeg=0;mybeg<candi[myCR[cur_node].id].size();mybeg++){
				int pNode=candi[myCR[cur_node].id][mybeg];//父节点对应的一个候选节点
				if(pNode==-1){
					//按理不会执行
					cout<<"父节点不存在-2"<<endl;
					continue;
				}

				vector<int> collector;
				collector.reserve(50);

				auto& neighbors=candiEdge[myCR[cur_node].id][pNode][queryG.queryTree[target_id].vertexId];
				if(neighbors.empty()){
					//按理不会执行
					cout<<"neighbors为空-2"<<endl;
					continue;
				}
				
				for(auto it=neighbors.begin();it!=neighbors.end();it++){
					if( visit_status[*it] != true){
						visit_status[*it] = true;
						//accept_status[*it] = false;

						//myCR[cur_node].id=queryG.queryTree[cur_node].vertexId;
						accept_status[*it]=true;
					}else if(accept_status[*it]==false){
						//该节点访问过并且状态为false
						continue;
					}

					collector.push_back(*it);
				}

				if(!collector.empty()){
					xnte_cands.push_back(std::pair<int, vector<int> >(pNode, collector));
					collector.clear();
					continue;
				}else{
					//collector为空，需删除父节点
					//按道理不会执行
					cout<<"collector不空"<<endl;
					exit(1);
				}
			}
			std::sort(xnte_cands.begin(), xnte_cands.end(),DataCompare());
			myCR[cur_node].nte_cands.insert(std::pair<int, std::vector<Data> >(target_id, xnte_cands));
			xnte_cands.clear();
		}
		
	}

	//edge_cands,nte_edge排序
	for(int curr_queryNode = 0; curr_queryNode < queryG.queryTree.size(); ++curr_queryNode){
		std::sort(myCR[curr_queryNode].edge_cands.begin(), myCR[curr_queryNode].edge_cands.end(),DataCompare());
		//std::sort(myCR[curr_queryNode].nte_cands.begin(), myCR[curr_queryNode].nte_cands.end(),DataCompare);
	}

	//基数计算
	for(int curr_queryNode = queryG.queryTree.size()-1; curr_queryNode > 0; --curr_queryNode){
		// get the parent node
		int parent = queryG.queryTree[curr_queryNode].parent;
		for(auto edgItr = myCR[curr_queryNode].edge_cands.begin(); edgItr != myCR[curr_queryNode].edge_cands.end(); ++edgItr){
			int score = 0;
			if(edgItr->second.empty()){continue;}
			for(auto sItr = edgItr->second.begin(); sItr != edgItr->second.end(); ++sItr){
				if(myCR[curr_queryNode].cardinality[*sItr] == 0){
					myCR[curr_queryNode].cardinality[*sItr] = 1;
				}
				score += myCR[curr_queryNode].cardinality[*sItr];
			}
			if(myCR[parent].cardinality[edgItr->first]==0){
				myCR[parent].cardinality[edgItr->first]=1;
			}
			myCR[parent].cardinality[edgItr->first] *= score;
		}
	}
	
	// //生成匹配序列
	// for(int i = 0; i< queryG.queryTree.size(); ++i){
	// 	queryMatchingSequence[i] = queryG.queryTree[i].id;
	// 	queryMatchingSequence_inv[queryG.queryTree[i].id] = i;
	// }
}
*/
void GQ::prepare(){
	std::vector<std::pair<long long,int> > works;		//<候选节点权重，候选节点>
	works.reserve(candi[startVert].size());
	threshold = 0;
	totalwork = 0;
	for(auto itr = candi[startVert].begin(); itr != candi[startVert].end(); ++itr){
		if(myCR[startVert].cardinality[*itr] != 0){
			works.push_back({myCR[startVert].cardinality[*itr], *itr});
			totalwork += myCR[startVert].cardinality[*itr];
		}
	}
	//sort(works.begin(), works.end(),[](const pair<int,int>& A,const pair<int,int>& B){return A.first>B.first;});
	threshold = totalwork/(THRESHORD_RATIO*MATCHING_THREAD_NUM);

	prepare_work(work_units, 0, preset, works, totalwork);
}

void GQ::prepare_work(std::vector<std::vector<int>>& work_units, int pi, std::vector<int>& pre, std::vector<std::pair<long long,int> >& works, unsigned long long int card_old) {
	unsigned long long int total = 0;	//works中总权重
	for (int i = 0; i < works.size(); i++) {
		total += works[i].first;
	}
	for (int i = 0; i < works.size(); i++) {
		//long long x=works[i].first  * card_old;
		double rate=((double)card_old)/total;
		long long myWork = works[i].first  * rate;	//当前候选节点权重占比
		if (myWork <= threshold) {
			pre.push_back(works[i].second);
			work_units.push_back(pre);
			pre.pop_back();
		}
		else {
			int p=visitOrder[pi];
			int q=visitOrder[pi+1];
			int u = works[i].second;
			auto inters=candiEdge[p][u][q];

			std::vector<std::pair<long long, int> > nextwork;
			for (int i = 0; i < inters.size(); i++) {
				nextwork.push_back(std::pair<long long, int>(myCR[q].cardinality[inters[i]], inters[i]));
			}
			pre.push_back(works[i].second);
			prepare_work(work_units, pi + 1, pre, nextwork, myWork);
			pre.pop_back();
		}
	}
}

vector<int> intersection(std::vector<int> &v1, std::vector<int> &v2)
{
	vector<int> v3;
	//sort(v1.begin(), v1.end());
	//sort(v2.begin(), v2.end());
	set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(v3));
	return v3;
}
/*
void GQ::subgraphSearch(int thid,QGraph& queryG){
	int matching_node = myTCB[thid].curr_query_node;
	numberofRecursiveCalls[thid]++;

	if(myTCB[thid].curr_query_node == q_vert_num){
		numberofEmbeddings[thid]++;
		return;
	}
	QueryNode& nectree_u = queryG.queryTree[matching_node];

	vector<int> inters;
	if(nectree_u.parent!=-1){
		// the mapping of the parent in the current embedding 
		// it is in position equal to id of parent
		int lookup_value = myTCB[thid].embedding[nectree_u.parent];

		//	List of candidates corresponding to parent query tree in tree 
		auto candidateListIterator = std::lower_bound(myCR[matching_node].edge_cands.begin(), myCR[matching_node].edge_cands.end(), lookup_value,DataCompare());

		if(candidateListIterator == myCR[matching_node].edge_cands.end()){
			//printf("Candidate List not found");
			return;
		}
		if(candidateListIterator->first!=lookup_value){
			cout<<"not match2"<<endl;
		}
		std::vector<int> inters_temp = candidateListIterator->second;//根据树边找到的候选集
		inters=inters_temp;
	}
	
	//form a vector with intersection of children of each previously visited nodes
	for(int adjItr = 0; adjItr < nectree_u.ntEdgeCount; ++adjItr){
		int nteId = queryG.visitTreeOrder[nectree_u.ntEdges[adjItr]];
		if(nectree_u.id <=  nteId){ continue;}//只考虑序号在该节点之前的非树边
	
		int new_lookup = myTCB[thid].embedding[nteId];
		// Find the corresponding neighbors of that neighbors
		auto mItr = myCR[nectree_u.id].nte_cands.find(nteId);
		if(mItr != myCR[nectree_u.id].nte_cands.end()){
			auto vItr = std::lower_bound(mItr->second.begin(), mItr->second.end(), new_lookup,DataCompare());
			if(vItr != mItr->second.end()){
				if(vItr->first!=new_lookup){
					cout<<"not match1"<<endl;
				}
				std::vector<int> x = vItr->second;
				if(nectree_u.parent==-1){
					inters=x;
				}else{
					inters = intersection(x, inters);
				}
			}
		}
	}
	
	// Go over the whole children list
	for(auto candidateIterator = inters.begin(); candidateIterator != inters.end(); ++candidateIterator){
		//check if task is completed
		if(myTCB[thid].go == false){
			return;
		}

		if(*candidateIterator == -1){
			continue;
		}

		// check if the given vertex is already matched
		int i = 0;
		for( ; i < myTCB[thid].curr_query_node; ++i){
			if(myTCB[thid].embedding[i] == *candidateIterator){
				break;
			}
		}
		if(i != myTCB[thid].curr_query_node){ 
			continue;
		}
		
		// add the node to embedding and call recursively
		myTCB[thid].embedding[myTCB[thid].curr_query_node] = *candidateIterator;

		myTCB[thid].curr_query_node++;

		subgraphSearch(thid,queryG);
		myTCB[thid].curr_query_node--;
	}
}
*/
void threadTimePrint(timeType st,timeType end){
    //cout<<str<<"的时间是："<<(double)(end-st)/(CLOCKS_PER_SEC/1000)<<" ms."<<endl;
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - st); // 计算程序运行时间
    std::cout << elapsed_time.count() / 1000000.0 << " seconds" << std::endl;
}
double threadgetTime(timeType st,timeType end){
    //return (double)(end-st)/(CLOCKS_PER_SEC/1000);
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - st);
    return elapsed_time.count() / 1000.0;
}
void GQ::my_subgraphSearch(int thid,QGraph& queryG,int weight){
	match_et=std::chrono::steady_clock::now();
	if(threadgetTime(match_st,match_et)>MAX_TIME){
		thread_stop=1;
	}
	if(thread_stop==1){
		return ;
	}
	int matching_node = myTCB[thid].curr_query_node;
	//numberofRecursiveCalls[thid]++;

	int cur_id=visitOrder[matching_node];

	vector<int> inters=candi[cur_id];
	// sort(inters.begin(),inters.end());
	//用于拓扑排序
	if(ORDERTYPE==TOP){
		for(int i=0;i<matching_node;i++){
			int p=visitOrder[i];
			if(queryG.edge[p][cur_id]==0){
				continue;
			}
			int pNode=myTCB[thid].embedding[i];
			vector<int> x=candiEdge[p][pNode][cur_id];
			inters=intersection(inters,x);
			if(inters.empty()){
				return;
			}
		}
	}else if(ORDERTYPE==JO||ORDERTYPE==RI||ORDERTYPE==QUICKSI){
		for(int i=0;i<matching_node;i++){
			int p=visitOrder[i];
			if(queryG.edge[p][cur_id]==1){
				int pNode=myTCB[thid].embedding[i];
				vector<int> x=candiEdge[p][pNode][cur_id];
				inters=intersection(inters,x);
			}
			if(queryG.edge[cur_id][p]==1){
				int pNode=myTCB[thid].embedding[i];
				vector<int> x=re_candiEdge[p][pNode][cur_id];
				inters=intersection(inters,x);
			}
			if(inters.empty()){
				return;
			}
		}
	}else if(ORDERTYPE==JO2){
		auto& pre0=queryG.pre[0][matching_node];
		auto& pre1=queryG.pre[1][matching_node];
		for(int i=0;i<pre0.size();i++){
			int p=pre0[i];
			int id=idToVisit[p];
			int pNode=myTCB[thid].embedding[id];
			auto x=candiEdge[p][pNode][cur_id];
			inters=intersection(inters,x);
			if(inters.empty()){
				return;
			}
		}
		for(int i=0;i<pre1.size();i++){
			int q=pre1[i];
			int id=idToVisit[q];
			int qNode=myTCB[thid].embedding[id];
			auto x=re_candiEdge[q][qNode][cur_id];
			inters=intersection(inters,x);
			if(inters.empty()){
				return;
			}
		}
	}else if(ORDERTYPE==KRU||ORDERTYPE==MYORDER||ORDERTYPE==CECI||ORDERTYPE==CUTORDER){
		auto& node=queryG.queryTree[matching_node];
		int parent=node.parent;
		if(parent!=-1){
			int p=visitOrder[parent];
			int pNode=myTCB[thid].embedding[parent];
			if(queryG.edge[p][cur_id]==1){
				inters=candiEdge[p][pNode][cur_id];
			}else{
				inters=re_candiEdge[p][pNode][cur_id];
			}
			if(inters.empty()){
				return;
			}
		}
		for(int i=0;i<node.ntEdges[0].size();i++){
			int p=node.ntEdges[0][i];
			int id=idToVisit[p];
			int pNode=myTCB[thid].embedding[id];
			auto x=candiEdge[p][pNode][cur_id];
			inters=intersection(inters,x);
			if(inters.empty()){
				return;
			}
		}
		for(int i=0;i<node.ntEdges[1].size();i++){
			int q=node.ntEdges[1][i];
			int id=idToVisit[q];
			int qNode=myTCB[thid].embedding[id];
			auto x=re_candiEdge[q][qNode][cur_id];
			inters=intersection(inters,x);
			if(inters.empty()){
				return;
			}
		}
	}
	if(myTCB[thid].curr_query_node == visitOrder.size()-1){
		//是最后个匹配节点
		int sumEmb=0;
		for(int i=0;i<inters.size();i++){
			int j=0;
			for(;j<myTCB[thid].curr_query_node;j++){
				if(myTCB[thid].embedding[j]==inters[i]){
					break;;
				}
			}
			if(j!=myTCB[thid].curr_query_node){
				continue;
			}
			if(ORDERTYPE==CUTORDER){
				if(queryG.isCutV[cur_id]==2){
					int temp=cut_candi_weight[cur_id][inters[i]];
					sumEmb+=(weight*temp);
				}else{
					sumEmb+=weight;
				}
			}else{
				sumEmb++;
			}
		}
		numberofEmbeddings[thid]+=sumEmb;
		if(SET_LIMIT==true){
			addEmbedding(sumEmb);
			if(getEmbedding()>=MAX_MATCHING_NUM){
				thread_stop=1;
			}
		}
		return;
	}
#ifdef WORKSTEAL
	if(inters.size()>=WIDTH||matching_node<visitOrder.size()-DEPTH){
		int free_thread=getFreeThread();
		if(free_thread>0&&queEmpty()){
			vector<int> matched,nextCandi;
			matched.assign(myTCB[thid].embedding.begin(),myTCB[thid].embedding.begin()+myTCB[thid].curr_query_node);
			int len=inters.size()/(free_thread+1);
			if(len==0){
				for(int i=1;i<inters.size();i++){
					nextCandi.assign(1,inters[i]);
					quePut(matched,nextCandi);
				}
			}else{
				for(int i=1;i<=free_thread;i++){
					int left=i*len,right=(i+1)*len;
					if(i==free_thread){
						right=inters.size();
					}
					nextCandi.assign(inters.begin()+left,inters.begin()+right);
					quePut(matched,nextCandi);
				}
				inters.assign(inters.begin(),inters.begin()+len);
			}
		}
	}
	
#endif
	
	for(int i=0;i<inters.size();i++){
		if(thread_stop==1){
			break;
		}
		int j=0;
		for(;j<myTCB[thid].curr_query_node;j++){
			if(myTCB[thid].embedding[j]==inters[i]){
				break;
			}
		}
		if(j!=myTCB[thid].curr_query_node){
			continue;
		}
		if(ORDERTYPE==CUTORDER&&queryG.isCutV[cur_id]==2){
			int temp=cut_candi_weight[cur_id][inters[i]];
			if(temp==0){
				continue;
			}
			myTCB[thid].embedding[myTCB[thid].curr_query_node]=inters[i];
			myTCB[thid].curr_query_node++;

			my_subgraphSearch(thid,queryG,weight*temp);
			myTCB[thid].curr_query_node--;
		}else{
			myTCB[thid].embedding[myTCB[thid].curr_query_node]=inters[i];
			myTCB[thid].curr_query_node++;

			my_subgraphSearch(thid,queryG,weight);
			myTCB[thid].curr_query_node--;
		}

	}
}
void GQ::my_subgraphSearch_steal(int thid,QGraph& queryG,vector<int> inters){
	if(thread_stop==1){
		return;
	}
	int matching_node = myTCB[thid].curr_query_node;
	int cur_id=visitOrder[matching_node];
	for(int i=0;i<inters.size();i++){
		if(thread_stop==1){
			break;
		}
		int j=0;
		for(;j<myTCB[thid].curr_query_node;j++){
			if(myTCB[thid].embedding[j]==inters[i]){
				break;
			}
		}
		if(j!=myTCB[thid].curr_query_node){
			continue;
		}
		myTCB[thid].embedding[myTCB[thid].curr_query_node]=inters[i];
		myTCB[thid].curr_query_node++;

		my_subgraphSearch(thid,queryG,1);
		myTCB[thid].curr_query_node--;
		
	}
}

void match_work(GQ *gq,int tid,QueryQue<int,int>& matchQue,QGraph& queryG){
	timeType st=std::chrono::steady_clock::now();
	timeType et;
	gq->match_st=st;
#ifdef BALANCE
	int left=gq->work_idx[tid].first,right=gq->work_idx[tid].second;
	auto& que=gq->candi[gq->startVert];
	for(int i=left;i<right;i++){
		int data=que[i];
		gq->myTCB[tid].embedding[0]=data;
		gq->myTCB[tid].curr_query_node++;
		gq->my_subgraphSearch(tid,queryG,1);
		gq->myTCB[tid].curr_query_node--;
	}
#else
	while(true){
		int data=matchQue.get();
		if(data==-1){
			break;
		}
		if(gq->thread_stop==1){
			continue;	//此处continue是为了把matchQue中的数据读完防止阻塞
		}
		gq->myTCB[tid].embedding[0]=data;
		gq->myTCB[tid].curr_query_node++;
		gq->my_subgraphSearch(tid,queryG,1);
		gq->myTCB[tid].curr_query_node--;
	}
#endif
	et=std::chrono::steady_clock::now();
	//cout<<"thread:"<<tid<<":	";
	//threadTimePrint(st,et);
	gq->threadTime[tid]=threadgetTime(st,et);
}

void match_work_cutV(GQ *gq,int tid,QueryQue<int,int>& matchQue,QGraph& queryG){
	gq->match_st=std::chrono::steady_clock::now();
	while(true){
		int data=matchQue.get();
		if(data==-1){
			break;
		}
		if(gq->thread_stop==1){
			continue;	//此处continue是为了把matchQue中的数据读完防止阻塞
		}
		gq->myTCB[tid].embedding[0]=data;
		gq->myTCB[tid].curr_query_node++;
		int num=gq->numberofEmbeddings[tid];
		gq->my_subgraphSearch(tid,queryG,1);
		int result=gq->numberofEmbeddings[tid]-num;
		int v=gq->visitOrder[0];
		gq->cut_candi_weight[v][data]=result;
		gq->myTCB[tid].curr_query_node--;
	}
}

void match_work_fenjie(GQ *gq,int tid,QueryQue<int,int>& matchQue,QGraph& queryG){
	timeType st=std::chrono::steady_clock::now();
	timeType et;
	gq->match_st=st;
#ifdef BALANCE
	int left=gq->work_idx[tid].first,right=gq->work_idx[tid].second;
	for(int i=left;i<right;i++){
		auto& work=gq->work_units[i];
		for(int j=0;j<work.size();j++){
			gq->myTCB[tid].embedding[j]=work[j];
		}
		gq->myTCB[tid].curr_query_node=work.size();
		gq->my_subgraphSearch(tid,queryG,1);
	}
#else
	while(true){
		int idx=gq->get_idx();
		if(idx==-1){
			break;
		}
		if(gq->thread_stop==1){
			break;
		}
		auto& work=gq->work_units[idx];
		for(int i=0;i<work.size();i++){
			gq->myTCB[tid].embedding[i]=work[i];
		}
		gq->myTCB[tid].curr_query_node=work.size();
		gq->my_subgraphSearch(tid,queryG,1);
	}
#endif

	et=std::chrono::steady_clock::now();
	//cout<<"thread:"<<tid<<":	";
	//threadTimePrint(st,et);
	gq->threadTime[tid]=threadgetTime(st,et);
}

void match_work_stealing(GQ *gq,int tid,QueryQue<int,int>& matchQue,QGraph& queryG){
	timeType st=std::chrono::steady_clock::now();
	timeType et;
	gq->match_st=st;
	bool stealStart=false;
#ifdef BALANCE
	int left=gq->work_idx[tid].first,right=gq->work_idx[tid].second;
	auto& que=gq->candi[gq->startVert];
	for(int i=left;i<right;i++){
		int data=que[i];
		gq->myTCB[tid].embedding[0]=data;
		gq->myTCB[tid].curr_query_node++;
		gq->my_subgraphSearch(tid,queryG,1);
		gq->myTCB[tid].curr_query_node--;
	}
	gq->setFreeThread(tid,1);

	while(true){
		if(gq->queEmpty()&&gq->getFreeThread()==MATCHING_THREAD_NUM){
			break;
		}else if(gq->queEmpty()){
			//std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
		else{
			vector<int> matched,nextCandi;
			gq->queGet(matched,nextCandi);
			gq->setFreeThread(tid,0);
			for(int i=0;i<matched.size();i++){
				gq->myTCB[tid].embedding[i]=matched[i];
			}
			gq->myTCB[tid].curr_query_node=matched.size();
			gq->my_subgraphSearch_steal(tid,queryG,std::move(nextCandi));
			gq->setFreeThread(tid,1);
		}
	}
#else
	while(true){
		if(stealStart){
			if(gq->queEmpty()&&gq->getFreeThread()==MATCHING_THREAD_NUM){
				break;
			}else if(gq->queEmpty()){
				//std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
			else{
				vector<int> matched,nextCandi;
				gq->queGet(matched,nextCandi);
				gq->setFreeThread(tid,0);
				for(int i=0;i<matched.size();i++){
					gq->myTCB[tid].embedding[i]=matched[i];
				}
				gq->myTCB[tid].curr_query_node=matched.size();
				gq->my_subgraphSearch_steal(tid,queryG,std::move(nextCandi));
				gq->setFreeThread(tid,1);
			}
		}else{
			int data=matchQue.get();
			if(data==-1){
				stealStart=true;
				gq->setFreeThread(tid,1);
				continue;
			}
			if(gq->thread_stop==1){
				continue;	//此处continue是为了把matchQue中的数据读完防止阻塞
			}
			gq->myTCB[tid].embedding[0]=data;
			gq->myTCB[tid].curr_query_node++;
			gq->my_subgraphSearch(tid,queryG,1);
			gq->myTCB[tid].curr_query_node--;
		}
	}
#endif

	et=std::chrono::steady_clock::now();
	//cout<<"thread:"<<tid<<":	";
	//threadTimePrint(st,et);
	gq->threadTime[tid]=threadgetTime(st,et);
}

void dfs_cutV(GQ& gq,QGraph& queryG,int v){
	if(queryG.isCutV[v]!=1){
		return;
	}
	auto& child=queryG.cutChild[v];
	vector<int> visited(queryG.vert_num,1);
	visited[v]=0;
	for(int i=0;i<child.size();i++){
		int q=child[i];
		if(queryG.isCutV[q]==1){
			dfs_cutV(gq,queryG,q);
		}
		visited[q]=0;
	}
	queryG.startVert=v;
	queryG.generateCut(gq.candi,visited);
	//queryG.printTree();
	gq.initCR(queryG);
	gq.parl_matching_cutV(queryG);
	queryG.isCutV[v]=2;
}
void GQ::match_cutV(QGraph& queryG){
	for(int i=0;i<q_vert_num;i++){
		if(queryG.isCutV[i]==1){
			dfs_cutV(*this,queryG,i);
		}
	}
}
void GQ::parl_matching_cutV(QGraph& queryG){
	myTCB.assign(MATCHING_THREAD_NUM,thread_status(q_vert_num));

	vector<thread> matchThread;
	QueryQue<int,int> matchQue;
	for (int i = 0; i < MATCHING_THREAD_NUM; i++) {
		matchThread.push_back(thread(match_work_cutV,this,i,std::ref(matchQue),std::ref(queryG)));
	}
	for(int i=0;i<candi[startVert].size();i++){
		matchQue.put(candi[startVert][i]);
	}
	//发送结束查询信号
	for(int i=0;i<MATCHING_THREAD_NUM;i++){
		matchQue.put(-1);
	}
    for (int i = 0; i < MATCHING_THREAD_NUM; i++) {
		matchThread[i].join();
	}
}
void GQ::parl_matching(QGraph& queryG){
	myTCB.assign(MATCHING_THREAD_NUM,thread_status(q_vert_num));

	vector<thread> matchThread;
	QueryQue<int,int> matchQue;

	threadTime.assign(MATCHING_THREAD_NUM,0);

#ifdef MATCH_FENJIE
	for (int i = 0; i < MATCHING_THREAD_NUM; i++) {
		matchThread.push_back(thread(match_work_fenjie,this,i,std::ref(matchQue),std::ref(queryG)));
	}
#else

	#ifdef WORKSTEAL
		thread_free.assign(MATCHING_THREAD_NUM,0);
		for (int i = 0; i < MATCHING_THREAD_NUM; i++) {
			matchThread.push_back(thread(match_work_stealing,this,i,std::ref(matchQue),std::ref(queryG)));
		}
	#else
		for (int i = 0; i < MATCHING_THREAD_NUM; i++) {
			matchThread.push_back(thread(match_work,this,i,std::ref(matchQue),std::ref(queryG)));
		}
	#endif
#ifndef BALANCE
	for(int i=0;i<candi[startVert].size();i++){
		matchQue.put(candi[startVert][i]);
	}
	//发送结束查询信号
	for(int i=0;i<MATCHING_THREAD_NUM;i++){
		matchQue.put(-1);
	}
#endif
#endif
    for (int i = 0; i < MATCHING_THREAD_NUM; i++) {
		matchThread[i].join();
	}
}

void GQ::workDistribute(){
	long long workload=0;
	int size=0;
#ifdef MATCH_FENJIE
	size=work_units.size();
	/*workload=totalwork/MATCHING_THREAD_NUM;
	int tid=0;
	int left=0;
	while(tid<MATCHING_THREAD_NUM){
		if(tid==MATCHING_THREAD_NUM-1){
			work_units[tid].push_back({left,size})
		}
		long long curwork=0;
		int right=left;
		while(right<size&&curwork<workload){
			curwork+=
		}
	}*/
#else
	size=candi[startVert].size();
#endif
	workload=size/MATCHING_THREAD_NUM;
	if(workload==0){
		for(int i=0;i<size;i++){
			work_idx.push_back({i,i+1});
		}
		return;
	}
	int idx=0;
	while(idx<MATCHING_THREAD_NUM){
		int left=idx*workload,right=(idx+1)*workload;
		if(idx==MATCHING_THREAD_NUM-1){
			right=size;
		}
		work_idx.push_back({left,right});
		idx++;
	}
}


void GQ::printInfo(long long &sum){
	cout<<"搜索结果："<<endl;
	cout<<"线程号\t\t匹配数\t\t循环次数\n";
	sum=0;
	for(int i=0;i<MATCHING_THREAD_NUM;i++){
		sum+=numberofEmbeddings[i];
		cout<<i<<"\t\t"<<numberofEmbeddings[i]<<"\t\t"<<numberofRecursiveCalls[i]<<endl;
	}
	cout<<"总匹配数："<<sum<<endl;
}

int GQ::get_mseq(int p,int u,int v){
	//mseq_mtx[p].lock();
	std::shared_lock<MyMutex> lock(mseq_mtx);
	auto it=mseq.find(u);
	if(it==mseq.end()){
		return 0;
	}
	auto it2=mseq[u].find(v);
	if(it2!=mseq[u].end()){
		return it2->second;
	}else{
		return 0;
	}
	return 0;
	//mseq_mtx[p].unlock();
}

void GQ::set_mseq(int p,int u,int v,int val){
	//mseq_mtx[p].lock();
	std::unique_lock<MyMutex> lock(mseq_mtx);
	mseq[u][v]=val;
	//mseq_mtx[p].unlock();
}

bool GQ::get_pruned(int p,int u){
	//pruned_mtx.lock();
	std::shared_lock<MyMutex> lock(pruned_mtx);
	//std::lock_guard<MyMutex> lock(pruned_mtx);
	// bool ans;
	// ans=isPruned[u];
	// return ans;

	auto it=visPruned[p].find(u);
	if(it!=visPruned[p].end()){
		return true;
	}
	return false;

	// if(isPruned[u]==1){
	// 	return true;
	// }
	// return false;
	//pruned_mtx.unlock();
}
void GQ::set_pruned(int p,int u,bool val){
	//pruned_mtx.lock();
	std::unique_lock<MyMutex> lock(pruned_mtx);
	//std::lock_guard<MyMutex> lock(pruned_mtx);
	//isPruned[u]=val;
	//isPruned.insert(u);
	visPruned[p][u]=true;
	//pruned_mtx.unlock();
}

int GQ::get_idx(){
	std::unique_lock<MyMutex> lock(idx_mtx);
	if(units_idx>=work_units.size()){
		return -1;
	}
	return units_idx++;
}

void GQ::addEmbedding(int num){
	std::unique_lock<MyMutex> lock(emb_mtx);
	totalEmbeddings+=num;
}
long long GQ::getEmbedding(){
	std::shared_lock<MyMutex> lock(emb_mtx);
	return totalEmbeddings;
}

int GQ::getFreeThread(){
	std::shared_lock<MyMutex> lock(free_mtx);
	int res=0;
	for(int i=0;i<thread_free.size();i++){
		if(thread_free[i]==1){
			res++;
		}
	}
	return res;
}

void GQ::setFreeThread(int idx,int val){
	std::unique_lock<MyMutex> lock(free_mtx);
	thread_free[idx]=val;
}

bool GQ::queEmpty(){
	std::shared_lock<MyMutex> lock(que_mtx);
	if(que_stealed.empty()){
		return true;
	}
	return false;
}

void GQ::quePut(vector<int> matched,vector<int> nextCandi){
	std::unique_lock<MyMutex> lock(que_mtx);
	que_stealed.push({std::move(matched),std::move(nextCandi)});
}

void GQ::queGet(vector<int> &matched,vector<int> &nextCandi){
	std::unique_lock<MyMutex> lock(que_mtx);
	if(que_stealed.empty()){
		return;
	}
	auto data=que_stealed.front();
	matched=data.matched;
	nextCandi=data.nextCandi;
	que_stealed.pop();
}

