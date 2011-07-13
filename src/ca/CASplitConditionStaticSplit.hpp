class CASplitConditionStaticSplit: public CASplitCondition{
	private: CellArray* ca;
	private: TaskData* td;

	public: CASplitConditionStaticSplit(CellArray* ca, TaskData* td){
		CASplitConditionStaticSplit::ca = ca;
		CASplitConditionStaticSplit::td = td;
	}

	public: bool doSplit(Cell* cell){
		HierarchyCell* hcell = (HierarchyCell*) cell;

//		return false;

		return hcell->getDeep() < 1
			&& hcell->getR(0) >= -0.2 && hcell->getR(0) <= 0.2
			&& hcell->getR(1) >= -0.015 && hcell->getR(1) <= 0.015
			&& hcell->getR(2) >= -0.015 && hcell->getR(2) <= 0.015
			&& cell->getMark() == Cell::INTERNAL_CELL_MARK;
	}

}; 
