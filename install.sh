#!/bin/bash

echo "building & installing..."

for f in `ls build`;
	do echo $f; 
	(cd build/$f && make install); 
done

echo "generating documentation..."
rm -r docs/* && doxygen
