#
#  simple parallelism implementation makefile
# 



test:: Paralleltest.cpp
	mpic++ -o Paralleltest Paralleltest.cpp
	mpirun -np 2 ./Paralleltest
