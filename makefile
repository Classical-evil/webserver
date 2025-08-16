server:
	mkdir ./build
	mkdir ./test
	g++ util.cpp client.cpp -o ./build/client && \
	g++ util.cpp server.cpp Epoll.cpp InetAddress.cpp Ssocket.cpp -o ./build/server
	echo ./build/server > ./test/server
	echo ./build/client > ./test/client
	chmod +x ./test/server
	chmod +x ./test/client

clean:
	rm -r ./build && rm -r ./test


.PHONY: clean
