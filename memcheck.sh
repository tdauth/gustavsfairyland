#!/bin/bash
valgrind --tool=memcheck --track-origins=yes --leak-check=full --show-leak-kinds=all ./build/gustavsfairyland &> memcheck.txt