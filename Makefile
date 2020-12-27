ermc: ermcmain.o log.o if_ermd.o common.o
	gcc -o ermc src/ermcmain.o src/log.o src/if_ermd.o src/common.o -lpthread -lhiredis

log.o: src/log.c
	gcc -o src/log.o -c src/log.c

common.o: src/common.c
	gcc -o src/common.o -c src/common.c

if_ermd.o: src/if_ermd.c
	gcc -o src/if_ermd.o -c src/if_ermd.c

ermcmain.o: src/ermcmain.c
	gcc -o src/ermcmain.o -c src/ermcmain.c

clean:
	rm -rf *.o ermc
