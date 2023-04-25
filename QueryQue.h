#ifndef __QUERY_QUE_H__
#define __QUERY_QUE_H__
#include <deque>
#include <vector>
#include <mutex>
#include <condition_variable>
using namespace std;

const int MAXQUERY = 200;

template<class QueryType,class AnsType>
class QueryQue {
public:
	deque<QueryType> qu;
	deque<AnsType> res;
	std::mutex mtx[2];
	std::condition_variable cv[2];

public:
	void put(QueryType data) {
		std::unique_lock<std::mutex> lock(mtx[0]);
		while (qu.size() == MAXQUERY) {
			cv[0].wait(lock);
		}
		qu.push_back(data);
		//lock.unlock();
		cv[0].notify_one();
	}
	QueryType get() {
		std::unique_lock<std::mutex> lock(mtx[0]);
		while (qu.empty()) {
			cv[0].wait(lock);
		}
		QueryType ret = qu.front();
		qu.pop_front();
		//lock.unlock();
		cv[0].notify_one();
        return ret;
	}

	void putRes(const AnsType& ans) {
		std::unique_lock<std::mutex> lock(mtx[1]);
		while (res.size() == MAXQUERY) {
			cv[1].wait(lock);
		}
		res.emplace_back(ans);
		lock.unlock();
		cv[1].notify_one();
	}
	AnsType getRes() {
		std::unique_lock<std::mutex> lock(mtx[1]);
		while (res.empty()) {
			cv[1].wait(lock);
		}
		auto ans = res.front();
		res.pop_front();
		lock.unlock();
		cv[1].notify_one();
		return ans;
	}
};

#endif