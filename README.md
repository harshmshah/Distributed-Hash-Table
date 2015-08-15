a) Name : Harsh M. Shah

c) I have done my project in C++. Inititally I have read the inputs from textfiles and saved them in a structure of hash maps (maps<string, string>) which are the functionality of C++ STL libraries. After the initialization phase the communication between the client and servers happen and the required key-value pairs are retrived from the memory of the respective server and returned back to the client.

d) Code Files:
	client1.cpp :
	- Initially reads the search-key pairs from the input file and stores it in the hash map structure
	- Requests the User for the search keyword
	- Establishes the UDP Connection with Server 1 and exchanges data on the established connection
	- Displays the required result to the User 

	client2.cpp :
	- Initially reads the search-key pairs from the input file and stores it in the hash map structure
	- Requests the User for the search keyword
	- Establishes the UDP Connection with Server 1 and exchanges data on the established connection
	- Displays the required result to the User 

	dhtserver1.cpp :
	- Initially reads the key-value pairs from the input file and stores it in the hash map structure
	- It establishes the UDP connection on the specified port number and waits to receive information from the client 
	- It check if its map contains the desired value for the key, if exists then return the value back to the client
	- If the value is not in the local table it connects to Server 2 via TCP connection and asks for the value for a particular key.
	- On receiving the value it closes the connection with Server 2 and adds the value in its own mapping
	- Then return the value to client 1

	dhtserver2.cpp :
	- Initially reads the key-value pairs from the input file and stores it in the hash map structure
	- It establishes the TCP connection on the specified port number and waits to receive information from the Server 1
	- It check if its map contains the desired value for the key, if exists then returns the value back to the Server 1
	- If the value is not in the local table it connects to Server 3 via TCP connection and asks for the value for a particular key
	- On receiving the value it closes the connection with Server 3 and adds the value in its own mapping
	- Then return the value to Server 1

	dhtserver3.cpp :
	- Initially reads the key-value pairs from the input file and stores it in the hash map structure
	- It establishes the TCP connection on the specified port number and waits to receive information from the Server 2
	- It returns the value mapped to a particular key recieved from Server 2 and returns it back to the Server 2

	buildstructure.h :
	- This is the header file which performs the operation of adding the key-value pairs taken from the input file and adding it to the memory. 
	- This header file also contains some other important functions. 
	- It is common for all client and servers

e) I have created the makefile which compiles all the programs together. Then all need to be executed individually n the sequence as follows :
		1) ./dhtserver1.o 
		2) ./dhtserver2.o
		3) ./dhtserver3.o
		4) ./client1.o
		5) ./client2.o
	All the client and server programs run infinitley in a while loop. So the User needs to end the execution of programs using	Ctrl + C. 


