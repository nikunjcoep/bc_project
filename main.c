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
#include "mynumber.h"
#include "stack.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#define OPERAND 10 
#define OPERATOR 20
#define	END	30
#define ERROR	40
static int i_base = 10, scale = 0, o_base = 10;

enum states { SPC, DIG, OPR, SQRT, STOP, ERR };

 /* structur of token */
typedef struct token {
	int type;       // type takes values OPERAND/OPERATOR/END/
	my_num number;
	char op;
}token;



 /*Reads the expression and stores it in arr . Returns length of expression.*/
int readline(char *arr, int len) {
	int i = 0;
	int ch;
	while((ch = getchar()) != '\n' && i < len - 1) {
		arr[i] = ch;
		i++;
	}
	arr[i] = '\0';
	return i;
}

 /* takes a character operator and returns its precedence*/
int precedence(char op) {
	if(op == '$')
		return 6;	
	else if(op == '(' || op == ')')
		return 5;
	else if(op == '^')
		return 4;
	else if(op == '%')
		return 3;
	else if(op == '*' || op == '/')
		return 2;
	else if(op == '+' || op == '-')
		return 1;
	
	return 0;
}

  /*carry the operation  andreturn the result */

my_num applyoperator(my_num op1, my_num op2, char oprtr) {
	my_num result, intmin;
	intmin = bc_new_num(10000,1000);
	bc_int2num(&intmin, INT_MIN);
	result = bc_new_num(0, 0);
	int tmp;	
	switch(oprtr) {
    		case '+':{
    			bc_add(op1, op2, &result, scale);
			return result;
		}
    		case '-':{
    			bc_sub(op1, op2,  &result, scale);
			return result;
		}
    		case '*':{
    			bc_multiply(op1, op2, &result, scale);
			return result;
		}
    		case '/':{
			tmp = bc_divide(op1, op2, &result, scale);
			if(tmp == 0)
				return result;
			else
				printf("Error: Cannot divide\n");
			break;
		}
		case '%':{
			tmp = bc_modulo(op1, op2, &result, 0);
			if(tmp == 0)
				return result;
			else
				printf("Error: Cannot do modulo\n");
			break;			
		}
    		case '^':{
			bc_raise(op1, op2, &result, scale);
			return result;
		}
		
    	}
	return intmin;
}

 /* Gets token from arr and returns token pointer */
token *getnext(char *arr, int *reset) {
	static int currstate;
	int nextstate;
	char string[10000];
	char *retstr;
	int len;
	static int i;
	if(*reset == 1) {
		i = 0;
		currstate = SPC;
		*reset = 0;
	}
	token *t = (token *)malloc(sizeof(token));
	t->number = bc_new_num(10000, 1000);
	while(1) {
		switch(arr[i]) {
			case '0': case '1': case '2': case '3':
			case '4': case '5': case '6': case '7':
			case '8': case '9': case '.': 
				nextstate = DIG;
				break;
			case '+': case '-': case '*': case '/': 
			case '%': case '(': case ')': case '^':
				if( (arr[i-1]>='0'&&arr[i-1]<='9') || (arr[i-1] == ' '&&(arr[i-2]>='0'&&arr[i-2]<='9')) || arr[i] == '(' ){
					nextstate = OPR;
				}
				else if(arr[i-1] == ')' || (arr[i-1] == ' ' && arr[i-2] == ')'))
					nextstate = OPR;
				else
					nextstate = DIG;
				break;
			case '\0':
				nextstate = STOP;
				break;
			case ' ':
				nextstate = SPC;
				break;
			case '$':
				nextstate = SQRT;
				break;
			default:
				nextstate = ERR;
				break;
		}
		switch(currstate) {
			case SPC:
				len = 0;
				if(nextstate == DIG && arr[i] != '.')
					string[len++] = arr[i];			
				break;

			case DIG:
				if(nextstate == DIG)
					string[len++] = arr[i];
				else {
					string[len++] = '\0';
					
					t->type = OPERAND;
					if(i_base == 10)
						bc_str2num(&(t->number), string, scale);
					else{
						bc_str2num(&(t->number), string, scale);
						retstr = bc_out_num_base(t->number, i_base, scale);
						bc_str2num(&(t->number), retstr, scale);
					}
					
					currstate = nextstate;
					i++;
					return t;
				}
				break;

			case OPR:
				len = 0;
				if(nextstate == DIG && arr[i] != '.')
					string[len++] = arr[i];
				t->type = OPERATOR;
				t->op = arr[i - 1];
				currstate = nextstate;
				i++;				
				return t;
				
			case SQRT:
				len = 0;
				if(nextstate == DIG && arr[i] != '.')
					string[len++] = arr[i];
				t->type = OPERATOR;
				t->op = arr[i-1];
				currstate = nextstate;
				i++;
				return t;
				
			case STOP:
				t->type = END;
				currstate = nextstate;
				i++;
				return t;
				
			case ERR:
				t->type = ERROR;
				currstate = nextstate;
				i++;
				return t;
				
		}
		currstate = nextstate;
		i++;
	}
}





my_num infixeval(char *infix) {
	cstack c_stk;
	stack num_stk;
	token *t;
	my_num q, intmin, p, result;
	result = bc_new_num(100000, 1000);
	q = bc_new_num(10000, 1000);
	intmin = bc_new_num(10000,1000);
	p = bc_new_num(10000,1000);
	bc_int2num(&intmin, INT_MIN);
  /* Initialize stacks*/	
	cinit(&c_stk);
	init(&num_stk);
	char x;
	int reset = 1, a, b, flag;

	while(1) {
		t = getnext(infix, &reset);
		if(t->type == OPERAND) {
			push(&num_stk, t->number);
		}
		else if (t->type == OPERATOR) {
			if(!cempty(&c_stk)) {
				flag = 0;
				x = ctop(&c_stk);	
				a = precedence(x);	
				b = precedence(t->op);
				if(t->op == '$' && t->type == OPERATOR){
					t = getnext(infix, &reset);
					if(bc_sqrt(&t->number, scale))
						push(&num_stk, t->number);
					else
						printf("Error: cannot do squareroot\n");
					flag = 1;
								
				}	
				while(a >= b && x != '(' && flag == 0) {
					x = cpop(&c_stk);
					if(x == '(') {
						x = cpop(&c_stk);
						break;	
					}
					if(!empty(&num_stk)) 
						q = bc_copy_num(pop(&num_stk));
					else 
						return intmin; 
					if(!empty(&num_stk)) 
						p = bc_copy_num(pop(&num_stk));
					else
						return intmin;
					result = bc_copy_num(applyoperator(p, q, x));
					if( !bc_compare(result, intmin) )
						return intmin;
					else					
						push(&num_stk, result);				
					if(!cempty(&c_stk)) {
						x = ctop(&c_stk);	
						a = precedence(x);
					}
					else
						break;
				}
				if(t->op == ')') {
					x = ctop(&c_stk);	
					while( x != '(') {
						x = cpop(&c_stk);
						if(!empty(&num_stk)) 
							q = bc_copy_num(pop(&num_stk));
						else 
							return intmin; 
						if(!empty(&num_stk)) 
							p = bc_copy_num(pop(&num_stk));
						else
							return intmin;
						result = bc_copy_num(applyoperator(p, q, x));
						push(&num_stk, result);				
						if(!cempty(&c_stk)) {
							x = ctop(&c_stk);	
							a = precedence(x);
						}
						else
							break;
					}
					x = cpop(&c_stk);

					
				}
			}
			if(t->op != ')' && t->op != '$' && t->type == OPERATOR){
				cpush(&c_stk, t->op);
			
			}
			if(t->op == '$' && t->type == OPERATOR){
					t = getnext(infix, &reset);
					if(bc_sqrt(&t->number, scale))
						push(&num_stk, t->number);
					else
						printf("Error: Cannot do squareroot\n");
					flag = 1;				

			}
			
		}
		else if (t->type == END) {
			while(!cempty(&c_stk)) {
				x = cpop(&c_stk);
				if(!empty(&num_stk)) {
					q = bc_copy_num(pop(&num_stk));
				
				}
				else
					return intmin;
				if(!empty(&num_stk)) {
					p = bc_copy_num(pop(&num_stk));			
				}
				else
					return intmin;
				
				result = bc_copy_num(applyoperator(p, q, x));
				push(&num_stk, result);		
			}
			if(!empty(&num_stk))
				result = bc_copy_num(pop(&num_stk));
			else
				return intmin;	
				return result;
		} else
			return intmin;
	}
}

  /*  Main */
int main(int argc, char *argv[]) {
	bc_init_numbers();
	char str[10000], msg[7];
	int  x, idx;
	int number, set;
	my_num ans;
	ans = bc_new_num(10000, 1000);

	if(argc == 2 && !(strcmp(argv[1], "-h")))
		printhelp();

	while((x = readline(str, 10000))){
		number = 0;
		set = -1;
		if( !strcmp(str, "quit") )
			break;
		for(idx = 0; idx < 5; idx++)
			msg[idx] = str[idx];
			
		if(!strcmp(msg, "ibase"))
			set = 0;
		else if(!strcmp(msg, "obase"))
			set = 1;
		else if(!strcmp(msg, "scale"))
			set = 2;

		if(set != -1){
			for(idx = 6; idx < strlen(str); idx++)
				if(str[idx] >= '0' && str[idx] <= '9')
					number = number*10 + (str[idx]-'0');
			switch(set){
				case 0:
					i_base = number;
					break;
				case 1:
					o_base = number;
					break;
				case 2:
					scale = number;
					break;
			}
			continue;
		}
		
		ans = infixeval(str);
		bc_out_num(ans, o_base, out_char, 0);
		printf("\n");
	}
	
	return 0;
}
