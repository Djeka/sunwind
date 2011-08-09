#!/bin/bash

#
#   
#
echo "gengraph.sh called with arguments: 1:$1 2:$2 3:$3 4:$4 5:$5" > gengraph.log
scotch_gmk_m3 $2 $3 $4 $1.grf -g$1.xyz
./src/parallelism/scotch/scotch_gpart $5 $1.grf > $1
