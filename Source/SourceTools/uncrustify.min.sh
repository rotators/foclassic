#!/bin/bash

if [ ! -f uncrustify.cfg ]; then
	echo "uncrustify.cfg not found"
	exit
fi

cp uncrustify.cfg __uncrustify__
./uncrustify.exe --update-config > __uncrustify_default__

diff --new-line-format="" --unchanged-line-format="" <(sort __uncrustify__ | sed -e 's! !!g') <(sort __uncrustify_default__ | sed -e 's! !!g')

rm -f __uncrustify__ __uncrustify_default__
