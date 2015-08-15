all : client1 client2 server1 server2 server3

client1 : client1.cpp 
	g++ -o client1.o client1.cpp -lsocket -lnsl -lresolv

client2 : client2.cpp 
	g++ -o client2.o client2.cpp -lsocket -lnsl -lresolv

server1 : dhtserver1.cpp
	g++ -o dhtserver1.o dhtserver1.cpp -lsocket -lnsl -lresolv

server2 : dhtserver2.cpp
	g++ -o dhtserver2.o dhtserver2.cpp -lsocket -lnsl -lresolv

server3 : dhtserver3.cpp
	g++ -o dhtserver3.o dhtserver3.cpp -lsocket -lnsl -lresolv		

clean :
	yes|rm *.o

