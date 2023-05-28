#!/bin/bash



#echo "$(dirname -- "$(readlink -f "${BASH_SOURCE}")")"



cd "$(dirname -- "$(readlink -f "${BASH_SOURCE}")")"/out/;
MSBuild.exe ./DCC-Miner.sln;
cd DCC-Miner/Debug;
./DCC-Miner.exe;
