#ifndef FAILUREDETECTOR_H
#define FAILUREDETECTOR_H
/*
	Centralized failure detector, one master, one backup master, the rest are slaves
	Once the master fails the backup master steps up to be the new master, and Paxos is 
	used to find a new backup master
*/
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#include "BufferManager.h"

// class Message {
// public:
// 	Message(CommandType t, string m);

// 	CommandType type;
// 	string message;
// };

class ClusterNode {
public:
	struct sockaddr_in nodeAddr;
};

class CmdMaster {
public:
	CmdMaster(JobType type);

	bool StartInternalThreadListen();
	bool StartInternalThreadSend();

	void WaitForInternalThreadToExit() {
		(void) pthread_join(_thread, NULL);
	}

	void Listen();
	void Send();

private:
	static void * ListenFunc(void * This) {((CmdMaster *)This)->Listen(); return NULL;}
	static void * SendFunc(void * This) {((CmdMaster *)This)->Send(); return NULL;}

	void MessageSerialize(char* message);
	void SendToClient(string des, char* msg);

	string readMessage(char* buffer, size_t& pos);
	pthread_t _thread;
};

class CmdSlave {
public:
	CmdSlave();

	bool StartInternalThread();

	void WaitForInternalThreadToExit() {
		(void) pthread_join(_thread, NULL);
	}

	void Listen();

private:
	static void * ListenFunc(void * This) {((CmdSlave *)This)->Listen(); return NULL;}

	string readMessage(char* buffer, size_t& pos);
	pthread_t _thread;
};

#endif