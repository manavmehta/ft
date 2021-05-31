server:
	gcc server.cpp -o server

client:
	gcc client.cpp -o client

clean:
	rm server
	rm client

all: server client