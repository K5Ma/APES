#!/bin/bash 
echo "User Information: $USER"
echo "Operating System Type/Brand: $OSTYPE"
echo "OS Distribution: $(lsb_release -is)"
echo "OS Version: $(lsb_release -rs)"
echo "Kernel Version: $(uname -sr)"
echo "Kernel gcc version build: $(gcc -dumpversion)"
echo "Kernel build time: $(uname -v)"
echo "System Architecture information: $(uname -m)"
echo "Information on File System Memory: $(df)"


