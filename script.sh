#!/bin/bash

# Script to compile and link client and server programs

# Client compilation
gcc Cl.c -o Client -Wall -Wextra -pedantic -std=c99
if [ $? -eq 0 ]; then
    echo "Client compilation successful"
else
    echo "Client compilation failed"
    exit 1
fi

# Server compilation
gcc Ser.c -o Server -Wall -Wextra -pedantic -std=c99 -pthread
if [ $? -eq 0 ]; then
    echo "Server compilation successful"
else
    echo "Server compilation failed"
    exit 1
fi

echo "Compilation completed successfully"

# Clean up object files
rm -f *.o
