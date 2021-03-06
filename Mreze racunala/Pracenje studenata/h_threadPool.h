#ifndef __THREADPOOL_H_
#define __THREADPOOL_H_

#include <pthread.h>
#include <utility>
#include <list>
//#include <unistd.h>

using namespace std;

class threadPool {
	private:
		int		maxThreads;
		pthread_mutex_t	*threadsLock;
		list<pair<int, pthread_t*> > threads;
		list<int> freeID;
		bool *canEnd;
	public:
		threadPool(int maxThreads, pthread_mutex_t *threadsLock, bool *canEnd);
		bool addThread(int threadID, pthread_t *thread);
		void notifyFinished(int threadID);
		void programToEnd();
		int getFreeID();
};

#endif
