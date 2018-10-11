#!/usr/bin/env bash
git submodule update --remote
source chapel-1.18.0/util/setchplenv.bash
cd RoutingTable
chmod 755 run.sh
./run.sh graph50.in #*.in
