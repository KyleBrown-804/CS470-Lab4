#!/bin/bash

# This script opens 4 terminal windows.

i="0"
# compile hotel and agency executables
#g++ -o hotel utilities.cpp Server.cpp -pthread
#g++ -o agency utilities.cpp Client.cpp -pthread

while [ $i -lt 4 ]
do
bash -e ./agency 127.0.0.1 16000 automatic &
i=$[$i+1]
done