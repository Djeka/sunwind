
class SubCellIterator: public Iterator{
	HierarchyCell* currentCell;
	HierarchyCell* rootCell;
	bool isFresh;

	public: SubCellIterator(){
		isFresh = true;
	}

	public: SubCellIterator(HierarchyCell* cell){
		currentCell = cell;
		rootCell = cell;
		isFresh = true;
	}

	public: void setRootCell(HierarchyCell* cell){
		rootCell = cell;
		drop();
	}

	public: void drop(){
		isFresh = true;
		currentCell = rootCell;
	}
	
	public: bool hasNext(){
		if(!rootCell->isSplitted()) return false;

		bool isLastCell = true;
		if(currentCell->getIndex() == 7){
			HierarchyCell* parent = currentCell->getParent();
			while(parent != NULL){
				if(parent->getIndex() == 7 || parent->getParent() == NULL) parent = parent->getParent();
				else {
					isLastCell = false;
					break;
				}
			}
		} else isLastCell = false;

		if(!currentCell->isSplitted() && isLastCell) return false;
		return true;
	};

	public: Cell* next(){
		if(isFresh){
			isFresh = false;
			return currentCell;
		}

		if(currentCell->isSplitted()) currentCell = (HierarchyCell*) currentCell->getSubCell(0);
		else	currentCell = getNextUpCell(currentCell);

		return currentCell;
	}

	private: HierarchyCell* getNextUpCell(HierarchyCell* cell){
		HierarchyCell* parent = cell->getHParent();

		if(cell->getIndex() == 7){
			return getNextUpCell(parent);
		}
		else if (cell->getIndex() < 7){
			return (HierarchyCell*) parent->getSubCell(cell->getIndex() + 1);
		} else return NULL;
	}
};
