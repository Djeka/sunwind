#include <iostream>
#include <fstream>
#include <mpi.h>
#include <stdlib.h>
#include <sys/stat.h>
using namespace std;

// SELF_SENDER_HACK - workaround for sending boards over mpi to myself
//                    untill method Glue is not written...

#define SELF_SENDER_HACK 1
#define DEBUG 1




class ParallelArray {
	public: HierarchyCellArray** Arrays;
	protected: HierarchyCellArray** SBoards;
        protected: HierarchyCellArray** RBoards;
        protected: char** RBuffers; ///
        protected: char** SBuffers; /// WARNING!
                                    /// pointers to buffers are handled by 
                                    /// malloc/realloc/free !!!
        protected: int* RBuffersize;
        protected: int* SBuffersize;
        protected: int* RBufferusesize;
        protected: int* SBufferusesize;
        
	protected: TaskData* td;
	public: TaskData** mytds;
	public: bool update_buffers;
        protected: double dt;
	protected: int dim[3];
	protected: int myrank;
	protected: int commsize;
	protected: int nmyboards;
        protected: int *ranks;
        protected: int *cellnums;
        protected: int **mpicomms;
        protected: int nranks; 
        public: int nmycells;
        protected: int *mycellxyz;
        protected: double time;
//	protected: Iterator* iterator;

	public: ParallelArray(){}
        public: static const signed int MODE_FLUX=0;
        public: static const signed int MODE_VALUES=1;

#define SEND_DOUBLES_VALUES (2+/*density+heat energy*/9/*momentum+field*/)
#define SEND_DOUBLES_FLUX (4*3/* 2 axes 2 sides 3 components of field*/)
        private: int mode_sizes[2];
        private: static const signed int  MAX_MODE_SIZE=SEND_DOUBLES_FLUX;

        private: void createBoardSendBuffer(HierarchyCellArray ca,int axe,int dir,int buffer_number) {
/*
 *      creates "Send" buffer from cellarray ca
 *      NOT packing data into the buffer.
 */            

//3+/* r */+3  /*h*/ )
               int bufferposition=0;
               int step=0;
               int step1=0;
               char** bptr=SBuffers+buffer_number;
               if(*bptr != NULL) { free(*bptr); };
               for(BorderCellsIterator it = BorderCellsIterator((CellArray*)(&ca), axe, dir, true); it.hasNext();){
                  HierarchyCell* c = (HierarchyCell*) it.next();
                  if(c->isSplitted()) {
                       MPI_Pack_size(1,MPI_CHAR,MPI_COMM_WORLD,&step);
                       step1=0;
                  } else {
                       MPI_Pack_size(1,MPI_CHAR,MPI_COMM_WORLD,&step);
                       int psize=0;
     
                        MPI_Pack_size(MAX_MODE_SIZE
                                     , MPI_DOUBLE,MPI_COMM_WORLD,&step1);
                             
                  };
                  if(bufferposition==0) {
                      *bptr=(char*)malloc(step+step1); 
                  } else {
                       *bptr=(char*)realloc(*bptr,bufferposition+step+step1);
                  };                          
                  bufferposition+=(step+step1);
               };
               SBuffersize[buffer_number]=bufferposition;
               SBufferusesize[buffer_number]=bufferposition;
        };

        private: void packSendBuffer(HierarchyCellArray* ca,int axe,int dir, int buffer_number,int mode) {
/*
 *       Only packs data into Send buffer. NOT allocating or reallocating.
 */ 
               int bufferposition=0;
               int step=0;
               double* data;
               char** bptr=SBuffers+buffer_number;
               char t='t';
               char f='f';
               int bufsize =  SBuffersize[buffer_number];
#ifdef DEBUG
            cout << "(i am "<< myrank <<") Packing buffer " << buffer_number << " in mode " << mode << " axe: " << axe << " dir: " << dir << "\n";
#endif
               data=new double[mode_sizes[mode]];
               for(BorderCellsIterator it = BorderCellsIterator((CellArray*)(ca), axe, dir, true); it.hasNext();){
                  HierarchyCell* c = (HierarchyCell*) it.next();
                  if(c->isSplitted()) {
                       MPI_Pack(&t,1,MPI_CHAR,*bptr,bufsize,&bufferposition,MPI_COMM_WORLD);
                  } else {
                       MPI_Pack(&f,1,MPI_CHAR,*bptr,bufsize,&bufferposition,MPI_COMM_WORLD);
                       switch (mode) {
                          case MODE_VALUES: {
                       data[0] = c->getDensity();
                       data[1] = c->getTermEnergy();
                       data[2] = c->getMomentum(0);
                       data[3] = c->getMomentum(1);
                       data[4] = c->getMomentum(2);
                       data[5] = c->getSide(0,Cell::FORWARD_SIDE)->getB(); 
                       data[6] = c->getSide(0,Cell::BACKWARD_SIDE)->getB(); 
                       data[7] = c->getSide(1,Cell::FORWARD_SIDE)->getB(); 
                       data[8] = c->getSide(1,Cell::BACKWARD_SIDE)->getB(); 
                       data[9] = c->getSide(2,Cell::FORWARD_SIDE)->getB(); 
                       data[10] = c->getSide(2,Cell::BACKWARD_SIDE)->getB();
                           break;
                           };
                          case MODE_FLUX: {
                            int axe1=(axe+1)%3;
                            int axe2=(axe+2)%3;
                            double* F;
                            F=c->getSide(axe1,Cell::FORWARD_SIDE)->getFlow();
                            data[0]=F[5]; 
                            data[1]=F[6]; 
                            data[2]=F[7];
                            F=c->getSide(axe1,Cell::BACKWARD_SIDE)->getFlow();
                            data[3]=F[5]; 
                            data[4]=F[6]; 
                            data[5]=F[7];
                            F=c->getSide(axe2,Cell::FORWARD_SIDE)->getFlow();
                            data[6]=F[5]; 
                            data[7]=F[6]; 
                            data[8]=F[7];
                            F=c->getSide(axe2,Cell::BACKWARD_SIDE)->getFlow();
                            data[9]=F[5]; 
                            data[10]=F[6]; 
                            data[11]=F[7];
                            break;
                          };
                          default: {
                             assert(false);
                          };
                         };
/*                       data[11] = c->getR(0);
                       data[12] = c->getR(1);
                       data[13] = c->getR(2);
                       data[14] = c->getH(0);
                       data[15] = c->getH(1);
                       data[16] = c->getH(2);*/
                       MPI_Pack(data, mode_sizes[mode]
                                     , MPI_DOUBLE, *bptr,bufsize,&bufferposition,MPI_COMM_WORLD);

                  };
               };
               SBufferusesize[buffer_number]=bufferposition;
               delete data;
        };
        private: void unpackRecvBuffer(HierarchyCellArray* ca, int axe,int dir,int buffer_number, int mode) {
/*
 *            Unpacks buffer, creates cells, if necessary.
 */
               int bufferposition=0;
               int step=0;
               double* data;
               char** bptr=RBuffers+buffer_number;
               int bufsize =  RBufferusesize[buffer_number];
               char t='t';
               char f='f';
               char flag;
              
#ifdef DEBUG
            cout << "(i am " << myrank << ") unpacking recieve buffer" << buffer_number << " axe: " << axe << " dir: " << dir << ".\n";
#endif 
              // First - find first border cell, create binded one, if necessary.
              BorderCellsIterator bit = BorderCellsIterator(ca, axe, dir, true); 
              HierarchyCell* c;
              assert(bit.hasNext());
              c=(HierarchyCell*) bit.next();
              HierarchyCell* bc=c->bindCell(axe,dir);

              for(BindedCellsIterator it = BindedCellsIterator((CellArray*)ca, axe, dir, true, true); it.hasNext();){
                  HierarchyCell* c = (HierarchyCell*) it.next();
                  MPI_Unpack(*bptr,bufsize,&bufferposition,&flag,1,MPI_CHAR,MPI_COMM_WORLD);
                  if(flag==t) {
                    if(!(c->isSplitted())) {
                      ca->splitCell(c);
                    };
                  } else {
                    if(c->isSplitted()) {
                    // TBD: join!
                    assert(false);
                    };
                    MPI_Unpack(*bptr,bufsize,&bufferposition,data,mode_sizes[mode],MPI_DOUBLE,MPI_COMM_WORLD);
                    switch (mode) {
                      case MODE_VALUES: {
                        c->setDensity(data[0]);
                        c->setTermEnergy(data[1]);
                        c->setMomentum(data[2],0);
                        c->setMomentum(data[3],1);
                        c->setMomentum(data[4],2);
                        c->getSide(0,Cell::FORWARD_SIDE)->setB(data[5]); 
                        c->getSide(0,Cell::BACKWARD_SIDE)->setB(data[6]); 
                        c->getSide(1,Cell::FORWARD_SIDE)->setB(data[7]); 
                        c->getSide(1,Cell::BACKWARD_SIDE)->setB(data[8]); 
                        c->getSide(2,Cell::FORWARD_SIDE)->setB(data[9]); 
                        c->getSide(2,Cell::BACKWARD_SIDE)->setB(data[10]);
                        break;
                      }; 
             
   /*                 c->setR(data[11]);
                       data[12] = c->setR(1);
                       data[13] = c->setR(2);
                       data[14] = c->setH(0);
                       data[15] = c->setH(1);
                       data[16] = c->setH(2);*/
                     case MODE_FLUX: {
                            int axe1=(axe+1)%3;
                            int axe2=(axe+2)%3;
                            double* F;
                            F=c->getSide(axe1,Cell::FORWARD_SIDE)->getFlow();
                            F[5]=data[0]; 
                            F[6]=data[1]; 
                            F[7]=data[2];
                            F=c->getSide(axe1,Cell::BACKWARD_SIDE)->getFlow();
                            F[5]=data[3]; 
                            F[6]=data[4]; 
                            F[7]=data[5];
                            F=c->getSide(axe2,Cell::FORWARD_SIDE)->getFlow();
                            F[5]=data[6]; 
                            F[6]=data[7]; 
                            F[7]=data[8];
                            F=c->getSide(axe2,Cell::BACKWARD_SIDE)->getFlow();
                            F[5]=data[9]; 
                            F[6]=data[10]; 
                            F[7]=data[11];
                            break;
                      
                     };
                     default: { 
                        assert(false);
                     };
                  };
              };
            };
        };

        private: void updateSendBuffers() {
           //// Now we have to link all Arrays, having common board.
                int brdi=0;
                for(int i=0;i<nmycells;i++) {
                   int* X;
                   int Y[3];
                   X=mycellxyz + 3*i;
                   Y[0]=X[0];
                   Y[1]=X[1];
                   Y[2]=X[2];
                   for(int axe=0;axe<3;axe++) {                     
                     if(X[axe]>0) {
                       // Checking left
#ifndef SELF_SENDER_HACK
                       Y[axe]=X[axe]-1;   
                       if(*getRankPtr(Y) != myrank) {
///// Create sending buffer
//                         SBoards[brdi]=Arrays[i].CreateBoard(axe,-1);
#endif
                         createBoardSendBuffer(*(Arrays[i]),axe,Cell::BACKWARD_SIDE,brdi);
                         brdi++;
#ifndef SELF_SENDER_HACK
                       } else {
///// Glue two Arrays together!!!
                       for(j=0;j<nmycells&&(!(
                                 (mycellxyz[3*j]==Y[0])&&
                                 (mycellxyz[3*j+1]==Y[1])&&
                                 (mycellxyz[3*j+2]==Y[2])
                                             ));j++);
                       Glue(Arrays[i],Arrays[j],axe,Cell::BACKWARD_SIDE);
                       };
                       Y[axe]=X[axe];
#endif
                     };
                     if(X[axe]+1<dim[axe]) {
#ifndef SELF_SENDER_HACK
                       Y[axe]=X[axe]+1;
                       if(*getRankPtr(Y) != myrank) {
#endif
                           createBoardSendBuffer(*(Arrays[i]),axe,Cell::FORWARD_SIDE,brdi);
                           brdi++;
#ifndef SELF_SENDER_HACK
                       } else {
                       int j;
                       for(j=0;j<nmycells&&(!(
                                 (mycellxyz[3*j]==Y[0])&&
                                 (mycellxyz[3*j+1]==Y[1])&&
                                 (mycellxyz[3*j+2]==Y[2])
                                             ));j++);
                       Glue(Arrays[i],Arrays[j],axe,Cell::FORWARD_SIDE);
                       };
                       Y[axe]=X[axe];
#endif
                     };
                   };


                };

        };

        private: int messagetag(int* X,int axe,int dir) {
//// provide a message tag for a board, identified by it's coords, axe and direction
          int intdir=((dir == Cell::FORWARD_SIDE)? 1:0);
          return X[0]+dim[0]*(X[1]+dim[1]*(X[2]+dim[2]*(axe+3*intdir)));
        };
        private: void syncTimeSteps() {
           double cdt=10000;
           double dt;
           for(int i=0;i<nmycells;i++) {
              dt=Arrays[i]->getTimeStep();
              if(dt<cdt) { cdt=dt; };
           };
           MPI_Allreduce(&cdt,&dt,1,MPI_DOUBLE,MPI_MIN,MPI_COMM_WORLD);
           for(int i=0;i<nmycells;i++) {
             Arrays[i]->setTimeStep(dt);
           }; 
        };

        private: void syncBuffers(int mode) {
          int brdi=0;
          MPI_Request* mySendRequest=(MPI_Request*)malloc(sizeof(MPI_Request)*nmyboards);
          MPI_Request* myRecvRequest=(MPI_Request*)malloc(sizeof(MPI_Request)*nmyboards);
          MPI_Status* myRecvStatus=(MPI_Status*)malloc(sizeof(MPI_Status)*nmyboards);
          MPI_Status* mySendStatus=(MPI_Status*)malloc(sizeof(MPI_Status)*nmyboards);
         
          for(int i=0;i<nmycells;i++) {
#ifdef DEBUG
            cout << "(i am "<< myrank <<") Packing buffers for " << i << " of " << nmycells << "\n";
#endif
            int* X = mycellxyz + 3*i;
            int Y[3];
            Y[0]=X[0];
            Y[1]=X[1];
            Y[2]=X[2];
        
            for(int axe=0;axe<3;axe++) {
                if(X[axe]>0) {
                     // Checking left
                     Y[axe]=X[axe]-1;   
                     int sendto=*getRankPtr(Y);
#ifndef SELF_SENDER_HACK
                     if(sendto != myrank) {
#endif
                     packSendBuffer(*(Arrays+i),axe,Cell::BACKWARD_SIDE,brdi,mode);
                     MPI_Isend(SBuffers[brdi],SBuffersize[brdi],MPI_PACKED,sendto,messagetag(X,axe,Cell::BACKWARD_SIDE),MPI_COMM_WORLD,mySendRequest+brdi);
                     MPI_Irecv(RBuffers[brdi],RBuffersize[brdi],MPI_PACKED,sendto,messagetag(Y,axe,Cell::FORWARD_SIDE),MPI_COMM_WORLD,myRecvRequest+brdi);

                     brdi++;
                     Y[axe]=X[axe];
#ifndef SELF_SENDER_HACK
                     };
#endif
                };
                if(X[axe]+1<dim[axe]) {
                       Y[axe]=X[axe]+1;
                       int sendto=*getRankPtr(Y);
#ifndef SELF_SENDER_HACK
                       if(sendto != myrank) {
#endif
                           packSendBuffer(*(Arrays+i),axe,Cell::FORWARD_SIDE,brdi,mode);
                           MPI_Isend(SBuffers[brdi],SBuffersize[brdi],MPI_PACKED,sendto,messagetag(X,axe,Cell::FORWARD_SIDE),MPI_COMM_WORLD,mySendRequest+brdi);
                           MPI_Irecv(RBuffers[brdi],RBuffersize[brdi],MPI_PACKED,sendto,messagetag(Y,axe,Cell::BACKWARD_SIDE),MPI_COMM_WORLD,myRecvRequest+brdi);
                           brdi++;
#ifndef SELF_SENDER_HACK
                       };
#endif
                       Y[axe]=X[axe];
                       }; 
                 }; 
            };  
/***********  Here we can get better performance by doing MPI_Waitany + some intelegent cicle. **************/
#ifdef DEBUG
            cout << "(i am " << myrank << ") Waiting for recv requests:\n";
#endif 
            MPI_Waitall(nmyboards,myRecvRequest,myRecvStatus);
#ifdef DEBUG
            cout << "(i am " << myrank << ") Waiting for send requests:\n";
#endif 
            MPI_Waitall(nmyboards,mySendRequest,mySendStatus);
#ifdef DEBUG
            cout << "(i am " << myrank << ") requests completed.\n";
#endif 
            free(myRecvRequest);
            free(mySendRequest);
            free(myRecvStatus);
            free(mySendStatus);
//////////////////////////////////////////////////////////////
//  //  //  Ok, now let's unpack them...
            for(int i=0;i<nmycells;i++) {
            int* X = mycellxyz + 3*i;
            int Y[3];
            Y[0]=X[0];
            Y[1]=X[1];
            Y[2]=X[2];
        
            for(int axe=0;axe<3;axe++) {
                if(X[axe]>0) {
                     // Checking left
                     Y[axe]=X[axe]-1;   
#ifndef SELF_SENDER_HACK
                     if(*getRankPtr(Y) != myrank) {
#endif
                     unpackRecvBuffer(*(Arrays+i),axe,Cell::FORWARD_SIDE,brdi,mode);
                     brdi++;
                     Y[axe]=X[axe];
#ifndef SELF_SENDER_HACK
                     };
#endif
                };
                if(X[axe]+1<dim[axe]) {
                       Y[axe]=X[axe]+1;
#ifndef SELF_SENDER_HACK
                       if(*getRankPtr(Y) != myrank) {
#endif
                           unpackRecvBuffer(Arrays[i],axe,Cell::BACKWARD_SIDE,brdi,mode);
                           brdi++;
#ifndef SELF_SENDER_HACK
                       };
#endif
                       Y[axe]=X[axe];
                       }; 
                 }; 
            };  
        

        };
        
        private: void syncBufferSizes() {
          int brdi=0;
          MPI_Request* mySendRequest=(MPI_Request*)malloc(sizeof(MPI_Request)*nmyboards);
          MPI_Request* myRecvRequest=(MPI_Request*)malloc(sizeof(MPI_Request)*nmyboards);
          MPI_Status* myRecvStatus=(MPI_Status*)malloc(sizeof(MPI_Status)*nmyboards);
          MPI_Status* mySendStatus=(MPI_Status*)malloc(sizeof(MPI_Status)*nmyboards);
#ifdef DEBUG
            cout << "(i am " << myrank << ") syncBufferSizes:\n";
#endif 
          for(int i=0;i<nmycells;i++) {
            int* X = mycellxyz + 3*i;
            int Y[3];
            Y[0]=X[0];
            Y[1]=X[1];
            Y[2]=X[2];
        
            for(int axe=0;axe<3;axe++) {
                if(X[axe]>0) {
                     // Checking left
                     Y[axe]=X[axe]-1;   
                     int sendto=*getRankPtr(Y);
#ifndef SELF_SENDER_HACK
                     if(sendto != myrank) {
#endif

#ifdef DEBUG
                     cout << "(i am " << myrank << ") organising Isend for size of " << brdi << "board. " << "\n";
                    
#endif 
                     MPI_Isend(SBuffersize+brdi,1,MPI_INTEGER,sendto,messagetag(X,axe,Cell::BACKWARD_SIDE),MPI_COMM_WORLD,mySendRequest+brdi);
                     MPI_Irecv(RBuffersize+brdi,1,MPI_INTEGER,sendto,messagetag(Y,axe,Cell::FORWARD_SIDE),MPI_COMM_WORLD,myRecvRequest+brdi);
                     brdi++;
                     Y[axe]=X[axe];
#ifndef SELF_SENDER_HACK
                     };
#endif
                };
                if(X[axe]+1<dim[axe]) {
                       Y[axe]=X[axe]+1;
                       int sendto=*getRankPtr(Y);
#ifndef SELF_SENDER_HACK
                       if(sendto != myrank) {
#endif
                           MPI_Isend(SBuffersize+brdi,1,MPI_INTEGER,sendto,messagetag(X,axe,Cell::FORWARD_SIDE),MPI_COMM_WORLD,mySendRequest+brdi);
                           MPI_Irecv(RBuffersize+brdi,1,MPI_INTEGER,sendto,messagetag(Y,axe,Cell::BACKWARD_SIDE),MPI_COMM_WORLD,myRecvRequest+brdi);
                           brdi++;
                           Y[axe]=X[axe];
#ifndef SELF_SENDER_HACK
                       };
#endif
                       }; 
                 }; 
            };  
            MPI_Waitall(nmyboards,myRecvRequest,myRecvStatus);
            MPI_Waitall(nmyboards,mySendRequest,mySendStatus);
            free(myRecvRequest);
            free(mySendRequest);
            free(myRecvStatus);
            free(mySendStatus);
//
//            Ok, now we now buffer sizes. Let's recreate them
//       
#ifdef DEBUG
            cout << "(i am "<< myrank <<") Buffer sizes redistributed.\n";
#endif
            for(int i=0;i<nmyboards;i++) {
                
               if(RBuffers[i]==NULL) {
                  RBufferusesize[i]=RBuffersize[i];
                  RBuffers[i]=(char*)malloc(RBuffersize[i]);
               } else {
                  if(RBufferusesize[i]>RBuffersize[i]) {
                     free(RBuffers[i]);
                     RBuffersize[i]=RBufferusesize[i];
                     RBuffers[i]=(char*)malloc(RBuffersize[i]);
                  };
               };
            }; 

        };

	public: ParallelArray(TaskData* td,int argc,char** argv){
		ParallelArray::td = td;
                
		Cell* cell;
		int ind = 0;
                ifstream paralleldata;     
                ifstream xyzdata;     
                ifstream grfdata;     
                char *filename;
                char *xyzfilename;
                char *grffilename;
                char *command;
		time = 0;
		dt =0;

// MPI INIT
                if(!(MPI_Init(&argc,&argv)==MPI_SUCCESS)) {
                   cout << "MPI_Init failed\n";
                   exit(1);
                };
                MPI_Comm_rank(MPI_COMM_WORLD, &myrank); 
                MPI_Comm_size(MPI_COMM_WORLD, &commsize); 
                mode_sizes[MODE_FLUX]=SEND_DOUBLES_FLUX;
                mode_sizes[MODE_VALUES]=SEND_DOUBLES_VALUES;
                filename=new char[100];
                xyzfilename=new char[100];
                grffilename=new char[100];
               
                dim[0]=td->getNP(0); 
                dim[1]=td->getNP(1); 
                dim[2]=td->getNP(2); 
                sprintf(filename, "graph-%d-%d-%d-%d.txt",dim[0],dim[1],dim[2],commsize);
                sprintf(xyzfilename, "graph-%d-%d-%d-%d.txt.xyz",dim[0],dim[1],dim[2],commsize);
                sprintf(grffilename, "graph-%d-%d-%d-%d.txt.grf",dim[0],dim[1],dim[2],commsize);
                if(myrank == 0) {
//
//              I am root node. I have to check wheter I can find appropriate 
//              parallel-related graph info and try to create one if possible. 
//              Others will wait.
//                 
                   paralleldata.open(filename,ios::in );
                   if(paralleldata.is_open()) {
                     cout << "Paralleldata is already generated. No need to do it again\n";
                   } else {
//              We have to generate appropriate file
//
                       command=new char[200];
                       sprintf(command,"./gengraph.sh %s %d %d %d %d",
                                                      filename,
                                                         dim[0],
                                                            dim[1],
                                                               dim[2],
                                                                  commsize);
                       int state=system(command);
                       if(state!=0) {
                          cout << "Return value of " << command << "not zero. Aborting.";
                          MPI_Abort(MPI_COMM_WORLD,2);
                       };
                       paralleldata.open(filename,ios::in );
                       if(!paralleldata.is_open()) {
                          cout << "Problems generating paralleldata";
                          MPI_Abort(MPI_COMM_WORLD,2);
                       };
                   };
//              Here file exists and is open on master.
//              Wait for others.
                  MPI_Barrier(MPI_COMM_WORLD);
                } else {
//                
//              I am not root node. Wait for master, than open file
                  MPI_Barrier(MPI_COMM_WORLD);
                  paralleldata.open(filename,ios::in );
                  if(!paralleldata.is_open()) {
                      cout << "I am " << myrank << " can't open parallel data file: " << filename << " Aborting all";
                      MPI_Abort(MPI_COMM_WORLD,2);
                  };
                  
                };
//----------------------------------- File created and is opened everywhere.                
//              Script gengraph has a SIDE EFFECT: it generates two additional files: filename.xyz and filename.grf
//              We are opening them now.
                xyzdata.open(xyzfilename, ios::in);
                if(!xyzdata.is_open()) {
                      cout << "I am " << myrank << " can't open parallel XYZ data file: " << xyzfilename << " Aborting all";
                      MPI_Abort(MPI_COMM_WORLD,3);
                };
                grfdata.open(grffilename, ios::in);
                if(!grfdata.is_open()) {
                      cout << "I am " << myrank << " can't open parallel XYZ data file: " << grffilename << " Aborting all";
                      MPI_Abort(MPI_COMM_WORLD,3);
                };
//----------------  OK, files opened. 
//                     XYZ file contains coord triplets for each cell
//                     grf file contains links between cells
//                     .txt file contains ranks of hosting systems for each cell
                
                cout << "FILES OPENED\n";              
		nranks = dim[0]*dim[1]*dim[2];
		ranks = new int[nranks];
		cellnums = new int[nranks];
//                allocateComm();
//----------------  Ok, now reading files.
//                  Consult scotch manual for file format
                int xyzv;
                xyzdata >> xyzv;
                assert(xyzv==3);
                int xyzsize;
                xyzdata >> xyzsize;
                assert(xyzsize==nranks);
                int grsize;
                paralleldata >> grsize;
                assert(grsize==nranks);
                int grfv;
                grfdata >> grfv;
                assert(grfv==0);
                int grfsize;
                grfdata >> grfsize;
                assert(grfsize==nranks);
                int links;
                grfdata >> links;
                grfdata >> links;
                grfdata >> links;
//----------------  Headers parsed. Parsing bodies.
                nmycells=0;
                mycellxyz=new int[3*nranks]; // Overestimate. 
                for(int i=0; i<nranks; i++) {
                    int n;
                    int rank;
                    paralleldata >> n;
                    assert(i==n);
                    paralleldata >> rank;
                    assert(rank>0);
                    assert(rank<commsize);
                    int x,y,z;
                    xyzdata >> n;
                    assert(i==n);
                    xyzdata >> x;
                    xyzdata >> y;
                    xyzdata >> z;
                    int *ptr;
                    ptr=getCellNumPtr(x,y,z);
                    *ptr=i;
                    ptr=getRankPtr(x,y,z);
                    *ptr=rank;
                    if(rank==myrank) {
//
//           This cell is MINE!
//                    
                      mycellxyz[3*nmycells]=x;
                      mycellxyz[3*nmycells+1]=y;
                      mycellxyz[3*nmycells+2]=z;
                      nmycells++; 
                    };
                };


                cout << "FILES GOT\n";

//----------------  Ok, now data on who is where is obtained.
//                  for now, we actually ignoreconnection data from grf file
//              Each node is initialising it's cells now
                mytds=new TaskData*[nmycells];
                Arrays=new HierarchyCellArray*[nmycells];
                nmyboards=0;
                for(int i=0;i<nmycells;i++) {
                   int* X;
                   int Y[3];
                   X=mycellxyz + 3*i;
                   mytds[i]=new TaskData(td,X,dim);
/// Suppose new constructor for TaskData WILL BE WRITTEN.
                   Arrays[i]=new HierarchyCellArray(mytds[i]);
                   Y[0]=X[0];
                   Y[1]=X[1];
                   Y[2]=X[2];
                   for(int axe=0;axe<3;axe++) {                     
                     if(X[axe]>0) {
                       // Checking right
                       Y[axe]=X[axe]-1;   
#ifndef SELF_SENDER_HACK 
                       if(*getRankPtr(Y) != myrank) {
                         nmyboards++;
                       };
#else
                       nmyboards++;
#endif
                       Y[axe]=X[axe];
                     };
                     if(X[axe]+1<dim[axe]) {
                       Y[axe]=X[axe]+1;
#ifndef SELF_SENDER_HACK 
                       if(*getRankPtr(Y) != myrank) {
                         nmyboards++;
                       };
#else
                       nmyboards++;
#endif
                       Y[axe]=X[axe];
                     };
                   };
                };
//                SBoards=new HierarchyCellArray*[nmyboards];
//                RBoards=new CellArray*[nmyboards];
                RBuffers=new char*[nmyboards];
                SBuffers=new char*[nmyboards];
                
                RBuffersize=new int[nmyboards];
                SBuffersize=new int[nmyboards];
                RBufferusesize=new int[nmyboards];
                SBufferusesize=new int[nmyboards];
                for(int i=0;i<nmyboards;i++) {
                   RBuffersize[i]=0;
                   SBuffersize[i]=0;
                   RBufferusesize[i]=0;
                   SBufferusesize[i]=0;
                   RBuffers[i]=NULL;
                   SBuffers[i]=NULL;
                };
                cout << "updating send buffers:\n";
                updateSendBuffers(); 
                cout << "done. syncing buffers:\n";
                syncBufferSizes();
                cout << "done.\n";
                update_buffers=false;
                
        }
        protected: int* getCellNumPtr(int x,int y,int z) {
                assert(x>=0);
                assert(x<dim[0]);
                assert(y>=0);
                assert(y<dim[1]);
                assert(z>=0);
                assert(z<dim[2]);
                return cellnums+z*dim[1]*dim[0]
                            +y*dim[0]
                            +x;

        }
        protected: int* getRankPtr(int X[3]) {
//
//              Computing pointer to rank of cell at position 
//              given by it's coords.
//
                assert(X[0]>=0);
                assert(X[0]<dim[0]);
                assert(X[1]>=0);
                assert(X[1]<dim[1]);
                assert(X[2]>=0);
                assert(X[2]<dim[2]);
                return ranks+X[2]*dim[1]*dim[0]
                            +X[1]*dim[0]
                            +X[0];
        }


        protected: int* getRankPtr(int x,int y,int z) {
//
//              Computing pointer to rank of cell at position 
//              given by it's coords.
//
                assert(x>=0);
                assert(x<dim[0]);
                assert(y>=0);
                assert(y<dim[1]);
                assert(z>=0);
                assert(z<dim[2]);
                return ranks+z*dim[1]*dim[0]
                            +y*dim[0]
                            +x;
        }


        public: void prepareStep(int mode) {
/*
 *           
 */ 
          if(mode==MODE_VALUES) {
            syncTimeSteps();
          };
//        First: exchange sizes of boards.
          if(update_buffers) {
             updateSendBuffers();
             syncBufferSizes();
             update_buffers=false;
          };
//        Next: exchange data          
          syncBuffers(mode); 
          
        };


//
//    Not sure the idea of one communicator per board is a good idea.
//    Looks like the problem can be solved by just knowing who is where.
//    But not removing corresponding functions right now.
//
        protected: void allocateComm() {
          mpicomms=new int*[3];
          mpicomms[0]=new int[(dim[0]-1)*dim[1]*dim[2]];
          mpicomms[1]=new int[(dim[1]-1)*dim[2]*dim[0]];
          mpicomms[2]=new int[(dim[2]-1)*dim[0]*dim[1]];
        }
        protected: int* getCommPtr(int x,int y,int z,int axe,int dir) {
//
//              Computing pointer to MPI_COMM, representing link between
//              point and it's neighbour. dir=1 for positive direction
//                                        dir=-1 for negative direction
// 
//              axe=0,1,2 == x,y,z
//
            int X[3];
            X[0]=x;
            X[1]=y;
            X[2]=z;
            return getCommPtr(X,axe,dir);
        };

        protected: int* getCommPtr(int* X,int axe,int dir) {
            int cdim[3];
            int shifts[3];
            assert(axe>=0);
            assert(axe<3);
            assert(X[0]>=0);
            assert(X[1]>=0);
            assert(X[2]>=0);
            assert(X[0]<dim[0]);
            assert(X[1]<dim[1]);
            assert(X[2]<dim[2]);
            assert((dir==1)||(dir==-1));
            cdim[0]=dim[0];
            cdim[1]=dim[1];
            cdim[2]=dim[2];
            cdim[axe]--;
            if(-1==dir) {
            shifts[2]=cdim[1]*cdim[0];
            shifts[1]=cdim[0];
            shifts[0]=1;
              assert(X[axe]>1);
              return mpicomms[axe]+X[2]*shifts[2]
                                  +X[1]*shifts[1]
                                  +X[0]-shifts[axe];
            } else {
              assert(X[axe]<cdim[axe]);
              return mpicomms[axe]+X[2]*cdim[1]*cdim[0]
                                  +X[1]*cdim[0]
                                  +X[0];
              
            }
        }
        protected: int getMyCellNum(int* X) {
// SLOW IMPLEMENTATION - NOT TO BE USED IN CYCLES!!!!
            for(int i=0;i<nmycells;i++) {
              if((mycellxyz[3*i]  ==X[0])&&
                 (mycellxyz[3*i+1]==X[1])&&
                 (mycellxyz[3*i+2]==X[2])) {
                   return i;
              };
            };
            throw "Bad usage of getMyCellNum - cell supplied is not mine.";
        };

	public: void calculateIncrements(){
               
/*
 *          First we have to prepare and organise ASYNCRONOUS data transfer of 
 *          information about sizes of our boards.
 *
 *          To do it, we FIRST pack each of OUR BOARD and start ASYNC send of it's size to corresponding 
 *          peer.
 */
            for(int i=0;i<nmycells;i++) {
              printf("---- calculate increments %d - not implemented --- TBD!\n",i);
            };
            exit(1);
 
        }

};
