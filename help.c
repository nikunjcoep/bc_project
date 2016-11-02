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

#include <stdio.h>

 /* Prints help for -h */

void printhelp(){
	printf("bc is an arbitrary precision numeric processing language\n");
	printf("Following are the commands to use it:\n\n");
	printf("->""scale"" is the no. of digits after decimal in answer (by default set to 0)\n");
	printf("To change scale :\n");
	printf("->scale=num  ...(changes scale to num entered by user)\n\n");	
	printf("->""ibase"" is the base of input numbers (by default set to 10)\n");
	printf("To change ibase:\n");
	printf("->ibase=num  (1<num<=10)...(changes ibase to num entered by user)\n\n");
	printf("->""obase"" is the base of output answer (by default set to 10)\n");
	printf("To change obase:\n");
	printf("->obase=num  (1<num<=16)...(changes obase to num entered by user)\n\n");
	printf("Expressions:\n");
	printf("ibase = num  ...changes ibase\n");
	printf("obase = num  ...changes obase\n");
	printf("scale = num  ...changes scale\n");
	printf("->n1+n2  ...Adds n1 and n2 and prints answer\n");
	printf("->n1-n2  ...Subtracts n2 and n1 and prints answer\n");
	printf("->n1*n2  ...Multiplies n1 and n2 and prints answer\n");
	printf("->n1/n2  ...Divides n1 by n2 and prints answer\n");
	printf("->n1%%n2  ...Prints remainder of n1/n2 \n");
	printf("->n1^n2  ...n1 raise to power n2\n");
	printf("->$num   ...Prints Square root of num \n");
	printf("->(n1 op n2)  ...Bracket operations\n");
	printf("->n1/n2+n3*($n4) ...Expression evaluation\n\n");
	printf("->Type ""quit"" to exit\n\n");
	printf("Precedence: (higher to lower)\n");
	printf("1. ( , )\n2. $\n3. ^\n4. %%\n5. * , /\n6. + , -\n");
	
}
	
