#include "FailureDetector.h"

CmdReceiver::CmdReceiver() {

}

bool CmdReceiver::StartInternalThread() {
	return (pthread_create(&_thread, NULL, ListenFunc, this) == 0);
}

void CmdReceiver::Listen() {
    int server_fd, new_socket, valread, bd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    char const *hello = "Hello from the outside";
      
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Create socket failed");
        exit(EXIT_FAILURE);
    }
      
    // Forcefully attaching socket to the port 8080
    // if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
    //                                               &opt, sizeof(opt)))
    // {
    //     perror("Attach socket to PORT failed");
    //     exit(EXIT_FAILURE);
    // }
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

	    valread = read(new_socket , buffer, 1024);
	    cout << "Commend received: " << buffer << endl;


	    send(new_socket , hello , strlen(hello) , 0 );
	    printf("Hello message sent\n");

	    close(new_socket);
	}
}

CmdSender::CmdSender() {

}

bool CmdSender::StartInternalThread() {
	return (pthread_create(&_thread, NULL, SendFunc, this) == 0);
}

void CmdSender::Send() {
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char const *hello = "Hello from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error client");
        return;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "69.25.143.32", &serv_addr.sin_addr)<=0) 
    {
        perror("Invalid address/ Address not supported");
        return;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection Failed");
        return;
    }
    send(sock , hello , strlen(hello) , 0 );
    cout << "Hello message sent" << endl;

    valread = read( sock , buffer, 1024);
    cout << "buffer is: " << buffer << endl;

    close(sock);

    return;
}

// FailureDetector::FailureDetector() {

// }