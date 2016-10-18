/* mynumber.c: Implements arbitrary precision numbers. */
#include <stdio.h>
#include "mynumber.h"
#include "bcconfig.h"
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Storage used for special numbers. */
bc_num _zero_;
bc_num _one_;
bc_num _two_;

/* new_num allocates a number and sets fields to known values. */

bc_num bc_new_num(int length,int scale) {
	bc_num temp;
	temp = (bc_num) malloc(sizeof(bc_struct));
//	if (temp == NULL) 
//		bc_out_of_memory();
	temp->n_sign = PLUS;
	temp->n_len = length;
	temp->n_scale = scale;
	temp->n_refs = 1;
	temp->n_ptr = (char *) malloc(length+scale);
//	if (temp->n_ptr == NULL)
//		bc_out_of_memory();
	temp->n_value = temp->n_ptr;
	memset(temp->n_ptr, 0, length+scale);
	return temp;
}

/* "Frees" a bc_num NUM. */

void bc_free_num(bc_num *num) {
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

/* Intitialize the extern number */

void bc_init_numbers() {
	_zero_ = bc_new_num(1,0);
	_one_  = bc_new_num(1,0);
	_one_->n_value[0] = 1;
	_two_  = bc_new_num(1,0);
	_two_->n_value[0] = 2;
}

/* Make a copy of a number!  Just increments the reference count! */

bc_num bc_copy_num(bc_num num) {
	num->n_refs++;
	return num;
}

/* Initialize a number NUM by making it a copy of zero. */

void bc_init_num(bc_num *num) {
	*num = bc_copy_num(_zero_);
}

/* For many things, we may have leading zeros in a number NUM.
   _bc_rm_leading_zeros just moves the data "value" pointer to the
   correct place and adjusts the length. */

static void _bc_rm_leading_zeros(bc_num num) {
  /* We can move n_value to point to the first non zero digit! */
	while(*num->n_value == 0 && num->n_len > 1) {
		num->n_value++;
		num->n_len--;
	}
}

/* Compare two bc numbers.  Return value is 0 if equal, -1 if N1 is less
   than N2 and +1 if N1 is greater than N2.  If USE_SIGN is false, just
   compare the magnitudes. */

static int _bc_do_compare(bc_num n1, bc_num n2, int use_sign, int ignore_last) {
	char *n1ptr, *n2ptr;
	int  count;

  /* First, compare signs. */
	if(use_sign && n1->n_sign != n2->n_sign) {
		if(n1->n_sign == PLUS)
			return (1);     /* Positive N1 > Negative N2 */
		else
			return (-1);    /* Negative N1 < Positive N1 */
	}

  /* Now compare the magnitude. */
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
				return (-1);
			else
				return (1);
        	}
	}

  /* If we get here, they have the same number of integer digits.
     check the integer part and the equal length part of the fraction. */
	count = n1->n_len + MIN(n1->n_scale, n2->n_scale);
	n1ptr = n1->n_value;
	n2ptr = n2->n_value;

	while((count > 0) && (*n1ptr == *n2ptr)) {
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
				return (1);
			else
				return (-1);
		}
		else {
          /* Magnitude of n1 < n2. */
			if(!use_sign || n1->n_sign == PLUS)
				return (-1);
			else
				return (1);
		}
	}

  /* They are equal up to the last part of the equal part of the fraction. */
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
					return (-1);
				else
					return (1);
			}
		}
	}

  /* They must be equal! */
	return (0);
}

/* This is the "user callable" routine to compare numbers N1 and N2. */

int bc_compare(bc_num n1, bc_num n2) {
	return _bc_do_compare (n1, n2, TRUE, FALSE);
}

/* In some places we need to check if the number is negative. */

int bc_is_neg(bc_num num) {
	return num->n_sign == MINUS;
}

/* In some places we need to check if the number NUM is zero. */

int bc_is_zero(bc_num num) {
	int  count;
	char *nptr;

  /* Quick check. */
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
   Specifically, all but the last digit is 0 and the last digit is 1.
   Last digit is defined by scale. */

int bc_is_near_zero(bc_num num, int scale) { /* Keep this function if necessary */
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

/* Perform addition: N1 is added to N2 and the value is
   returned.  The signs of N1 and N2 are ignored.
   SCALE_MIN is to set the minimum scale of the result. */

static bc_num _bc_do_add(bc_num n1, bc_num n2, int scale_min) {
	bc_num sum;
	int sum_scale, sum_digits;
	char *n1ptr, *n2ptr, *sumptr;
	int carry, n1bytes, n2bytes;
	int count;

  /* Prepare sum. */
	sum_scale = MAX(n1->n_scale, n2->n_scale);
	sum_digits = MAX(n1->n_len, n2->n_len) + 1;
	sum = bc_new_num(sum_digits, MAX(sum_scale, scale_min));

  /* Zero extra digits made by scale_min. */
	if(scale_min > sum_scale) {
		sumptr = (char *)(sum->n_value + sum_scale + sum_digits);
		for(count = scale_min - sum_scale; count > 0; count--)
			*sumptr++ = 0;
	}

  /* Start with the fraction part.  Initialize the pointers. */
	n1bytes = n1->n_scale;
	n2bytes = n2->n_scale;
	n1ptr = (char *) (n1->n_value + n1->n_len + n1bytes - 1);
	n2ptr = (char *) (n2->n_value + n2->n_len + n2bytes - 1);
	sumptr = (char *) (sum->n_value + sum_scale + sum_digits - 1);

  /* Add the fraction part.  First copy the longer fraction.*/
	if(n1bytes != n2bytes) {
		if(n1bytes > n2bytes)
			while(n1bytes>n2bytes) {
				*sumptr-- = *n1ptr--; 
				n1bytes--;
			}
		else
			while(n2bytes>n1bytes) {
				*sumptr-- = *n2ptr--;
				n2bytes--;
			}
	}

  /* Now add the remaining fraction part and equal size integer parts. */
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

  /* Now add carry the longer integer part. */
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

/* Perform subtraction: N2 is subtracted from N1 and the value is
   returned.  The signs of N1 and N2 are ignored.  Also, N1 is
   assumed to be larger than N2.  SCALE_MIN is the minimum scale
   of the result. */

static bc_num _bc_do_sub(bc_num n1, bc_num n2, int scale_min) {
	bc_num diff;
	int diff_scale, diff_len;
	int min_scale, min_len;
	char *n1ptr, *n2ptr, *diffptr;
	int borrow, count, val;

  /* Allocate temporary storage. */
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

  /* Initialize the subtract. */
	n1ptr = (char *)(n1->n_value + n1->n_len + n1->n_scale -1);
	n2ptr = (char *)(n2->n_value + n2->n_len + n2->n_scale -1);
	diffptr = (char *) (diff->n_value + diff_len + diff_scale -1);

  /* Subtract the numbers. */
	borrow = 0;

  /* Take care of the longer scaled number. */
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

  /* Now do the equal length scale and integer parts. */

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

  /* If n1 has more digits then n2, we now do that subtract. */
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

  /* Clean up and return. */
	_bc_rm_leading_zeros(diff);
	return diff;
}

/* Here is the full subtract routine that takes care of negative numbers.
   N2 is subtracted from N1 and the result placed in RESULT.  SCALE_MIN
   is the minimum scale for the result. */

void bc_sub(bc_num n1, bc_num n2, bc_num *result, int scale_min) {
	bc_num diff = NULL;
	int cmp_res;
	int res_scale;

	if(n1->n_sign != n2->n_sign) {
		diff = _bc_do_add(n1, n2, scale_min);
		diff->n_sign = n1->n_sign;
	}
	else {
      /* subtraction must be done. */
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

  /* Clean up and return. */
	bc_free_num(result);
	*result = diff;
}

void
bc_add(bc_num n1, bc_num n2, bc_num *result, int scale_min) {
	bc_num sum = NULL;
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

  /* Clean up and return. */
	bc_free_num(result);
	*result = sum;
}

 /* Simple multiplication algorithm to multiply two numbers*/
static void _bc_simp_mul(bc_num n1, int n1len, bc_num n2, int n2len, bc_num *prod, int full_scale) {
	char *n1ptr, *n2ptr, *pvptr;
	char *n1end, *n2end;		
	int i, sum, prodlen;

	prodlen = n1len + n2len + 1;
	*prod = bc_new_num(prodlen, 0);

	n1end = (char *) (n1->n_value + n1len - 1);
	n2end = (char *) (n2->n_value + n2len - 1);
	pvptr = (char *) ((*prod)->n_value + prodlen - 1);
	sum = 0;

  /* Here is the loop... */
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

void bc_raise(bc_num num1, bc_num num2, bc_num *result, int scale) {
	bc_num temp, power;
	long exponent;
	int rscale;
	int pwrscale;
	int calcscale;
	char neg;

   /* Check the exponent for scale digits and convert to a long. */
	if(num2->n_scale != 0);
//		bc_rt_warn("non-zero scale in exponent");
	exponent = bc_num2long(num2);
	if(exponent == 0 && (num2->n_len > 1 || num2->n_value[0] != 0));
//		bc_rt_error("exponent too large in raise");

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
   /* Do the calculation. */
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
//		bc_divide(_one_, temp, result, rscale);
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

void bc_multiply(bc_num n1, bc_num n2, bc_num *prod, int scale) {
	bc_num prodval; 
	int len1, len2;
	int full_scale, prod_scale;

  /* Initialize things. */
	len1 = n1->n_len + n1->n_scale;
	len2 = n2->n_len + n2->n_scale;
	full_scale = n1->n_scale + n2->n_scale;
	prod_scale = MIN(full_scale,MAX(scale,MAX(n1->n_scale,n2->n_scale)));

  /* Do the multiply */
	_bc_simp_mul(n1, len1, n2, len2, &prodval, full_scale);
  /* Assign to prod and clean up the number. */
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

 //  Finds the square root using Newton's method.
     
int bc_sqrt(bc_num *num, int scale) {
	int rscale, cmp_res, done;
	int cscale;
	bc_num guess, guess1, point5, diff;

  
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
//		bc_divide(*num, guess, &guess, cscale);         write divide
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
//	bc_divide(guess,_one_,num,rscale);            Write divide
	bc_free_num(&guess);
	bc_free_num(&guess1);
	bc_free_num(&point5);
	bc_free_num(&diff);
	return 1;
}

/* Convert a number NUM to a long.  The function returns only the integer
   part of the number.  For numbers that are too large to represent as
   a long, this function returns a zero.  */

long bc_num2long(bc_num num) {
	long val;
	char *nptr;
	int  index;

  /* Extract the int value, ignore the fraction. */
	val = 0;
	nptr = num->n_value;
	for(index = num->n_len; (index>0) && (val <= (LONG_MAX/BASE)); index--)
		val = val*BASE + *nptr++;

  /* Check for overflow. If yes then return zero. */
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

void bc_int2num(bc_num *num, int val) {
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
		ix++; 		/* Count the digits. */
	}

  /* Make the number. */
	bc_free_num(num);
	*num = bc_new_num(ix, 0);
	if(neg) 
		(*num)->n_sign = MINUS;

  /* Assign the digits. */
	vptr = (*num)->n_value;
	while(ix-- > 0)
		*vptr++ = *--bptr;
}

