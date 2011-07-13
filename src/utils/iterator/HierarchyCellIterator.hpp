
class HierarchyCellIterator: public Iterator{
	CellArray* ca;
	HierarchyCell* currentCell;
	bool isFresh;

	public: HierarchyCellIterator(){}

	public: HierarchyCellIterator(CellArray* ca){
		HierarchyCellIterator::ca = ca;
		currentCell = (HierarchyCell*) ca->getCell(0);
		isFresh = true;
	}

	public: void drop(){
		isFresh = true;
		currentCell = (HierarchyCell*) ca->getCell(0);
	}
	
	public: bool hasNext(){
		if(!currentCell->isSplitted() && getUpCell(currentCell)->getIndex() == (ca->getSize()-1)) return false;
		return true;
	};

	public: Cell* next(){
		if(isFresh){
			isFresh = false;
			return currentCell;
		}

		if(currentCell->getParent() == NULL && !currentCell->isSplitted()) 
			currentCell = (HierarchyCell*) ca->getCell(currentCell->getIndex() + 1);
		else if(currentCell->isSplitted()) currentCell = (HierarchyCell*) currentCell->getSubCell(0);
		else	currentCell = getNextUpCell(currentCell);

		return currentCell;
	}

	private: HierarchyCell* getNextUpCell(HierarchyCell* cell){
		HierarchyCell* parent = cell->getHParent();

		if(parent != NULL && cell->getIndex() == 7){
			return getNextUpCell(parent);
		}
		else if (parent != NULL && cell->getIndex() < 7){
			return (HierarchyCell*) parent->getSubCell(cell->getIndex() + 1);
		}
		else if(parent == NULL){
			return (HierarchyCell*) ca->getCell(cell->getIndex() + 1);
		}
	}

	private: HierarchyCell* getUpCell(HierarchyCell* cell){
		HierarchyCell* parent = cell->getHParent();

		if(parent == NULL) return cell;
		else if(cell->getIndex() != 7) return cell;
		else if(cell->getIndex() == 7) return getUpCell(parent);
	}
};
