# bc
# Nikunj Soni
# 111503065
   
   bc is an arbitrary precision numeric processing language. 
   The most basic element in bc is the number.  Numbers are
arbitrary precision numbers.  This precision is both in the integer part
and the fractional part.  All numbers are represented internally in
decimal and all computation is done in decimal. There are two attributes
of numbers, the length and the scale.  The length is the total number of
significant decimal digits in a number and the scale is the total number
of decimal digits after the decimal point.  For example, .000001 has a
length of 6 and scale of 6, while 1935.000 has a length of 7 and a scale
of 3.

  There are there special variables, scale, ibase and obase. 'scale' defines how some operations use digits after the
decimal point.  The default value of scale is 0. 'ibase'
and 'obase' define the conversion base for input and output
numbers. The default for both input and output is base 10.

   Basic Expressions
   In the following descriptions of legal expressions, "expr" refers to a
complete expression.

   Unless specifically mentioned the scale of the result is the maximum
scale of the expressions involved.

 - expr
The result is the negation of the expression.

 ++ var
The variable is incremented by one and the new value is the result of
the expression.

 -- var
The variable is decremented by one and the new value is the result of the
expression.

 expr + expr
The result of the expression is the sum of the two expressions.

 expr - expr
The result of the expression is the difference of the two expressions.

 expr * expr
The result of the expression is the product of the two expressions.

 expr / expr
The result of the expression is the quotient of the two expressions.
The scale of the result is the value of the variable 'scale'

 expr % expr
The result of the expression is the "remainder" and it is computed in the
following way.  To compute a%b, first a/b is computed to scale
digits.  That result is used to compute a-(a/b)*b to the scale of the
maximum of scale+scale(b) and scale(a).  If scale is set
to zero and both expressions are integers this expression is the
integer remainder function.

 expr ^ expr
The result of the expression is the value of the first raised to the
second. The second expression must be an integer.  (If the second
expression is not an integer, a warning is generated and the
expression is truncated to get an integer value.)  The scale of the
result is scale if the exponent is negative.  If the exponent
is positive the scale of the result is the minimum of the scale of the
first expression times the value of the exponent and the maximum of
scale and the scale of the first expression.  (e.g. scale(a^b)
= min(scale(a)*b, max(scale, scale(a))).)  It should be noted
that expr^0 will always return the value of 1.

 ( expr )
This alters the standard precedence to force the evaluation of the
expression.

 $expression
The value of the function is the square root of the
expression.  If the expression is negative, a run time error is
generated.

   All numbers that are printed are printed in the base specified by the
variable 'obase'. The legal values for obase are 2 through
BC_BASE_MAX .For bases 2 through 16,
the usual method of writing numbers is used. 
