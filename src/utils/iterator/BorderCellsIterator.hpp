class BorderCellsIterator: public Iterator{
	HierarchyCellIterator it;
	HierarchyCell* nextCell;
	int axis;
	int dir;
	bool includeParentCells;

	public: BorderCellsIterator(){}

	public: BorderCellsIterator(CellArray* ca, int axis, int dir){
		it = HierarchyCellIterator(ca);
		BorderCellsIterator::axis = axis;
		BorderCellsIterator::dir = dir;
		BorderCellsIterator::includeParentCells = false;

		bool isNextCell = true;
		while(isNextCell && it.hasNext()){
			nextCell = (HierarchyCell*) it.next();

			if(nextCell->getCell(axis, dir) == NULL && (!nextCell->isSplitted() || includeParentCells)) isNextCell = false;
			else if(nextCell->getCell(axis, dir) != NULL 
					&& nextCell->getCell(axis, dir)->getMark() == Cell::MPI_CELL_MARK 
					&& (!nextCell->isSplitted() || includeParentCells)) 
				isNextCell = false; 
			else isNextCell = true;
		}
		if(isNextCell) nextCell = NULL;
	}

	public: BorderCellsIterator(CellArray* ca, int axis, int dir, bool includeParentCells){
		it = HierarchyCellIterator(ca);
		BorderCellsIterator::axis = axis;
		BorderCellsIterator::dir = dir;
		BorderCellsIterator::includeParentCells = includeParentCells;

		bool isNextCell = true;
		while(isNextCell && it.hasNext()){
			nextCell = (HierarchyCell*) it.next();

			if(nextCell->getCell(axis, dir) == NULL && (!nextCell->isSplitted() || includeParentCells)) isNextCell = false;
			else if(nextCell->getCell(axis, dir) != NULL 
					&& nextCell->getCell(axis, dir)->getMark() == Cell::MPI_CELL_MARK 
					&& (!nextCell->isSplitted() || includeParentCells)) 
				isNextCell = false; 
			else isNextCell = true;
		}
		if(isNextCell) nextCell = NULL;
	}

	public: void drop(){
		it.drop();
	}
	
	public: bool hasNext(){
		if(nextCell != NULL) return true;
		else return false;
	};

	public: Cell* next(){
		Cell* currentCell = nextCell;

		bool isNextCell = true;
		while(isNextCell && it.hasNext()){
			nextCell = (HierarchyCell*) it.next();

			if(nextCell->getCell(axis, dir) == NULL && (!nextCell->isSplitted() || includeParentCells)) isNextCell = false;
			else if(nextCell->getCell(axis, dir) != NULL 
					&& nextCell->getCell(axis, dir)->getMark() == Cell::MPI_CELL_MARK 
					&& (!nextCell->isSplitted() || includeParentCells)) 
				isNextCell = false; 
			else isNextCell = true;
		}
		if(isNextCell) nextCell = NULL;

		return currentCell;
	}
};
