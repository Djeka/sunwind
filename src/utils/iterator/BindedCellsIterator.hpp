class BindedCellsIterator: public Iterator{
	BorderCellsIterator bit;
	SubCellIterator sit;
	HierarchyCell* nextCell;
	int axis;
	int dir;
	bool includeParentCells;
	bool isInSubCellMode;
	bool doIndeepIterations;

	public: BindedCellsIterator(CellArray* ca, int axis, int dir){
		bit = BorderCellsIterator(ca, axis, dir);
		sit = SubCellIterator();
		BindedCellsIterator::axis = axis;
		BindedCellsIterator::dir = dir;
		BindedCellsIterator::includeParentCells = false;
		BindedCellsIterator::isInSubCellMode = false;
		BindedCellsIterator::doIndeepIterations = false;

		setNextCellByBIT();
	}

	public: BindedCellsIterator(CellArray* ca, int axis, int dir, bool includeParentCells, bool doIndeepIterations){
		bit = BorderCellsIterator(ca, axis, dir);
		sit = SubCellIterator();
		BindedCellsIterator::axis = axis;
		BindedCellsIterator::dir = dir;
		BindedCellsIterator::includeParentCells = includeParentCells;
		BindedCellsIterator::isInSubCellMode = false;
		BindedCellsIterator::doIndeepIterations = doIndeepIterations;

		setNextCellByBIT();
	}

	public: void drop(){
		bit.drop();
		sit.drop();
		BindedCellsIterator::isInSubCellMode = false;
	}
	
	public: bool hasNext(){
		if(nextCell != NULL) return true;
		else return false;
	};

	public: Cell* next(){
		HierarchyCell* currentCell = nextCell;

		if(!bit.hasNext() && (!isInSubCellMode || !sit.hasNext())){
			nextCell = NULL;
			return currentCell;
		}

		if(nextCell->getParent() == NULL && (!nextCell->isSplitted() || !doIndeepIterations)){
			setNextCellByBIT();
		}

		if(isInSubCellMode){
			if(sit.hasNext()){
				nextCell = (HierarchyCell*) sit.next();
				while(nextCell->isSplitted() && !includeParentCells) nextCell = (HierarchyCell*) sit.next();
			} else {
				isInSubCellMode = false;
				setNextCellByBIT();
			}
		}

		return currentCell;
	}

	private: void setNextCellByBIT(){
		bool isNextCell = true;
		while(isNextCell && bit.hasNext()){
			nextCell = (HierarchyCell*) bit.next();
			if(nextCell->getCell(axis, dir) != NULL){
				nextCell = (HierarchyCell*) nextCell->getCell(axis, dir);
				isNextCell = false;
			}			
		}

		if(doIndeepIterations){
			if(nextCell->isSplitted() && !isInSubCellMode){
				sit.setRootCell(nextCell);
				isInSubCellMode = true;
			}
		}
	}
};
