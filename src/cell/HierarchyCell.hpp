#include <vector>

class Rib;
class Side;

class HierarchyCell : public Cell
{
	private: HierarchyCell* subcells[8];
	private: bool splitted;
	private: HierarchyCell* parent;
	private: int deep;

	public: HierarchyCell(double r[],int point[], double h[], int index, int deep){
		HierarchyCell::dDensity = 0;
		HierarchyCell::dTermEnergy = 0;
		HierarchyCell::density = 0;
		HierarchyCell::termEnergy = 0;

		HierarchyCell::deep = deep;

		Cell::index = index;
		Cell::momentum = new double[3];
		Cell::dMomentum = new double[3];
		Cell::point = new int[3];
		HierarchyCell::r = new double[3];
		Cell::h = new double[3];

		for(int i=0; i<3; i++){
			Cell::momentum[i] = 0;
			Cell::dMomentum[i] = 0;

			Cell::point[i] = point[i];
			HierarchyCell::r[i] = r[i];
			Cell::h[i] = h[i];

			for(int j=0; j<4; j++){
				setRib(i, j, NULL);
			}
			setSide(i, Cell::FORWARD_SIDE, NULL);
			setSide(i, Cell::BACKWARD_SIDE, NULL);
		}

		for(int i=0; i<8; i++) subcells[i] = NULL;

		splitted = false;
		parent = NULL;
	}

	public: ~HierarchyCell(){
		delete[] momentum;
		delete[] dMomentum;
		delete[] point;
		delete[] r;
		delete[] h;
	}

	public: int getDeep(){
/*		int deep = 0;
		if(this->getParent() != NULL)
			deep = ((HierarchyCell*) this->getParent())->getDeep() + 1;
*/
		return deep;
	}

	public: HierarchyCell* getSubCell(int ind){
		return subcells[ind];
	}

	public: HierarchyCell* getSubCell(int x[3]){
		return subcells[ x[0]*2*2 + x[1]*2 + x[2] ];
	}
	public: HierarchyCell* getSubCell(int x, int y, int z){
		return subcells[ x*2*2 + y*2 + z ];
	}
	public: void setSubCell(int ind, HierarchyCell* cell){
		subcells[ind] = cell;
	}

	public: bool isSplitted(){
		return splitted;
	}

	public: void setIsSplitted(bool is){
		splitted = is;
	}

	public: void setParent(Cell* parent){
		HierarchyCell::parent = (HierarchyCell*) parent;
	}
	public: HierarchyCell* getParent(){
		return parent;
	}
	public: string getPath(){
		if(parent != NULL) return parent->getPath() + " -> " + Utils::intToStr(getIndex()) + ": " + getPointString();
		else return Utils::intToStr(getIndex()) + ": " + getPointString();
	}

	public: HierarchyCell* getHParent(){
		return (HierarchyCell*) getParent();
	}

	public: double getDensity(){
		if(this->isSplitted()){
			double dens = 0;
			for(int i=0; i<8; i++){
				dens += subcells[i]->getDensity();
			}
			return dens/8.0;
		} else{
			return HierarchyCell::density;
		}
	};

	public: double getMomentum(int ind){
		if(this->isSplitted()){
			double moment = 0;
			for(int i=0; i<8; i++)
				moment += subcells[i]->getMomentum(ind);
			return moment/8.0;
		} else{
			return HierarchyCell::momentum[ind];
		}
	};

	public: double* getMomentum(){
		if(this->isSplitted()){
			for(int axis=0; axis<3; axis++){
				momentum[axis] = 0;
				for(int i=0; i<8; i++) momentum[axis] += subcells[i]->getMomentum(axis) / 8.0;
			}
		} else{
			return HierarchyCell::momentum;
		}
	};
	public: double getTermEnergy(){
		if(this->isSplitted()){
			double e = 0;
			for(int i=0; i<8; i++)
				e += subcells[i]->getTermEnergy();
			return e/8.0;
		} else{
			return HierarchyCell::termEnergy;
		}
	};
	public: double getFullEnergy(){
		if(this->isSplitted()){
			double e = 0;
			for(int i=0; i<8; i++)
				e += subcells[i]->getFullEnergy();
			return e/8.0;
		} else{
			double Ek = 0;
			double Em = 0;
			for(int axis=0; axis<3; axis++){
				Em += pow(0.5*(getSide(axis, Cell::FORWARD_SIDE)->getB() + getSide(axis, Cell::BACKWARD_SIDE)->getB()), 2);
				Ek += pow(getMomentum(axis), 2);
			}
			Em = Em / Constants::Pi / 8.0;
			Ek = Ek / getDensity() / 2.0;

			return getTermEnergy() + Em + Ek;
		}
	};




	public: HierarchyCell* bindCell(int axis, int dir){
		if(getCell(axis, dir) != NULL) return (HierarchyCell*) getCell(axis, dir);

		double br[3];
		int bpoint[3];
		double bh[3];

		for(int baxis=0; baxis<3; baxis++){
			br[baxis] = getPoint(baxis);
			bh[baxis] = getH(baxis);
			bpoint[baxis] = 0;
		}
		br[axis] = br[axis] + (2*dir - 1) * (getH(axis)/2.0 + bh[axis]/2.0);

		HierarchyCell* bindedCell = new HierarchyCell(br,bpoint,bh, 0, this->getDeep());
		bindCellsBySide(1 - dir, this->getSide(axis, dir), bindedCell, this);
		Cell* tmpCell;

/////////////////////////
		if(dir == FORWARD_SIDE){
			tmpCell = bindedCell->getRib((axis+1)%3, Cell::ul)->getCell(Cell::ur);
			if(tmpCell != NULL) bindCellsBySide(Cell::FORWARD_SIDE, tmpCell->getSide((axis+2)%3, Cell::BACKWARD_SIDE), bindedCell, tmpCell);

			tmpCell = bindedCell->getRib((axis+1)%3, Cell::dl)->getCell(Cell::dr);
			if(tmpCell != NULL) bindCellsBySide(Cell::BACKWARD_SIDE, tmpCell->getSide((axis+2)%3, Cell::FORWARD_SIDE), bindedCell, tmpCell);

			tmpCell = bindedCell->getRib((axis+2)%3, Cell::dl)->getCell(Cell::ul);
			if(tmpCell != NULL) bindCellsBySide(Cell::BACKWARD_SIDE, tmpCell->getSide((axis+1)%3, Cell::FORWARD_SIDE), bindedCell, tmpCell);

			tmpCell = bindedCell->getRib((axis+2)%3, Cell::dr)->getCell(Cell::ur);
			if(tmpCell != NULL) bindCellsBySide(Cell::FORWARD_SIDE, tmpCell->getSide((axis+1)%3, Cell::BACKWARD_SIDE), bindedCell, tmpCell);
		} else{
			tmpCell = bindedCell->getRib((axis+1)%3, Cell::ur)->getCell(Cell::ul);
			if(tmpCell != NULL) bindCellsBySide(Cell::FORWARD_SIDE, tmpCell->getSide((axis+2)%3, Cell::BACKWARD_SIDE), bindedCell, tmpCell);

			tmpCell = bindedCell->getRib((axis+1)%3, Cell::dr)->getCell(Cell::dl);
			if(tmpCell != NULL) bindCellsBySide(Cell::BACKWARD_SIDE, tmpCell->getSide((axis+2)%3, Cell::FORWARD_SIDE), bindedCell, tmpCell);

			tmpCell = bindedCell->getRib((axis+2)%3, Cell::ul)->getCell(Cell::dl);
			if(tmpCell != NULL) bindCellsBySide(Cell::BACKWARD_SIDE, tmpCell->getSide((axis+1)%3, Cell::FORWARD_SIDE), bindedCell, tmpCell);

			tmpCell = bindedCell->getRib((axis+2)%3, Cell::ur)->getCell(Cell::dr);
			if(tmpCell != NULL) bindCellsBySide(Cell::FORWARD_SIDE, tmpCell->getSide((axis+1)%3, Cell::BACKWARD_SIDE), bindedCell, tmpCell);
		}
/////////////////////////

		for(int baxis=0; baxis<3; baxis++){
			for(int bdir=0; bdir<2; bdir++){
				if(bindedCell->getSide(baxis, bdir) == NULL)
					defineSideWithRibs(baxis, bdir, bindedCell);
			}
		}
/////////////////////////
		bindedCell->setMark(Cell::MPI_CELL_MARK);
		for(int iaxis=0; iaxis<3; iaxis++){
			for(int idir=0; idir<2; idir++){
				if(axis == iaxis && dir == idir) continue;
				bindedCell->getSide(iaxis, idir)->setMark(Side::MPI_SIDE_MARK);
			}
		}

		for(int irib=0; irib<4; irib++)
			bindedCell->getRib(axis, irib)->setMark(Rib::MPI_RIB_MARK);

		if(dir == Cell::FORWARD_SIDE){
			bindedCell->getRib((axis+1)%3, Cell::ur)->setMark(Rib::MPI_RIB_MARK);
			bindedCell->getRib((axis+1)%3, Cell::dr)->setMark(Rib::MPI_RIB_MARK);

			bindedCell->getRib((axis+2)%3, Cell::ur)->setMark(Rib::MPI_RIB_MARK);
			bindedCell->getRib((axis+2)%3, Cell::ul)->setMark(Rib::MPI_RIB_MARK);
		} else{
			bindedCell->getRib((axis+1)%3, Cell::ul)->setMark(Rib::MPI_RIB_MARK);
			bindedCell->getRib((axis+1)%3, Cell::dl)->setMark(Rib::MPI_RIB_MARK);

			bindedCell->getRib((axis+2)%3, Cell::dr)->setMark(Rib::MPI_RIB_MARK);
			bindedCell->getRib((axis+2)%3, Cell::dl)->setMark(Rib::MPI_RIB_MARK);
		}
////////////////////////
		return bindedCell;
	}

	private: void bindCellsBySide(int dir, Side* side, Cell* bindedCell, Cell* mountCell){
		int axis = side->getAxis();
		bindedCell->setSide(axis, dir, side);
		side->setCell(1 - dir, bindedCell);

		Rib* rib;
		if(dir == Cell::BACKWARD_SIDE){
			rib = mountCell->getRib((axis+1)%3, Cell::ur);
			bindedCell->setRib((axis+1)%3, Cell::ul, rib);
			rib->setCell(Cell::dr, bindedCell);

			rib = mountCell->getRib((axis+1)%3, Cell::dr);
			bindedCell->setRib((axis+1)%3, Cell::dl, rib);
			rib->setCell(Cell::ur, bindedCell);

			rib = mountCell->getRib((axis+2)%3, Cell::ur);
			bindedCell->setRib((axis+2)%3, Cell::dr, rib);
			rib->setCell(Cell::ul, bindedCell);

			rib = mountCell->getRib((axis+2)%3, Cell::ul);
			bindedCell->setRib((axis+2)%3, Cell::dl, rib);
			rib->setCell(Cell::ur, bindedCell);
		} else{
			rib = mountCell->getRib((axis+1)%3, Cell::ul);
			bindedCell->setRib((axis+1)%3, Cell::ur, rib);
			rib->setCell(Cell::dl, bindedCell);

			rib = mountCell->getRib((axis+1)%3, Cell::dl);
			bindedCell->setRib((axis+1)%3, Cell::dr, rib);
			rib->setCell(Cell::ul, bindedCell);

			rib = mountCell->getRib((axis+2)%3, Cell::dr);
			bindedCell->setRib((axis+2)%3, Cell::ur, rib);
			rib->setCell(Cell::dl, bindedCell);

			rib = mountCell->getRib((axis+2)%3, Cell::dl);
			bindedCell->setRib((axis+2)%3, Cell::ul, rib);
			rib->setCell(Cell::dr, bindedCell);
		}
	}

	private: void defineSideWithRibs(int axis,int dir, Cell* bindedCell){
		Side* side;
		if(dir == Cell::FORWARD_SIDE) side = new HierarchySide(axis, bindedCell, NULL);
		else side = new HierarchySide(axis, NULL, bindedCell);
		bindedCell->setSide(axis, dir, side);

		if(dir == Cell::FORWARD_SIDE){
			setRibOnNewSide((axis+2)%3, bindedCell, side, Cell::ul, Rib::UP_SIDE, Side::LEFT_RIB);
			setRibOnNewSide((axis+2)%3, bindedCell, side, Cell::ur, Rib::DOWN_SIDE, Side::RIGHT_RIB);
			setRibOnNewSide((axis+1)%3, bindedCell, side, Cell::ur, Rib::LEFT_SIDE, Side::UP_RIB);
			setRibOnNewSide((axis+1)%3, bindedCell, side, Cell::dr, Rib::RIGHT_SIDE, Side::DOWN_RIB);

		} else{
			setRibOnNewSide((axis+2)%3, bindedCell, side, Cell::dl, Rib::UP_SIDE, Side::LEFT_RIB);
			setRibOnNewSide((axis+2)%3, bindedCell, side, Cell::dr, Rib::DOWN_SIDE, Side::RIGHT_RIB);
			setRibOnNewSide((axis+1)%3, bindedCell, side, Cell::ul, Rib::LEFT_SIDE, Side::UP_RIB);
			setRibOnNewSide((axis+1)%3, bindedCell, side, Cell::dl, Rib::RIGHT_SIDE, Side::DOWN_RIB);
		}
	}

	private: void setRibOnNewSide(int ribAxis, Cell* bindedCell, Side* side, int ribOnCellIndex, int sideOnRibIndex, int ribOnSideIndex){
		int invRibIndex = 0;
		if(ribOnCellIndex == Cell::ul) invRibIndex = Cell::dr;
		else if(ribOnCellIndex == Cell::ur) invRibIndex = Cell::dl;
		else if(ribOnCellIndex == Cell::dl) invRibIndex = Cell::ur;
		else if(ribOnCellIndex == Cell::dr) invRibIndex = Cell::ul;

		Rib* rib = bindedCell->getRib(ribAxis, ribOnCellIndex);
		if(rib == NULL){
			rib = new HierarchyRib(ribAxis);
			rib->setCell(invRibIndex, bindedCell);
			bindedCell->setRib(ribAxis, ribOnCellIndex, rib);
		}
		rib->setSide(sideOnRibIndex, side);
		side->setRib(rib, ribOnSideIndex);
	}

	public: void free(){
		for(int axis=0; axis<3; axis++){
			for(int dir=0; dir<2; dir++){
				HierarchySide* side = (HierarchySide*) getSide(axis, dir);
				freeSide(axis, dir);
				setSide(axis, dir, NULL);
			}

			for(int ind=0; ind<4; ind++){
				HierarchyRib* rib = (HierarchyRib*) getRib(axis, ind);
				freeRib(axis, ind);
				setRib(axis, ind, NULL);
			}
		}

		if(isSplitted()){
			for(int i=0; i<8; i++){
				HierarchyCell* subcell = getSubCell(i);
				subcell->free();
				setSubCell(i, NULL);

			}
		}

		delete this;
	}

	private: void freeSide(int axis, int dir){
		HierarchySide* side = (HierarchySide*) getSide(axis, dir);
		if(side == NULL) return;

		if(side->getCell(dir) == NULL || this->getDeep() != ((HierarchyCell*) side->getCell(dir))->getDeep()){
			side->free();
			return;
		}

		side->setCell(1-dir, NULL);
		setCellForAllSubSides(1-dir, side, NULL);
		if(side->isSplitted()){
			if(!((HierarchyCell*) side->getCell(dir))->isSplitted()) side->setIsSplitted(false);
		}

	}

	private: void setCellForAllSubSides(int dir, HierarchySide* side, HierarchyCell* cell){
		if(!side->isSplitted() || this->isSplitted()) return;

		for(int iside=0; iside<4; iside++){
			HierarchySide* subSide = side->getSubSide(iside);
			subSide->setCell(dir, cell);
			setCellForAllSubSides(dir, subSide, cell);
		}
	}

	private: void freeRib(int axis, int ind){
		HierarchyRib* rib = (HierarchyRib*) getRib(axis, ind);
		if(rib == NULL) return;

		int cnt = 0;
		for(int i=0; i<4; i++){
			HierarchyCell* cell = (HierarchyCell*) rib->getCell(i);
			if(cell != NULL && cell != this && cell->getDeep() == this->getDeep()) cnt++;
		}

		if(cnt == 0){
			rib->free();
			return;
		}

		int vind = 0;
		if(ind == Cell::ur) vind = Cell::dl;
		else if(ind == Cell::ul) vind = Cell::dr;
		else if(ind == Cell::dr) vind = Cell::ul;
		else if(ind == Cell::dl) vind = Cell::ur;

		rib->setCell(vind, NULL);
		setCellForAllSubRibs(vind, rib, NULL);
		if(rib->isSplitted()){
			bool isThereSplittedCellNearThisOne = false;
			for(int i=0; i<4; i++){
				HierarchyCell* cell = (HierarchyCell*) rib->getCell(i);
				if(cell != NULL && cell->isSplitted()){
					isThereSplittedCellNearThisOne = true;
					break;
				}
			}

			if(!isThereSplittedCellNearThisOne) rib->setIsSplitted(false);
		}
	}

	private: void setCellForAllSubRibs(int ind, HierarchyRib* rib, HierarchyCell* cell){
		if(!rib->isSplitted() || this->isSplitted()) return;

		for(int irib=0; irib<2; irib++){
			HierarchyRib* subRib = rib->getSubRib(irib);
			subRib->setCell(ind, cell);
			setCellForAllSubRibs(ind, subRib, cell);
		}
	}
};
