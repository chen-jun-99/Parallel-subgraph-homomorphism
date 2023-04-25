#ifndef __CSR_H__
#define __CSR_H__
#include <iostream>
#include <cstring>
#include <vector>
using namespace std;

/*
构建图的操作步骤：
	获取图的顶点数目vert_num
	根据顶点数初始存储度的向量(申请空间)				in/out.initDegree()    
	记录每个顶点的度
	记录边的数目edge_num
	初始化csr数组，确定beg_pos							in/out.initCSR()	
	将degree数组置0		(在initCSR函数内)
	再次读取图的每一条边，修改csr数组，修改degree数组		in/out.insert()
*/
class CSR {

public:
	vector<int> beg_pos,csr,degree;

public:
	void initDegree(int n) {
		degree.assign(n,0);
	}
	void initCSR(int n, int m) {
		beg_pos.assign(n+1,0);
		csr.assign(m,0);
		beg_pos[0] = 0;
		for (int i = 1; i < n + 1; ++i)
		{
			beg_pos[i] = beg_pos[i - 1] + degree[i - 1];
		}
		degree.assign(degree.size(),0);
	}
	//插入一条边u->v
	void insert(int u, int v) {
		csr[beg_pos[u] + degree[u]] = v;
		degree[u] ++;
	}
};

#endif