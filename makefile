try: main.o mynumber.o 
	cc main.o mynumber.o -o try
main.o: main.c mynumber.h
	cc -Wall -c main.c
mynumber.o: mynumber.c mynumber.h bcconfig.h
	cc -Wall -c mynumber.c

