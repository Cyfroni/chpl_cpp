#!/usr/bin/env bash
git submodule update --remote
source chapel-1.18.0/util/setchplenv.bash

cd RoutingTable
chmod 755 run.sh
./run.sh
cd ..

cd DNN
chmod 755 run.sh
./run.sh
cd ..

cd CenPowell
chmod 755 run.sh
./run.sh
cd ..