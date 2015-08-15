//
//  main.cpp
//  SocketProgrammingProject
//
//  Created by HarshRashiMeet on 3/29/15.
//  Copyright (c) 2015 HarshRashiMeet. All rights reserved.
//

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "buildstructure.h"
#define MYPORT 21991

using namespace std;

void CommunicateServer2(int, string, map<string,string> &server1map, struct sockaddr_storage their_addr, socklen_t addrlen, int );

// Description : Search if the key-value mapping is in Server 1 and return the value to Client. Else communicate to the Server 2
//               to get the required value for the key.
// Param : sockfd - UDP socket for communication with client
//          addrinfo - Data structure used to store important fields of the server. This is filled by getaddrinfo().
//          server1map - Reference to the hash map of key-value pairs in Server 1's memory
//          client - The Client which is connecting to the Server 1 (Client 1 or Client 2)
void SearchforMappings(int sockfd, struct addrinfo *serverinfo, map<string,string> &server1map, int client ) {
  struct sockaddr_storage their_addr;
  socklen_t addrlen = sizeof(their_addr);
  char client_msg[KEY_LENGTH];
  int bytes_recv = recvfrom(sockfd, &client_msg, KEY_LENGTH, 0,(struct sockaddr *) &their_addr, &addrlen);
  cout << "The Server 1 has received a request with key " << client_msg << " form Client " << client <<" with port number "<< get_port_num((struct sockaddr *)&their_addr) << " and IP address "   << get_IPaddress(their_addr) <<"." <<endl;
  if (CheckForKey(server1map, client_msg)) {
    string value = server1map[client_msg];
    char value_server1[VALUE_LENGTH];
    Convert_to_char(value, value_server1, VALUE_LENGTH);
    int bytes_sent_server1 = sendto(sockfd, &value_server1, VALUE_LENGTH, 0, (struct sockaddr *) &their_addr, addrlen);
    cout << "The Server 1 sends the reply POST " << value_server1 << " to the Client "<< client <<" with port number " << get_port_num((struct sockaddr *)&their_addr) << " and IP address " << get_IPaddress(their_addr) <<"." << endl;
  }
  else
    CommunicateServer2(sockfd, client_msg, server1map,their_addr, addrlen, client);
}

// Description : Establishes Connection of Server 1 with Server 2. Server 1 sends the key to Server 2 and receives the required value from
//               Server2 and sends it back to the client.
// Param : client_sockfd - UDP socket for communication with client
//         server1map - Reference to the hash map of key-value pairs in Server 1's memory
//         their_addr - Information of the client
//         addrlen - Size of their_adr
//         client - The Client which is connecting to the Server 1 (Client 1 or Client 2)
void CommunicateServer2(int client_sockfd, string msg, map<string,string> &server1map, struct sockaddr_storage their_addr,socklen_t addrlen, int client) {
  char client_msg[KEY_LENGTH];
  Convert_to_char(msg, client_msg, KEY_LENGTH);
  struct sockaddr myaddr;
  struct addrinfo *curr;
  int sockfd = GetSockfd("22991", SOCK_STREAM, curr);
  if (curr == NULL) {
    cout << "listener: failed to bind socket" << endl;
    exit(-1);
  }
  int con = connect(sockfd, curr -> ai_addr, curr -> ai_addrlen);
  if (con == -1) {
    cout << "Error in connect";
    exit(-1);
  }
  socklen_t s_len = sizeof(myaddr);
  getsockname(sockfd, (struct sockaddr*)&myaddr, &s_len);
  cout << "The Server 1 sends the request GET " << client_msg << " to the Server 2. " << endl;
  cout << "The TCP port number is " << get_port_num(&myaddr) << " and the IP address is " << get_IPaddress(&myaddr) <<"." <<endl;
  int bytes_sent = send(sockfd, &client_msg, KEY_LENGTH, 0);
  
  // RECIEVE MSG FROM SERVER 2
  char server2_msg[VALUE_LENGTH];
  int bytes_recv_serv2 = recv(sockfd, &server2_msg, VALUE_LENGTH, 0);
  
  // Put Values into its own map
  server1map[client_msg] = server2_msg;
  cout << "The Server 1 has received the value " << server2_msg << " from the Server 2 with port number " << get_port_num(curr->ai_addr) <<"." << " and IP address " << get_myIP(curr) << endl;
  
  // Close connection with Server 2
  close(sockfd);
  cout << "The Server 1 closed the TCP connection with the Server 2" << endl;
  int bytes_sent_client = sendto(client_sockfd, &server2_msg, VALUE_LENGTH, 0, (struct sockaddr *)&their_addr, addrlen);
  cout << "The Server 1, sent reply POST " << server2_msg << " to Client "<< client <<" with port number " << get_port_num((struct sockaddr *)&their_addr) << " and IP address " << get_IPaddress(their_addr) <<"." << endl;
}

int main(int argc, const char * argv[]) {
  struct addrinfo *curr;
  map <string, string> server1map;
  // Build the hash map to store the key-value pairs in the memory.
  BuildServerMappings(server1map, "server1.txt", 4);
  int sockfd =  GetSockfd("21991", SOCK_DGRAM, curr);
  if (curr == NULL) {
    cout << "listener: failed to bind socket" << endl;
    exit(-1);
  }
  cout << "The Server 1 has UDP port number " << get_port_num(curr->ai_addr) <<" and IP address " << get_myIP(curr) <<"." <<endl;
  // Call the Server 1 function infinitely until the User commands Ctrl + C.
  int i = 1;
  while (1) {
    SearchforMappings(sockfd, curr, server1map, i);
    i++;
  }
  return 0;
}
