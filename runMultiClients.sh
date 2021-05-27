#!/bin/bash

# This script opens 4 terminal windows.

i="0"

while [ $i -lt 4 ]
do
xterm -e ./agency 127.0.0.1 16000 automatic &
i=$[$i+1]
done