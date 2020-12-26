ermc: ermcmain.o log.o if_ermd.o
	gcc -o ermc src/ermcmain.o src/log.o src/if_ermd.o -lpthread 

log.o: src/log.c
	gcc -o src/log.o -c src/log.c

if_ermd.o: src/if_ermd.c
	gcc -o src/if_ermd.o -c src/if_ermd.c

ermcmain.o: src/ermcmain.c
	gcc -o src/ermcmain.o -c src/ermcmain.c

clean:
	rm -rf *.o ermc
