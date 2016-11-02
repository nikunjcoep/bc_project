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

#include "stack.h"

void cpush(cstack *s, char num) {
	s->a[s->i]= num;
	(s->i)++;
}

char cpop(cstack *s) {
	char t = s->a[s->i - 1];
	(s->i)--;
	return t;
}

int cempty(cstack *s) {
	return s->i == 0;
}

int cfull(cstack *s) {
	return s->i == 10000;
}

char ctop(cstack *cs) {
	char x = cpop(cs);
	cpush(cs, x);
	return x;
}

void cinit(cstack *s) {
	s->i = 0;
}


void push(stack *s, my_num num) {
	s->a[s->i]= num;
	(s->i)++;
}

my_num pop(stack *s) {
	my_num t;
	t = bc_new_num(10000, 1000);	
	t = bc_copy_num(s->a[s->i - 1]);
	(s->i)--;
	return t;
}

int empty(stack *s) {
	return s->i == 0;
}

int full(stack *s) {
	return s->i == 10000;
}

void init(stack *s) {
	s->i = 0;
}


