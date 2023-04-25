#include <iostream>
#include <unordered_map>
#include <thread>
#include <string>
#include "my_algorithm.h"
#include <fstream>

using namespace std;

timeType startT,endT;
int main()
{
    Result result;
    Graph dataG;
    string dataName="dataset/graphs/ep_lb20.gra";
    string queryName="dataset/queries/big.qry";
    //string dataName="../Graph_dataset/dataset/patents/data_graph/patents.graph";
    //string queryName="../Graph_dataset/mergeQuerys/dblp_dense_32.txt";
    //string queryName="../Graph_dataset/5/5.qry";
    //string queryName="../Graph_dataset/20/20.qry";
    result.dataGraName=getName(dataName);
    result.queryGraName=getName(queryName);
    result.threadNum=THREADNUM;
    startT=std::chrono::steady_clock::now();
    dataG.readGraph(dataName,HasDegree);
    result.dataGraVNum=dataG.vert_num;
    result.dataGraENum=dataG.edge_num;
    //queryG.readQueryOnce(queryName);
    endT=std::chrono::steady_clock::now();
    timePrint("读取图");
    //queryG.print();

    startT=std::chrono::steady_clock::now();
    dataG.constructIndex();
    endT=std::chrono::steady_clock::now();
    timePrint("数据图构建索引");
    result.dataGraIndexTime=getTime();

    int cnt=1;
    bool isOver=false;
    ifstream fin(queryName,ios::in);
    string outFile=getNameBeforePoint(result.dataGraName)
                +"__"+getNameBeforePoint(result.queryGraName)
                +"__"+to_string(THREADNUM)
                +".txt";
    outFile="result/"+getNameBeforePoint(result.queryGraName)+"/"+outFile;
    outFile="result.txt";
    ofstream fout(outFile,std::ios::out | std::ios::trunc);
    string title;
    while(!isOver){
        QGraph queryG;
        cout<<"查询图"<<cnt++<<endl;
        queryG.readQueryLoop(fin,title,isOver,QHasDegree);
        if(isOver||cnt>51){
            break;
        }

        result.queryTitle=eraseSpace(eraseTab(title));
        result.queryGraVNum=queryG.vert_num;
        result.queryGraENum=queryG.edge_num;
        result.queryGraENum_bfl=queryG.edge_bfl_num;
        subgraph_match(dataG,queryG,result);
        outToFile(fout,result);
        cout<<endl<<endl<<endl;      
    }
    fin.close();
    fout.close();
    return 0;
}