all: init schedgen schedsim schedstats

init: 
	mkdir -p build

schedgen: 
	gcc -o build/schedgen src/schedgen.c 

schedsim:
	gcc -o build/schedsim src/schedsim.c motor/motor.c -lpthread

schedstats:
	gcc -o build/schedstats src/schedstats.c motor/motor.c -lpthread

clean:
	 rm build/*
