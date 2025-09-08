server:
	mkdir ./build
	mkdir ./test
	g++ src/util.cpp client.cpp -o ./build/client && \
	g++ src/util.cpp src/Epoll.cpp src/InetAddress.cpp src/Ssocket.cpp \
	src/Channel.cpp src/EventLoop.cpp src/Server.cpp src/Acceptor.cpp src/Connection.cpp \
	src/Buffer.cpp server.cpp -o ./build/server
	echo ./build/server > ./test/server
	echo ./build/client > ./test/client
	chmod +x ./test/server
	chmod +x ./test/client

clean:
	rm -r ./build && rm -r ./test


.PHONY: clean
