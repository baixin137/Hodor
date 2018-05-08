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

class ClusterNode {
public:
	struct sockaddr_in nodeAddr;
};

class CmdReceiver {
public:
	CmdReceiver();

	bool StartInternalThread();

	void WaitForInternalThreadToExit() {
		(void) pthread_join(_thread, NULL);
	}

	void Listen();

private:
	static void * ListenFunc(void * This) {((CmdReceiver *)This)->Listen(); return NULL;}

	pthread_t _thread;
};

class CmdSender {
public:
	CmdSender();

	bool StartInternalThread();

	void WaitForInternalThreadToExit() {
		(void) pthread_join(_thread, NULL);
	}

	void Send();

private:
	static void * SendFunc(void * This) {((CmdSender *)This)->Send(); return NULL;}

	pthread_t _thread;
};

// class FailureDetector {
// private:
// 	bool ismaster;
// 	bool isbackup;
// 	ClusterNode* master;
// 	ClusterNode* backupMaster;

// public:
// 	FailureDetector(bool m, bool s); // is master/backup master
// 	unordered_map<string, ClusterNode*> MembershipList;
// };

#endif