#!/bin/bash
gcc -std=c99 -g -Wall main.c mpc.c lisputils.c builtin.c lvalue.c lenviron.c -ledit -lm -o lispy;
