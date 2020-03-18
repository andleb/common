#!/bin/bash

# NOTE: this file is used for synchronizing the system-wide installed version of the library with the state 
# in the development directory. As such, it just builds & installs for all the toolkits that have been defined 
# by your cMake setup in build/.

echo "building & installing..."

for f in `ls build`;
	do echo $f; 
	(cd build/$f && make install); 
done

echo "\n\n\ngenerating documentation...\n\n\n"
rm -r docs/* && doxygen
