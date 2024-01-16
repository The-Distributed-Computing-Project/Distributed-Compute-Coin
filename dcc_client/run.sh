#!/bin/bash

cd "$(dirname -- "$(readlink -f "${BASH_SOURCE}")")"/out/;
MSBuild.exe ./dcc_client.sln;
#MSBuild.exe ./dcc_client.sln -property:Configuration=Release;

cd dcc_client/Debug;
./dcc_client.exe;
