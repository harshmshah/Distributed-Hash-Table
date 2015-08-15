//
//  server3.h
//  SocketProgrammingProject
//
//  Created by HarshRashiMeet on 4/14/15.
//  Copyright (c) 2015 HarshRashiMeet. All rights reserved.
//

#include <stdio.h>
#include <map>
#include <iostream>
#include <fstream>
#include <string>
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

#define KEY_LENGTH 5
#define VALUE_LENGTH 7

using namespace std;

void Convert_to_char(string s, char * arr, int n) {
  for (int i = 0; i<n; i++) {
    arr[i] = s[i];
  }
}

void Convert_to_string(string &s, char *arr, int n) {
  s.append(arr);
}

// Builds the key-value structure (hash maps) fby reading the input from the text file for the server.
void BuildServerMappings(map<string,string> &mymap, const char* inputfile, int rows) {
  ifstream input_file;
  input_file.open(inputfile,std::ifstream::in);
  string key;
  string value;
  string line;
  for (int i=0; i<4; i++) {
    getline(input_file, line);
      //cout << "line " << line << endl;
    key = line.substr(0,5);
      // cout << "key" << key << endl;
    value = line.substr(6);
      //cout << "value" << value << endl;
    mymap[key] = value;
 }
}

// Builds the key-value structure (hash maps) fby reading the input from the text file for the client.
void BuildClientMappings(map<string,string> &mymap, const char *inputfile, int rows) {
  ifstream input_file;
  input_file.open(inputfile,ifstream::in);
  string key;
  string value;
  string line;
  while (!input_file.eof()) {
    getline(input_file,line);
      //cout << "line" << line << endl;
    size_t found_elem = line.find(" ");
    key = line.substr(0,(found_elem));
      //cout << "key" << key << endl;
    value = line.substr(found_elem + 1);
      //cout << "value" << value <<endl;
    mymap[key] = value;
  }
}

// Checks if the given key has a corresponding mapped value in the hash
// table for a particular server.
bool CheckForKey(map <string,string> &mymap, string key) {
  if (mymap.find(key) == mymap.end())
    return false;
  else
    return true;
}

// Description : Creates a Socket connection and binds it.
// Parameters : port_num - Port number to which the host is accepting request
//              socktype - Socket type for the connection i.e. SOCK_DGRAM or SOCK_STREAM
//              curr - It points to the address structure for the host
// Return val : Socket File Descriptor for the connection
int GetSockfd(const char *port_num, int socktype, struct addrinfo * &curr) {
  
  int status, sockfd;
  struct addrinfo hints;
  struct  addrinfo *res, *p;
  
  // Code from Beej’s guide to network programming
  memset(&hints, 0, sizeof(addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = socktype;
  if ((status = getaddrinfo("nunki.usc.edu",port_num, &hints, &res)) != 0) {
    cout<<"Error in getadrr";
    exit(-1);
  }
  for(p = res; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
                         p->ai_protocol)) == -1) {
      perror("listener: socket");
      continue; }
    bind(sockfd, p->ai_addr, p->ai_addrlen);
    break;
  }
  curr  = p;
  return sockfd;
}

// Returns the typecast of sockaddr
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//Return the port number the host is connected to
int get_port_num(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return (((struct sockaddr_in*)sa)->sin_port);
  }
  return (((struct sockaddr_in6*)sa)->sin6_port);
}

// Returns the IP address of the host
string get_IPaddress(struct sockaddr * agent) {
  char ipstr[INET6_ADDRSTRLEN];
    //struct sockaddr *agent = (struct sockaddr *)&host;
  string ip = inet_ntop(agent->sa_family, get_in_addr(agent), ipstr, sizeof(ipstr));
  return ip;
}

// Returns the IP address of the host
string get_IPaddress(struct sockaddr_storage host) {
  char ipstr[INET6_ADDRSTRLEN];
  struct sockaddr *agent = (struct sockaddr *)&host;
  string ip = inet_ntop(agent->sa_family, get_in_addr(agent), ipstr, sizeof(ipstr));
  return ip;
}

// Returns the IP address of the host
string get_myIP(struct addrinfo * curr) {
  void *addr;
  char ipstr[INET6_ADDRSTRLEN];
  if (curr->ai_family == AF_INET) { // IPv4
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)curr->ai_addr;
    addr = &(ipv4->sin_addr);
    
  } else { // IPv6
    struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)curr->ai_addr;
    addr = &(ipv6->sin6_addr);
  }
  string myip = inet_ntop(curr->ai_family, addr, ipstr, sizeof(ipstr));
  return myip;
  
}

