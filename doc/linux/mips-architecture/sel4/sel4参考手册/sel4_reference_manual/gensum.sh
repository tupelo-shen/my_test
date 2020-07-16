#!/bin/bash

find . -ipath './*/*.md' | \
xargs grep -H -e ^# | \
LC_ALL=C sort | \
gawk -se 'BEGIN { print "* [关于](README.md)\n* [原文重要信息](README1.md)" } \
	{match($0, /(^.*)(:#{1,} )(.*$)/,ary); { \
	if (ary[2] ~ /####/) printf "\t\t\t"; \
	else if (ary[2] ~ /###/) printf "\t\t"; \
	else if (ary[2] ~ /##/) printf "\t"; \
	print "* ["ary[3]"]("ary[1]")" }}' \
	> SUMMARY.md 

