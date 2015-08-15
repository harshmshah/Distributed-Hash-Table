//
//  server3.cpp
//  SocketProgrammingProject
//
//  Created by HarshRashiMeet on 4/14/15.
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

// Description : Search if the key-value mapping is in Server 3 and return the value to Server 1.
// Param : sockfd - TCP socket for communication with Server 2
//         serverinfo - Data structure used to store important fields of the server. This is filled by getaddrinfo().
//         server1map - Reference to the hash map of key-value pairs in Server 3's memory
void SearchforMappings(int sockfd, struct addrinfo *serverinfo, map<string,string> &server3map) {
  char server2_msg[KEY_LENGTH];
  struct sockaddr_storage incoming_info;
  int l =  listen(sockfd, 10);
  if (l == -1) {
    cout<<"error in listen";
    exit(1);
  }
  
  // Call to accept.
  // Code from Beej’s guide to network programming
  
  socklen_t addr_size  = sizeof(incoming_info);
  int new_sock_desc = accept(sockfd, (struct sockaddr *)&incoming_info, &addr_size);
  if (new_sock_desc == -1) {
    cout<<"Error in new_sock_desc";
    exit(1);
  }
  int bytes = recv(new_sock_desc, &server2_msg, KEY_LENGTH, 0);
  cout << "The Server 3 has received a request with key " << server2_msg <<" from the Server 2 with port number "<< get_port_num((struct sockaddr *)&incoming_info)  << " and IP address " << get_IPaddress(incoming_info) <<"." <<endl;
    string value = server3map[server2_msg];
    char server3_value[VALUE_LENGTH];
    Convert_to_char(value, server3_value, VALUE_LENGTH);
    int bytes_sent = send(new_sock_desc, &server3_value, VALUE_LENGTH, 0);
    cout << " The Server 3, sent reply POST " << server3_value << " to the Server 2 with port number " << get_port_num((struct sockaddr *)&incoming_info) << " and IP address " << get_IPaddress(incoming_info) <<"." << endl;
  
}

int main() {
  map <string, string> server3_map;
  struct addrinfo *curr;
  // Build the hash map to store the key-value pairs in the memory.
  BuildServerMappings(server3_map, "server3.txt", 4);
  int sockfd =  GetSockfd("23991", SOCK_STREAM, curr);
  if (curr == NULL) {
    cout << "listener: failed to bind socket" << endl;
    exit(-1);
  }
  cout << "The Server 3 has TCP port number " << get_port_num(curr->ai_addr) <<" and IP address " << get_myIP(curr) << "." <<endl;
  // Call the Server 3 function infinitely until the User commands Ctrl + C.
  while (1) {
    SearchforMappings(sockfd, curr, server3_map);
  }
  return 0;
}

