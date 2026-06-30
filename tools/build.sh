# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

#!/bin/bash

set -e
clear

echo "============================"
echo " Cleaning..."
echo "============================"

make clean

echo ""
echo "============================"
echo " Building..."
echo "============================"

make 

echo ""
echo "============================"
echo " Building ISO..."
echo "============================"

make iso

echo ""
echo "============================"
echo " Starting QEMU..."
echo "============================"

make run
