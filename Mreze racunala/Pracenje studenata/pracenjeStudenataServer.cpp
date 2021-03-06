#include "pracenjeStudenataProtokol.h"
#include "h_server_core.h"
#include "h_map_server_commands.h"
#include "data.h"
#include "h_threadPool.h"

#include <pthread.h>
#include <map>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

static const int	MAX_CONN	= 100;

static bool		canEnd = false;

pthread_mutex_t		dataLock	= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t		outputLock	= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t		threadsLock	= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t		addThreadsLock	= PTHREAD_MUTEX_INITIALIZER;

static threadPool	*pool		= new threadPool(MAX_CONN, &threadsLock, &canEnd);

static map<int, server_command*> 	commands;

typedef struct {
	int commSock;
	int threadID;
} thread_info;

void *run_core(void *v_param) {
	thread_info *param = (thread_info*) v_param;

	int	commSock	= param->commSock,
		threadID	= param->threadID;;
	core c(commands, &outputLock, pool);
	c.run(commSock, threadID);
}

int main(int args, char **argv) {
	
	//provjerava broj unesenih argumenata:
	
	if (args != 2) {
		fprintf(stderr, "Koristenje: ./server port");
		exit(0);
	}
	
	//ucitava port:
	
	unsigned short port;
	sscanf (argv[1], "%hu", &port);
	if (port < 1) {
		fprintf(stderr, "Port %hu je nedozvoljen", port);
		exit(0);
	}
	
	//inicijalizira podatke:
	
	data	*studentsAndClassroomData	= new data();
	map_server_commands	*mapper		= new map_server_commands(
								studentsAndClassroomData,
								&dataLock,
								&outputLock
							);
	
	//inicijalizira listener socket:
	
	int listenerSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (listenerSocket == -1) {
		perror("Socket");
		exit(0);
	}
	commands = mapper->get_mapped_commands();
	
	//bindanje i osluskivanje:
	
	struct sockaddr_in serverAdress;
	serverAdress.sin_family		=	AF_INET;
	serverAdress.sin_port		=	htons(port);
	serverAdress.sin_addr.s_addr	=	INADDR_ANY;
	memset(serverAdress.sin_zero, '\0', 8);
	
	if (
		bind(
			listenerSocket,
			(struct sockaddr *) &serverAdress,
			sizeof(serverAdress)
		)
		== -1
	) {
		perror("Bind");
		exit(0);
	}
	
	if (
		listen(
			listenerSocket,
			MAX_CONN
		)
		== -1
	) {
		perror("Listen");
		exit(0);
	}
	
	//accept:
	while(1) {
		struct sockaddr_in	clientAdress;
		unsigned int		clientAdressLength = sizeof(clientAdress);
		int 			commSock = accept(
						listenerSocket,
						(struct sockaddr *) &clientAdress,
						&clientAdressLength
					);
		if (commSock == -1) {
			perror("Accept");
		}
		string client = inet_ntoa(clientAdress.sin_addr);
		
		pthread_mutex_lock(&outputLock);
		cout << "Prihvacen klijent sa adrese " << client << ", pod brojem " << commSock << "." << endl;
		pthread_mutex_unlock(&outputLock);
		
		pthread_t *newThread = new pthread_t();
		
		pthread_mutex_lock(&addThreadsLock);
		
		thread_info param;
		param.commSock = commSock;
		param.threadID = pool->getFreeID();
		if (
			pthread_create(
				newThread,
				NULL,
				run_core,
				(void *)&param
			)
		) {
			pthread_mutex_lock(&outputLock);
			cout << "Greska prilikom kreiranja dretve za " << client << endl;
			pthread_mutex_unlock(&outputLock);
			close(commSock);
			continue;
		} else {
			pool->addThread(commSock, newThread);
			
		}
		pthread_mutex_unlock(&addThreadsLock);
		
		/*
		pthread_mutex_lock(&outputLock);
		cout << "Klijent " << client << " je zavrsio sesiju." << endl;
		pthread_mutex_unlock(&outputLock);*/
		
	}
	pool->programToEnd();
	while(!canEnd){
	}
	delete studentsAndClassroomData;
	
	close(listenerSocket);
	
	pthread_mutex_destroy(&dataLock);
	pthread_mutex_destroy(&outputLock);
	pthread_mutex_destroy(&threadsLock);
	pthread_mutex_destroy(&addThreadsLock);
	
	cout << "Server se ugasio." << endl;
	
	return 0;
}
