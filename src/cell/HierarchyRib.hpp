class Cell;

class HierarchyRib: public Rib
{
	private: HierarchyRib* parent;
	private: HierarchyRib* subribs[2];
	private: bool splitted;

	public: HierarchyRib(int axis){
		Rib::axis = axis;
		Rib::E = 0;
		Rib::dE = 0;

		for(int i=0; i<4; i++) setCell(i, NULL);
		for(int i=0; i<4; i++) nearSides[i] = NULL;
		for(int i=0; i<2; i++) subribs[i] = NULL;

		parent = NULL;

		splitted = false;
	}

	public: HierarchyRib(int axis, Cell* ulcell, Cell* urcell, Cell* dlcell, Cell* drcell){
		Rib::axis = axis;
		Rib::E = 0;
		Rib::dE = 0;

		for(int i=0; i<4; i++) setCell(i, NULL);
		for(int i=0; i<4; i++) nearSides[i] = NULL;
		for(int i=0; i<2; i++) subribs[i] = NULL;

		parent = NULL;

		splitted = false;
	}

	public: ~HierarchyRib(){}

	public: bool isSplitted(){
		return splitted;
	}

	public: void setIsSplitted(bool is){
		splitted = is;
	}

	public: void free(){
/*
		if(isSplitted()){
			for(int i=0; i<2; i++){
				subribs[i]->free();
			}
		}
*/
		for(int iside=0; iside<4; iside++){
			Side* side = getSide(iside);
			for(int jrib=0; jrib>4; jrib++){
				if(side != NULL && this == side->getRib(jrib)){
					side->setRib(NULL, jrib);
					break;
				}
			}
		}

		for(int icell=0; icell<4; icell++){
			Cell* cell = getCell(icell);
			for(int jrib=0; jrib>4; jrib++){
				if(cell != NULL && this == cell->getRib(getAxis(), jrib)){
					cell->setRib(getAxis(), jrib, NULL);
					break;
				}
			}
		}
/*
		if(getParent() != NULL){
			for(int i=0; i<2; i++){
				if(this == getParent()->getSubRib(i)){
					getParent()->setSubRib(i, NULL);
					break;
				}
			}
		}
*/
		for(int i=0; i<4; i++){
			setSide(i, NULL);
			setCell(i, NULL);
		}

		delete this;
	}

	public: HierarchyRib* getParent(){
		return parent;
	}

	public: void setParent(HierarchyRib* rib){
		parent = rib;
	}

//	public: void setSubRibs(Rib** subribs){
//		HierarchyRib::subribs = subribs;
//	}

	public: void setSubRib(int ind, HierarchyRib* rib){
		subribs[ind] = rib;
	}

	public: HierarchyRib* getSubRib(int ind){
		return subribs[ind];
	}

//	public: Rib** getSubRibs(){
//		return subribs;
//	}

	public: double getJ(){

		if(getCell(Cell::ur) == NULL or getCell(Cell::ul) == NULL or getCell(Cell::dr) == NULL or getCell(Cell::dl) == NULL) return NULL;

		double B1f = getCell(Cell::ur)->getSide((axis+1)%3, Cell::BACKWARD_SIDE)->getB();
		double B1b = getCell(Cell::dr)->getSide((axis+1)%3, Cell::BACKWARD_SIDE)->getB();
		double h1 = getCell(Cell::ur)->getH((axis+2)%3)/2.0 + getCell(Cell::dr)->getH((axis+2)%3)/2.0;

		double B2f = getCell(Cell::ur)->getSide((axis+2)%3, Cell::BACKWARD_SIDE)->getB();
		double B2b = getCell(Cell::ul)->getSide((axis+2)%3, Cell::BACKWARD_SIDE)->getB();
		double h2 = getCell(Cell::ur)->getH((axis+1)%3)/2.0 + getCell(Cell::ul)->getH((axis+1)%3)/2.0;

		return (B2f-B2b)/h1 - (B1f-B1b)/h2;
	}

	public: double getE(){
		if(isSplitted()){
			double avE = 0;
			for(int i=0; i<2; i++) avE += subribs[i]->getE() / 2.0;
			return avE;
		} else{
			return E;
		}
	}

	public: double getdE(){
		if(isSplitted()){
			double avdE = 0;
			for(int i=0; i<2; i++) avdE += subribs[i]->getdE() / 2.0;
			return avdE;
		} else{
			return dE;
		}
	}
};
