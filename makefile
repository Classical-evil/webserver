server:
	mkdir ./build
	mkdir ./test
	g++ util.c server.c -o ./build/server
	g++ util.c client.c -o ./build/client
	echo ./build/server > ./test/server
	echo ./build/client > ./test/client
	chmod +x ./test/server
	chmod +x ./test/client

clean:
	rm -r ./build && rm -r ./test


.PHONY: clean
