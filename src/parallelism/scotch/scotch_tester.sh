#!/bin/bash

#
#  Идея: создаем сетки и распределения различных размеров и считаем следующие параметры:
#  1) min и max количества внешних ребер, приходящихся на раздел
#  2) min и max количества узлов на раздел
#  3) время декомпозиции
#
maxp=100
maxj=100
#for j in `seq 1 $maxj`; do
#   if [ ! -f srcmesh-$j-$j-$j.msh ]; then
#      echo "Making mesh data file for j=$j"
#      scotch_mmk_m3 $j $j $j srcmesh-$j-$j-$j.msh -gsrcmesh-$j-$j-$j.xyz
#      scotch_gmk_msh srcmesh-$j-$j-$j.msh > srcmesh-$j-$j-$j.grf
#      scotch_amk_grf 
#   fi
#done

for p in `seq 1 $maxj`; do
   if [ ! -f srcproc-$p-$p-$p.grf ]; then
      echo "Making source graph data file for p=$p"
      scotch_gmk_m3 $p $p $p srcproc-$p-$p-$p.grf -gsrcmesh-$p-$p-$p.xyz
   fi
done

for p in `seq 2 $maxp`; do
  for j in `seq 2 $maxj`; do 
      echo $p: $j of $lmaxj
      ./scotch_gpart $j srcproc-$p-$p-$p.grf > srcmesh-$p-$j.map
      ./analisepair.pl $p $j  >> analiser.log 
  done
  echo "">> analiser.log
done
