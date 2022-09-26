
all: clean dirty task5 task4 task3 task2 task1 

task5:
	g++ -Wall -g -o  task5 src/task5.cpp  src/utils.cpp -pthread -lrt -lstdc++fs -std=c++17 -fno-stack-protector
	g++ -Wall -g -o task5_server src/task5_server.cpp  src/utils.cpp -pthread -lrt -lstdc++fs -std=c++17 -fno-stack-protector

task4: src/utils.o src/task4.o
	g++ -Wall -g -o  task4 src/task4.o src/utils.o -pthread -lrt -lstdc++fs -std=c++17 -fno-stack-protector

task3: src/utils.o src/task3.o
	g++ -Wall -g -o  task3 src/task3.o src/utils.o -pthread -lrt -lstdc++fs -std=c++17 -fno-stack-protector

task2: src/utils.o src/task2.o
	g++ -Wall -g -o  task2 src/task2.o src/utils.o -pthread -lrt -lstdc++fs -std=c++17 -fno-stack-protector


clean_obj:
	rm -f ./src/*.0
	
task1: src/utils.o src/task1.o
	g++ -Wall -g -o  task1 src/task1.o src/utils.o -pthread -lrt -lstdc++fs -std=c++17
	clean_obj

task1_sh:
	sh task1.sh


clean:
	rm -f ./src/*.o task5 task4 task3 task2 task1 task5_server


.PHONY: all clean