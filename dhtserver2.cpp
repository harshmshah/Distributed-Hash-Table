//
//  server2.cpp
//  SocketProgrammingProject
//
//  Created by HarshRashiMeet on 4/13/15.
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

using namespace std;

void CommunicateServer3(int sockfd, string key, map<string,string> &server2map, struct sockaddr_storage incoming_info);

// Description : Search if the key-value mapping is in Server 2 and return the value to Server 1. Else communicate to the Server 3
//               to get the required value for the key.
// Param : sockfd - TCP socket for communication with Server 1
//          serverinfo - Data structure used to store important fields of the server. This is filled by getaddrinfo().
//          server1map - Reference to the hash map of key-value pairs in Server 2's memory
void SearchforMappings(int sockfd, struct addrinfo *serverinfo, map<string,string> &server2map ) {
  char server1_msg[KEY_LENGTH] = {0};
  struct sockaddr_storage incoming_info;
  int l =  listen(sockfd, 10);
  if (l == -1) {
    cout<<"error in listen";
    exit(1);
  }
    // Call to accept
  socklen_t addr_size  = sizeof(incoming_info);
  int new_sock_desc = accept(sockfd, (struct sockaddr *)&incoming_info, &addr_size);
  if (new_sock_desc == -1) {
    cout<<"Error in new_sock_desc";
    exit(1);
  }
  int bytes = recv(new_sock_desc, &server1_msg, KEY_LENGTH, 0);
  cout << "The Server 2 has received a request with key " << server1_msg <<" from the Server1 with port number "<< get_port_num((struct sockaddr *)&incoming_info)  << " and IP address " << get_IPaddress(incoming_info) <<"." <<endl;
  if (CheckForKey(server2map, server1_msg)) {
    string value = server2map[server1_msg];
    char server2_value[VALUE_LENGTH];
    Convert_to_char(value, server2_value, VALUE_LENGTH);
    int bytes_sent = send(new_sock_desc, &server2_value, VALUE_LENGTH, 0);
    cout << " The Server 2 sends the reply POST " << server2_value << " to the Server 1 with port number " << get_port_num((struct sockaddr *)&incoming_info) << " and IP address " << get_IPaddress(incoming_info) <<"." << endl;
  }
  else
    CommunicateServer3(new_sock_desc,server1_msg, server2map, incoming_info);
}
// Description : Establishes Connection of Server 2 with Server 3. Server 2 sends the key to Server 3 and receives the required value from
//               Server 3 and sends it back to the Server 1.
// Param : sockfd - TCP socket for communication with Server 1
//         server1map - Reference to the hash map of key-value pairs in Server 2's memory
//         incoming_info - Information of the client
void CommunicateServer3(int sockfd, string key, map<string,string> &server2map, struct sockaddr_storage incoming_info) {
  struct sockaddr myaddr;
  char key_server1[KEY_LENGTH];
  Convert_to_char(key, key_server1, KEY_LENGTH);
  struct addrinfo *curr;
  int sock_server3 = GetSockfd("23991", SOCK_STREAM, curr);

  if (curr == NULL) {
    cout << "listener: failed to bind socket" << endl;
    exit(-1);
  }
  int con = connect(sock_server3, curr -> ai_addr, curr -> ai_addrlen);
  if (con == -1) {
    cout << "Error in connect";
    exit(-1);
  }
  socklen_t s_len = sizeof(myaddr);
  getsockname(sock_server3, (struct sockaddr*)&myaddr, &s_len);
  int bytes_sent_s3 = send(sock_server3, &key_server1, KEY_LENGTH, 0);
  cout << "Server 2 sends the request GET " << key_server1 << " to the Server 3 " << endl;
  cout << "The TCP port number is " << get_port_num(&myaddr) << " and the IP address is " << get_IPaddress(&myaddr) <<"." << endl;
  
  //RECEIVE MESSAGE From Server 3
  char value_server3[VALUE_LENGTH];
  int bytes_recv_s3 = recv(sock_server3, &value_server3, VALUE_LENGTH, 0);

  // Server 2 saves the value in its own map for future usage.
  server2map[key_server1] = value_server3;
  
  cout << "The Server 2 received the value " << value_server3 << " from the Server 3 with port number " << get_port_num(curr->ai_addr) << " and IP address " << get_myIP(curr) <<"." <<endl;
  
  //Server 2 Closes the connection with Server 3
  close(sock_server3);
  cout << "The Server 2 closed the TCP connection with the Server 3" << endl;
  
  // Server 2 sends the value back to Server 1
  int bytes_sent_s1 = send(sockfd, &value_server3, VALUE_LENGTH, 0);
  cout << "The Server 2, sent reply POST " << value_server3 << " to the Server 1 with port number " << get_port_num((struct sockaddr *)&incoming_info) << " and IP address" << get_IPaddress(incoming_info) <<"." << endl;
  
}

int main() {
  map<string,string> server2_map;
  struct addrinfo *curr;
  // Build the hash map to store the key-value pairs in the memory.
  BuildServerMappings(server2_map, "server2.txt", 4);
  int sockfd =  GetSockfd("22991", SOCK_STREAM, curr);
  if (curr == NULL) {
    cout << "listener: failed to bind socket" << endl;
    exit(-1);
  }
  cout << "The Server 2 has TCP port number " << get_port_num(curr->ai_addr) <<" and IP address " << get_myIP(curr) <<"." << endl;
  // Call the Server 2 function infinitely until the User commands Ctrl + C.
  while (1) {
    SearchforMappings(sockfd, curr, server2_map);
  }
  return 0;
  
}

