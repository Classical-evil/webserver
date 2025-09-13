TCP=$(wildcard Tcp/*.cpp)

server:
	mkdir ./build
	g++ -std=c++14 -pthread -g \
	$(TCP) \
	echo_server.cpp \
	-o ./build/server

	g++ -pthread Tcp/Buffer.cpp Tcp/ThreadPool.cpp test.cpp -o ./build/test



clean:
	rm -r ./build


commit:
	git add .
	git commit -m "$(msg)"
	git push 

.PHONY: clean commit