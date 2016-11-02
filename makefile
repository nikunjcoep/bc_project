# Makefile for bc_command by Nikunj Soni liscensed under GPLv2
project: main.o mynumber.o stack.o help.o
	cc main.o mynumber.o stack.o help.o -o project
main.o: main.c mynumber.h stack.h
	cc -Wall -c main.c
mynumber.o: mynumber.c mynumber.h bcconfig.h
	cc -Wall -c mynumber.c
stack.o: stack.c stack.h mynumber.h
	cc -Wall -c stack.c
help.o: help.c
	cc -Wall -c help.c

