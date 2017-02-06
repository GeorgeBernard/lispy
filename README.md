# Lispy

"Author": George Bernard, ghb5

 Author : Daniel Holden

## Overview

The Lisp interpreter is my implementation of an amazing tutorial by Daniel 
Holden on building a lisp interpreter in C. It can be found at 
http://www.buildyourownlisp.com/ and I would happily recommend anyone to do this
nifty and approachable project.

The purpose was to not only teach myself to write better C but also to learn new
concepts in lisp (and hopefully transition to reading the glorious tome that is
the "Structure and Interpretation of Computer Programs").

## Compiling

Compiling lispy should be as simple as running the makefile, either in a batch
script (if you are on windows) or a the shell script (if on linux/mac). It should
be portable, but no promises.

## Syntax

## Additions

The first addition I made was to seperate up the behemoth that was this program.
In my opinion any file over 500 lines should be deeply questioned. I seperated it
first into 