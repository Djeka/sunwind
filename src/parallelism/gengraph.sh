#!/bin/bash

#
#   
#

scotch_gmk_m3 $2 $3 $4 $1.grf -g$1.xyz
./scotch/scotch_gpart $5 $1.grf > $1
