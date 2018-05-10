#include "FailureDetector.h"

// Message::Message(CommandType t, string m) {
// 	type = t;
// 	message = m;
// }

CmdMaster::CmdMaster(JobType type) {
	if (type == kServerListen) {
		StartInternalThreadListen();
	}
	else if (type == kServerSend) {
		StartInternalThreadSend();
	}
	else {
		cout << "Please specify thread role: kServerListen or kServerSend" << endl;
		return;
	}
}

bool CmdMaster::StartInternalThreadListen() {
	return (pthread_create(&_thread, NULL, ListenFunc, this) == 0);
}

bool CmdMaster::StartInternalThreadSend() {
	return (pthread_create(&_thread, NULL, SendFunc, this) == 0);
}

void CmdMaster::MessageSerialize(char* message) {
	pthread_mutex_lock(&NewChangeLock);

	memcpy(message, "REQ", 3);

	unsigned char a1 = 172;
	unsigned char a2 = 31;
	unsigned char a3 = 11;
	unsigned char a4 = 202;

	memcpy(message + 3, &a1, 1);
	memcpy(message + 4, &a2, 1);
	memcpy(message + 5, &a3, 1);
	memcpy(message + 6, &a4, 1);

	int pos = 7;

	for (auto change : newChanges) {
		string b1, b2, b3, b4;

		istringstream ss(change.first);

		getline(ss, b1, '.');
		getline(ss, b2, '.');
		getline(ss, b3, '.');
		getline(ss, b4, '.');

		unsigned char c1 = char(stoi(b1));
		unsigned char c2 = char(stoi(b2));
		unsigned char c3 = char(stoi(b3));
		unsigned char c4 = char(stoi(b4));

		if (change.second->type == kJoin) {
			memcpy(message + pos, "JON", 3);
		}
		else if (change.second->type == kLeave) {
			memcpy(message + pos, "LEV", 3);
		}
		else {
			cout << "I don't think this is going to happen unless we have a new command type." << endl;
		}
		pos += 3;

		memcpy(message + (pos++), &c1, 1);
		memcpy(message + (pos++), &c2, 1);
		memcpy(message + (pos++), &c3, 1);
		memcpy(message + (pos++), &c4, 1);
	}
	newChanges.clear();
	pthread_mutex_unlock(&NewChangeLock);
	return;
}

void CmdMaster::SendToClient(string des, char* msg) {
	struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;

    char recvBuffer[2048] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error client");
        return;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, des.c_str(), &serv_addr.sin_addr)<=0) 
    {
        perror("Invalid address/ Address not supported");
        return;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection Failed");
        // fail the node
        pthread_mutex_lock(&NewChangeLock);
        newChanges[des] = new Change(kLeave, des);
        membershipList.erase(des);
        pthread_mutex_unlock(&NewChangeLock);
        
        return;
    }
    send(sock, msg, strlen(msg), 0);
    cout << "sendBuffer message sent: " << msg << endl;
    cout << "destination: " << des << endl;

    close(sock);

    return;
}

void CmdMaster::Listen() {
	cout << "Master starts to listen" << endl;
    int server_fd, new_socket, valread, bd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[2048] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Create socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
      
    if ((bd = ::bind(server_fd, (struct sockaddr *)&address, sizeof(address))) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 256) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) > 0) {

	    valread = read(new_socket , buffer, 2048);
	    cout << "Command received: " << buffer << endl;
	    size_t pos = 0;

	    while (pos < valread) {
	    	if (!strncmp(buffer + pos, "ACK", 3)) {
	    		string address = readMessage(buffer, pos);

	    		pthread_mutex_lock(&NewChangeLock);
	    		newChanges[address] = new Change(kJoin, address);
	    		membershipList.insert(address);
	    		pthread_mutex_unlock(&NewChangeLock);
	    	}
	    	else if (!strncmp(buffer + pos, "JON", 3)) {
	    		string address = readMessage(buffer, pos);

	    		pthread_mutex_lock(&NewChangeLock);
	    		newChanges[address] = new Change(kJoin, address);
	    		membershipList.insert(address);
	    		pthread_mutex_unlock(&NewChangeLock);
	    	}
	    	// else if (!strncmp(buffer + pos, "LEV", 3)) {
	    	// 	string address = readMessage(buffer, pos);

	    	// 	pthread_mutex_lock(&NewChangeLock);
	    	// 	newChanges[address] = new Change(kLeave);
	    	// 	pthread_mutex_unlock(&NewChangeLock);
	    	// }
	    	else {
	    		cout << "Unrecognized message type from slave." << endl;
	    		return;
	    	}
	    }

	    close(new_socket);
	}
}

void CmdMaster::Send() {
	while (true) {
		sleep(HEARTBEATPERIOD);

		// check timeout
		// use tcp connection to check time out

		// send pings to slaves
		char message[2048];
		MessageSerialize(message);

		for (auto slave : membershipList) {
			SendToClient(slave, message);
		}
	}
}

string CmdMaster::readMessage(char* buffer, size_t& pos) {
    pos += 3;
    string address;
    for (size_t i = 0; i < 4; i++) {
            address += to_string((unsigned int)((unsigned char)(buffer[pos++])));
            if (i != 3)
                    address += ".";
    }
    cout << address << endl;
    return address;
}

CmdSlave::CmdSlave() {
	// checkin with master first
	struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;

    char msg[100];

    memcpy(msg, "JON", 3);
    memcpy(msg + 3, &A1, 1);
    memcpy(msg + 4, &A2, 1);
    memcpy(msg + 5, &A3, 1);
    memcpy(msg + 6, &A4, 1);

    char recvBuffer[2048] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error client");
        return;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "172.31.11.202", &serv_addr.sin_addr)<=0) 
    {
        perror("Invalid address/ Address not supported");
        return;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection Failed");
        return;
    }
    send(sock, msg, strlen(msg), 0);
    cout << "Join message sent" << endl;

    close(sock);

	StartInternalThread();

	return;
}

bool CmdSlave::StartInternalThread() {
	return (pthread_create(&_thread, NULL, ListenFunc, this) == 0);
}

void CmdSlave::Listen() {
	cout << "Slave starts to listen" << endl;
    int server_fd, new_socket, valread, bd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[2048];

    char ack[100];
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Create socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
      
    if ((bd = ::bind(server_fd, (struct sockaddr *)&address, sizeof(address))) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) > 0) {

	    valread = read(new_socket , buffer, 2048);
	    cout << "Commend received: " << buffer << endl;
	    size_t pos = 0;

	    while (pos < valread) {
	    	if (!strncmp(buffer + pos, "REQ", 3)) {
	    		char myaddress[100];
	    		memcpy(myaddress, "ACK", 3);
	    		memcpy(myaddress + 3, &A1, 1);
	    		memcpy(myaddress + 4, &A2, 1);
	    		memcpy(myaddress + 5, &A3, 1);
	    		memcpy(myaddress + 6, &A4, 1);

	    		send(new_socket, myaddress, strlen(myaddress), 0);
	    		cout << "sent ack back to master" << endl;
	    		pos += 7;
	    	}
	    	else if (!strncmp(buffer + pos, "JON", 3)) {
	    		string address = readMessage(buffer, pos);

	    		pthread_mutex_lock(&NewChangeLock);
	    		newChanges[address] = new Change(kJoin, address);
	    		membershipList.insert(address);
	    		pthread_mutex_unlock(&NewChangeLock);
	    		cout << "received new join: " << address << endl;
	    	}
	    	else if (!strncmp(buffer + pos, "LEV", 3)) {
	    		string address = readMessage(buffer, pos);

	    		pthread_mutex_lock(&NewChangeLock);
	    		newChanges[address] = new Change(kLeave, address);
	    		membershipList.erase(address);
	    		pthread_mutex_unlock(&NewChangeLock);
	    		cout << "received new leave: " << address << endl;
	    	}
	    	else {
	    		cout << "Unrecognized message type from master." << endl;
	    		return;
	    	}
	    }

	    close(new_socket);
	}
}

string CmdSlave::readMessage(char* buffer, size_t& pos) {
    pos += 3;
    string address;
    for (size_t i = 0; i < 4; i++) {
            address += to_string((unsigned int)((unsigned char)(buffer[pos++])));
            if (i != 3)
                    address += ".";
    }
    cout << address << endl;
    return address;
}