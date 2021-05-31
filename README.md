# TCP File Transfer Application

The application has two primary parts - Client and Server - and the reliable transfer is ensured using TCP with help of socket() API specifying SOCK_STREAM.

### The Client’s flow:

* Checks for validity of arguments - needs server’s ip and port to fire up
* Creates sockfd which controls the flow or the control of the connection
* Connects socket ref by sockfd to server_addr (address and port specified)
* Set up data connection in listenfd descriptor and binds the socket to data_addr port
* Then until user quits, listens to commands, parses it using helpers, and executes them using the respective helper functions

### The Server’s flow:
* Opens a TCP connection and starts listening to incoming connections
* Binds the listening file descriptor to the specified port
* Creates a fork of its process for every incoming connection, closing listenfd for that particular fork
* Receives client’s data and command
* Takes the necessary actions
