ermc: ermcmain.o log.o 
	gcc -o ermc src/ermcmain.o src/log.o  -lpthread 

log.o: src/log.c
	gcc -o src/log.o -c src/log.c

ermcmain.o: src/ermcmain.c
	gcc -o src/ermcmain.o -c src/ermcmain.c

clean:
	rm -rf *.o ermc
