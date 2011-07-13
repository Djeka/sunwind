class CASplitConditionEarthDipoleInitial: public CASplitCondition{
	private: CellArray* ca;
	private: TaskData* td;
	private: InitialParametersFunction* initParams;

	public: CASplitConditionEarthDipoleInitial(CellArray* ca, TaskData* td, InitialParametersFunction* initParams){
		CASplitConditionEarthDipoleInitial::ca = ca;
		CASplitConditionEarthDipoleInitial::td = td;
		CASplitConditionEarthDipoleInitial::initParams = initParams;
	}

	public: bool doSplit(Cell* cell){
		HierarchyCell* hcell = (HierarchyCell*) cell;
		double r[3];
		double R = Constants::Re * 3;

		int ind = 0;
		for(int axis=0; axis<3; axis++){
			for(int dir=0; dir<2; dir++){
				r[axis] = cell->getR(axis) + cell->getH(axis) / 2.0;

				r[(axis+1)%3] = cell->getR((axis+1)%3) + cell->getH((axis+1)%3) / 2.0;
				r[(axis+2)%3] = cell->getR((axis+2)%3) + cell->getH((axis+2)%3) / 2.0;
				if(pow(pow(r[0],2) + pow(r[1],2) + pow(r[2],2), 0.5)  > R) ind++;

				r[(axis+1)%3] = cell->getR((axis+1)%3) - cell->getH((axis+1)%3) / 2.0;
				r[(axis+2)%3] = cell->getR((axis+2)%3) + cell->getH((axis+2)%3) / 2.0;
				if(pow(pow(r[0],2) + pow(r[1],2) + pow(r[2],2), 0.5)  > R) ind++;

				r[(axis+1)%3] = cell->getR((axis+1)%3) + cell->getH((axis+1)%3) / 2.0;
				r[(axis+2)%3] = cell->getR((axis+2)%3) - cell->getH((axis+2)%3) / 2.0;
				if(pow(pow(r[0],2) + pow(r[1],2) + pow(r[2],2), 0.5)  > R) ind++;

				r[(axis+1)%3] = cell->getR((axis+1)%3) - cell->getH((axis+1)%3) / 2.0;
				r[(axis+2)%3] = cell->getR((axis+2)%3) - cell->getH((axis+2)%3) / 2.0;
				if(pow(pow(r[0],2) + pow(r[1],2) + pow(r[2],2), 0.5)  > R) ind++;
			}
		}
		if(ind == 0) return false;
/*
		double* rad = new double[3];
		rad[0] = 0.0;
		rad[1] = 0.0;
		rad[2] = Constants::Re * 3;
		double maxs = initParams->getMaxSpeed(rad);
		delete[] rad;

		double measure = 0;
		for(int axis=0;axis<3;axis++){
			if(hcell->getMark() != Cell::INTERNAL_CELL_MARK) return false;

			double cspeed = initParams->getMaxSpeed(cell);
			double fspeed = initParams->getMaxSpeedOnSide(cell->getSide(axis, Cell::FORWARD_SIDE));
			double bspeed = initParams->getMaxSpeedOnSide(cell->getSide(axis, Cell::BACKWARD_SIDE));
	
			double curmeasure = (abs(cspeed - bspeed) + abs(cspeed - fspeed)) / maxs;

			measure = max(measure, curmeasure);
		}

		if(measure >= 0.05 && hcell->getDeep() < 2 && hcell->getRadius() / Constants::Re < 10) return true;
		else return false;
*/
		double minH = min(min(hcell->getH(0), hcell->getH(1)), hcell->getH(2));

		if(hcell->getRadius() < 30 * Constants::Re && minH > 2.0 * Constants::Re) return true;
		if(hcell->getRadius() < 20 * Constants::Re && minH > 1.0 * Constants::Re) return true;
		if(hcell->getRadius() < 10 * Constants::Re && minH > 0.5 * Constants::Re) return true;
		if(hcell->getRadius() < 5 * Constants::Re && minH > 0.25 * Constants::Re) return true;

		return false;
	}

}; 
