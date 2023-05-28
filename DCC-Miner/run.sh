#!/bin/bash

cd ./out/;
MSBuild.exe ./DCC-Miner.sln;
cd DCC-Miner/Debug;
./DCC-Miner.exe;
