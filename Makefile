SecureRouter: main.cpp routerInterface.cpp cmd.o cparser_tree.o libcli.a
	g++ -g -std=c++11 main.cpp routerInterface.cpp cmd.o cparser_tree.o libcli.a -lpthread -I./inc -o SecureRouter -lcurl -DDEBUG
	sudo chmod u+s ./SecureRouter
cparser_tree.o:  cparser_tree.c	
	gcc -std=c99 -c cparser_tree.c -I./inc 
cmd.o:  cmd.c	
	gcc -std=c99 -c cmd.c -I./inc 

.PHONY: clean
clean:
	rm -f *.o
	rm -f SecureRouter

