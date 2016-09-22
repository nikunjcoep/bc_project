#ifndef _NUMBER_H_
#define _NUMBER_H_

/*Errors*/
#define BC_ERROR_OUT_OF_MEMORY 0
#define BC_ERROR_EXPONENT_TOO_LARGE_IN_RAISE 1
#define BC_WARNING_NON_ZERO_SCALE_IN_EXPONENT -1
#define BC_WARNING_NON_ZERO_SCALE_IN_BASE -2
#define BC_WARNING_NON_ZERO_SCALE_IN_MODULUS -3

typedef enum {PLUS, MINUS} sign;

typedef struct bc_struct *bc_num;

typedef struct bc_struct {
	sign  n_sign;
	int   n_len;	/* The number of digits before the decimal point. */
	int   n_scale;	/* The number of digits after the decimal point. */
	int   n_refs;     /* The number of pointers to this number. */
	bc_num n_next;	/* Linked list for available list. */
	char *n_ptr;	/* The pointer to the actual storage.
			   If NULL, n_value points to the inside of
			   another number (bc_multiply...) and should
			   not be "freed." */
	char *n_value;	/* The number. Not zero char terminated.
			   May not point to the same place as n_ptr as
			   in the case of leading zeros generated. */
} bc_struct;


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
extern bc_num _zero_;
extern bc_num _one_;
extern bc_num _two_;


/* Function Prototypes */

void bc_init_numbers(void);

bc_num bc_new_num(int length, int scale);

void bc_free_num(bc_num *num);

bc_num bc_copy_num(bc_num num);

void bc_init_num(bc_num *num);

void bc_str2num(bc_num *num, char *str, int scale);

char *bc_num2str(bc_num num);

void bc_int2num(bc_num *num, int val);

long bc_num2long(bc_num num);

int bc_compare(bc_num n1, bc_num n2);

char bc_is_zero(bc_num num);

char bc_is_near_zero(bc_num num, int scale);

char bc_is_neg(bc_num num);

void bc_add(bc_num n1, bc_num n2, bc_num *result, int scale_min);

void bc_sub(bc_num n1, bc_num n2, bc_num *result, int scale_min);

void bc_multiply(bc_num n1, bc_num n2, bc_num *prod, int scale);

int bc_divide(bc_num n1, bc_num n2, bc_num *quot, int scale);

int bc_modulo(bc_num num1, bc_num num2, bc_num *result, int scale);

int bc_divmod(bc_num num1, bc_num num2, bc_num *quot, bc_num *rem, int scale);

int bc_raisemod(bc_num base, bc_num expo, bc_num mod, bc_num *result, int scale);

void bc_raise(bc_num num1, bc_num num2, bc_num *result, int scale);

int bc_sqrt(bc_num *num, int scale);

void bc_out_num(bc_num num, int o_base, void (* out_char)(int), int leading_zero);

#endif
