# common
Common/helper classes, used in other projects. 

## Overview

#### trees
Array implementations of various trees.

#### functional
Helpers that facilitate functional programming in *C++*.

#### io
I/O helper routines.

#### numeric
Numeric helpers.

#### patterns
Generic adapters that implement design patterns on provided classes.

#### stack & stackcontainer
A *std::stack* interface that exposes the underlying container for custom operations.
*stackcontainer* is an implementation of such a container in a *std::vector* for random-access.

#### thread
Concurrency helpers, such as an implementation of *std::experimental::barrier* & *std::experimental::flex_barrier*.

#### tools
Miscellaneous other routines.

## Dependencies & Installation

[*cmake*](https://github.com/andleb/cmake) repository for the *CMake* scripts; included as a submodule.

*CMake* installation currently configured for Unix, installs under */usr/local/include* & */usr/local/lib*.
The *install.sh* script performs the compilation & installation for every build configuration defined by *CMake* in the *build/* directory - useful for installing after making changes to the source.

