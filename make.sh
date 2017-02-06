#!/bin/bash
gcc -std=c99 -g -Wall evaluation.c mpc.c -ledit -lm -o lispy;
