#!/bin/bash

cd "$(dirname -- "$(readlink -f "${BASH_SOURCE}")")"/out/;
MSBuild.exe ./DCC-Miner.sln;
#MSBuild.exe ./DCC-Miner.sln -property:Configuration=Release;

cd DCC-Miner/Debug;
./DCC-Miner.exe;
