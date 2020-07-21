all: init schedgen schedsim schedstats

init: 
	mkdir -p build

schedgen: 
	gcc -o build/schedgen schedgen.c 

schedsim:
	gcc -o build/schedsim schedsim.c -lpthread

schedstats:
	gcc -o build/schedstats schedstats.c -lpthread

clean:
	 rm build/*
