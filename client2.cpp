  //
  //  client1.cpp
  //  SocketProgrammingProject
  //
  //  Created by HarshRashiMeet on 4/3/15.
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
#include <map>
#include <vector>
using namespace std;

// Prompts User for the search keyword.
// Creates Connection with the Server 1 and requests value for the respective
// search keyword entered by the User
void CommunicateToServer1(map<string,string> & client2map) {
  cout << "Please Enter Your Search (USC, UCLA etc.):" << endl;
  string search;
  cin >> search;
  string key = client2map[search];
  cout << "The Client 2 has received a request with search word " << search << " which maps to key " << key <<endl;
  // Code from Beej’s guide to network programming
  int status, socket_desc;
  struct sockaddr_storage incoming_info, their_addr;
  struct addrinfo hints;
  struct  addrinfo *res, *p;
  memset(&hints, 0, sizeof(addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  struct sockaddr myaddr;
  char ipstr[INET6_ADDRSTRLEN];
  if ((status = getaddrinfo("nunki.usc.edu", "21991", &hints, &res)) != 0) {
    cout<<"Error in getadrr";
    exit(-1);
  }
  for(p = res; p != NULL; p = p->ai_next) {
    if ((socket_desc = socket(p->ai_family, p->ai_socktype,
                              p->ai_protocol)) == -1) {
      perror("listener: socket");
      continue; }
    bind(socket_desc, p->ai_addr, p->ai_addrlen);
    break;
  }
  if (p == NULL) {
    cout << "listener: failed to bind socket" << endl;
    exit(-1);
  }
  void *addr;
  if (p->ai_family == AF_INET) { // IPv4
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
    addr = &(ipv4->sin_addr);
      //ipver = "IPv4";
  } else { // IPv6
    struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)res->ai_addr;
    addr = &(ipv6->sin6_addr);
      //ipver = "IPv6";
  }
  
  inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
  socklen_t s_len = sizeof(myaddr);
  getsockname(socket_desc, (struct sockaddr*)&myaddr, &s_len);
  char msg[] = "Hi I am client";
  int len = key.length();
  char key_client[KEY_LENGTH];
  Convert_to_char(key, key_client, KEY_LENGTH);
  int bytes_sent = sendto(socket_desc, &key_client, KEY_LENGTH, 0, p->ai_addr, p->ai_addrlen);
  cout << "The Client 2 sends the request GET " << key << " to the Server 1 with port number " << get_port_num(p->ai_addr) << " and IP address " << ipstr <<"." << endl;
  cout << " bytes sent" << bytes_sent << endl;
  cout << "The Client 2's port number is " << get_port_num(&myaddr) << "and the IP address is " << ipstr <<"." << endl;
  
  
  // Receive value from Server1
  string msg_from_server1;
  char msg_s1[VALUE_LENGTH];
  socklen_t addrlen = sizeof(their_addr);
  int bytes_recv = recvfrom(socket_desc, &msg_s1, VALUE_LENGTH, 0,(struct sockaddr *) &their_addr, &addrlen);
  struct sockaddr *server1ip = (struct sockaddr *)&their_addr;
  char server1_ipstr[INET6_ADDRSTRLEN];
  inet_ntop(server1ip->sa_family, get_in_addr(server1ip), server1_ipstr, sizeof(server1_ipstr));
  int server1_portnum = get_port_num(server1ip);
  msg_from_server1.append(msg_s1);
  cout << " The Client 2 received the value " << msg_s1 << " from the Server 1 with port number " << server1_portnum << " and IP address " << get_IPaddress(their_addr) <<"." <<endl;
  cout << "The Client 2's port number is " << get_port_num(&myaddr) << " and the IP address is " << ipstr <<"." << endl;
  close(socket_desc);
  
}


int main(int argc, const char * argv[]) {
  map <string,string> client2map;
  // Build the hash map to store the key-value pairs in the memory.
  BuildClientMappings(client2map, "client2.txt", 12);
  // Call Client 2's function infinitely till the User commands Ctrl + C
  while (1) {
    CommunicateToServer1(client2map);
  }
  return 0;
}
