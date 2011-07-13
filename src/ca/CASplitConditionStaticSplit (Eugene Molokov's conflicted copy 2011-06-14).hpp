class CASplitConditionStaticSplit: public CASplitCondition{
	private: CellArray* ca;
	private: TaskData* td;

	public: CASplitConditionStaticSplit(CellArray* ca, TaskData* td){
		CASplitConditionStaticSplit::ca = ca;
		CASplitConditionStaticSplit::td = td;
	}

	public: bool doSplit(Cell* cell){
		HierarchyCell* hcell = (HierarchyCell*) cell;
/*
		double measure = 0;
		for(int axis=0;axis<3;axis++){
			if(hcell->getCell(axis, Cell::FORWARD_SIDE) == NULL || hcell->getCell(axis, Cell::BACKWARD_SIDE) == NULL) return;

			double cspeed = hcell->getMaxSpeed();
			double fspeed = fspeed = hcell->getMaxSpeedOnSide(axis, Cell::FORWARD_SIDE);
			double bspeed = bspeed = hcell->getMaxSpeedOnSide(axis, Cell::BACKWARD_SIDE);		
	
			double curmeasure = (abs(cspeed - bspeed) + abs(cspeed - fspeed))/maxs;

			measure = max(measure, curmeasure);
		}
*/

//		return Utils::sqrtAverage(hcell->getR(), 3) < 0.3 && hcell->getDeep() <1;
		return hcell->getDeep() < 2
			&& hcell->getR(0) >= -0.2 && hcell->getR(0) <= 0.2
			&& hcell->getR(1) >= -0.01 && hcell->getR(1) <= 0.01
			&& hcell->getR(2) >= -0.01 && hcell->getR(2) <= 0.01
			&& cell->getMark() == Cell::INTERNAL_CELL_MARK;
	}

};
