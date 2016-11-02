# bc

   

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

  Numbers are stored in two types of variables, simple variables and
arrays.  Both simple variables and array variables are named.  Names
begin with a letter followed by any number of letters, digits and
underscores.  All letters must be lower case.

  There are four special variables, scale, ibase, obase, and
last. 'scale' defines how some operations use digits after the
decimal point.  The default value of scale is 0. 'ibase'
and obase define the conversion base for input and output
numbers. The default for both input and output is base 10.
last (an extension) is a variable that has the value of the last
printed number.  These will be discussed in further detail where
appropriate.  All of these variables may have values assigned to them
as well as used in expressions.
  
   Basic Expressions, Relational Expressions, About Expressions and Special Variables, Expressions
   Basic Expressions

   In the following descriptions of legal expressions, "expr" refers to a
complete expression and "var" refers to a simple variable.
A simple variable is just a name.

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

var = expr
The variable is assigned the value of the expression.

var <op>= expr
This is equivalent to "var = var <op> expr" with the
exception that the "var" part is evaluated only once. 

   Relational expressions are a special kind of expression that always
evaluate to 0 or 1, 0 if the relation is false and 1 if the relation is
true.The relational operators are

 expr1 < expr2
The result is 1 if expr1 is strictly less than expr2.

 expr1 <= expr2
The result is 1 if expr1 is less than or equal to expr2.

 expr1 > expr2
The result is 1 if expr1 is strictly greater than expr2.

 expr1 >= expr2
The result is 1 if expr1 is greater than or equal to expr2.

 expr1 == expr2
The result is 1 if expr1 is equal to expr2.

 expr1 != expr2
The result is 1 if expr1 is not equal to expr2.

 Boolean Expressions, Precedence, Relational Expressions, Expressions
 Boolean Expressions

   The result of all boolean operations are 0 and 1
(for false and true) as in relational expressions.  The boolean
operators are:

 expr && expr
The result is 1 if both expressions are non-zero.

 expr || expr
The result is 1 if either expression is non-zero.

 Precedence, Special Expressions, Boolean Expressions, Expressions
 Precedence

The expression precedence is as follows: (lowest to highest)

|| operator, left associative
&& operator, left associative, nonassociative
Relational operators, left associative
Assignment operator, right associative
+ and - operators, left associative
*, / and % operators, left associative
^ operator, right associative
unary - operator, nonassociative
++ and -- operators, nonassociative

 sqrt( expression )
The value of the sqrt function is the square root of the
expression.  If the expression is negative, a run time error is
generated.

   All numbers that are printed are printed in the base specified by the
variable 'obase'. The legal values for obase are 2 through
BC_BASE_MAX .For bases 2 through 16,
the usual method of writing numbers is used. 

   Math Library Functions.

The math library defines the following functions:

 sine x
The sine of x where x is in radians.

 cos x
The cosine of x, x is in radians.

 atan x
The arctangent of x, arctangent returns radians.

 log x
The natural logarithm of x.

 e^x
The exponential function of raising e to the value x.
