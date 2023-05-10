object_files = ./build/lkby_scheduler.o ./build/lkby_transmitter.o ./build/lkby_discovery.o ./build/lkby_queue.o
object_files_debug = ./build/lkby_scheduler_debug.o ./build/lkby_transmitter_debug.o ./build/lkby_discovery_debug.o ./build/lkby_queue_debug.o
header_loc = -I ./include/

GCC = gcc
libraries = -lpthread

c_flags = -Wall
c_debug_flags = -g -Wall
c_features = -D _GNU_SOURCE

all: dir_make ${object_files}
	${GCC} ${c_features} ${c_flags} ${header_loc} ./src/main.c ${object_files} ${libraries} -o ./bin/lkby

debug: dir_make ${object_files_debug}
	${GCC} ${c_features} ${c_debug_flags} ${header_loc} ./src/main.c ${object_files_debug} ${libraries} -o ./bin/lkby_debug

./build/lkby_scheduler.o: ./src/lkby_scheduler.c
	${GCC} ${c_features} ${c_flags} ${header_loc} -c ./src/lkby_scheduler.c ${libraries} -o ./build/lkby_scheduler.o

./build/lkby_transmitter.o: ./src/lkby_transmitter.c
	${GCC} ${c_features} ${c_flags} ${header_loc} -c ./src/lkby_transmitter.c ${libraries} -o ./build/lkby_transmitter.o

./build/lkby_discovery.o: ./src/lkby_discovery.c
	${GCC} ${c_features} ${c_flags} ${header_loc} -c ./src/lkby_discovery.c ${libraries} -o ./build/lkby_discovery.o

./build/lkby_queue.o: ./src/lkby_queue.c
	${GCC} ${c_features} ${c_flags} ${header_loc} -c ./src/lkby_queue.c ${libraries} -o ./build/lkby_queue.o

./build/lkby_scheduler_debug.o: ./src/lkby_scheduler.c
	${GCC} ${c_features} ${c_debug_flags} ${header_loc} -c ./src/lkby_scheduler.c ${libraries} -o ./build/lkby_scheduler_debug.o

./build/lkby_transmitter_debug.o: ./src/lkby_transmitter.c
	${GCC} ${c_features} ${c_debug_flags} ${header_loc} -c ./src/lkby_transmitter.c ${libraries} -o ./build/lkby_transmitter_debug.o

./build/lkby_discovery_debug.o: ./src/lkby_discovery.c
	${GCC} ${c_features} ${c_flags} ${header_loc} -c ./src/lkby_discovery.c ${libraries} -o ./build/lkby_discovery_debug.o

./build/lkby_queue_debug.o: ./src/lkby_queue.c
	${GCC} ${c_features} ${c_flags} ${header_loc} -c ./src/lkby_queue.c ${libraries} -o ./build/lkby_queue_debug.o

dir_make:
	mkdir -p ./bin/
	mkdir -p ./build/

clean:
	rm -rf ./build/

cleanall:
	rm -rf ./build/
	rm -rf ./bin/
