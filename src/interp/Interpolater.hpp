#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>

class Interpolater{

	public: static void interpolateInsideGDparameters(HierarchyCell* cell){
/*
		interpolateHDonInsideSubCellPair(cell, method, 0, 1, 2, Cell::FORWARD_SIDE, Cell::FORWARD_SIDE);
		interpolateHDonInsideSubCellPair(cell, method, 0, 1, 2, Cell::BACKWARD_SIDE, Cell::FORWARD_SIDE);
		interpolateHDonInsideSubCellPair(cell, method, 0, 1, 2, Cell::FORWARD_SIDE, Cell::BACKWARD_SIDE);
		interpolateHDonInsideSubCellPair(cell, method, 0, 1, 2, Cell::BACKWARD_SIDE, Cell::BACKWARD_SIDE);
*/

		double* h = cell->getH();
		double* U0 = getU(cell);
		U0[4] = U0[4] - (U0[1]*U0[1]+U0[2]*U0[2]+U0[3]*U0[3])/2.0/U0[0] - (U0[5]*U0[5]+U0[6]*U0[6]+U0[7]*U0[7])/8.0/Constants::Pi;
		double** gradU = new double*[5];
		for(int i=0; i<5; i++) gradU[i] = new double[3];

		for(int axis = 0; axis < 3; axis++){
			double* Uf = getU(cell->getCell(axis, Cell::FORWARD_SIDE));
			double* Ub = getU(cell->getCell(axis, Cell::BACKWARD_SIDE));

			Uf[4] = Uf[4] - (Uf[1]*Uf[1]+Uf[2]*Uf[2]+Uf[3]*Uf[3])/2.0/Uf[0] - (Uf[5]*Uf[5]+Uf[6]*Uf[6]+Uf[7]*Uf[7])/8.0/Constants::Pi;
			Ub[4] = Ub[4] - (Ub[1]*Ub[1]+Ub[2]*Ub[2]+Ub[3]*Ub[3])/2.0/Ub[0] - (Ub[5]*Ub[5]+Ub[6]*Ub[6]+Ub[7]*Ub[7])/8.0/Constants::Pi;

			double h0 = cell->getH(axis);
			double hf = cell->getCell(axis, Cell::FORWARD_SIDE)->getH(axis);
			double hb = cell->getCell(axis, Cell::BACKWARD_SIDE)->getH(axis);

			for(int i=0; i<5; i++){
				gradU[i][axis] = Utils::minmod((Uf[i] - U0[i])/(h0/2.0 + hf/2.0), (U0[i] - Ub[i])/(h0/2.0 + hf/2.0));
			}

			delete[] Uf;
			delete[] Ub;
		}

		for(int i=0; i<8; i++){
			HierarchyCell* subCell = (HierarchyCell*) cell->getSubCell(i);
			int* p = subCell->getPoint();
			subCell->setDensity(U0[0] 
					+ (2 * p[0] - 1) * gradU[0][0] * h[0] / 4.0
					+ (2 * p[1] - 1) * gradU[0][1] * h[1] / 4.0
					+ (2 * p[2] - 1) * gradU[0][2] * h[2] / 4.0);
			for(int axis=0; axis<3; axis++){
				subCell->setMomentum(U0[1+axis]
					+ (2 * p[0] - 1) * gradU[1+axis][0] * h[0] / 4.0
					+ (2 * p[1] - 1) * gradU[1+axis][1] * h[1] / 4.0
					+ (2 * p[2] - 1) * gradU[1+axis][2] * h[2] / 4.0, axis);
			}
			subCell->setP((Constants::gamma - 1.0) * (U0[4] 
					+ (2 * p[0] - 1) * gradU[4][0] * h[0] / 4.0
					+ (2 * p[1] - 1) * gradU[4][1] * h[1] / 4.0
					+ (2 * p[2] - 1) * gradU[4][2] * h[2] / 4.0));
		}

		delete[] U0;
		for(int i=0; i<5; i++) delete[] gradU[i];
		delete[] gradU;
	};

/*
	private: static void interpolateHDonInsideSubCellPair(HierarchyCell* cell, Method* method, int axis, int axis1, int axis2, int dir0, int dir1){
		Cell* fcell;
		double Bc;
		int* point = new int[3];

		double* U0 = getU(cell);
		fcell = cell->getCell(axis, dir0);
		double* Uxf = getU(fcell);
		double x = 0.25*cell->getH(axis) / (0.5*cell->getH(axis) + 0.5*fcell->getH(axis)); //TODO: Проверить относительно чего дожен быть x
		Bc = cell->getSide(axis, dir0)->getB();

		double* U1l;
		if(dir0 == Cell::FORWARD_SIDE) U1l = method->getValues(axis, U0, Uxf, Bc, x);
		else U1l = method->getValues(axis, Uxf, U0, Bc, x);

		double* U1r;
		fcell = cell->getCell(axis1, dir1);
		if( ((HierarchyCell*) fcell)->isSplitted()){
			int* point = new int[3];
			point[axis] = dir0; point[axis1] = 1-dir1; 

			point[axis2] = 0;
			Cell* cl = ((HierarchyCell*) fcell)->getSubCell(point);
			double* Ultmp = getU(cl);
			point[axis2] = 1;
			Cell* cr = ((HierarchyCell*) fcell)->getSubCell(point);
			double* Urtmp = getU(cr);

			x = 0.5*cl->getH(axis2) / (0.5*cl->getH(axis2) + 0.5*cr->getH(axis2)); //TODO: Проверить x
			Bc = cl->getSide(axis2, Cell::FORWARD_SIDE)->getB();
			U1r = method->getValues(axis2, Ultmp, Urtmp, Bc, x);

			x = 0.5*cell->getH(axis1) / (0.5*cell->getH(axis1) + 0.5*cl->getH(axis1));//TODO: Проверить x
			delete[] point;
			delete[] Ultmp;
			delete[] Urtmp;
		} else {
			double* U0tmp = getU(fcell);
			Cell* fcelltmp = fcell->getCell(axis, dir0);
			double* Uxftmp = getU(fcelltmp);
			x = 0.25*fcell->getH(axis) / (0.5*fcell->getH(axis) + 0.5*fcelltmp->getH(axis)); //TODO: Проверить x
			Bc = fcell->getSide(axis, dir0)->getB();
			if(dir0 == Cell::FORWARD_SIDE) U1r = method->getValues(axis, U0tmp, Uxftmp, Bc, x);
			else U1r = method->getValues(axis, Uxftmp, U0tmp, Bc, x);

			x = 0.25*cell->getH(axis1) / (0.5*cell->getH(axis1) + 0.5*fcell->getH(axis1)); //TODO: Проверить x
			delete[] U0tmp;
			delete[] Uxftmp;
		}

		point[axis] = dir0; point[axis1] = dir1; 

		point[axis2] = 0;

		Bc = 0.5 * ((HierarchyCell*) cell)->getSubCell(point)->getSide(axis1, dir1)->getB();
		point[axis2] = 1;
		Bc += 0.5 * ((HierarchyCell*) cell)->getSubCell(point)->getSide(axis1, dir1)->getB();
		double* U1;
		if(dir1 == Cell::FORWARD_SIDE) U1 = method->getValues(axis1, U1l, U1r, Bc, x);
		else U1 = method->getValues(axis1, U1r, U1l, Bc, x);

		for(int dir=0; dir<2; dir++){
			double* U2;
			HierarchyCell* fcell = (HierarchyCell*) cell->getCell(axis2, dir);
			if(fcell->isSplitted()){
				point[axis] = dir0; point[axis1] = dir1; point[axis2] = 1-dir;
				Cell* c = fcell->getSubCell(point);
				U2 = getU(c);
				x = 0.25*cell->getH(axis2) / (0.5*cell->getH(axis2) + 0.5*c->getH(axis2)); //TODO: Проверить x
			} else {
				U2 = getMiddleU(fcell, method, axis, axis1, axis2, dir0, dir1);
				x = 0.25*cell->getH(axis2) / (0.5*cell->getH(axis2) + 0.5*fcell->getH(axis2)); //TODO: Проверить x
			}

			point[axis] = dir0; point[axis1] = dir1; point[axis2] = dir;
			Cell* subCell = cell->getSubCell(point);
			Bc = subCell->getSide(axis2, dir)->getB();

			double* U;
			if(dir == Cell::FORWARD_SIDE) U = method->getValues(axis2, U1, U2, Bc, x);
			else if(dir == Cell::BACKWARD_SIDE) U = method->getValues(axis2, U2, U1, Bc, x);

			setU(U, subCell);

			delete[] U2;
			delete[] U;
		}

		delete[] U0;
		delete[] Uxf;
		delete[] U1l;
		delete[] U1r;
		delete[] U1;
		delete[] point;
	}

	private: static double* getMiddleU(HierarchyCell* cell, Method* method, int axis, int axis1, int axis2, int dir0, int dir1){
		Cell* fcell;
		double Bc;
		int* point = new int[3];

		double* U0 = getU(cell);
		fcell = cell->getCell(axis, dir0);
		double* Uxf = getU(fcell);
		double x = 0.25*cell->getH(axis) / (0.5*cell->getH(axis) + 0.5*fcell->getH(axis)); //TODO: Проверить относительно чего дожен быть x
		Bc = cell->getSide(axis, dir0)->getB();
		double* U1l = method->getValues(axis, U0, Uxf, Bc, x);

		double* U1r;
		fcell = cell->getCell(axis1, dir1);
		if( ((HierarchyCell*) fcell)->isSplitted()){
			int* point = new int[3];
			point[axis] = dir0; point[axis1] = 1-dir1; 

			point[axis2] = 0;
			Cell* cl = ((HierarchyCell*) fcell)->getSubCell(point);
			double* Ultmp = getU(cl);
			point[axis2] = 1;
			Cell* cr = ((HierarchyCell*) fcell)->getSubCell(point);
			double* Urtmp = getU(cr);

			x = 0.5*cl->getH(axis2) / (0.5*cl->getH(axis2) + 0.5*cr->getH(axis2)); //TODO: Проверить x
			Bc = cl->getSide(axis2, Cell::FORWARD_SIDE)->getB();
			U1r = method->getValues(axis2, Ultmp, Urtmp, Bc, x);

			x = 0.5*cell->getH(axis1) / (0.5*cell->getH(axis1) + 0.5*cl->getH(axis1));//TODO: Проверить x
			delete[] point;
			delete[] Ultmp;
			delete[] Urtmp;
		} else {
			double* U0tmp = getU(fcell);
			Cell* fcelltmp = fcell->getCell(axis, dir0);
			double* Uxftmp = getU(fcelltmp);
			x = 0.25*fcell->getH(axis) / (0.5*fcell->getH(axis) + 0.5*fcelltmp->getH(axis)); //TODO: Проверить x
			Bc = fcell->getSide(axis, dir0)->getB();
			U1r = method->getValues(axis, U0tmp, Uxftmp, Bc, x);

			x = 0.25*cell->getH(axis1) / (0.5*cell->getH(axis1) + 0.5*fcell->getH(axis1)); //TODO: Проверить x
			delete[] U0tmp;
			delete[] Uxftmp;
		}

		if(cell->isSplitted()){
			point[axis] = dir0; point[axis1] = dir1;
			point[axis2] = 0;
			Bc = 0.5 * ((HierarchyCell*) cell)->getSubCell(point)->getSide(axis1, dir1)->getB();
			point[axis2] = 1;
			Bc += 0.5 * ((HierarchyCell*) cell)->getSubCell(point)->getSide(axis1, dir1)->getB();
		} else {
			Bc = cell->getSide(axis, dir0)->getB();
		}
		double* U1 = method->getValues(axis1, U1l, U1r, Bc, x);

		delete[] U0;
		delete[] Uxf;
		delete[] U1l;
		delete[] U1r;
		delete[] point;

		return U1;
	}

	private: static void* setU(double* U, Cell* cell){
		cell->setDensity(U[0]);
		cell->setMomentum(0, U[1]);
		cell->setMomentum(1, U[2]);
		cell->setMomentum(2, U[3]);

		double Ek = (U[1]*U[1]+U[2]*U[2]+U[3]*U[3])/2.0/U[0];
		double Em = (U[5]*U[5]+U[6]*U[6]+U[7]*U[7])/8.0/Constants::Pi;
		double P = (Constants::gamma-1.0)*(U[4] - Ek - Em);

		cell->setP(P);
	}
*/
	private: static double* getU(Cell* cell){
		double* U = new double[8];

		U[0] = cell->getDensity();
		U[1] = cell->getMomentum(0);
		U[2] = cell->getMomentum(1);
		U[3] = cell->getMomentum(2);
		U[4] = cell->getFullEnergy();
		U[5] = (cell->getSide(0, Cell::FORWARD_SIDE)->getB() + cell->getSide(0, Cell::BACKWARD_SIDE)->getB() ) / 2.0;
		U[6] = (cell->getSide(1, Cell::FORWARD_SIDE)->getB() + cell->getSide(1, Cell::BACKWARD_SIDE)->getB() ) / 2.0;
		U[7] = (cell->getSide(2, Cell::FORWARD_SIDE)->getB() + cell->getSide(2, Cell::BACKWARD_SIDE)->getB() ) / 2.0;

		return U;
	}

	public: static void interpolateFieldsOnSubCells(HierarchyCell* cell){
//		if(cell->isSplitted() == false) return;

		int x[3];
		Cell* bcell;
		Cell* fcell;
/*
		double** outB = new double*[3];
		for(int i=0; i<3; i++) outB[i] = new double[10];

		for(int axis=0; axis<3; axis++){
			outB[axis][0] = cell->getSide(axis, Cell::FORWARD_SIDE)->getB();
			outB[axis][1] = cell->getSide(axis, Cell::BACKWARD_SIDE)->getB();

			outB[axis][2] = cell->getCell((axis+1)%3, Cell::FORWARD_SIDE)->getSide(axis, Cell::FORWARD_SIDE)->getB();
			outB[axis][3] = cell->getCell((axis+1)%3, Cell::BACKWARD_SIDE)->getSide(axis, Cell::FORWARD_SIDE)->getB();
			outB[axis][4] = cell->getCell((axis+2)%3, Cell::FORWARD_SIDE)->getSide(axis, Cell::FORWARD_SIDE)->getB();
			outB[axis][5] = cell->getCell((axis+2)%3, Cell::BACKWARD_SIDE)->getSide(axis, Cell::FORWARD_SIDE)->getB();

			outB[axis][6] = cell->getCell((axis+1)%3, Cell::FORWARD_SIDE)->getSide(axis, Cell::BACKWARD_SIDE)->getB();
			outB[axis][7] = cell->getCell((axis+1)%3, Cell::BACKWARD_SIDE)->getSide(axis, Cell::BACKWARD_SIDE)->getB();
			outB[axis][8] = cell->getCell((axis+2)%3, Cell::FORWARD_SIDE)->getSide(axis, Cell::BACKWARD_SIDE)->getB();
			outB[axis][9] = cell->getCell((axis+2)%3, Cell::BACKWARD_SIDE)->getSide(axis, Cell::BACKWARD_SIDE)->getB();
		}
		double** outsideB = QPCGALout::calculateQP(outB);
		delete[] outB;
*/
		double** outsideB = interpolateOutsideFields(cell);
		double** indsideB = QPCGAL::calculateQP(outsideB);

		for(int axis=0; axis<3; axis++){
			for(int dir=0; dir<2; dir++){
				x[axis] = dir;
				x[(axis+1)%3]=1; x[(axis+2)%3]=1;
				cell->getSubCell(x)->getSide(axis, dir)->setB(outsideB[axis][4*dir + 0]);

				x[(axis+1)%3]=1; x[(axis+2)%3]=0;
				cell->getSubCell(x)->getSide(axis, dir)->setB(outsideB[axis][4*dir + 1]);

				x[(axis+1)%3]=0; x[(axis+2)%3]=1;
				cell->getSubCell(x)->getSide(axis, dir)->setB(outsideB[axis][4*dir + 2]);

				x[(axis+1)%3]=0; x[(axis+2)%3]=0;
				cell->getSubCell(x)->getSide(axis, dir)->setB(outsideB[axis][4*dir + 3]);
			}

			x[axis] = 1;
			x[(axis+1)%3]=1; x[(axis+2)%3]=1;
			cell->getSubCell(x)->getSide(axis, Cell::BACKWARD_SIDE)->setB(indsideB[axis][0]);
			x[(axis+1)%3]=1; x[(axis+2)%3]=0;
			cell->getSubCell(x)->getSide(axis, Cell::BACKWARD_SIDE)->setB(indsideB[axis][1]);
			x[(axis+1)%3]=0; x[(axis+2)%3]=1;
			cell->getSubCell(x)->getSide(axis, Cell::BACKWARD_SIDE)->setB(indsideB[axis][2]);
			x[(axis+1)%3]=0; x[(axis+2)%3]=0;
			cell->getSubCell(x)->getSide(axis, Cell::BACKWARD_SIDE)->setB(indsideB[axis][3]);
		}

		delete[] indsideB;
		delete[] outsideB;
	}

	private: static double** interpolateOutsideFields(HierarchyCell* cell){
		double** outsideB = new double*[3];
		for(int i=0; i<3; i++) outsideB[i] = new double[8];

		for(int axis=0; axis<3; axis++){
			for(int dir=0; dir<2; dir++){
				double Bff;
				double Bfb;
				double Bbf;
				double Bbb;

				if(cell->getCell(axis, dir) != NULL && ((HierarchyCell*) cell->getCell(axis, dir))->isSplitted()
					&& cell->getDeep() == ((HierarchyCell*) cell->getCell(axis, dir))->getDeep() ) {
					Bff = ((HierarchySide*) cell->getSide(axis, dir))->getSubSide(1, 1)->getB();
					Bfb = ((HierarchySide*) cell->getSide(axis, dir))->getSubSide(1, 0)->getB();
					Bbf = ((HierarchySide*) cell->getSide(axis, dir))->getSubSide(0, 1)->getB();
					Bbb = ((HierarchySide*) cell->getSide(axis, dir))->getSubSide(0, 0)->getB();
				} else {
					HierarchyCell* cell1f = (HierarchyCell*) cell->getCell((axis+1)%3, Cell::FORWARD_SIDE);
					HierarchyCell* cell1b = (HierarchyCell*) cell->getCell((axis+1)%3, Cell::BACKWARD_SIDE);
					HierarchyCell* cell2f = (HierarchyCell*) cell->getCell((axis+2)%3, Cell::FORWARD_SIDE);
					HierarchyCell* cell2b = (HierarchyCell*) cell->getCell((axis+2)%3, Cell::BACKWARD_SIDE);


					double B1f = cell1f->getSide(axis, dir)->getB();
					double B1b = cell1b->getSide(axis, dir)->getB();
					double B2f = cell2f->getSide(axis, dir)->getB();
					double B2b = cell2b->getSide(axis, dir)->getB();

					double h1 = cell->getH((axis+1)%3);
					double h2 = cell->getH((axis+2)%3);
					double B0 = cell->getSide(axis, dir)->getB();

					double Bxx = Utils::minmod(cell1f->getSide(axis, dir)->getB() - cell->getSide(axis, dir)->getB(), 
							 cell->getSide(axis, dir)->getB() - cell1b->getSide(axis, dir)->getB()) / h1;
	//				double Bxx = (B1f - B1b) /2/h1;

					double Byy = Utils::minmod(cell2f->getSide(axis, dir)->getB() - cell->getSide(axis, dir)->getB(), 
							 cell->getSide(axis, dir)->getB() - cell2b->getSide(axis, dir)->getB()) / h2;
	//				double Byy = (B2f - B2b) /2/h2;

					Bff = B0 + Bxx*h1/4 + Byy*h2/4;
					Bfb = B0 + Bxx*h1/4 - Byy*h2/4;
					Bbf = B0 - Bxx*h1/4 + Byy*h2/4;
					Bbb = B0 - Bxx*h1/4 - Byy*h2/4;
				}

				outsideB[axis][4*dir + 0] = Bff;
				outsideB[axis][4*dir + 1] = Bfb;
				outsideB[axis][4*dir + 2] = Bbf;
				outsideB[axis][4*dir + 3] = Bbb;
			}
		}
		return outsideB;
	}

	private: static double** interpolateMagneticFieldOnInternalFaces(double** outB){
/*
		double Bxf = outB[0][0];
		double Bxb = outB[0][1];
		double Bxf1f=outB[0][2];
		double Bxf1b=outB[0][3];
		double Bxf2f=outB[0][4];
		double Bxf2b=outB[0][5];
		double Bxb1f=outB[0][6];
		double Bxb1b=outB[0][7];
		double Bxb2f=outB[0][8];
		double Bxb2b=outB[0][9];

		double Byf = outB[1][0];
		double Byb = outB[1][1];
		double Byf1f=outB[1][2];
		double Byf1b=outB[1][3];
		double Byf2f=outB[1][4];
		double Byf2b=outB[1][5];
		double Byb1f=outB[0][6];
		double Byb1b=outB[0][7];
		double Byb2f=outB[0][8];
		double Byb2b=outB[0][9];

		double Bzf = outB[2][0];
		double Bzb = outB[2][1];
		double Bzf1f=outB[0][2];
		double Bzf1b=outB[0][3];
		double Bzf2f=outB[0][4];
		double Bzf2b=outB[0][5];
		double Bzb1f=outB[0][6];
		double Bzb1b=outB[0][7];
		double Bzb2f=outB[0][8];
		double Bzb2b=outB[0][9];
*/
		int axis = 1;
		double** outsideB = QPCGALout::calculateQP(outB);
		double** indsideB = QPCGAL::calculateQP(outsideB);

		double** B = outsideB;

/*
		double Bxbff = outB[0][0];
		double Bxbfb = outB[0][1];
		double Bxbbf = outB[0][2];
		double Bxbbb = outB[0][3];
		double Bxfff = outB[0][4];
		double Bxffb = outB[0][5];
		double Bxfbf = outB[0][6];
		double Bxfbb = outB[0][7];

		double Bybff = outB[1][0];
		double Bybfb = outB[1][1];
		double Bybbf = outB[1][2];
		double Bybbb = outB[1][3];
		double Byfff = outB[1][4];
		double Byffb = outB[1][5];
		double Byfbf = outB[1][6];
		double Byfbb = outB[1][7];

		double Bzbff = outB[2][0];
		double Bzbfb = outB[2][1];
		double Bzbbf = outB[2][2];
		double Bzbbb = outB[2][3];
		double Bzfff = outB[2][4];
		double Bzffb = outB[2][5];
		double Bzfbf = outB[2][6];
		double Bzfbb = outB[2][7];

		double Bxcff = B[0][0];
		double Bxcfb = B[0][1];
		double Bxcbf = B[0][2];
		double Bxcbb = B[0][3];

		double Bycff = B[1][0];
		double Bycfb = B[1][1];
		double Bycbf = B[1][2];
		double Bycbb = B[1][3];

		double Bzcff = B[2][0];
		double Bzcfb = B[2][1];
		double Bzcbf = B[2][2];
		double Bzcbb = B[2][3];

		if(
		Bxcff < min(Bxbff,Bxfff)  || Bxcff > max(Bxbff,Bxfff) ||
		Bxcfb < min(Bxbfb,Bxffb) || Bxcfb > max(Bxbfb,Bxffb) ||
		Bxcbf < min(Bxbbf,Bxfbf) || Bxcbf > max(Bxbbf,Bxfbf) ||
		Bxcbb < min(Bxbbb,Bxfbb) || Bxcbb > max(Bxbbb,Bxfbb)
		){
			cout << "double Bxbff = " << Bxbff << ";" << endl;
			cout << "double Bxbfb = " << Bxbfb << ";" << endl;
			cout << "double Bxbbf = " << Bxbbf << ";" << endl;
			cout << "double Bxbbb = " << Bxbbb << ";" << endl;
			cout << "double Bxfff = " << Bxfff << ";" << endl;
			cout << "double Bxffb = " << Bxffb << ";" << endl;
			cout << "double Bxfbf = " << Bxfbf << ";" << endl;
			cout << "double Bxfbb = " << Bxfbb << ";" << endl;

			cout << "double Bybff = " << Bybff << ";" << endl;
			cout << "double Bybfb = " << Bybfb << ";" << endl;
			cout << "double Bybbf = " << Bybbf << ";" << endl;
			cout << "double Bybbb = " << Bybbb << ";" << endl;
			cout << "double Byfff = " << Byfff << ";" << endl;
			cout << "double Byffb = " << Byffb << ";" << endl;
			cout << "double Byfbf = " << Byfbf << ";" << endl;
			cout << "double Byfbb = " << Byfbb << ";" << endl;

			cout << "double Bzbff = " << Bzbff << ";" << endl;
			cout << "double Bzbfb = " << Bzbfb << ";" << endl;
			cout << "double Bzbbf = " << Bzbbf << ";" << endl;
			cout << "double Bzbbb = " << Bzbbb << ";" << endl;
			cout << "double Bzfff = " << Bzfff << ";" << endl;
			cout << "double Bzffb = " << Bzffb << ";" << endl;
			cout << "double Bzfbf = " << Bzfbf << ";" << endl;
			cout << "double Bzfbb = " << Bzfbb << ";" << endl;



			cout << Bxbff << "\t" << Bxcff << "\t" << Bxfff << endl;
			cout << Bxbfb << "\t" << Bxcfb << "\t" << Bxffb << endl;
			cout << Bxbbf << "\t" << Bxcbf << "\t" << Bxfbf << endl;
			cout << Bxbbb << "\t" << Bxcbb << "\t" << Bxfbb << endl;
			cout << endl;
		}
*/
		return B;
	}
	
};
