class InitialParametersFunctionEarth: public InitialParametersFunction{
	TaskData* td;
	public: InitialParametersFunctionEarth(TaskData* td){
		InitialParametersFunctionEarth::td = td;
	}

	public: double getDensity(Cell* cell){
		return getDensity(cell->getR());
	};
	public: double getDensity(double* r){
		double mu0[6] = {4.002602 ,1.00794  ,14.0067  ,15.9994  ,31.9988  ,28.0134};
		double n0[6]  = {2.631E+07,2.466E+05,1.268E+07,4.918E+09,2.685E+08,3.538E+09};


		double T0=831.0;
		double R0=6570.0E+5;
		double ns=0.0;
		double h = pow(pow(r[0],2) + pow(r[1],2) + pow(r[2],2), 0.5);

		for(int i=0; i<6; i++){
			ns +=  n0[i] * exp( -(Constants::g * pow(Constants::Re,2) * mu0[i] 
					/ (Constants::R * T0) ) * (1.0/R0-1.0/h) );
		}

		return ns * Constants::mp;
	};

	public: double getMomentum(int axis, Cell* cell){
		return getMomentum(axis, cell->getR());
	};
	public: double getMomentum(int axis, double* r){
		return 0;
	};

	public: double getPressure(Cell* cell){
		return getPressure(cell->getR());
	};
	public: double getPressure(double* r){
		return (Constants::gamma - 1.0) * getDensity(r) * Constants::Cv * 831.0;
	};

	public: double getB(Side* side){
		return 0;
	};

	public: double getBackGroundField(Side* side){
		Cell* cell;
		int dir;
		int axis = side->getAxis();
		double* r = new double[3];
		double* p = new double[3];
		double* rd = new double[3];
		double B;
		p[0] = 0;
		p[1] = 0;
		p[2] = td->getEarthMagneticDipole();
		rd[0] = 0;
		rd[1] = 0;
		rd[2] = 0;

		if(side->getBCell() != NULL && side->getFCell() != NULL
				&& dynamic_cast<HierarchyCell*>(side->getBCell()) && dynamic_cast<HierarchyCell*>(side->getFCell())){
			if( ((HierarchyCell*) side->getBCell())->getDeep() > ((HierarchyCell*) side->getFCell())->getDeep()) {
				cell = side->getBCell();
				dir == Cell::BACKWARD_SIDE;
			}
			else {
				cell = side->getFCell();
				dir == Cell::FORWARD_SIDE;
			}
		} else if(side->getBCell() == NULL) {
			cell = side->getFCell();
			dir == Cell::FORWARD_SIDE;
		}
		else if(side->getFCell() == NULL) {
			cell = side->getBCell();
			dir == Cell::BACKWARD_SIDE;
		}

		double h0 = cell->getH(axis);
		double h1 = cell->getH((axis+1)%3);
		double h2 = cell->getH((axis+2)%3);
		r[0] = cell->getR(0); r[1] = cell->getR(1); r[2] = cell->getR(2);
		r[axis] = r[axis] + (dir*2-1) * h0 /2.0;

		B = B_by_vp(axis, p, r, rd, h1, h2);
	
		delete[] r;
		delete[] p;
		delete[] rd;

		return B;
	}

	private: double B_by_vp(int axis, double* P, double* r0, double* rd, double h1, double h2){
		int axe1 = (axis+1)%3;
		int axe2 = (axis+2)%3;
		double B=0.0;

		double* r = new double[3];
		double* r1 = new double[3];
		double* A;
		for(int i=0; i<3; i++) r[i] = r0[i] - rd[i];

		for(int i=0; i<3; i++) r1[i] = r[i];
		r1[axe1] = r[axe1] + h1 * 0.5;
		A = vectorpotential(r1, P);
		B = B + A[axe2] * h2;

		r1[axe1] = r[axe1] - h1 * 0.5;
		A = vectorpotential(r1, P);
		B = B - A[axe2] * h2;

		for(int i=0; i<3; i++) r1[i] = r[i];
		r1[axe2] = r[axe2] + h2 * 0.5;
		A = vectorpotential(r1, P);
		B = B - A[axe1] * h1;

		r1[axe2] = r[axe2] - h2 * 0.5;
		A = vectorpotential(r1, P);
		B = B + A[axe1] * h1;

		B = B / (h1*h2);

		delete[] r1;
		delete[] A;
		delete[] r;

		return B;
	}

	private: double* vectorpotential(double* r, double* P){
		double* A = new double[3];
		double r3 = pow(Utils::sqrtAverage(r, 3), 3);

		A[0] = (P[1] * r[2] - P[2] * r[1]) / r3;
		A[1] = (P[2] * r[0] - P[0] * r[2]) / r3;
		A[2] = (P[0] * r[1] - P[1] * r[0]) / r3;

		return A;
	}

	public: double getMaxSpeed(Cell* cell){
		double density = getDensity(cell);
		double maxs = 0;

		for(int axis=0;axis<3;axis++){
			double u = getMomentum(axis, cell) / density;

			Side* fside = cell->getSide(axis,Cell::FORWARD_SIDE);
			Side* bside = cell->getSide(axis,Cell::BACKWARD_SIDE);
			double Bn = (getB(bside) + getBackGroundField(bside) + getB(fside) + getBackGroundField(fside)) / 2.0;

			fside = cell->getSide((axis+1)%3,Cell::FORWARD_SIDE);
			bside = cell->getSide((axis+1)%3,Cell::BACKWARD_SIDE);
			double B1 = (getB(bside) + getBackGroundField(bside) + getB(fside) + getBackGroundField(fside)) / 2.0;

			fside = cell->getSide((axis+2)%3,Cell::FORWARD_SIDE);
			bside = cell->getSide((axis+2)%3,Cell::BACKWARD_SIDE);
			double B2 = (getB(bside) + getBackGroundField(bside) + getB(fside) + getBackGroundField(fside)) / 2.0;

			double ce =  sqrt(Constants::gamma * getPressure(cell) / density );

			double aa = abs(Bn) / sqrt(4*Constants::Pi*density);
			double a1 = sqrt( pow(ce,2) + (pow(Bn,2) + pow(B1,2) + pow(B2,2))/4/Constants::Pi + 2*aa*ce);
			double a2 = sqrt( pow(ce,2) + (pow(Bn,2) + pow(B1,2) + pow(B2,2))/4/Constants::Pi - 2*aa*ce);
			double af = 0.5*(a1+a2);

			if(maxs < max(max(abs(u+aa),abs(u-aa)), max(abs(u+af),abs(u-af)))) maxs = max(max(abs(u+aa),abs(u-aa)), max(abs(u+af),abs(u-af)));
		}

		return maxs;
	}

	public: double getMaxSpeed(double* r){
		double density = getDensity(r);
		double maxs = 0;
		double* B = getDipoleField(r);

		for(int axis=0;axis<3;axis++){
			double u = getMomentum(axis, r) / density;

			double Bn = B[axis];
			double B1 = B[(axis+1)%3];
			double B2 = B[(axis+2)%3];

			double ce =  sqrt(Constants::gamma * getPressure(r) / density );

			double aa = abs(Bn) / sqrt(4*Constants::Pi*density);
			double a1 = sqrt( pow(ce,2) + (pow(Bn,2) + pow(B1,2) + pow(B2,2))/4/Constants::Pi + 2*aa*ce);
			double a2 = sqrt( pow(ce,2) + (pow(Bn,2) + pow(B1,2) + pow(B2,2))/4/Constants::Pi - 2*aa*ce);
			double af = 0.5*(a1+a2);

			if(maxs < max(max(abs(u+aa),abs(u-aa)), max(abs(u+af),abs(u-af)))) maxs = max(max(abs(u+aa),abs(u-aa)), max(abs(u+af),abs(u-af)));
		}

		delete[] B;

		return maxs;
	}

	private: double* getDipoleField(double* r){
		double p[3];
		double rd[3];
		double* B = new double[3];
		p[0] = 0;
		p[1] = 0;
		p[2] = td->getEarthMagneticDipole();
		rd[0] = 0;
		rd[1] = 0;
		rd[2] = 0;

		double rabs = pow(pow(r[0],2) + pow(r[1],2) + pow(r[2],2), 0.5);

		B[0] = 3.0 * (p[0]*r[0] + p[1]*r[1] + p[2]*r[2]) / pow(rabs, 5) * r[0] - 1.0 / pow(rabs, 3) * p[0];
		B[1] = 3.0 * (p[0]*r[0] + p[1]*r[1] + p[2]*r[2]) / pow(rabs, 5) * r[1] - 1.0 / pow(rabs, 3) * p[1];
		B[2] = 3.0 * (p[0]*r[0] + p[1]*r[1] + p[2]*r[2]) / pow(rabs, 5) * r[2] - 1.0 / pow(rabs, 3) * p[2];

		return B;
	}

	public: double getMaxSpeedOnSide(Side* side){
		int axis = side->getAxis();
		double B[3];
		Cell* bc = side->getBCell();
		Cell* fc = side->getFCell();
		double density = 0.5* (getDensity(bc) + getDensity(fc));

		B[axis] = getB(side) + getBackGroundField(side);
		B[(axis+1)%3] = 0.25 * (
				   getB(bc->getSide((axis+1)%3,Cell::FORWARD_SIDE)) + getBackGroundField(bc->getSide((axis+1)%3,Cell::FORWARD_SIDE))
				 + getB(bc->getSide((axis+1)%3,Cell::BACKWARD_SIDE)) + getBackGroundField(bc->getSide((axis+1)%3,Cell::BACKWARD_SIDE))
				 + getB(fc->getSide((axis+1)%3,Cell::FORWARD_SIDE)) + getBackGroundField(fc->getSide((axis+1)%3,Cell::FORWARD_SIDE))
				 + getB(fc->getSide((axis+1)%3,Cell::BACKWARD_SIDE)) + getBackGroundField(fc->getSide((axis+1)%3,Cell::BACKWARD_SIDE))
				   );
		B[(axis+2)%3] = 0.25 * (
				   getB(bc->getSide((axis+2)%3,Cell::FORWARD_SIDE)) + getBackGroundField(bc->getSide((axis+2)%3,Cell::FORWARD_SIDE))
				 + getB(bc->getSide((axis+2)%3,Cell::BACKWARD_SIDE)) + getBackGroundField(bc->getSide((axis+2)%3,Cell::BACKWARD_SIDE))
				 + getB(fc->getSide((axis+2)%3,Cell::FORWARD_SIDE)) + getBackGroundField(fc->getSide((axis+2)%3,Cell::FORWARD_SIDE))
				 + getB(fc->getSide((axis+2)%3,Cell::BACKWARD_SIDE)) + getBackGroundField(fc->getSide((axis+2)%3,Cell::BACKWARD_SIDE))
				   );

		double maxs = 0;
		for(int iaxis=0; iaxis<3; iaxis++){
			double u = 0.5 * (getMomentum(iaxis, bc)/getDensity(bc) + getMomentum(iaxis, fc)/getDensity(fc));
			double Bn = B[iaxis];
			double B1 = B[(iaxis+1)%3];
			double B2 = B[(iaxis+1)%3];
			double ce =  sqrt(Constants::gamma*(getPressure(bc) + getPressure(fc)) * 0.5 / density);

			double aa = abs(Bn) / sqrt(4*Constants::Pi*density);
			double a1 = sqrt( pow(ce,2) + (pow(Bn,2) + pow(B1,2) + pow(B2,2))/4/Constants::Pi + 2*aa*ce);
			double a2 = sqrt( pow(ce,2) + (pow(Bn,2) + pow(B1,2) + pow(B2,2))/4/Constants::Pi - 2*aa*ce);
			double af = 0.5*(a1+a2);

			if(maxs < max(max(abs(u+aa),abs(u-aa)), max(abs(u+af),abs(u-af)))) maxs = max(max(abs(u+aa),abs(u-aa)), max(abs(u+af),abs(u-af)));
		}

		return maxs;
	}
};
