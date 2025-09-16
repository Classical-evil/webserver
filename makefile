TCP=$(wildcard Tcp/*.cpp)

server:
	mkdir ./build
	g++ -std=c++14 -pthread -g \
	$(TCP) \
	Http/*cpp \
	test/echo_server.cpp \
	-o ./build/server

	g++ -pthread Tcp/Buffer.cpp test/ThreadPool.cpp test/test.cpp -o ./build/test



clean:
	rm -r ./build


commit:
	git add .
	git commit -m "$(msg)"
	git push 

http_context:
	g++ Http/*.cpp test/http_context.cpp -o build/http_context

http_server:
	g++ Http/*.cpp $(TCP) test/http_server.cpp -o build/http_server


.PHONY: clean commit http_context http_server