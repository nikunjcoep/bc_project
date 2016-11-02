/* mynumber.c: Implements arbitrary precision numbers. */

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
#include "bcconfig.h"
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
char ref_str[] = "0123456789ABCDEF";

/* special numbers. */
my_num _zero_;
my_num _one_;
my_num _two_;

/* allocate a number of given length and scale */

my_num bc_new_num(int length,int scale) {
	my_num temp;
	temp = (my_num) malloc(sizeof(bc_struct));
	if (temp == NULL) 
		bc_out_of_memory();
	temp->n_sign = PLUS;
	temp->n_len = length;
	temp->n_scale = scale;
	temp->n_refs = 1;
	temp->n_ptr = (char *) malloc(length+scale);
	if (temp->n_ptr == NULL)
		bc_out_of_memory();
	temp->n_value = temp->n_ptr;
	memset(temp->n_ptr, 0, length+scale);
	return temp;
}

/* Frees a my_num  */

void bc_free_num(my_num *num) {
	if(*num == NULL)
		return;
	(*num)->n_refs--;
	if((*num)->n_refs == 0) {
		if ((*num)->n_ptr)
			free((*num)->n_ptr);
		free(*num);
	}
	*num = NULL;
}

/* Intitialize the above special numbers */

void bc_init_numbers() {
	_zero_ = bc_new_num(1,0);
	_one_  = bc_new_num(1,0);
	_one_->n_value[0] = 1;
	_two_  = bc_new_num(1,0);
	_two_->n_value[0] = 2;
}

/* function to make copy of number */

my_num bc_copy_num(my_num num) {
	num->n_refs++;
	return num;
}

/* Initialize a number 'num' by making it a copy of zero. */

void bc_init_num(my_num *num) {
	*num = bc_copy_num(_zero_);
	
}

/* _bc_rm_leading_zeros just moves the data "value" pointer to the
   correct place after leading zeros and adjusts the length. */

void _bc_rm_leading_zeros(my_num num) {
  /* We can move n_value to point to the first non zero digit! */
	while(*num->n_value == 0 && num->n_len > 1) {
		num->n_value++;
		num->n_len--;
	}
}

/* Compare two bc numbers.  Returns 0 if equal, -1 if n1 is less
   than n2 and +1 if n1 is greater than n2. */

static int _bc_do_compare(my_num n1, my_num n2, int use_sign, int ignore_last) {
	char *n1ptr, *n2ptr;
	int  count;
	
  /* compare signs. */
	if(use_sign && n1->n_sign != n2->n_sign) {
		if(n1->n_sign == PLUS)
			return 1;     /* Positive n1 > Negative n2 */
		else
			return -1;    /* Negative n1 < Positive n2 */
	}
	
  /* compare the magnitude. */
	if(n1->n_len != n2->n_len) {
		if(n1->n_len > n2->n_len) {
          /* Magnitude of n1 > n2. */
			if(!use_sign || n1->n_sign == PLUS)
				return (1);
			else
				return (-1);
		}
		else {
          /* Magnitude of n1 < n2. */
			if(!use_sign || n1->n_sign == PLUS)
				return -1;
			else
				return 1;
        	}
	}

  /*  check the integer part and the equal length part of the fraction. */
	count = n1->n_len + MIN(n1->n_scale, n2->n_scale);
	n1ptr = n1->n_value;
	n2ptr = n2->n_value;

	while((*n1ptr == *n2ptr) && (count > 0)) {
		n1ptr++;
		n2ptr++;
		count--;
	}
	if(ignore_last && count == 1 && n1->n_scale == n2->n_scale)
		return (0);
	if(count != 0) {
		if(*n1ptr > *n2ptr) {
          /* Magnitude of n1 > n2. */
			if(!use_sign || n1->n_sign == PLUS)
				return 1;
			else
				return -1;
		}
		else {
          /* Magnitude of n1 < n2. */
			if(!use_sign || n1->n_sign == PLUS)
				return -1;
			else
				return 1;
		}
	}

	if(n1->n_scale != n2->n_scale) {
		if(n1->n_scale > n2->n_scale) {
			for(count = n1->n_scale-n2->n_scale; count>0; count--)
				if(*n1ptr++ != 0) {
                /* Magnitude of n1 > n2. */
					if(!use_sign || n1->n_sign == PLUS)
						return (1);
					else
						return (-1);
				}
		}
		else {
			for(count = n2->n_scale-n1->n_scale; count>0; count--)
			if(*n2ptr++ != 0) {
                /* Magnitude of n1 < n2. */
				if(!use_sign || n1->n_sign == PLUS)
					return -1;
				else
					return 1;
			}
		}
	}

  /* They are equal */
	return 0;
}



int bc_compare(my_num n1, my_num n2) {
	return _bc_do_compare (n1, n2, TRUE, FALSE);
}

/* check if the number is negative. */

int bc_is_neg(my_num num) {
	return num->n_sign == MINUS;
}

/* check if the number is zero. */

int bc_is_zero(my_num num) {
	int  count;
	char *nptr;

	if(num == _zero_)
		return TRUE;

  /* Initialize */
	count = num->n_len + num->n_scale;
	nptr = num->n_value;

  /* The check */
	while((count > 0) && (*nptr++ == 0)) 
		count--;

	if(count != 0)
		return FALSE;
	else
		return TRUE;
}

/* In some places we need to check if the number NUM is almost zero.
   all but the last digit is 0 and the last digit is 1.
   Last digit is defined by scale. */

int bc_is_near_zero(my_num num, int scale) { /* Keep this function if necessary */
	int  count;
	char *nptr;

  /* Error checking */
	if(scale > num->n_scale)
		scale = num->n_scale;

  /* Initialize */
	count = num->n_len + scale;
	nptr = num->n_value;

  /* The check */
	while((count > 0) && (*nptr++ == 0)) 
		count--;

	if(count != 0 && (count != 1 || *--nptr != 1))
		return FALSE;
	else
		return TRUE;
}

/* Perform addition: n1 is added to n2 and the value is
   returned.  The signs of N1 and N2 are ignored.
   SCALE_MIN is to set the minimum scale of the result. */

my_num _bc_do_add(my_num n1, my_num n2, int scale_min) {
	my_num sum;
	int sum_scale, sum_digits;
	char *n1ptr, *n2ptr, *sumptr;
	int carry, n1bytes, n2bytes;
	int count;

  /* initialise sum index values. */
	sum_scale = MAX(n1->n_scale, n2->n_scale);
	sum_digits = MAX(n1->n_len, n2->n_len) + 1;
	sum = bc_new_num(sum_digits, MAX(sum_scale, scale_min));

  /* Zero extra digits made by scale_min. */
	if(scale_min > sum_scale) {
		sumptr = (char *)(sum->n_value + sum_scale + sum_digits);
		for(count = scale_min - sum_scale; count > 0; count--)
			*sumptr++ = 0;
	}

  /* for the fraction part. */
	n1bytes = n1->n_scale;
	n2bytes = n2->n_scale;
	n1ptr = (char *) (n1->n_value + n1->n_len + n1bytes - 1);
	n2ptr = (char *) (n2->n_value + n2->n_len + n2bytes - 1);
	sumptr = (char *) (sum->n_value + sum_scale + sum_digits - 1);

  /* Add the fraction part. */
	if(n1bytes != n2bytes) {
		if(n1bytes > n2bytes)
			while(n1bytes > n2bytes) {
				*sumptr-- = *n1ptr--; 
				n1bytes--;
			}
		else
			while(n2bytes > n1bytes) {
				*sumptr-- = *n2ptr--;
				n2bytes--;
			}
	}

  /*  adding the remaining fraction part and equal size integer parts. */
	n1bytes += n1->n_len;
	n2bytes += n2->n_len;
	carry = 0;
	while((n1bytes > 0) && (n2bytes > 0)) {
		*sumptr = *n1ptr-- + *n2ptr-- + carry;
      		if(*sumptr > (BASE-1)) {
			carry = 1;
			*sumptr -= BASE;
		}
		else
			carry = 0;
		sumptr--;
		n1bytes--;
		n2bytes--;
	}

  /* adding carry the longer integer part. */
	if(n1bytes == 0) {
	n1bytes = n2bytes; 
	n1ptr = n2ptr; 
	}
	while(n1bytes-- > 0) {
		*sumptr = *n1ptr-- + carry;
		if(*sumptr > (BASE-1)) {
			carry = 1;
			*sumptr -= BASE;
		}
		else
			carry = 0;
		sumptr--;
	}

  /* Set final carry. */
	if(carry == 1)
		*sumptr += 1;

  /* Adjust sum and return. */
	_bc_rm_leading_zeros (sum);
	return sum;
}

/* Perform subtraction: n2 is subtracted from n1 and the value is
   returned.  The signs of n1 and n2 are ignored.  Also, n1 is
   assumed to be larger than n2.  SCALE_MIN is the minimum scale
   of the result. */

my_num _bc_do_sub(my_num n1, my_num n2, int scale_min) {
	my_num diff;
	int diff_scale, diff_len;
	int min_scale, min_len;
	char *n1ptr, *n2ptr, *diffptr;
	int borrow, count, val;

  /* Allocate storage. */
	diff_len = MAX(n1->n_len, n2->n_len);
	diff_scale = MAX(n1->n_scale, n2->n_scale);
	min_len = MIN(n1->n_len, n2->n_len);
	min_scale = MIN(n1->n_scale, n2->n_scale);
	diff = bc_new_num(diff_len, MAX(diff_scale, scale_min));

  /* Zero extra digits made by scale_min. */
	if(scale_min > diff_scale) {
		diffptr = (char *) (diff->n_value + diff_len + diff_scale);
		for(count = scale_min - diff_scale; count > 0; count--)
			*diffptr++ = 0;
	}

  /* Initialize */
	n1ptr = (char *)(n1->n_value + n1->n_len + n1->n_scale -1);
	n2ptr = (char *)(n2->n_value + n2->n_len + n2->n_scale -1);
	diffptr = (char *) (diff->n_value + diff_len + diff_scale -1);

  /* Subtract the numbers. */
	borrow = 0;

	if(n1->n_scale != min_scale) {
      /* n1 has the longer scale */
		for (count = n1->n_scale - min_scale; count > 0; count--)
			*diffptr-- = *n1ptr--;
	}
	else {
      /* n2 has the longer scale */
		for(count = n2->n_scale - min_scale; count > 0; count--) {
			val = - *n2ptr-- - borrow;
			if(val < 0) {
				val += BASE;
				borrow = 1;
			}
			else
				borrow = 0;
			*diffptr-- = val;
		}
	}

  /* the equal length scale and integer parts. */

	for(count = 0; count < min_len + min_scale; count++) {
		val = *n1ptr-- - *n2ptr-- - borrow;
		if(val < 0) {
			val += BASE;
			borrow = 1;
		}
		else
			borrow = 0;
		*diffptr-- = val;
	}

  /* n1 has more digits then n2, subtract. */
	if(diff_len != min_len) {
		for(count = diff_len - min_len; count > 0; count--) {
			val = *n1ptr-- - borrow;
			if(val < 0) {
				val += BASE;
				borrow = 1;
			}
			else
				borrow = 0;
			*diffptr-- = val;
		}
	}

  /* return. */
	_bc_rm_leading_zeros(diff);
	return diff;
}

/* full subtract routine that takes care of negative numbers.
   n2 is subtracted from n1 and the result placed in 'result'.  'scale_min'
   is the minimum scale for the result. */

void bc_sub(my_num n1, my_num n2, my_num *result, int scale_min) {
	my_num diff = NULL;
	int cmp_res;
	int res_scale;

	if(n1->n_sign != n2->n_sign) {
		diff = _bc_do_add(n1, n2, scale_min);
		diff->n_sign = n1->n_sign;
	}
	else {
      /* Compare magnitudes. */
		cmp_res = _bc_do_compare(n1, n2, FALSE, FALSE);
		switch (cmp_res) {
			case -1:
          /* n1 is less than n2, subtract n1 from n2. */
				diff = _bc_do_sub(n2, n1, scale_min);
				diff->n_sign = (n2->n_sign == PLUS ? MINUS : PLUS);
				break;
			case  0:
          /* They are equal! return zero! */
				res_scale = MAX(scale_min, MAX(n1->n_scale, n2->n_scale));
				diff = bc_new_num(1, res_scale);
				memset(diff->n_value, 0, res_scale+1);
				break;
			case  1:
          /* n2 is less than n1, subtract n2 from n1. */
				diff = _bc_do_sub(n1, n2, scale_min);
				diff->n_sign = n1->n_sign;
				break;
		}
	}

  /*return. */
	bc_free_num(result);
	*result = diff;
}

/* Full adition process */
void bc_add(my_num n1, my_num n2, my_num *result, int scale_min) {
	my_num sum = NULL;
	int cmp_res;
	int res_scale;

	if(n1->n_sign == n2->n_sign) {
		sum = _bc_do_add(n1, n2, scale_min);
		sum->n_sign = n1->n_sign;
	}
	else {
      /* subtraction must be done. */
		cmp_res = _bc_do_compare(n1, n2, FALSE, FALSE);  /* Compare magnitudes. */
		switch (cmp_res) {
			case -1:
          /* n1 is less than n2, subtract n1 from n2. */
				sum = _bc_do_sub(n2, n1, scale_min);
				sum->n_sign = n2->n_sign;
				break;
			case  0:
          /* They are equal! return zero with the correct scale! */
				res_scale = MAX(scale_min, MAX(n1->n_scale, n2->n_scale));
				sum = bc_new_num(1, res_scale);
				memset(sum->n_value, 0, res_scale+1);
				break;
			case  1:
          /* n2 is less than n1, subtract n2 from n1. */
				sum = _bc_do_sub(n1, n2, scale_min);
				sum->n_sign = n1->n_sign;
		}
	}

  /*return*/
	bc_free_num(result);
	*result = sum;
}


 /* Simple multiplication algorithm to multiply two numbers*/
void _bc_simp_mul(my_num n1, int n1len, my_num n2, int n2len, my_num *prod, int full_scale) {
	char *n1ptr, *n2ptr, *pvptr;
	char *n1end, *n2end;		
	int i, sum, prodlen;

	prodlen = n1len + n2len + 1;
	*prod = bc_new_num(prodlen, 0);

	n1end = (char *) (n1->n_value + n1len - 1);
	n2end = (char *) (n2->n_value + n2len - 1);
	pvptr = (char *) ((*prod)->n_value + prodlen - 1);
	sum = 0;

  /* loop... */
	for(i = 0; i < prodlen-1; i++) {
		n1ptr = (char *) (n1end - MAX(0, i-n2len+1));
		n2ptr = (char *) (n2end - MIN(i, n2len-1));
		while ((n1ptr >= n1->n_value) && (n2ptr <= n2end))
			sum += *n1ptr-- * *n2ptr++;
		*pvptr-- = sum % BASE;
		sum = sum / BASE;
	}
		*pvptr = sum;
}

void bc_raise(my_num num1, my_num num2, my_num *result, int scale) {
	my_num temp, power;
	long exponent;
	int rscale;
	int pwrscale;
	int calcscale;
	char neg;

   /* Check the exponent for scale digits and convert to a long. */
	if(num2->n_scale != 0)
		bc_rt_warn("non-zero scale in exponent");
	exponent = bc_num2long(num2);
	if(exponent == 0 && (num2->n_len > 1 || num2->n_value[0] != 0))
		bc_rt_error("exponent too large in raise");

   /* Special case if exponent is a zero. */
	if(exponent == 0) {
		bc_free_num(result);
		*result = bc_copy_num(_one_);
		return;
	}

   /* Other initializations. */
	if(exponent < 0) {
		neg = TRUE;
		exponent = -exponent;
		rscale = scale;
	}
	else {
		neg = FALSE;
		rscale = MIN(num1->n_scale*exponent, MAX(scale, num1->n_scale));
	}

   /* Set initial value of temp.  */
	power = bc_copy_num (num1);
	pwrscale = num1->n_scale;
	while((exponent & 1) == 0) {
		pwrscale = 2*pwrscale;
		bc_multiply(power, power, &power, pwrscale);
		exponent = exponent >> 1;
	}
	temp = bc_copy_num (power);
	calcscale = pwrscale;
	exponent = exponent >> 1;
   /* calculation. */
 	while(exponent > 0) {
		pwrscale = 2*pwrscale;
		bc_multiply(power, power, &power, pwrscale);
		if((exponent & 1) == 1) {
			calcscale = pwrscale + calcscale;
			bc_multiply(temp, power, &temp, calcscale);
		}
		exponent = exponent >> 1;
	}

   /* Assign the value. */
	if(neg) {
		bc_divide(_one_, temp, result, rscale);
		bc_free_num(&temp);
	}
	else {
		bc_free_num(result);
		*result = temp;
		if((*result)->n_scale > rscale)
			(*result)->n_scale = rscale;
	}
	bc_free_num(&power);
}

void bc_multiply(my_num n1, my_num n2, my_num *prod, int scale) {
	my_num prodval; 
	int len1, len2;
	int full_scale, prod_scale;

  /* Initialize things. */
	len1 = n1->n_len + n1->n_scale;
	len2 = n2->n_len + n2->n_scale;
	full_scale = n1->n_scale + n2->n_scale;
	prod_scale = MIN(full_scale,MAX(scale,MAX(n1->n_scale,n2->n_scale)));

  /* multiply */
	_bc_simp_mul(n1, len1, n2, len2, &prodval, full_scale);
  /* Assign to prod */
	prodval->n_sign = ( n1->n_sign == n2->n_sign ? PLUS : MINUS );
	prodval->n_value = prodval->n_ptr;
	prodval->n_len = len2 + len1 + 1 - full_scale;
	prodval->n_scale = prod_scale;
	_bc_rm_leading_zeros(prodval);
	if(bc_is_zero(prodval))
		prodval->n_sign = PLUS;
	bc_free_num(prod);
	*prod = prodval;
}

void _one_mult(unsigned char *num, int size, int digit, unsigned char *result) {
	int carry, value;
	unsigned char *nptr, *rptr;

	if(digit == 0)
		memset(result, 0, size);
	else {
		if(digit == 1)
			memcpy(result, num, size);
		else {
	  		nptr = (unsigned char *) (num+size-1);
			rptr = (unsigned char *) (result+size-1);
			carry = 0;

			while(size-- > 0) {
				value = *nptr-- * digit + carry;
				*rptr-- = value % BASE;
				carry = value / BASE;
			}

			if(carry != 0)
				*rptr = carry;
		}
	}
}



/* Division. (n1 / n2).  It returns 0 if the division is ok and the result is in quot.It returns -1 if division
   by zero is tried.  */

int bc_divide(my_num n1, my_num n2, my_num *quot, int scale) {
	my_num qval;
	unsigned char *num1, *num2;
	unsigned char *ptr1, *ptr2, *n2ptr, *qptr;
	int  scale1, val;
	unsigned int  len1, len2, scale2, qdigits, extra, count;
	unsigned int  qdig, qguess, borrow, carry;
	unsigned char *mval;
	char zero;
	unsigned int  norm;

  /* Test for divide by zero. */
	if(bc_is_zero (n2))
		return -1;

  /* Test for divide by 1.  If it is we must truncate. */
	if(n2->n_scale == 0) {
		if(n2->n_len == 1 && *n2->n_value == 1) {
			qval = bc_new_num(n1->n_len, scale);
			qval->n_sign = (n1->n_sign == n2->n_sign ? PLUS : MINUS);
			memset(&qval->n_value[n1->n_len],0,scale);
			memcpy(qval->n_value, n1->n_value, n1->n_len + MIN(n1->n_scale,scale));
			bc_free_num(quot);
			*quot = qval;
		}
	}

  /* Move the decimal point on n1 by n2's scale.  zeros on the end of num2 are wasted effort for dividing. */
	scale2 = n2->n_scale;
	n2ptr = (unsigned char *) n2->n_value+n2->n_len+scale2-1;
	while((scale2 > 0) && (*n2ptr-- == 0))
		scale2--;

	len1 = n1->n_len + scale2;
	scale1 = n1->n_scale - scale2;
	if(scale1 < scale)
		extra = scale - scale1;
	else
		extra = 0;
	num1 = (unsigned char *) malloc (n1->n_len+n1->n_scale+extra+2);
	if(num1 == NULL)
		bc_out_of_memory();
	memset(num1, 0, n1->n_len+n1->n_scale+extra+2);
	memcpy(num1+1, n1->n_value, n1->n_len+n1->n_scale);

	len2 = n2->n_len + scale2;
	num2 = (unsigned char *) malloc (len2+1);
	if(num2 == NULL)
		bc_out_of_memory();
	memcpy(num2, n2->n_value, len2);
	*(num2+len2) = 0;
	n2ptr = num2;
	while(*n2ptr == 0) {
		n2ptr++;
		len2--;
	}

  /* Calculate the number of quotient digits. */
	if(len2 > len1+scale) {
		qdigits = scale+1;
		zero = TRUE;
	}
	else {
		zero = FALSE;
		if(len2 > len1)
			qdigits = scale+1;  	/* One for the zero integer part. */
		else
			qdigits = len1-len2+scale+1;
	}

  /* Allocate and zero the storage for the quotient. */
	qval = bc_new_num(qdigits-scale,scale);
	memset(qval->n_value, 0, qdigits);

  /* Allocate storage for the temporary storage mval. */
	mval = (unsigned char *) malloc (len2+1);
	if(mval == NULL)
		bc_out_of_memory();

  
	if(!zero) {
      /* Normalize */
		norm =  10 / ((int)*n2ptr + 1);
		if(norm != 1) {
			_one_mult(num1, len1+scale1+extra+1, norm, num1);
			_one_mult(n2ptr, len2, norm, n2ptr);
		}

      /* Initialize divide loop. */
		qdig = 0;
		if(len2 > len1)
			qptr = (unsigned char *) qval->n_value+len2-len1;
		else
			qptr = (unsigned char *) qval->n_value;

      /* Loop */
		while(qdig <= len1+scale-len2) {
	  /* Calculate the quotient digit guess. */
			if(*n2ptr == num1[qdig])
				qguess = 9;
			else
				qguess = (num1[qdig]*10 + num1[qdig+1]) / *n2ptr;
	
	  /* Test qguess. */
			if(n2ptr[1]*qguess > (num1[qdig]*10 + num1[qdig+1] - *n2ptr*qguess)*10 + num1[qdig+2]) {
				qguess--;
	    
				if(n2ptr[1]*qguess > (num1[qdig]*10 + num1[qdig+1] - *n2ptr*qguess)*10 + num1[qdig+2])
					qguess--;
			}

	  /* Multiply and subtract. */
			borrow = 0;
			if(qguess != 0) {
				*mval = 0;
				_one_mult(n2ptr, len2, qguess, mval+1);
				ptr1 = (unsigned char *) num1+qdig+len2;
				ptr2 = (unsigned char *) mval+len2;
				for(count = 0; count < len2+1; count++) {
					val = (int) *ptr1 - (int) *ptr2-- - borrow;
					if(val < 0) {
						val += 10;
						borrow = 1;
					}
					else
						borrow = 0;
					*ptr1-- = val;
				}
			}

	  /* Test for negative result. */
			if(borrow == 1) {
				qguess--;
				ptr1 = (unsigned char *) num1+qdig+len2;
				ptr2 = (unsigned char *) n2ptr+len2-1;
				carry = 0;
				for(count = 0; count < len2; count++) {
					val = (int) *ptr1 + (int) *ptr2-- + carry;
					if(val > 9) {
						val -= 10;
						carry = 1;
					}
					else
						carry = 0;
					*ptr1-- = val;
				}
				if(carry == 1)
					*ptr1 = (*ptr1 + 1) % 10;
			}

	  
			*qptr++ =  qguess;
			qdig++;
		}	
	}

  /* Clean up and return the number. */
	qval->n_sign = ( n1->n_sign == n2->n_sign ? PLUS : MINUS );
	if(bc_is_zero (qval))
		qval->n_sign = PLUS;
	_bc_rm_leading_zeros (qval);
	bc_free_num (quot);
	*quot = qval;

  /* Clean up temporary storage. */
	free (mval);
	free (num1);
	free (num2);

	return 0;	
}

/* Division *and* modulo for numbers.  This computes both NUM1 / NUM2 and
   NUM1 % NUM2  and puts the results in QUOT and REM, except that if QUOT
   is NULL then that store will be omitted.
 */

int bc_divmod(my_num num1, my_num num2, my_num *quot, my_num *rem, int scale) {
	my_num quotient = NULL;
	my_num temp;
	int rscale;
	
  /* Test if num2 is 0 */
	if(bc_is_zero(num2))
		return -1;
	
  /* Calculate final scale. */
	rscale = MAX(num1->n_scale, num2->n_scale+scale);
	bc_init_num(&temp);
	
  /* Calculate it. */
	bc_divide(num1, num2, &temp, scale);
	
	if(quot)
		quotient = bc_copy_num(temp);
	bc_multiply(temp, num2, &temp, rscale);
	bc_sub(num1, temp, rem, rscale);
	bc_free_num(&temp);

	if(quot) {
		bc_free_num(quot);
		*quot = quotient;
	}
	
	return 0;	
}


  /* Modulo */
int bc_modulo(my_num num1, my_num num2, my_num *result, int scale) {
	return bc_divmod(num1, num2, NULL, result, scale);
}

 /* Finds the square root using Newton's method. */
     
int bc_sqrt(my_num *num, int scale) {
	int rscale, cmp_res, done;
	int cscale;
	my_num guess, guess1, point5, diff;
		
	cmp_res = bc_compare(*num, _zero_);
	if(cmp_res < 0)
		return 0;	
	else {
		if(cmp_res == 0) {
			bc_free_num(num);
			*num = bc_copy_num(_zero_);
			return 1;
		}
	}
	cmp_res = bc_compare(*num, _one_);
	if(cmp_res == 0) {
		bc_free_num(num);
		*num = bc_copy_num(_one_);
		return 1;
	}


	rscale = MAX(scale, (*num)->n_scale);
	bc_init_num(&guess);
	bc_init_num(&guess1);
	bc_init_num(&diff);
	point5 = bc_new_num(1,1);
	point5->n_value[1] = 5;

	if(cmp_res < 0) {
		guess = bc_copy_num(_one_);
		cscale = (*num)->n_scale;
	}
	else {

		bc_int2num(&guess,10);
		bc_int2num(&guess1,(*num)->n_len);
		bc_multiply(guess1, point5, &guess1, 0);
		guess1->n_scale = 0;
		bc_raise(guess, guess1, &guess, 0);
		bc_free_num(&guess1);
		cscale = 3;
	}


	done = FALSE;
	while(!done) {
		bc_free_num(&guess1);
		guess1 = bc_copy_num(guess);
		bc_divide(*num, guess, &guess, cscale);         
		bc_add(guess, guess1, &guess, 0);
		bc_multiply(guess, point5, &guess, cscale);
		bc_sub(guess, guess1, &diff, cscale+1);
		if(bc_is_near_zero (diff, cscale)) {
			if(cscale < rscale+1)
				cscale = MIN(cscale*3, rscale+1);
			else
				done = TRUE;
		}
	}

 
	bc_free_num(num);
	bc_divide(guess,_one_,num,rscale);            
	bc_free_num(&guess);
	bc_free_num(&guess1);
	bc_free_num(&point5);
	bc_free_num(&diff);
	return 1;
}

/* Convert a number to a long if possible else return 0 */

long bc_num2long(my_num num) {
	long val;
	char *nptr;
	int  index;

  /* ignore fraction. */
	val = 0;
	nptr = num->n_value;
	for(index = num->n_len; (index>0) && (val <= (LONG_MAX/BASE)); index--)
		val = val*BASE + *nptr++;

  /* if greaterthan return 0 */
	if(index > 0)
		val = 0;
	if(val < 0)
		val = 0;

  /* Return the value. */
	if(num->n_sign == PLUS)
		return (val);
	else
		return (-val);
}

  /* Convert an integer VAL to a bc number NUM. */

void bc_int2num(my_num *num, int val) {
	char buffer[30];
	char *bptr, *vptr;
	int  ix = 1;
	char neg = 0;

  /* Check Sign. */
	if(val < 0) {
		neg = 1;
		val = -val;
	}

	bptr = buffer;
	*bptr++ = val % BASE;
	val = val / BASE;

	while(val != 0) {
		*bptr++ = val % BASE;
		val = val / BASE;
		ix++; 		
	}

  /* number */
	bc_free_num(num);
	*num = bc_new_num(ix, 0);
	if(neg) 
		(*num)->n_sign = MINUS;

  /* Assign the digits. */
	vptr = (*num)->n_value;
	while(ix-- > 0)
		*vptr++ = *--bptr;
}

/* Convert strings to bc numbers.  Base 10 only.*/
void bc_str2num(my_num *num, char *str, int scale) {
	int digits, strscale;
	char *ptr, *nptr;
	char zero_int;
	
  /* Prepare num. */
	bc_free_num(num);

  /* Check for valid number and count digits. */
	ptr = str;
	digits = 0;
	strscale = 0;
	zero_int = FALSE;
	if( (*ptr == '+') || (*ptr == '-'))
		ptr++;  			/* Sign */
	while (*ptr == '0')
		ptr++;				/* Skip leading zeros. */
	while(isdigit((int)*ptr))
		 ptr++, digits++;		/* digits */
	if(*ptr == '.')
		ptr++;				/* decimal point */
	while(isdigit((int)*ptr))
		ptr++, strscale++;		/* digits */
	if((*ptr != '\0') || (digits+strscale == 0)) {
		*num = bc_copy_num(_zero_);
		return;
	}

  /* Adjust numbers and allocate storage and initialize fields. */
	strscale = MIN(strscale, scale);
	if(digits == 0) {
		zero_int = TRUE;
		digits = 1;
	}
	*num = bc_new_num(digits, strscale);

  /* Build the whole number. */
	ptr = str;
	if(*ptr == '-') {
		(*num)->n_sign = MINUS;
		ptr++;
	}
	else {
		(*num)->n_sign = PLUS;
		if(*ptr == '+')
			ptr++;
	}
	while(*ptr == '0')
		ptr++;				
	nptr = (*num)->n_value;
	if(zero_int) {
		*nptr++ = 0;
		digits = 0;
	}
	for(;digits > 0; digits--)
		*nptr++ = CH_VAL(*ptr++);


  /* Build the fractional part. */
	if(strscale > 0) {
		ptr++;				
		for(;strscale > 0; strscale--)
			*nptr++ = CH_VAL(*ptr++);
  	}
}

/* Convert a numbers to a string.  Base 10 only.*/

char *num2str(my_num num) {
	char *str, *sptr;
	char *nptr;
	int  index, signch;

  /* Allocate memory. */
	signch = ( num->n_sign == PLUS ? 0 : 1 );  /* Number of sign chars. */
	if(num->n_scale > 0)
		str = (char *) malloc (num->n_len + num->n_scale + 2 + signch);
	else
		str = (char *) malloc (num->n_len + 1 + signch);
	if(str == NULL)
		bc_out_of_memory();

  /* The negative sign if needed. */
	sptr = str;
	if(signch)
		*sptr++ = '-';

  /* Load the whole number. */
	nptr = num->n_value;
	for(index = num->n_len; index>0; index--)
		*sptr++ = BCD_CHAR(*nptr++);

  /* fraction part. */
	if(num->n_scale > 0) {
		*sptr++ = '.';
		for(index = 0; index < num->n_scale; index++)
			*sptr++ = BCD_CHAR(*nptr++);
	}

  /* Terminate the string and return it! */
	*sptr = '\0';
	return str;
}

/* output a number*/

void bc_out_num(my_num num, int o_base, void (*out_char)(int), int leading_zero) {
	
	char *nptr;
	int index, fdigit, pre_space;
	stk_rec *digits, *temp;
	my_num int_part, frac_part, base, cur_dig, t_num, max_o_digit;

  /* The negative sign if needed. */
	if(num->n_sign == MINUS)
		(*out_char) ('-');
	
  /* Output the number. */
	if(bc_is_zero(num))
		(*out_char) ('0');
	else
		if(o_base == 10) {
	/* The number is in base 10*/
			nptr = num->n_value;
			if(num->n_len > 1 || *nptr != 0)
				for(index=num->n_len; index>0; index--)
					(*out_char) (BCD_CHAR(*nptr++));
			else
				nptr++;

			if(leading_zero && bc_is_zero(num))
				(*out_char) ('0');

	/* Now the fraction. */
			if (num->n_scale > 0) {
				(*out_char) ('.');
				for(index = 0; index < num->n_scale; index++)
					(*out_char) (BCD_CHAR(*nptr++));
			}
		}
		else {
	/* special case ... */
			if(leading_zero && bc_is_zero (num))
				(*out_char) ('0');

	/* The number is some other base. */
			digits = NULL;
			bc_init_num(&int_part);
			bc_divide(num, _one_, &int_part, 0);
			bc_init_num(&frac_part);
			bc_init_num(&cur_dig);
			bc_init_num(&base);
			bc_sub(num, int_part, &frac_part, 0);
	/* Make the INT_PART and FRAC_PART positive. */
			int_part->n_sign = PLUS;
			frac_part->n_sign = PLUS;
			bc_int2num(&base, o_base);
			bc_init_num(&max_o_digit);
			bc_int2num(&max_o_digit, o_base-1);


	/* Get the digits of the integer part and push them on a stack. */
			while(!bc_is_zero(int_part)) {
	    			bc_modulo(int_part, base, &cur_dig, 0);
	    			temp = (stk_rec *) malloc (sizeof(stk_rec));
	    			if(temp == NULL) bc_out_of_memory();
	    				temp->digit = bc_num2long (cur_dig);
	    			temp->next = digits;
	    			digits = temp;
	    			bc_divide(int_part, base, &int_part, 0);
	  		}

	/* Print the digits on the stack. */
			if(digits != NULL) {
	    /* Output the digits. */
	    			while (digits != NULL) {
					temp = digits;
					digits = digits->next;
					if(o_base <= 16)
		  				(*out_char) (ref_str[ (int) temp->digit]);
					else
		  				bc_out_long(temp->digit, max_o_digit->n_len, 1, out_char);
					free(temp);
	      			}
	  		}

	/* Get and print the digits of the fraction part. */
			if(num->n_scale > 0) {
	    			(*out_char) ('.');
	    			pre_space = 0;
	    			t_num = bc_copy_num(_one_);
	    			while(t_num->n_len <= num->n_scale) {
	      				bc_multiply(frac_part, base, &frac_part, num->n_scale);
	      				fdigit = bc_num2long(frac_part);
	      				bc_int2num(&int_part, fdigit);
	      				bc_sub(frac_part, int_part, &frac_part, 0);
	      				if(o_base <= 16)
						(*out_char) (ref_str[fdigit]);
	      				else {
						bc_out_long(fdigit, max_o_digit->n_len, pre_space, out_char);
						pre_space = 1;
	      				}
	      				bc_multiply(t_num, base, &t_num, 0);
	    			}
	    			bc_free_num(&t_num);
	  		}

	/* Clean up. */
			bc_free_num(&int_part);
			bc_free_num(&frac_part);
			bc_free_num(&base);
			bc_free_num(&cur_dig);
			bc_free_num(&max_o_digit);
		}
	
}

void bc_out_long(long val, int size, int space, void (*out_char)(int)) {
	char digits[40];
	int len, idx;

	if(space)
		(*out_char) (' ');
	sprintf(digits, "%ld", val);
	len = strlen (digits);
	while(size > len) {
		(*out_char) ('0');
		size--;
	}
	for(idx=0; idx < len; idx++)
		(*out_char) (digits[idx]);
}


void out_char(int c) {
	putchar(c);
}

/* print number in base 10*/
void pn(my_num num) {
	bc_out_num(num, 10, out_char, 0);
	out_char('\n');
}

/* Errors */
void rt_warn(char *mesg ) {
	fprintf(stderr, "%s\n", mesg);
	exit(1);
}

void rt_error(char *mesg ){
	fprintf(stderr, "%s\n", mesg);
	exit(1);
}
void out_of_memory() {
	fprintf(stderr, "Fatal error: Out of memory for malloc.\n");
	exit (1);
}


 /*Converts a num to given i_base*/
char *bc_out_num_base(my_num num, int i_base, int scale) {
	int k = 0;
	my_num int_part, frac_part, base, cur_dig, t_num, ans, ten, tmp, tmk;
	char *result;
	
	bc_init_num(&ans);
	  /* The negative sign if needed. */
	if(num->n_sign == MINUS)
		ans->n_sign = MINUS;
	  /* Output the number. */
	if(bc_is_zero(num))
		bc_init_num(&ans);
	else {
		
	/* The number is some other base. */
		bc_init_num(&int_part);
		bc_divide(num, _one_, &int_part, 0);
		bc_init_num(&frac_part);
		bc_init_num(&cur_dig);
		bc_init_num(&base);
		bc_sub(num, int_part, &frac_part, 0);
	/* Make the INT_PART and FRAC_PART positive. */
		int_part->n_sign = PLUS;
		frac_part->n_sign = PLUS;
		bc_int2num(&base, i_base);
	
		//bc_int2num(&max_o_digit, o_base-1);
		bc_init_num(&t_num);
		bc_init_num(&ten);
		bc_int2num(&ten, 10);
		bc_init_num(&tmp);
		bc_init_num(&tmk);
		bc_int2num(&tmk, k);
		
	/* Get the digits of the integer part and push them on a stack. */
		while(!bc_is_zero(int_part)) {
	    		bc_modulo(int_part, ten, &cur_dig, 0);
			bc_divide(int_part, ten, &int_part, 0);
	    		bc_raise(base, tmk, &t_num, 0);
			bc_multiply(t_num, cur_dig, &tmp, 0);
			bc_add(ans, tmp, &ans, 0);
			bc_add(tmk, _one_, &tmk,  0);
			
	  	}
		k = frac_part->n_scale;
		bc_int2num(&tmk, -1);	
		
	/* Now the fraction part */
		while(k !=0 ){
			bc_multiply(frac_part, ten, &frac_part, 0);
			bc_int2num(&t_num, frac_part->n_value[0]);			
			bc_raise(base, tmk, &t_num, scale);
			bc_multiply(t_num, cur_dig, &tmp, 0);
			bc_add(ans, tmp, &ans, 0);
			bc_sub(tmk, _one_, &tmk,  0);
			k--;
			
		}
			

	}	
		
		bc_free_num(&int_part);
		bc_free_num(&frac_part);
		bc_free_num(&base);
		bc_free_num(&cur_dig);
		bc_free_num(&tmk);
		bc_free_num(&ten);
		bc_free_num(&t_num);
		bc_free_num(&tmp);
		result = bc_num2str(ans);
		bc_free_num(&ans);
		return result;
}

