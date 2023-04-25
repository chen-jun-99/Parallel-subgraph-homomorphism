#include "my_algorithm.h"


//度过滤
bool DF(Graph& dataG,QGraph& queryG,int di,int qi){
    if(queryG.degree[qi]>dataG.out.degree[di]){
        return true;
    }
    return false;
}

//邻域标签过滤
void NF(){
    
}
//按照标签找出查询图中每个节点在数据图中的候选节点
void initGQ(Graph& dataG,QGraph& queryG,GQ& gq){
    unordered_map<int,int> label_to_vert;
    for(int i=0;i<queryG.vert_num;i++){
        int qlabel=queryG.label[i];
        label_to_vert[qlabel]=i;
    }
   
    for(int i=0;i<dataG.vert_num;i++){
        int ulabel=dataG.label[i];
        //
        if(label_to_vert.find(ulabel)==label_to_vert.end()){
            continue;
        }
        //gq.addVert(i);
        
        int idx=label_to_vert[ulabel];//查询图顶点索引

        // if(DegreeFilter){
        //     //如果开启度过滤器
        //     if(DF(dataG,queryG,i,idx)){
        //         continue;
        //     }
        // }
        // if(NeighborFilter){
        //     //如果开启邻域标签过滤
        // }
        gq.candi[idx].push_back(i);
        //gq.candi_to_q[i]=idx;
    }
    for(int i=0;i<gq.candi.size();i++){
        gq.vert_num+=gq.candi[i].size();
    }
    //gq.isPruned.assign(dataG.vert_num,0);
    gq.isPruned.reserve(gq.vert_num);
    //gq.mseq.assign(gq.vert_num,unordered_map<int,int>(10));
    //gq.mseq.reserve(gq.vert_num);
}
//查询图有顶点标签相同的情况
void initGQ_2(Graph& dataG,QGraph& queryG,GQ& gq){
    unordered_map<int,vector<int>> label_to_vert;//value是vector是为了防止多个查询节点有相同的label12
    for(int i=0;i<queryG.vert_num;i++){
        int qlabel=queryG.label[i];
        label_to_vert[qlabel].push_back(i);
    }

    for(int i=0;i<dataG.vert_num;i++){
        int ulabel=dataG.label[i];
        //
        if(label_to_vert.find(ulabel)==label_to_vert.end()){
            continue;
        }
        //gq.addVert(i);
        for(int j=0;j<label_to_vert[ulabel].size();j++){
            int idx=label_to_vert[ulabel][j];
            gq.candi[idx].push_back(i);
            //gq.candi_to_q[i]=idx;
        }
    }
    for(int i=0;i<gq.candi.size();i++){
        gq.vert_num+=gq.candi[i].size();
    }
    //gq.isPruned.assign(dataG.vert_num,0);
    gq.isPruned.reserve(gq.vert_num);
}

//查询图边p->q，u是p的候选，看是否有边u->v与其对应
bool uHasEdge(GQ &gq,int p,int q,int u,int edge_label,Graph& dataG){
    for(int i=0;i<gq.candi[q].size();i++){
        int v=gq.candi[q][i];
        if(gq.get_pruned(q,v)||gq.mseq[u][v]==-1){
            continue;
        }
        if(gq.mseq[u][v]==1){
            return true;
        }
        //gq.mseq[u][v]==0
        if(edge_label==0){
            //直接边查询
            if(dataG.hasEdge(u,v)){
                gq.mseq[u][v]=1;
                return true;
            }else{
                gq.mseq[u][v]=-1;
            }
        }else{
            //可达性查询
            if(dataG.canReach(u,v,0)){
                gq.mseq[u][v]=1;
                return true;
            }else{
                gq.mseq[u][v]=-1;
            }
        }
    }
    return false;
}
//查询图边p->q，v是q的候选，看是否有边u->v与其对应
bool vHasEdge(GQ &gq,int p,int q,int v,int edge_label,Graph& dataG){
    for(int i=0;i<gq.candi[p].size();i++){
        int u=gq.candi[p][i];
        if(gq.get_pruned(p,u)||gq.mseq[u][v]==-1){
            continue;
        }
        if(gq.mseq[u][v]==1){
            return true;
        }
        //gq.mseq[u][v]==0  --待判断根据edge_label to_be_done
        if(edge_label==0){
            //直接边查询
            if(dataG.hasEdge(u,v)){
                gq.mseq[u][v]=1;
                return true;
            }else{
                gq.mseq[u][v]=-1;
            }
        }else{
            //可达性查询
            if(dataG.canReach(u,v,0)){
                gq.mseq[u][v]=1;
                return true;
            }else{
                gq.mseq[u][v]=-1;
            }
        }
    }
    return false;
}
bool forwardPrune(GQ &gq,QGraph &queryG,Graph& dataG){
    bool change=false;
    for(int p=0;p<queryG.vert_num;p++){
        for(int q=0;q<queryG.vert_num;q++){
            //查询图中有边p->q
            if(queryG.edge[p][q]==0){
                continue;
            }
            for(auto u=gq.candi[p].begin();u!=gq.candi[p].end();u++){
                //for each node vqi ∈ FB(qi)
                if(gq.get_pruned(p,*u)){
                    continue;
                }
                if(!uHasEdge(gq,p,q,*u,queryG.edgelabel[p][q],dataG)){
                    gq.set_pruned(p,*u);
                    change=true;
                }
            }
            vector<int> newCandi;
            newCandi.reserve(50);
            for(int i=0;i<gq.candi[p].size();i++){
                int u=gq.candi[p][i];
                if(gq.get_pruned(p,u)){
                    continue;
                }
                newCandi.push_back(u);
            }
            gq.candi[p].swap(newCandi);
        }
    }
    return change;
}

//并行化节点剪枝，按节点划分

void worker_for_forwardPrune(int tid,int p,int q,Graph &dataG,GQ &gq,QGraph &queryG,vector<bool> &change){
    int workload=gq.candi[p].size()/FBSTHREADNUM;
    int left=tid*workload,right=(tid+1)*workload;
    int cnt[2]={0};
    if(tid==FBSTHREADNUM-1){
        right=gq.candi[p].size();
    }
    for(int i=left;i<right;i++){
        int u=gq.candi[p][i];
        if(gq.get_pruned(p,u)){
            continue;
        }
        cnt[0]++;
        if(!uHasEdge_parl(gq,p,q,u,queryG.edgelabel[p][q],dataG,tid)){
            gq.set_pruned(p,u);
            change[tid]=true;
            cnt[1]++;
        }
    }
    #ifdef DEBUG_FBS
        cout<<"thread"<<tid<<"共判断"<<cnt[0]<<"个，裁剪了"<<cnt[1]<<endl;
    #endif    
}
bool parl_forwardPrune(GQ &gq,QGraph &queryG,Graph& dataG){
    bool haschange=false;
    for(int p=0;p<queryG.vert_num;p++){
        for(int q=0;q<queryG.vert_num;q++){
            //查询图中有边p->q
            if(queryG.edge[p][q]==0){
                continue;
            }
            // if(vertChanged[p]==false&&vertChanged[q]==false){
            //     continue;
            // }
            vector<bool> change(FBSTHREADNUM,false);
            if(gq.candi[p].size()<=MaxCandi){
                for(auto u=gq.candi[p].begin();u!=gq.candi[p].end();u++){
                    //for each node vqi ∈ FB(qi)
                    if(gq.get_pruned(p,*u)){
                        continue;
                    }
                    if(!uHasEdge_parl(gq,p,q,*u,queryG.edgelabel[p][q],dataG,0)){
                        gq.set_pruned(p,*u,true);
                        change[0]=true;
                    }
                }
            }else{
                #ifdef DEBUG_FBS
                    cout<<"forward"<<p<<"->"<<q<<endl;
                #endif
                vector<thread> queryThread;
                queryThread.reserve(FBSTHREADNUM);
                for(int i=0;i<FBSTHREADNUM;i++){
                    queryThread.push_back(thread(worker_for_forwardPrune,i,p,q,std::ref(dataG),std::ref(gq),std::ref(queryG),std::ref(change)));
                }
                for(int i=0;i<FBSTHREADNUM;i++){
                    queryThread[i].join();
                }
                #ifdef DEBUG_FBS
                    cout<<endl;
                #endif 
            }
            bool flag=false;
            for(int i=0;i<FBSTHREADNUM;i++){
                if(change[i]){
                    flag=true;
                    //vertChanged[p]=true;
                }
            }
            if(flag==false){
                continue;
            }
            haschange=true;
            vector<int> newCandi;
            newCandi.reserve(50);
            for(int i=0;i<gq.candi[p].size();i++){
                int u=gq.candi[p][i];
                if(gq.get_pruned(p,u)){
                    continue;
                }
                newCandi.push_back(u);
            }
            gq.candi[p].assign(newCandi.begin(),newCandi.end());
        }
    }
    return haschange;
}

bool parl_forwardPruneDag(GQ &gq,QGraph &queryG,Graph& dataG){
    bool haschange=false;
    for(auto data:queryG.topOrderEdge){
        int p=data.first,q=data.second;
        #ifdef DEBUG_FBS
            cout<<"forward"<<p<<"->"<<q<<endl;
        #endif
        vector<bool> change(FBSTHREADNUM,false);
        vector<thread> queryThread;
        queryThread.reserve(FBSTHREADNUM);
        for(int i=0;i<FBSTHREADNUM;i++){
            queryThread.push_back(thread(worker_for_forwardPrune,i,p,q,std::ref(dataG),std::ref(gq),std::ref(queryG),std::ref(change)));
        }
        for(int i=0;i<FBSTHREADNUM;i++){
            queryThread[i].join();
        }
        #ifdef DEBUG_FBS
            cout<<endl;
        #endif 
        bool flag=false;
        for(int i=0;i<FBSTHREADNUM;i++){
            if(change[i]){
                flag=true;
            }
        }
        if(flag==false){
            continue;
        }
        haschange=true;
        vector<int> newCandi;
        newCandi.reserve(50);
        for(int i=0;i<gq.candi[p].size();i++){
            int u=gq.candi[p][i];
            if(gq.get_pruned(p,u)){
                continue;
            }
            newCandi.push_back(u);
        }
        gq.candi[p].assign(newCandi.begin(),newCandi.end());
    }
    return haschange;
}


bool backwardPrune(GQ &gq,QGraph &queryG,Graph& dataG){
    bool change=false;
    for(int p=0;p<queryG.vert_num;p++){
        for(int q=0;q<queryG.vert_num;q++){
            //查询图中有边p->q
            if(queryG.edge[p][q]==0){
                continue;
            }
            for(auto v=gq.candi[q].begin();v!=gq.candi[q].end();v++){
                //for each node vqj ∈ FB(qj)
                if(gq.get_pruned(q,*v)){
                    continue;
                }
                if(!vHasEdge(gq,p,q,*v,queryG.edgelabel[p][q],dataG)){
                    gq.set_pruned(q,*v);
                    change=true;
                }
            }
        }
    }
    return change;
}

void worker_for_backwardPrune(int tid,int p,int q,Graph &dataG,GQ &gq,QGraph &queryG,vector<bool> &change){
    int workload=gq.candi[q].size()/FBSTHREADNUM;
    int left=tid*workload,right=(tid+1)*workload;
    int cnt[2]={0};
    if(tid==FBSTHREADNUM-1){
        right=gq.candi[q].size();
    }
    for(int i=left;i<right;i++){
        int v=gq.candi[q][i];
        if(gq.get_pruned(q,v)){
            continue;
        }
        cnt[0]++;
        if(!vHasEdge_parl(gq,p,q,v,queryG.edgelabel[p][q],dataG,tid)){
            gq.set_pruned(q,v,true);
            change[tid]=true;
            cnt[1]++;
        }
    }
    #ifdef DEBUG_FBS
        cout<<"thread"<<tid<<"共判断"<<cnt[0]<<"个，裁剪了"<<cnt[1]<<endl;
    #endif
}
bool parl_backwardPrune(GQ &gq,QGraph &queryG,Graph& dataG){
    bool haschange=false;
    for(int p=0;p<queryG.vert_num;p++){
        for(int q=0;q<queryG.vert_num;q++){
            //查询图中有边p->q
            if(queryG.edge[p][q]==0){
                continue;
            }
            vector<bool> change(FBSTHREADNUM,false);
            if(gq.candi[q].size()<=MaxCandi){
                for(auto v=gq.candi[q].begin();v!=gq.candi[q].end();v++){
                    //for each node vqi ∈ FB(qi)
                    if(gq.get_pruned(q,*v)){
                        continue;
                    }
                    if(!vHasEdge_parl(gq,p,q,*v,queryG.edgelabel[p][q],dataG,0)){
                        gq.set_pruned(q,*v,true);
                        change[0]=true;
                    }
                }
            }else{
                #ifdef DEBUG_FBS
                    cout<<"backward"<<p<<"->"<<q<<endl;
                #endif
                vector<thread> queryThread;
                queryThread.reserve(FBSTHREADNUM);
                for(int i=0;i<FBSTHREADNUM;i++){
                    queryThread.push_back(thread(worker_for_backwardPrune,i,p,q,std::ref(dataG),std::ref(gq),std::ref(queryG),std::ref(change)));
                }
                for(int i=0;i<FBSTHREADNUM;i++){
                    queryThread[i].join();
                }
                #ifdef DEBUG_FBS
                    cout<<endl;
                #endif 
            }
            bool flag=false;
            for(int i=0;i<FBSTHREADNUM;i++){
                if(change[i]){
                    flag=true;
                }
            }
            if(flag==false){
                continue;
            }
            haschange=true;
            vector<int> newCandi;
            newCandi.reserve(50);
            for(int i=0;i<gq.candi[q].size();i++){
                int u=gq.candi[q][i];
                if(gq.get_pruned(q,u)){
                    continue;
                }
                newCandi.push_back(u);
            }
            gq.candi[q].assign(newCandi.begin(),newCandi.end());
        }
    }
    return haschange;
}

bool parl_backwardPruneDag(GQ &gq,QGraph &queryG,Graph& dataG){
    bool haschange=false;
    for(auto it=queryG.topOrderEdge.rbegin();it!=queryG.topOrderEdge.rend();it++){
        int p=it->first,q=it->second;
        #ifdef DEBUG_FBS
            cout<<"forward"<<p<<"->"<<q<<endl;
        #endif
        vector<bool> change(FBSTHREADNUM,false);
        vector<thread> queryThread;
        queryThread.reserve(FBSTHREADNUM);
        for(int i=0;i<FBSTHREADNUM;i++){
            queryThread.push_back(thread(worker_for_backwardPrune,i,p,q,std::ref(dataG),std::ref(gq),std::ref(queryG),std::ref(change)));
        }
        for(int i=0;i<FBSTHREADNUM;i++){
            queryThread[i].join();
        }
        #ifdef DEBUG_FBS
            cout<<endl;
        #endif 
        bool flag=false;
        for(int i=0;i<FBSTHREADNUM;i++){
            if(change[i]){
                flag=true;
            }
        }
        if(flag==false){
            continue;
        }
        haschange=true;
        vector<int> newCandi;
        newCandi.reserve(50);
        for(int i=0;i<gq.candi[q].size();i++){
            int u=gq.candi[q][i];
            if(gq.get_pruned(q,u)){
                continue;
            }
            newCandi.push_back(u);
        }
        gq.candi[q].assign(newCandi.begin(),newCandi.end());
    }
    
    return haschange;
}

//查询图边p->q，u是p的候选，看是否有边u->v与其对应
bool uHasEdge_parl(GQ &gq,int p,int q,int u,int edge_label,Graph& dataG,int tid){
    for(int i=0;i<gq.candi[q].size();i++){
        int v=gq.candi[q][i];

        // int ms=gq.get_mseq(p,u,v);
        // if(gq.get_pruned(v)||ms==-1){
        //     continue;
        // }
        // if(ms==1){
        //     return true;
        // }

        //gq.mseq[u][v]==0
        if(edge_label==0){
            //直接边查询
            if(dataG.hasEdge(u,v)){
                //gq.set_mseq(p,u,v,1);
                return true;
            }else{
                //gq.set_mseq(p,u,v,-1);
            }
        }else{
            //可达性查询
            if(dataG.canReach(u,v,tid)){
                //gq.set_mseq(p,u,v,1);
                return true;
            }else{
                //gq.set_mseq(p,u,v,-1);
            }
        }
    }
    return false;
}
//查询图边p->q，u是p的候选，看是否有边u->v与其对应
bool uHasEdge_parl_mseq(GQ &gq,int p,int q,int u,int edge_label,Graph& dataG,int tid){
    for(int i=0;i<gq.candi[q].size();i++){
        int v=gq.candi[q][i];

        int ms=gq.get_mseq(p,u,v);
        if(gq.get_pruned(q,v)||ms==-1){
            continue;
        }
        if(ms==1){
            return true;
        }

        //gq.mseq[u][v]==0
        if(edge_label==0){
            //直接边查询
            if(dataG.hasEdge(u,v)){
                gq.set_mseq(p,u,v,1);
                return true;
            }else{
                gq.set_mseq(p,u,v,-1);
            }
        }else{
            //可达性查询
            if(dataG.canReach(u,v,tid)){
                gq.set_mseq(p,u,v,1);
                return true;
            }else{
                gq.set_mseq(p,u,v,-1);
            }
        }
    }
    return false;
}
//查询图边p->q，v是q的候选，看是否有边u->v与其对应
bool vHasEdge_parl(GQ &gq,int p,int q,int v,int edge_label,Graph& dataG,int tid){
    for(int i=0;i<gq.candi[p].size();i++){
        int u=gq.candi[p][i];

        // int ms=gq.get_mseq(p,u,v);
        // if(gq.get_pruned(u)||ms==-1){
        //     continue;
        // }
        // if(ms==1){
        //     return true;
        // }

        //gq.mseq[u][v]==0  --待判断根据edge_label to_be_done
        if(edge_label==0){
            //直接边查询
            if(dataG.hasEdge(u,v)){
                //gq.set_mseq(p,u,v,1);
                return true;
            }else{
                //gq.set_mseq(p,u,v,-1);
            }
        }else{
            //可达性查询
            if(dataG.canReach(u,v,tid)){
                //gq.set_mseq(p,u,v,1);
                return true;
            }else{
                //gq.set_mseq(p,u,v,-1);
            }
        }
    }
    return false;
}
//查询图边p->q，v是q的候选，看是否有边u->v与其对应
bool vHasEdge_parl_mseq(GQ &gq,int p,int q,int v,int edge_label,Graph& dataG,int tid){
    for(int i=0;i<gq.candi[p].size();i++){
        int u=gq.candi[p][i];

        int ms=gq.get_mseq(p,u,v);
        if(gq.get_pruned(p,u)||ms==-1){
            continue;
        }
        if(ms==1){
            return true;
        }

        //gq.mseq[u][v]==0  --待判断根据edge_label to_be_done
        if(edge_label==0){
            //直接边查询
            if(dataG.hasEdge(u,v)){
                gq.set_mseq(p,u,v,1);
                return true;
            }else{
                gq.set_mseq(p,u,v,-1);
            }
        }else{
            //可达性查询
            if(dataG.canReach(u,v,tid)){
                gq.set_mseq(p,u,v,1);
                return true;
            }else{
                gq.set_mseq(p,u,v,-1);
            }
        }
    }
    return false;
}
void worker_for_pfbs(int tid,Graph &dataG,GQ &gq,QueryQue<FBSType,int> &que,bool &change){
    FBSType data;
    while(true){
        data=que.get();
        int p=data.p,q=data.q,label=data.label,type=data.type;
        if(p==-1&&q==-1){
            break;
        }
        if(type==1){
            for(auto u=gq.candi[p].begin();u!=gq.candi[p].end();u++){
                //for each node vqi ∈ FB(qi)
                if(gq.get_pruned(p,*u)){
                    continue;
                }
                if(!uHasEdge_parl(gq,p,q,*u,label,dataG,tid)){
                    gq.set_pruned(p,*u,true);
                    change=true;
                }
            }
        }else if(type==2){
            for(auto v=gq.candi[q].begin();v!=gq.candi[q].end();v++){
                //for each node vqj ∈ FB(qj)
                if(gq.get_pruned(q,*v)){
                    continue;
                }
                if(!vHasEdge_parl(gq,p,q,*v,label,dataG,tid)){
                    gq.set_pruned(q,*v,true);
                    change=true;
                }
            }
        }else{
            cout<<"fbs线程错误！"<<endl;
        }
    }
}
void parl_fbs(GQ &gq,QGraph &queryG,Graph& dataG){
    bool change=false;
    QueryQue<FBSType,int> que;
    do{
        change=false;
        vector<thread> queryThread;
        for(int i=0;i<FBSTHREADNUM;i++){
            queryThread.push_back(thread(worker_for_pfbs,i,std::ref(dataG),std::ref(gq),std::ref(que),std::ref(change)));
        }

        //前向
        for(int p=0;p<queryG.vert_num;p++){
            for(int q=0;q<queryG.vert_num;q++){
                //查询图中有边p->q
                if(queryG.edge[p][q]==0){
                    continue;
                }
                que.put({p,q,queryG.edgelabel[p][q],1});
            }
        }
        //后向
        for(int p=0;p<queryG.vert_num;p++){
            for(int q=0;q<queryG.vert_num;q++){
                //查询图中有边p->q
                if(queryG.edge[p][q]==0){
                    continue;
                }
                que.put({p,q,queryG.edgelabel[p][q],2});
            }
        }
        for (int i = 0; i < FBSTHREADNUM; i++) {
            que.put({-1,-1,-1,-1});
        }
        for (int i = 0; i < FBSTHREADNUM; i++) {
            queryThread[i].join();
        }
        // if(!que.res.empty()){
        //     change=true;
        //     while(!que.res.empty()){
        //         gq.isPruned[que.res.front()]=true;
        //         que.res.pop_front();
        //     }
        // }
    }while(change);
}

//节点剪枝 to_be_done 能否并行化
void FBSimBas(GQ &gq,QGraph &queryG,Graph& dataG){
    bool change=false;
    //vector<bool> vertChanged(queryG.vert_num,true);
    gq.fbsTerm=0;
    int cnt=0;
    do{
        cnt++;
        change=false;
        if(parl_forwardPrune(gq,queryG,dataG)){
            change=true;
        }
        #ifdef DEBUG_FBS
            gq.print();
        #endif         
        if(parl_backwardPrune(gq,queryG,dataG)){
            change=true;
        }
        #ifdef DEBUG_FBS
            gq.print();
        #endif 
    }while(change);
    gq.fbsTerm=cnt;
}

void FBSimDag(GQ &gq,QGraph &queryG,Graph& dataG){
    bool change=false;
    int cnt=0;
    gq.fbsTerm=0;
    do{
        cnt++;
        change=false;
        if(parl_forwardPruneDag(gq,queryG,dataG)){
            change=true;
        }
        #ifdef DEBUG_FBS
            gq.print();
        #endif         
        if(parl_backwardPruneDag(gq,queryG,dataG)){
            change=true;
        }
        #ifdef DEBUG_FBS
            gq.print();
        #endif 
    }while(change);
    gq.fbsTerm=cnt;
}

struct hyFBS{
    int u;
    int v;
    long unsigned int usize;
    long unsigned int vsize;
    int type;
    bool operator < (const hyFBS& other) const {
        return usize*vsize>other.usize*other.vsize;
    }
};
void hybridFBS(GQ &gq,QGraph &queryG,Graph& dataG){
    bool change=false;
    vector<bool> vertChanged(queryG.vert_num,true);
    vector<vector<int>> visited;
    int cnt=0;
    gq.fbsTerm=0;
    do{
        change=false;
        cnt++;
        //vector<hyFBS> que;
        priority_queue<hyFBS> pri_que;
        for(int p=0;p<queryG.vert_num;p++){
            for(int q=0;q<queryG.vert_num;q++){
                //查询图中有边p->q
                if(queryG.edge[p][q]==0){
                    continue;
                }
                if(vertChanged[p]==false&&vertChanged[q]==false){
                    continue;
                }
                pri_que.push({p,q,gq.candi[p].size(),gq.candi[q].size(),1});
                pri_que.push({p,q,gq.candi[p].size(),gq.candi[q].size(),2});
            }
        }
        // sort(que.begin(),que.end(),[&](hyFBS a,hyFBS b){
        //     long long suma=(long long)gq.candi[a.u].size()*gq.candi[a.v].size();
        //     long long sumb=(long long)gq.candi[b.u].size()*gq.candi[b.v].size();
        //     if(suma<sumb){
        //         return true;
        //     }else if(suma==sumb){
        //         if(gq.candi[a.u].size()<gq.candi[b.u].size()){
        //             return true;
        //         }
        //     }
        //     return false;
        // });
        vertChanged.assign(queryG.vert_num,false);
        visited.assign(queryG.vert_num,vector<int>(queryG.vert_num,0));
        while(!pri_que.empty()){
            vector<bool> change2(FBSTHREADNUM,false);
            hyFBS data=pri_que.top();
            pri_que.pop();
            int p=data.u,q=data.v;
            if(visited[p][q]==3||visited[p][q]==data.type){
                continue;
            }
            visited[p][q]+=data.type;
            vector<thread> queryThread;
            queryThread.reserve(FBSTHREADNUM);
            if(data.type==1){
                for(int i=0;i<FBSTHREADNUM;i++){
                    queryThread.push_back(thread(worker_for_forwardPrune,i,p,q,std::ref(dataG),std::ref(gq),std::ref(queryG),std::ref(change2)));
                }
            }else{
                for(int i=0;i<FBSTHREADNUM;i++){
                    queryThread.push_back(thread(worker_for_backwardPrune,i,p,q,std::ref(dataG),std::ref(gq),std::ref(queryG),std::ref(change2)));
                }
            }
            for(int i=0;i<FBSTHREADNUM;i++){
                queryThread[i].join();
            }
            int idx;
            if(data.type==1){
                idx=data.u;
            }else{
                idx=data.v;
            }
            bool flag=false;
            for(int i=0;i<FBSTHREADNUM;i++){
                if(change2[i]){
                    flag=true;
                    vertChanged[idx]=true;
                }
            }
            if(flag==false){
                continue;
            }
            change=true;
            vector<int> newCandi;
            newCandi.reserve(50);
            for(int i=0;i<gq.candi[idx].size();i++){
                int u=gq.candi[idx][i];
                if(gq.get_pruned(idx,u)){
                    continue;
                }
                newCandi.push_back(u);
            }
            gq.candi[idx].assign(newCandi.begin(),newCandi.end());
            for(int i=0;i<queryG.vert_num;i++){
                if(queryG.edge[idx][i]==1){
                    pri_que.push({idx,i,gq.candi[idx].size(),gq.candi[i].size(),1});
                    pri_que.push({idx,i,gq.candi[idx].size(),gq.candi[i].size(),2});
                }
                if(queryG.edge[i][idx]==1){
                    pri_que.push({i,idx,gq.candi[i].size(),gq.candi[idx].size(),1});
                    pri_que.push({i,idx,gq.candi[i].size(),gq.candi[idx].size(),2});
                }
            }
        }
        // for(int i=0;i<que.size();i++){
        //     vector<bool> change2(FBSTHREADNUM,false);
        //     hyFBS &data=que[i];
        //     int p=data.u,q=data.v;
        //     vector<thread> queryThread;
        //     queryThread.reserve(FBSTHREADNUM);
        //     if(data.type==1){
        //         for(int i=0;i<FBSTHREADNUM;i++){
        //             queryThread.push_back(thread(worker_for_forwardPrune,i,p,q,std::ref(dataG),std::ref(gq),std::ref(queryG),std::ref(change2)));
        //         }
        //     }else{
        //         for(int i=0;i<FBSTHREADNUM;i++){
        //             queryThread.push_back(thread(worker_for_backwardPrune,i,p,q,std::ref(dataG),std::ref(gq),std::ref(queryG),std::ref(change2)));
        //         }
        //     }
        //     for(int i=0;i<FBSTHREADNUM;i++){
        //         queryThread[i].join();
        //     }
        //     int idx;
        //     if(data.type==1){
        //         idx=data.u;
        //     }else{
        //         idx=data.v;
        //     }
        //     bool flag=false;
        //     for(int i=0;i<FBSTHREADNUM;i++){
        //         if(change2[i]){
        //             flag=true;
        //             vertChanged[idx]=true;
        //         }
        //     }
        //     if(flag==false){
        //         continue;
        //     }
        //     change=true;
        //     vector<int> newCandi;
        //     newCandi.reserve(50);
        //     for(int i=0;i<gq.candi[idx].size();i++){
        //         int u=gq.candi[idx][i];
        //         if(gq.get_pruned(idx,u)){
        //             continue;
        //         }
        //         newCandi.push_back(u);
        //     }
        //     gq.candi[idx].assign(newCandi.begin(),newCandi.end());
        // }
    }while(change);
    gq.fbsTerm=cnt;
}

struct hybType{
    int p;
    int q;
    int type;
};
void worker_for_hyb(int tid,Graph &dataG,GQ &gq,QGraph &queryG,QueryQue<hybType,int>& que){
    timeType st,et;
    double totalTime=0;
    while(true){
        auto data=que.get();
        st=std::chrono::steady_clock::now();
        int p=data.p,q=data.q,type=data.type;
        if(p==-1&&q==-1){
            break;
        }
        vector<bool> change(FBSTHREADNUM,false);
        if(type==1){
            worker_for_forwardPrune(tid,p,q,dataG,gq,queryG,change);
        }else{
            worker_for_backwardPrune(tid,p,q,dataG,gq,queryG,change);
        }
        if(change[tid]){
            que.putRes(1);
        }else{
            que.putRes(0);
        }
        et=std::chrono::steady_clock::now();
        totalTime+=getTime(st,et);
    }
    gq.fbs_thread_time[tid]=totalTime;
}

void hybridFBS_threadpool(GQ &gq,QGraph &queryG,Graph& dataG){
    bool change=false;
    vector<bool> vertChanged(queryG.vert_num,true);
    vector<vector<int>> visited;
    vector<thread> queryThread;
    QueryQue<hybType,int> que[FBSTHREADNUM];
    gq.fbs_thread_time.assign(FBSTHREADNUM,0);
    for(int i=0;i<FBSTHREADNUM;i++){
        queryThread.push_back(thread(worker_for_hyb,i,std::ref(dataG),std::ref(gq),std::ref(queryG),std::ref(que[i])));
    }
    do{
        change=false;
        //vector<hyFBS> que;
        priority_queue<hyFBS> pri_que;
        for(int p=0;p<queryG.vert_num;p++){
            for(int q=0;q<queryG.vert_num;q++){
                //查询图中有边p->q
                if(queryG.edge[p][q]==0){
                    continue;
                }
                if(vertChanged[p]==false&&vertChanged[q]==false){
                    continue;
                }
                pri_que.push({p,q,gq.candi[p].size(),gq.candi[q].size(),1});
                pri_que.push({p,q,gq.candi[p].size(),gq.candi[q].size(),2});
            }
        }
        vertChanged.assign(queryG.vert_num,false);
        visited.assign(queryG.vert_num,vector<int>(queryG.vert_num,0));
        while(!pri_que.empty()){
            hyFBS data=pri_que.top();
            pri_que.pop();
            int p=data.u,q=data.v;
            if(visited[p][q]==3||visited[p][q]==data.type){
                continue;
            }
            visited[p][q]+=data.type;
            
            for(int i=0;i<FBSTHREADNUM;i++){
                que[i].put({p,q,data.type});
            }
            int idx;
            if(data.type==1){
                idx=data.u;
            }else{
                idx=data.v;
            }
            bool flag=false;
            for(int i=0;i<FBSTHREADNUM;i++){
                int c=que[i].getRes();
                if(c==1){
                    flag=true;
                    vertChanged[idx]=true;
                }
            }
            if(flag==false){
                continue;
            }
            change=true;
            vector<int> newCandi;
            newCandi.reserve(50);
            for(int i=0;i<gq.candi[idx].size();i++){
                int u=gq.candi[idx][i];
                if(gq.get_pruned(idx,u)){
                    continue;
                }
                newCandi.push_back(u);
            }
            gq.candi[idx].assign(newCandi.begin(),newCandi.end());
            for(int i=0;i<queryG.vert_num;i++){
                if(queryG.edge[idx][i]==1){
                    pri_que.push({idx,i,gq.candi[idx].size(),gq.candi[i].size(),1});
                    pri_que.push({idx,i,gq.candi[idx].size(),gq.candi[i].size(),2});
                }
                if(queryG.edge[i][idx]==1){
                    pri_que.push({i,idx,gq.candi[i].size(),gq.candi[idx].size(),1});
                    pri_que.push({i,idx,gq.candi[i].size(),gq.candi[idx].size(),2});
                }
            }
        }
    }while(change);
    for(int i=0;i<FBSTHREADNUM;i++){
        que[i].put({-1,-1,-1});
    }
    for(int i=0;i<FBSTHREADNUM;i++){
        queryThread[i].join();
    }
}

void worker(int tid,Graph &dataG,GQ &gq,QueryQue<BflQueryType,vector<int>> &qu){
    BflQueryType data;
	while (true) {
		data=qu.get();
		int u = data.u, v = data.v,q=data.q;

		if (u==-1&&v==-1) {
			//查询结束a
			break;
		}
        
        if(data.edge_label==0){
            //直接边查询
            if(dataG.hasEdge(u,v)){
                //gq.addEdge(tid,u,v,q);
            }
        }else{
            //可达性查询
            if(dataG.canReach(u,v,tid)){
                //gq.addEdge(tid,u,v,q);
            }
        }
	}
}
//原本的思路是先查询所有节点对间的边
//现在需改为先对节点剪枝再找边
void parl_bfl(Graph& dataG,GQ& gq,QGraph& queryG){
    //查询子线程
	vector<thread> queryThread;
	QueryQue<BflQueryType,vector<int>> queryQue;
	for (int i = 0; i < BFLTHREADNUM; i++) {
		queryThread.push_back(thread(worker,i,std::ref(dataG),std::ref(gq),std::ref(queryQue)));
	}

    for(int p=0;p<queryG.vert_num;p++){
        for(int q=0;q<queryG.vert_num;q++){
            //查询图中有边p->q
            if(queryG.edge[p][q]==0){
                continue;
            }
            for(auto u=gq.candi[p].begin();u!=gq.candi[p].end();u++){
                for(auto v=gq.candi[q].begin();v!=gq.candi[q].end();v++){
                    queryQue.put(BflQueryType(*u,*v,q,queryG.edgelabel[p][q]));
                }
            }
        }
    }
    //向子线程发送查询结束信号
	for (int i = 0; i < BFLTHREADNUM; i++) {
		queryQue.put(BflQueryType(-1,-1,-1,-1));
	}

    for (int i = 0; i < BFLTHREADNUM; i++) {
		queryThread[i].join();
	}
}

void worker_for_bfl2(int tid,Graph &dataG,GQ &gq,int p,int q,int edge_label){
    int workload=gq.candi[p].size()/BFLTHREADNUM;
    int left=tid*workload,right=(tid+1)*workload;
    if(tid==BFLTHREADNUM-1){
        right=gq.candi[p].size();
    }
    if(workload==0){
        //候选节点数比线程数少
        left=tid;
        right=left+1;
        if(left>=gq.candi[p].size()){
            return;
        }
    }
    for(int i=left;i<right;i++){
        int u=gq.candi[p][i];
        for(int j=0;j<gq.candi[q].size();j++){
            int v=gq.candi[q][j];
            if(edge_label==0){
                //直接边查询
                if(dataG.hasEdge(u,v)){
                    gq.addEdge(tid,u,v,p,q);
                }
            }else{
                //可达性查询
                if(dataG.canReach(u,v,tid)){
                    gq.addEdge(tid,u,v,p,q);
                }
            }
        }
    }
}
void parl_bfl_2(Graph& dataG,GQ& gq,QGraph& queryG){
    for(int p=0;p<queryG.vert_num;p++){
        for(int q=0;q<queryG.vert_num;q++){
            //查询图中有边p->q
            if(queryG.edge[p][q]==0){
                continue;
            }
            if(gq.candi[p].size()<=0){
                cout<<"节点"<<p<<"候选节点为0，直接跳过此过程"<<endl;
                return;
            }else{
                vector<thread> bflThread;
                for(int i=0;i<BFLTHREADNUM;i++){
                    bflThread.push_back(thread(worker_for_bfl2,i,std::ref(dataG),std::ref(gq),p,q,queryG.edgelabel[p][q]));
                }
                for(int i=0;i<BFLTHREADNUM;i++){
                    bflThread[i].join();
                }
            }
        }
    }
}
/*
void ceci(GQ& gq,QGraph& queryG){
    // queryG.chooseStartVertex(candisize);
    queryG.generateQueryTree();
    queryG.printQueryTree();
    gq.initCR(queryG);
    gq.explore(queryG);
    //gq.prepare();
    gq.parl_matching(queryG);
}
*/
double my_ceci(GQ& gq,QGraph& queryG){
    if(ORDERTYPE==TOP){
        queryG.generateTopOrder(gq.candi);
    }else if(ORDERTYPE==JO){
        queryG.generateOrder(gq.candi);
    }else if(ORDERTYPE==RI){
        queryG.generateRI();
    }else if(ORDERTYPE==CECI){
        queryG.chooseStartVertex(gq.candi);
        queryG.generateCECI();
    }else if(ORDERTYPE==MYORDER){
        //queryG.chooseStartVertex(gq.candi);
        queryG.generateMyOrder(gq.candi);
    }else if(ORDERTYPE==KRU){
        queryG.generateKruskal(KRUTYPE,gq.candi,gq.candiEdge);
        //queryG.generateKruskal_0(gq.candi); //以0为初始节点
    }else if(ORDERTYPE==JO2){
        queryG.generateOrder_2(gq.candi);
    }else if(ORDERTYPE==CUTORDER){
        queryG.findCutVertex();
        //queryG.printCut();
        gq.match_cutV(queryG);  //先处理割点的匹配
        //queryG.printTree();
        queryG.startVert=0;
        queryG.generateCut(gq.candi,queryG.cutErase);
        //queryG.printTree();
    }else if(ORDERTYPE==QUICKSI){
        queryG.generateQuickSI(gq.candi,gq.candiEdge);
    }

    gq.initCR(queryG);

    timeType st=std::chrono::steady_clock::now();
#ifdef MATCH_FENJIE
    gq.prepare();
#endif

#ifdef BALANCE
    gq.workDistribute();
#endif
    gq.parl_matching(queryG);
    timeType et=std::chrono::steady_clock::now();
    return getTime(st,et);
}

void timePrint(const string& str,timeType st,timeType end){
    //cout<<str<<"的时间是："<<(double)(end-st)/(CLOCKS_PER_SEC/1000)<<" ms."<<endl;
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - st); // 计算程序运行时间
    std::cout <<str<<"的时间是：" << elapsed_time.count() / 1000000.0 << " seconds" << std::endl;
}

double getTime(timeType st,timeType end){
    //return (double)(end-st)/(CLOCKS_PER_SEC/1000);
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - st);
    return elapsed_time.count() / 1000.0;
}

string getName(string str){
    std::size_t last_slash_index = str.find_last_of("/");
    std::string filename = str.substr(last_slash_index + 1);
    return filename;
}

string getNameBeforePoint(string str){
    std::size_t dot_index = str.find(".");
    std::string filename = str.substr(0, dot_index);
    return filename;
}

string eraseTab(string title){
    std::size_t last_not_tab = title.find_last_not_of("\r"); // 找到最后一个不是制表符的字符
    if (last_not_tab != std::string::npos) { // 如果找到了
        title.erase(last_not_tab + 1); // 删掉制表符及其后面的所有字符
    }
    return title;
}

string eraseSpace(string str){
    str.erase(0, str.find_first_not_of(' ')); // 去除开头的空格
    str.erase(str.find_last_not_of(' ') + 1); // 去除结尾的空格
    
    // 去除字符串中的其它空格
    str.erase(std::remove_if(str.begin(), str.end(), [](char c){ return std::isspace(c); }), str.end());
    return str;
}

void outToFile(ofstream &fout,Result &result){
    //fout<<result.queryTitle<<" ";
    //fout<<result.dataGraVNum<<" ";
    //fout<<result.dataGraENum<<" ";

    // fout<<result.queryGraVNum<<" ";
    // fout<<result.queryGraENum<<" ";
    // fout<<result.queryGraENum_bfl<<" ";
    //fout<<result.threadNum<<" ";

    //fout<<result.dataGraIndexTime<<" ";
    //fout<<result.findCandiTime<<" ";
    fout<<result.FBSTime<<" ";
    // for(int i=0;i<result.fbsThreadTime.size();i++){
    //     fout<<result.fbsThreadTime[i]<<" ";
    // }
    //fout<<result.deleteCandiTime<<" ";
    //fout<<result.fbsTerm<<" ";
    //fout<<result.candiSize[0]<<" ";
    //fout<<result.candiSize[1]<<" ";

    fout<<result.bflTime<<" ";
    fout<<result.matchTime<<" ";
    //fout<<result.matchNum<<" ";
    fout<<result.threadNum<<" ";
    for(int i=0;i<result.threadTime.size();i++){
        fout<<result.threadTime[i]<<" ";
    }
    //double total=result.FBSTime+result.deleteCandiTime+result.bflTime+result.matchTime;
    //fout<<total;
    fout<<endl;
}

void subgraph_match(Graph& dataG,QGraph& queryG,Result &result){
    GQ gq(queryG.vert_num);

    startT=std::chrono::steady_clock::now();
    initGQ_2(dataG,queryG,gq);
    endT=std::chrono::steady_clock::now();
    result.findCandiTime=getTime();
    gq.print();
    result.candiSize[0]=gq.vert_num;
    //edgeQuery(dataG,gq,queryG);

    if(FBSTYPE==FBSDAG){
        queryG.generateTopOrder();
    }
    startT=std::chrono::steady_clock::now();
    if(FBSTYPE==FBSBas){
        FBSimBas(gq,queryG,dataG);
    }else if(FBSTYPE==FBSDAG){
        FBSimDag(gq,queryG,dataG);
    }else if(FBSTYPE==FBSHyb){
        hybridFBS(gq,queryG,dataG);
        //hybridFBS_threadpool(gq,queryG,dataG);
        result.fbsThreadTime=gq.fbs_thread_time;
    }else if(FBSTYPE==FBS_CECI){
        
    }else{
        cout<<"fbsError!"<<endl;
        exit(0);
    }    
    //parl_fbs(gq,queryG,dataG);
    endT=std::chrono::steady_clock::now();
    result.FBSTime=getTime();
    result.fbsTerm=gq.fbsTerm;
    timePrint("并行摘要图剪枝");

    startT=std::chrono::steady_clock::now();
    gq.construct_Graph2();
    endT=std::chrono::steady_clock::now();
    result.deleteCandiTime=getTime();
    cout<<"剪枝后......"<<endl;
    gq.print();
    result.candiSize[1]=gq.vert_num;
    
    startT=std::chrono::steady_clock::now();
    //parl_bfl(dataG,gq,queryG);
    parl_bfl_2(dataG,gq,queryG);
    endT=std::chrono::steady_clock::now();
    result.bflTime=getTime();
    timePrint("摘要图并行bfl确定边");

    gq.constructGraph();
    gq.print();

    startT=std::chrono::steady_clock::now();
    //ceci(gq,queryG);
    double mTime=my_ceci(gq,queryG);
    endT=std::chrono::steady_clock::now();
    //result.matchTime=getTime();
    result.matchTime=mTime;
    long long match_num=0;
    gq.printInfo(match_num);
    result.matchNum=match_num;
    result.threadTime=gq.threadTime;
    timePrint("并行匹配枚举");
}

void calculateMatch(Graph& dataG,QGraph& queryG,GQ &gq,Result &result){
    queue<int> que;
    long long matchNum=1;
    vector<int> _indegree=queryG.indegree;
    vector<int> visited(queryG.vert_num,0);
    for(int i=0;i<queryG.vert_num;i++){
        if(visited[i]==0&&_indegree[i]==0){
            que.push(i);
            visited[i]=1;
            for(int j=0;j<gq.candi[i].size();j++){
                int pNode=gq.candi[i][j];
                gq.weight[i][pNode]=1;
            }
        }
    }
    while(!que.empty()){
        int cur=que.front();
        que.pop();
        for(int j=0;j<queryG.vert_num;j++){
            if(visited[j]){
                continue;
            }
            if(queryG.edge[cur][j]!=0){
                unordered_map<int,long long> tempWei;
                for(int i=0;i<gq.candi[cur].size();i++){
                    int pNode=gq.candi[cur][i];
                    auto &qcandi=gq.candiEdge[cur][pNode][j];
                    long long sum=0;
                    for(int k=0;k<qcandi.size();k++){
                        int qNode=qcandi[k];
                        tempWei[qNode]+=gq.weight[cur][pNode];
                    }
                }
                for(const auto &it:tempWei){
                    if(gq.weight[j][it.first]==0){
                        gq.weight[j][it.first]=it.second;
                    }else{
                        gq.weight[j][it.first]*=it.second;
                    }
                }
                _indegree[j]--;
                if(_indegree[j]==0){
                    if(queryG.outdegree[j]==0){
                        long long sum=0;
                        for(const auto &it:gq.candi[j]){
                            sum+=gq.weight[j][it];
                        }
                        matchNum*=sum;
                    }else{
                        que.push(j);
                        visited[j]=1;
                    }
                    
                }
            }
        }
    }
    cout<<"匹配数:"<<matchNum<<endl;
}