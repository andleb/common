#!/bin/bash
# cd build
for f in `ls build`;
	do echo $f; 
	(cd build/$f && make install); 
done
# cd ..