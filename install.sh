#!/bin/bash

echo "building & installing..."

for f in `ls build`;
	do echo $f; 
	(cd build/$f && make install); 
done

echo "\n\n\ngenerating documentation...\n\n\n"
rm -r docs/* && doxygen
