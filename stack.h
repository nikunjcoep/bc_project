/*
    This file is part of bc_command written by Nikunj Soni.
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License , or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING.  If not, write to:

      The Free Software Foundation, Inc.
      59 Temple Place, Suite 330
      Boston, MA 02111-1307 USA.
*/

#ifndef _STACK_H_
#define _STACK_H_
#include "mynumber.h"
 /* character stack*/
typedef struct cstack{
	char a[10000];
	int i;
}cstack;

void cpush(cstack *s, char num);

char cpop(cstack *s);

int cempty(cstack *s);

int cfull(cstack *s);

char ctop(cstack *cs);

void cinit(cstack *s);

 /* bc number stack*/
typedef struct stack{
	my_num a[10000];
	int i;
}stack;

void push(stack *s, my_num num);

my_num pop(stack *s);

int empty(stack *s);

int full(stack *s);

void init(stack *s);

void printhelp();
#endif
