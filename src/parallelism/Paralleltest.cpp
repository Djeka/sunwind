#include <iostream>
#include <mpi.h>
using namespace std;

int main(int argc,char** argv) 
{ 
  int commsize;
  int commrank;
  MPI_Init(&argc,&argv);
  cout << "MPI initialized.";
  MPI_Comm_size(MPI_COMM_WORLD,&commsize);
  cout << "Communicator size:" << commsize << endl;
  MPI_Comm_rank(MPI_COMM_WORLD,&commrank);
  cout << "Communicator rank:" << commrank << endl;
  
  MPI::Finalize(); 
};
