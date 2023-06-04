header_loc = -I ./include/

GCC = gcc

c_flags = -Wall
c_debug_flags = -g -Wall
c_features = -D _GNU_SOURCE

all: dir_make
	${GCC} ${c_features} ${header_loc} ./src/lkby_lib.c -o ./bin/lkby_lib

dir_make:
	mkdir -p ./bin/

clean:
	rm -rf ./build/

cleanall:
	rm -rf ./bin/

