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


#ifndef _NUMBER_H_
#define _NUMBER_H_

/*Errors*/
#define BC_ERROR_OUT_OF_MEMORY 0
#define BC_ERROR_EXPONENT_TOO_LARGE_IN_RAISE 1
#define BC_WARNING_NON_ZERO_SCALE_IN_EXPONENT -1
#define BC_WARNING_NON_ZERO_SCALE_IN_BASE -2
#define BC_WARNING_NON_ZERO_SCALE_IN_MODULUS -3

typedef enum {PLUS, MINUS} sign;

typedef struct bc_struct *my_num;

typedef struct bc_struct {
	sign  n_sign;
	int   n_len;	/* The number of digits before the decimal point. */
	int   n_scale;	/* The number of digits after the decimal point. */
	int   n_refs;     /* The number of pointers to this number. */
	char *n_ptr;	/* The pointer to the actual storage.
			   If NULL, n_value points to the inside of
			   another number (bc_multiply...) and should
			   not be "freed." */
	char *n_value;	/* The number. Not zero char terminated.
			   May not point to the same place as n_ptr as
			   in the case of leading zeros generated. */
} bc_struct;

typedef struct stk_rec {
	long  digit;
	struct stk_rec *next;
} stk_rec;

/* The base used in storing the numbers in n_value above.
   Currently this MUST be 10. */

#define BASE 10

/*  Some useful macros and constants. */

#define CH_VAL(c)     (c - '0')
#define BCD_CHAR(d)   (d + '0')

#ifdef MIN
#undef MIN
#undef MAX
#endif
#define MAX(a,b)      ((a)>(b)?(a):(b))
#define MIN(a,b)      ((a)>(b)?(b):(a))
#define ODD(a)        ((a)&1)

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifndef LONG_MAX
#define LONG_MAX 0x7ffffff
#endif


/* Global numbers. */
extern my_num _zero_;
extern my_num _one_;
extern my_num _two_;

/* For Error */
void rt_warn(char *mesg );
void rt_error(char *mesg );
void out_of_memory(void);

/* Function Prototypes */

void bc_init_numbers(void);

my_num bc_new_num(int length, int scale);

void bc_free_num(my_num *num);

my_num bc_copy_num(my_num num);

void bc_init_num(my_num *num);

void bc_str2num(my_num *num, char *str, int scale);   

char *bc_num2str(my_num num);        

void bc_int2num(my_num *num, int val);   

long bc_num2long(my_num num);

void bc_out_long(long val, int size, int space, void (*out_char)(int));

int bc_compare(my_num n1, my_num n2);

int bc_is_zero(my_num num);

int bc_is_near_zero(my_num num, int scale);

int bc_is_neg(my_num num);

void bc_add(my_num n1, my_num n2, my_num *result, int scale_min);

void bc_sub(my_num n1, my_num n2, my_num *result, int scale_min);

void bc_multiply(my_num n1, my_num n2, my_num *prod, int scale);

int bc_divide(my_num n1, my_num n2, my_num *quot, int scale);   

int bc_modulo(my_num num1, my_num num2, my_num *result, int scale);  

int bc_divmod(my_num num1, my_num num2, my_num *quot, my_num *rem, int scale);  

void bc_raise(my_num num1, my_num num2, my_num *result, int scale);

int bc_sqrt(my_num *num, int scale);

void bc_out_num(my_num num, int o_base, void (* out_char)(int), int leading_zero);  

void pn(my_num num);

void out_char(int c);

char *bc_out_num_base(my_num num, int i_base, int scale);

#endif
