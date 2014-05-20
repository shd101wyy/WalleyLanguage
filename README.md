Walley Language
=========
>  Copyright (c) 2012-2014 Yiyi Wang  (shd101wyy)  
>  All Rights Reserved

-----------------------------------  
<strong>
 Walley Language (Beta Version 0.3) is a simple lisp dialect written in C.
 Both Compiler and Virtual Machine are implemented in C.
 This Toy Language is still under development, lots of bugs exist, so please don't use it for production.  
 And this Readme mainly aims to teach you how to use walley language from the start.  
 Not finished yet...
 </strong>

----------------------------------
### Installation:
 under .nix (eg Linux,Mac OS X)  
 open terminal

```sh
cd [walley-language project folder that u download, where Makefile is there]
sudo make
sudo make install
```
### Uninstallation:  
 under .nix (eg Linux, Mac OS X)  
 open terminal  

```sh
cd [walley-language project folder that u download, where Makefile is there]  
sudo make uninstall  
```
#### Windows is not currently supported (T_T)
----------------------------------
### Mysterious S Expression (~#_#~)
 <strong>
 Walley Language, like all lisp dialects, uses S expression.  
 Therefore, its syntax is extremely simple.  
</strong>

Here are some questions below, just compare ur answers with the solutions later.
 - (+ 3 4)
 - (- 3 4)
 - (* 3 4)
 - (+ 3 4 5)  

Here are solutions for the questions above:
 - 7        ; from 3 + 4
 - -1       ; from 3 - 4
 - 12       ; from 3 * 4
 - 12       ; from 3 + 4 + 5

if you understand the solutions above, you have understood S expression:  
S expression is in format of (func-name, arg1, arg2)
its head is a function, with several parameters behind
for example, in S exp (+ 3 4), + is the function, and 3, 4 are parameters.  
so (+ 3 4) runs + function with 3 4 as parameters, which produces 7 as result
and here are lots of builtin functions in walley language  
we call those functions => <strong>lambda</strong>  

----------------------------------
### Hello World  

 <strong>display</strong> is a <strong>lambda</strong>, which can print out a its parameter.  
 for example  
```lisp
 (display "Hello World")
```
 will print out
```lisp
  Hello World
```
-----------------------------------
### How to run your program
 now u already know how the display lambda works, but how do we run it in walley language?

 after installation, open ur terminal,  
 type  
```sh
walley
```
then you will see something like
```sh
Walley Language 0.3.8465
Copyright (c) 2012-2014 Yiyi Wang
All Rights Reserved

walley>
```
This is Walley Language repl environment,  
try to type (display "Hello World") after walley> and then press "enter"
```sh
walley > (display "Hello World")
```
you will see the string Hello World print out.  
also, try to type something like (+ 3 4), (- 3 4 5) (* 1 2 3)  to see what u can get  

 -----------------------------------
### Define Your Own Lambda
 now we already know how to use S expression to run a <strong>lambda</strong>.  
 Sometimes, we want to define our own <strong>lambda</strong>, like
```lisp
   (@@my-handsome-lambda@@ 3 4),  
```
 and how do we do that?  
 First, let's do something simple,  
 Guess the answer of the following S expression:  
```lisp
((lambda [a b] (+ a b)) 3 4)
```
the solution is 7,
here we defined an <strong>anonymous lambda:</strong>
```lisp
(lambda [a b] (+ a b))
```
and we then passed 3 4 as parameters to it
```lisp
((lambda [a b] (+ a b)) 3 4)
```
which produced the answer 7.  
Now try to guess the answer of the following S expression:  
```lisp
((lambda [a b] (+ a b)) 5 6)
((lambda [a b] (+ a b)) 1 2)
((lambda [a b] (- a b)) 3 4)
((lambda [a b c] (+ a b c)) 1 2 3)
```
the solutions are:  
- 11  
- 3  
- 7  
- 6  

-------------------------------------
### However, it is so inconvenient to run lambda like that ((lamda [a b ...
So how do we give the lambda a name,  
like
```lisp
(my-lambda 3 4)  ;; which evaluates (+ 3 4) and produces 7 as result?
```
here we need to <strong> define a variable as lambda: </strong>  
and below is how we do that
```lisp
(def my-lambda (lambda [a b] (+ a b))) ;; don't forget to add space between lambda and [a b], otherwise it will cause error.
```
and call it like
```lisp
(my-lambda 3 4)
```
since
```lisp
my-lambda <=> (lambda [a b] (+ a b))
```
when evaluating <strong>(my-lambda 3 4)</strong>, we just replace <strong>my-lambda</strong> with <strong>(lambda [a b] (+ a b))</strong>
,  which is
```lisp
((lambda [a b] (+ a b)) 3 4)
```
Now, we don't need to write that long lambda as head anymore. ;)  
Try to define ur own lambda in walley repl, and run it!.

--------------------------------------
### More about defining a variable
we just learnt how to  bind a <strong>lambda</strong> to a <strong>variable</strong> called <strong>my-lambda</strong>
In fact, in walley language, you can also bind any other values to a variable.
for example
```lisp
(def a 12)
(display a)
```
will print out a's value, which is 12  
```lisp
(def b (+ 3 4))
(display b)
```
will print out b's value, which is 7  

However, when we try to run the following code, one error will occur
```lisp
(def x 12)
(def x 30)
```
In Walley Language, we can only define a variable once.  
Since we already defined a variable called <strong>x</strong>  
we <strong>cannot define it again</strong>  
So what should we do if we want to change the value of <strong>x</strong>

-----------------------------------------
### Change me!!!
just like <strong>def</strong>, a magical "lambda" that we used to define a variable,  
we can use another magical "lambda", <strong>set!</strong>, to change the original value of a variable.
see the following code:
```lisp
(def x 12)
(set! x (+ 4 5))
```
We first defined a variable called <strong>x</strong> with value <strong>12</strong>.  
then we changed it value to <strong>9</strong>
And that's all ;)

------------------------------------------
### Other than lambda and number???
Now, we learnt how to bind a lambda or number to a variable.  
Furthermore, walley language supports lot's of data types.  

we can bind a string to a variable like  
```lisp
(def x "Hello World")
(display x) ; this will print out Hello World
```
however, I am too lazy to type "" all the time, so I <strong>"choose"</strong> to use '(single quote) to help me
reduce the "workload"  

to define a string like "hi", we can do
```lisp
(def x 'hi)  ;; instead of (def x "hi")
```
here, we use only one single quote to quote a string.  
Now we don't need to use "" anymore.  
Wait, we still need to use "" sometimes. When here is space in the string,
we cannot use '.
eg "Hello World" is correct, but 'Hello World is wrong.  
in this case, we can only use ""

-------------------------------------------
### What's more.
Now, I am gonna introduce u guys a new data type. pair
First, can u tell me the answer of the S exp
```lisp
(+ 3 4)
```
If u answer 7, then u are correct.
Now try to answer the answer of the S exp
```lisp
'(+ 3 4)
```
If ur answer is still 7, then sorry, u are wrong.  
The answer of <strong>'(+ 3 4) </strong> is still <strong>(+ 3 4)</strong>.  
Notice that We put a <strong>single quote '</strong> before the <strong>parentheses (</strong>, thus this S expression is <strong>not evaluated</strong>.
The single quote is an abbreviation of lambda <strong>quote</strong>.  
so
```lisp
'(+ 3 4)
(quote (+ 3 4))
```
are the same, they both stopped the evaluation of the S expression <strong>(+ 3 4)</strong>.  

Now try to run the following code in walley repl and see that u can get
```lisp
(def x '(x 4 5))
(display x)
```
-------------------------------------------
### Quote?
<strong>Pair</strong>, like <strong>lambda</strong> and <strong>number</strong> and <strong>string</strong>, is a builtin data type in walley language.  
the code below
```lisp
(def x '(x 4 5))
(display x)
```
will print out
```lisp
(x 4 5)
```
as result.
the value of x is a <strong>pair</strong>.

Do u still remember the <strong>single quote string </strong>that we talked about before?
```lisp
(def x 'hi)
(display x)
```
it is actually the same as
```lisp
(def x (quote hi))
(display x)
```
the <strong>quote</strong> lambda stopped the evaluation of the variable <strong>hi</strong>.
Now consider the following code, which doesn't use <strong>quote</strong> lambda.
```lisp
(def hi 12)
(def x hi)
(display x)
```
will print out the value <strong>12</strong>.
In this case, without <strong>quote</strong>
variable <strong>hi</strong> is evaluated, which gives variable <strong>x</strong> the value 12.

<strong> Quote </strong> is a very import lambda, which can stop the evaluation of a variable.
And we will see more applications about it in the future.

----------------------------------------------
### More about Pair
So far, we have learnt just a little about the data type <strong> pair </strong>.
I know u want to know more about it ;)  
Now, I will talk more about it.  

NOW Introducing two new lambdas.  
<strong> car ,  cdr </strong>  
- <strong>car</strong> : give u the <strong>first</strong> element of the pair  
- <strong>cdr</strong> : give the <strong>rest</strong> elements(except for the first element) of the pair  

for example
```lisp
(def x '(a b c))
(def y (car x))
(display y)
```
will print out string <strong> a </strong>.
here <strong>(car x) </strong> gets the first element of the pair <strong>x</strong>,  
which is the string <strong> a </strong>

another example
```lisp
(def x '(a b c))
(def y (cdr x))
(display y)
```
will print output pair <strong>(b c) </strong>.  
here <strong>(cdr x) </strong> gives us the rest elements of the pair <strong>x</strong>,
which is the pair <strong> (b c) </strong>.  


with lambdas <strong> car </strong> and <strong> cdr </strong>, we can get the elements from a pair easily.  
We can also define some interesting lambdas using those two lambdas.  
for example.
```lisp
(def cadr (lambda [a] (car (cdr a))))        ;; give us the 2nd element of pair
(def caddr (lambda [a] (car (cdr (cdr a))))) ;; give us the 3rd element of pair
(def cddr (lambda [a] (cdr (cdr a))))        ;; give us the rest rest elements of pair
```

now try to define a variable <strong>x</strong> with <strong>pair</strong> value  
<strong>(1 2 3 4 5 6)</strong>.  
and try to think about the answer of the following S expressions:  
```lisp
(car x)
(cadr x)
(caddr x)
(cddr x)
```
PS: run it later in walley repl to check ur answers.
cadr caddr cddr are already defined, so u don't need to define them again.



----------------------------------------------
### Make Pair in other ways
We learnt how to use the <strong>quote</strong> to stop the evaluation of the S expression and get pair or string as result.  
In Walley Language, there are also other ways to make a pair.
The first new lambda that I will introduce to u
is <strong>list</strong>.  
Consider the following code:
```lisp
(list 'a 'b 'c)
```
this code will give use the pair <strong>(a b c)</strong>.  
In contrast to <strong>quote</strong>, <strong>list</strong> will evaluate all of its parameters and connect them to generate a list(pair)  
So the following code:  
```lisp
(def x 12)
(list x 'x)
```
will generate pair <strong>(12 x)</strong>  
where  
- the first param <strong>x</strong> generates <strong>number 12</strong>  
- the second param <strong>'x</strong> generates <strong>string x</strong>

The second new lambda that I will introduce to u  
is <strong>cons</strong>.  
Consider the following code:  
```lisp
(cons 'a ())
```
will produce the pair <strong> (a) </strong>.  
We can conside it as we connect <strong>string a</strong> to an <strong>empty pair ()</strong>  

So how about:
```lisp
(cons 'a 'b)
```
this code will produce the pair <strong> (a . b) </strong>.  
Before we discussion this result,  
U might think why it is not <strong> (a b) </strong>,  
because to get <strong> (a b) </strong>, we need to use <strong>the empty pair ()</strong> as the tail.  
So in order to get (a b), we need to run the following code:
```lisp
(cons 'a (cons 'b ()))
```
Now let's go back to <strong> (a . b) </strong>.  
the <strong> . </strong> between <strong>a</strong> and <strong>b</strong>  
means that we connect <strong>a</strong> with <strong>b</strong> directly,  
and here is no <strong>empty pair ()</strong> in the end.  

thus
```lisp
(def x (cons 'a 'b)) ;; x => (a . b)
(car x)              ;;   => a
(cdr x)              ;;   => b
```
while
```lisp
(def x (cons 'a (cons 'b ()))) ;; x => (a b), the same as (a . (b . ()))
(car x)                        ;;   => a
(cdr x)                        ;;   => (b)
(car (cdr x))                  ;;   => b
(cdr (cdr x))                  ;;   => ()
```

This part is quite hard.  
If u still have problem, I recommend u to read http://en.wikipedia.org/wiki/Cons  
to see more about <strong>cons</strong> and how it works.

-----------------------------------------------
### The "empty pair" and If
From the last part, we learnt a kind of empty pair written as <strong>()</strong>  
In Walley Language, <strong>()</strong> is considered as another data type:  
<strong>null</strong>  
So it is actually not a kind of pair.  
We use <strong>()</strong> to construct pair, and we can also use it in the new lambda <strong>if </strong>.
If you have ever programmed in C, then u understand the following code  
```C
if(1){
    printf("do-if"); // this will be print out
}
else{
    printf("do-else"); // this won't
}
```
and
```C
if(0){
    printf("do-if:); // this won't be print out
}
else{
    printf("do-else"); // this will be print out
}
```
This <strong>if</strong> in Walley Language is quite similar.  
we use <strong>(if test do-if do-else)</strong>.  
In C we check whether <strong>test</strong> is <strong>0</strong> to decide the branch.  
In Walley Language, we check whether <strong>test</strong> is <strong>()</strong> to decide the branch.
For example  
```lisp
(if 1 (display "do-if") (display "do-else"))  ;; print do-if
(if () (display "do-if") (display "do-else")) ;; print do-else
```
That's it, very easy ;)  

Sometimes, we want to run more than one S expression in <strong>do-if</strong> and <strong>do-else</strong>.  
To do this, we need to use a new lambda called <strong>begin</strong>.
<strong>(begin S-exp1 S-exp2 ...)</strong>  
For Instance
```lisp
(if 1
    (begin (display "do-if1")
           (display "do-if2"))  ;; will print do-if1do-if2
    (display "do-else")) ;; wont print out
```
-----------------------------------------
### The better use of If
From the last topic, we learnt how to use <strong>if</strong>.  

we put value to <strong>test</strong> and then decide whether we should run  
<strong>do-if</strong> or <strong>do-else</strong>.  
For the most times, we don't want to pass <strong>()</strong> or any other value directly to <strong>test</strong>  
For the most times, we want to pass the result of comparision to <strong>test</strong>.

Now, I will introduce a new lambda called <strong>eq?</strong>.  
this lambda will compare two objects, if they are equal, <string>string true</strong> will be returned, otherwise () will be returned.
we can also use this to compare two numbers.

Here are some examples:
```lisp
(eq? 3 3)      ;; true
(eq? 3 4)      ;; ()
(eq? 'a 'a)    ;; true
(eq? 'a 'c)    ;; ()
(eq? () ())    ;; true
(def x '(1 2))
(def y x)
(eq? x y)      ;; true.  x and y point to the same object, same memory locations.
(def z '(1 2))
(eq? x z)      ;; (). although x and z are both (1 2), the are actually two different objects. those two objects are at two different memory locations
```

so now we could define a lambda like:
```lisp
(def null?
    (lambda [n]
        (if (eq? n ())
            'true
            ())))
```
to check whether a value is <strong>()</strong>.
```lisp
(null? ())        ;; true
(null? 2)         ;; ()
(null? (eq? 2 2)) ;; ()
(null? (eq? 2 3)) ;; true
```

Walley Language also offeres many other lambdas for comparision use.  
<strong> = > < >= <= </strong> can be used to compare numbers.  
<strong>string=? string>? string<? string>=? string<=? </strong> can be used to compare strings.  

Attention: error might occur if u passed wrong type params.  

------------------------------------------
### Cond, another kind of If
Now we have learnt how to use <strong>if</strong> in walley language.  
In fact, here is another "lambda" that works in a similar way.  
It is called as <strong> cond </strong>.
```lisp
;; cond is in format of
(cond test1 do-cond1
      test2 do-cond2
      ...
      else  do-else)
```
So let's try to write a lambda that gets the absolute value of a number.  
```lisp
(def abs0 (lambda [n]
    (cond (= n 0) 0          ;; n equals 0
          (> n 0) n          ;; n is positive
          (< n 0) (- 0 n)))) ;; n is negative

;; or a much simpler way
(def abs1 (lambda [n]
    (cond (> n 0) n            ;; n is positive
          else    (- 0 n))))   ;; else: n is not positive
                               ;; if none of the exprs above else are run
                               ;; else will be evaluated.
;; or just use if
(def abs2 (lambda [n]
    (if (> n 0)
        n
        (- 0 n))))
```
That's all for <strong>cond ;)</strong>.  

------------------------------------------
### Eval Lisp in Lisp
So far, we have learnt several lambdas
- car
- cdr
- cond
- quote
- lambda
- eq?
- cons
- string?       which can help check whether the parameter is string
and so forth.  

Now I will teach u how to build a very simple lisp interpreter in walley language.  

<strong> To be continued </strong>

------------------------------------------
License
----
MIT

**Free Software, Hell Yeah!**

------------------------------------------
Thanks
----
- [sicp]         - Don't ask, just go read it.
- [dillinger]    - A very good markdown file editor.  
- [root-of-lisp] - Good article.
- ...

------------------------------------------
Attention
----
Some terms that I used in this tutorial are actually wrong, I use them just to offer a better explanation for the tutorial.(eg, "if" is actually not lambda)


[dillinger]:http://dillinger.io
[sicp]:http://mitpress.mit.edu/sicp/full-text/book/book.html
[root-of-lisp]:http://www.paulgraham.com/rootsoflisp.html

    
