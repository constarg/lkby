header_loc = -I ./include/ -I ../include/

GCC = gcc

c_flags = -Wall -Wpedantic -Wnull-dereference -Wextra -Wunreachable-code -Wpointer-arith -Wmissing-include-dirs -Wstrict-prototypes -Wunused-result -Waggregate-return -Wredundant-decls
c_debug_flags = -g -Wall -Wpedantic -Wnull-dereference -Wextra -Wunreachable-code -Wpointer-arith -Wmissing-include-dirs -Wstrict-prototypes -Wunused-result -Waggregate-return -Wredundant-decls
c_features = -D _GNU_SOURCE

all: dir_make
	${GCC} -c ${c_features} ${header_loc} ./src/lkby_lib.c -o ./bin/lib_lkby.o
	ar rcs ./lib/lib_lkby.a ./bin/lib_lkby.o 

dir_make:
	mkdir -p ./bin/
	mkdir -p ./lib/

clean:
	rm -rf ./build/

cleanall:
	rm -rf ./bin/

