#ifndef _CONSTANT_H__
#define _CONSTANT_H__

#define THREADNUM 12
#define MATCHING_THREAD_NUM THREADNUM
// int THREADNUM=12;
// int MATCHING_THREAD_NUM=THREADNUM;

#define THRESHORD_RATIO (2)   //threshold = totalwork/(THRESHORD_RATIO*MATCHING_THREAD_NUM);
// int THRESHORD_RATIO=2;
//#define MAX_QUERY_NUM 50

//#define CUTVERTEX
#define WORKSTEAL
#define DEPTH 0
#define WIDTH 0
// int DEPTH=0;
// int WIDTH=0;

//#define MATCH_FENJIE            //ORDERTYPE==CECI||MYORDER

#define BALANCE                //测试负载均衡


#define FBSTHREADNUM THREADNUM              
#define BFLTHREADNUM THREADNUM
// int FBSTHREADNUM=THREADNUM;
// int BFLTHREADNUM=THREADNUM;
const int MaxCandi=0;

#define MAX_MATCHING_NUM 10000000
#define SET_LIMIT false
//int MAX_MATCHING_NUM=10000000;
//int SET_LIMIT=false;

#define MAX_TIME 120000       //毫秒
//int MAX_TIME=120000;

//debug时使用
//#define DEBUG_FBS
//#define MINEMATCH

#define HasDegree 0
#define QHasDegree 0
// int HasDegree=0;
// int QHasDegree=0;

#define FBSTYPE 3
//int FBSTYPE=3;
#define FBSBas 1
#define FBSDAG 2
#define FBSHyb 3
#define FBS_CECI 4

#define ORDERTYPE 5
//int ORDERTYPE=5;
#define TOP 1           //拓扑排序
#define JO 2            //连接，候选集最小
#define RI 3            //连接，节点度最大
#define CECI 4          //生成树，度最大
#define MYORDER 5       //生成树，候选集最小
#define KRU 6           //最小生成树
#define JO2 7           //JO，选择初始节点
#define CUTORDER 8      //使用割点
#define QUICKSI 9       

#define KRUTYPE 2      //1:点积  else:边数量




#endif