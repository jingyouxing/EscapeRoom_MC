ermc: ermcmain.o log.o if_ermd.o if_web.o common.o cJSON.o
	gcc -o ermc src/ermcmain.o src/log.o src/if_ermd.o src/common.o src/if_web.o src/cJSON.o -lpthread -lhiredis -lm

log.o: src/log.c
	gcc -o src/log.o -c src/log.c

common.o: src/common.c
	gcc -o src/common.o -c src/common.c

cJSON.o: src/cJSON.c
	gcc -o src/cJSON.o -c src/cJSON.c

if_ermd.o: src/if_ermd.c
	gcc -o src/if_ermd.o -c src/if_ermd.c

if_web.o: src/if_web.c
	gcc -o src/if_web.o -c src/if_web.c

ermcmain.o: src/ermcmain.c
	gcc -o src/ermcmain.o -c src/ermcmain.c

clean:
	rm -rf *.o ermc
