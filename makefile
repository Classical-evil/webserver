TCP=$(wildcard Tcp/*.cpp)

server:
	mkdir ./build
	g++ -std=c++14 -pthread -g \
	$(TCP) \
	echo_server.cpp \
	-o ./build/server

	g++ -pthread Tcp/Buffer.cpp test/ThreadPool.cpp test/test.cpp -o ./build/test



clean:
	rm -r ./build


commit:
	git add .
	git commit -m "$(msg)"
	git push 

context:
	g++ Http/*.cpp test/text_httpcontext.cpp -o build/context

.PHONY: clean commit context