# set terminal postscript eps color size 6cm,6cm 10
 set terminal png
 set pm3d corners2color c1 map 
# set palette model HSV defined ( 0 0 1 1, 1 1 1 1 )
set palette model RGB defined ( 0 "black",0.25 "blue",0.5 "green", 0.75 "yellow", 1 "red" )
#set palette functions sqrt(gray), gray**3, sin(gray*2*pi)
# set palette grey
# set xrange [-4:0]
# set yrange [-4:4]
# set zrange [0:2.5E+08]
# set zrange [0:3.0E+010]
 set autoscale x
 set autoscale y
 set size square
# set size ratio 2
