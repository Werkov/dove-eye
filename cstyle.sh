#!/bin/bash

find tools lib -name "*.h" -o -name "*.cc" |\
	xargs contrib/cpplint.py \
	--filter=-legal/copyright,-whitespace/comments,-whitespace/line_length,-readability/todo,-build/c++11 \
	--root=lib/include
