#!/usr/bin/env bash
source chapel-1.18.0/util/setchplenv.bash
cd RoutingTable
chmod 755 run.sh
./run.sh *.in
