#include <iostream>
using namespace std;

class HierarchyCell;
class Rib;
class Side;

class HierarchyCellArray: public CellArray{

	public: HierarchyCellArray(TaskData* td){
		HierarchyCellArray::td = td;
		int ind = 0;
		time = 0;
		dt =100;
		maxSpeed = 0;
		max_speed_cell = NULL;

		isMaxSpeedCalculated = false;

		CellArray::size = td->getN(0)*td->getN(1)*td->getN(2);
		cells = (Cell**) new HierarchyCell*[size];

		for(int ix=0; ix<td->getN(0); ix++){
			for(int iy=0; iy<td->getN(1); iy++){
				for(int iz=0; iz<td->getN(2); iz++){
					double r[3]={	td->getLl(0) + (ix+0.5)*td->getH(0), 
							td->getLl(1) + (iy+0.5)*td->getH(1), 
							td->getLl(2) + (iz+0.5)*td->getH(2)};
					int point[3] = {ix, iy, iz};
					double h[3] = {td->getH(0), td->getH(1), td->getH(2)};

					cells[ind] = new HierarchyCell(r,point,h, ind, 0);
					ind++;
				}
			}
		}

		bindSidesForRegulatCA();
cout << "bindSidesForRegulatCA has done" << endl;
		bindRibsForRegulatCA();
cout << "bindRibsForRegulatCA has done" << endl;
		markBorderCells();
cout << "markBorderCells has done" << endl;

		iterator = new HierarchyCellIterator(this);
	}

	protected: Side* defineSideType(int axis, Cell* bcell, Cell* fcell){
		return new HierarchySide(axis, bcell, fcell);
	}

	protected: Rib* defineRibType(int axis,Cell* ulcell,Cell* urcell,Cell* dlcell,Cell* drcell){
		return new HierarchyRib(axis,ulcell,urcell,dlcell,drcell);
	}

	public: Iterator* getIterator(){
		iterator->drop();
		return iterator;
	}

	public: void defineInternalBorders(){
		vector<BorderCondition*> borders = getBorderConditions();
		for(int ind=0; ind < borders.size(); ind++){
			BorderCondition* bc = borders.at(ind);

		for(HierarchyCellIterator cit = HierarchyCellIterator(this); cit.hasNext();){
			HierarchyCell* cell = (HierarchyCell*) cit.next();
				if(bc->isBorder(cell)) bc->markCell(cell);
			}
		}
	}

	public: void splitCellArray(CASplitCondition* splitCondition){
		for(HierarchyCellIterator cit = HierarchyCellIterator(this); cit.hasNext();){
			HierarchyCell* hcell = (HierarchyCell*) cit.next();

			if(splitCondition->doSplit(hcell)){
				splitCell(hcell);
			}
		}
	}

	public: void splitCell(HierarchyCell* hcell){
		splitCell(hcell, true);
	}

	public: void splitCell(HierarchyCell* hcell, bool doInterpolate){
		if(hcell->isSplitted()) return;
//cout << "splitCell: " << hcell->getPath() << endl;
		if(doInterpolate) splitNearCells(hcell);
//cout << "splitCellOnSubCells" << endl;
		splitCellOnSubCells(hcell);
//cout << "spitSideOnSubSides" << endl;
		spitSideOnSubSides(hcell);
//cout << "splitBorderRibsOnSubRibs" << endl;
		splitBorderRibsOnSubRibs(hcell);
//cout << "interpolateValuesInsideCell" << endl;
		if(doInterpolate) interpolateValuesInsideCell(hcell);
//cout << "markSubCells" << endl;
		markSubCells(hcell);

		hcell->setIsSplitted(true);
		for(int axis=0; axis<3; axis++){
			for(int dir=0; dir<2; dir++){
				((HierarchySide*) hcell->getSide(axis, dir))->setIsSplitted(true);
			}

			for(int irib=0; irib<4; irib++){
				((HierarchyRib*) hcell->getRib(axis, irib))->setIsSplitted(true);
			}
		}
	}

	public: void splitNearCells(HierarchyCell* hcell){
		for(int axis=0; axis<3; axis++){
			for(int dir=0; dir<2; dir++){
				HierarchyCell* ncell = ((HierarchyCell*) hcell->getCell(axis, dir)); 
				if(hcell->getDeep() > ncell->getDeep()){
					splitCell(ncell);
				}
			}
		}
	}
/*
	public: void mergeSubCellsUnderCell(HierarchyCell* hcell){
		if(hcell->isSplitted()){
			for(int i=0; i<8; i++){
				mergeSubCellsUnderCell(hcell->getSubCell(i));
			}
			mergeSides(hcell);
			mergeRibs(hcell);
			mergeSubCells(hcell);
		}
	}

	public: void mergeSides(HierarchyCell* hcell){
		for(int axis=0; axis<3; axis++){
			for(int dir=0; dir<2; dir++){
				for(int ix=0; ix<2; ix++){
					for(int iy=0; iy<2; iy++){
						int* point = new int[3];
						point[axis] = dir;
						point[(axis+1)%3] = ix;
						point[(axis+2)%3] = iy;
						
						HierarchyCell* subcell =  hcell->getSubCell(point);
						HierarchyCell* nearCell = (HierarchyCell*) subcell->getCell(axis, dir);

						if(subcell->getDeep() <= nearCell->getDeep()){
							for(SubCellIterator it = SubCellIterator(nearCell); it.hasNext();){
								it.next()->getSide(axis, dir*2-1)->setCell(dir*2-1, hcell);
							}
						} else{
							subcell->getSide(axis, dir)->setCell(dir, NULL);
							subcell->getSide(axis, dir)->setCell(dir*2-1, NULL);
						}

						delete[] point;
					}
				}
			}
		}
	}

	public: void mergeRibs(HierarchyCell* hcell){
		for(int axis=0; axis<3; axis++){
			for(int dir=0; dir<2; dir++){
				for(int ix=0; ix<2; ix++){
					for(int iy=0; iy<2; iy++){
						int* point = new int[3];
						point[axis] = dir;
						point[(axis+1)%3] = ix;
						point[(axis+2)%3] = iy;
						
						HierarchyCell* subcell =  hcell->getSubCell(point);
						HierarchyCell* nearCell = (HierarchyCell*) subcell->getCell(axis, dir);

						if(subcell->getDeep() == nearCell->getDeep()){
							for(SubCellIterator it = SubCellIterator(nearCell); it.hasNext();){
								HierarchyCell* cell = (HierarchyCell*) it.next();
								Side* side = cell->getSide(axis, dir*2-1);
								
								int rib1_0 = 0;
								int rib1_1 = 0;
								int rib2_0 = 0;
								int rib2_1 = 0;
								if(dir == Cell::FORWARD_SIDE){
									rib1_0 = Cell::dr;
									rib1_1 = Cell::dl;
									rib2_0 = Cell::ul;
									rib2_1 = Cell::dl;
								} else if(dir == Cell::BACKWARD_SIDE){
									rib1_0 = Cell::ur;
									rib1_1 = Cell::ul;
									rib2_0 = Cell::ur;
									rib2_1 = Cell::dr;
								}

								side->getRib(Side::UP_RIB)->setCell(rib1_1, NULL);
								if(!isBorderRibOnCell((axis+2)%3, Cell::FORWARD_SIDE, nearCell, cell))
									side->getRib(Side::UP_RIB)->setCell(rib1_0, NULL);

								side->getRib(Side::DOWN_RIB)->setCell(rib1_0, NULL);
								if(!isBorderRibOnCell((axis+2)%3, Cell::BACKWARD_SIDE, nearCell, cell)) 
									side->getRib(Side::DOWN_RIB)->setCell(rib1_1, NULL);

								side->getRib(Side::RIGHT_RIB)->setCell(rib2_1, NULL);
								if(!isBorderRibOnCell((axis+1)%3, Cell::FORWARD_SIDE, nearCell, cell)) 
									side->getRib(Side::RIGHT_RIB)->setCell(rib2_0, NULL);

								side->getRib(Side::LEFT_RIB)->setCell(rib2_0, NULL);
								if(!isBorderRibOnCell((axis+1)%3, Cell::BACKWARD_SIDE, nearCell, cell)) 
									side->getRib(Side::LEFT_RIB)->setCell(rib2_1, NULL);
							}
						}
						delete[] point;
					}
				}
			}
		}
	}

	private: void mergeSubCells(HierarchyCell* hcell){
		for(int i=0; i<8; i++){
			Cell* subCell = hcell->getSubCell(i);

			for(int axis=0; axis<3; axis++){
				for(int dir=0; dir<2; dir++){
					subCell->setSide(axis, dir, NULL);
				}
				for(int irib=0; irib<4; irib++){
					subCell->setRib(axis, irib, NULL);
				}
			}
		}
		for(int axis=0; axis<3; axis++){
			for(int dir=0; dir<2; dir++){
				((HierarchySide*) hcell->getSide(axis, dir))->deleteSubsides();
			}
		}
		for(int axis=0; axis<3; axis++){
			for(int irib=0; irib<4; irib++){
				((HierarchyRib*) hcell->getRib(axis, irib))->deleteSubribs();
			}
		}

		hcell->deleteSubcells();
	}
*/
	private: bool isBorderRibOnCell(int axis, int dir, HierarchyCell* cell, HierarchyCell* subCell){
		if(subCell->getPoint(axis) == dir && subCell->getHParent()->getDeep() == cell->getDeep()){
			return true;
		} else if(subCell->getPoint(axis) == dir && subCell->getHParent()->getDeep() > cell->getDeep()){
			return isBorderRibOnCell(axis, dir, cell, subCell->getHParent());
		} else return false;
	}

	private:void splitCellOnSubCells(HierarchyCell* hcell){

//		HierarchyCell** subcells = new HierarchyCell*[8];
		int i = 0;
		int point[3];
		for(point[0]=0;point[0]<2;point[0]++)
			for(point[1]=0;point[1]<2;point[1]++)
				for(point[2]=0;point[2]<2;point[2]++){
					HierarchyCell* subcell = makeSubCell(hcell, point, i);
					subcell->setParent(hcell);
					hcell->setSubCell(i, subcell);
					i++;
				}
//		((HierarchyCell*) hcell)->setSubCells(subcells);
	}

	private: void spitSideOnSubSides(HierarchyCell* cell){
		HierarchyCell* hcell = (HierarchyCell*) cell;

		for(int axis=0; axis<3; axis++){
			for(int i=0; i<8; i++){
				Cell* bcell = hcell->getSubCell(i);
				if(bcell->getPoint(axis) == 0){
					int point[3] = {bcell->getPoint(0), bcell->getPoint(1), bcell->getPoint(2)};
					point[axis] = 1;
					Cell* fcell = hcell->getSubCell(point);
					defineSide(bcell, fcell, axis);
				}
			}
		}

		for(int axis=0; axis<3; axis++){
			for(int i=0; i<8; i++){
				Cell* bcell = hcell->getSubCell(i);
				if(bcell->getPoint(axis) == 0){
					int point[3] = {bcell->getPoint(0), bcell->getPoint(1), bcell->getPoint(2)};					
					if(point[(axis+2)%3] == 0){
						Cell* dlcell = bcell;
						point[axis] = 1;
						Cell* drcell = hcell->getSubCell(point);
						point[(axis+2)%3] = 1;
						Cell* urcell = hcell->getSubCell(point);
						point[axis] = 0;
						Cell* ulcell = hcell->getSubCell(point);

						defineRibOnFourCells((axis+1)%3, urcell, ulcell, drcell, dlcell, 
							dlcell->getSide(axis, Cell::FORWARD_SIDE),
							ulcell->getSide(axis, Cell::FORWARD_SIDE),
							dlcell->getSide((axis+2)%3, Cell::FORWARD_SIDE),
							drcell->getSide((axis+2)%3, Cell::FORWARD_SIDE));
					}
				}
			}
		}

		for(int axis=0; axis<3; axis++){
			if(hcell->getCell(axis, Cell::FORWARD_SIDE)!=NULL && ((HierarchyCell*) hcell->getCell(axis,Cell::FORWARD_SIDE))->isSplitted()){
				HierarchyCell* fhcell = (HierarchyCell*) hcell->getCell(axis, Cell::FORWARD_SIDE);
				int point[3] = {0,0,0};
				for(int m=0; m<2; m++)
					for(int n=0; n<2; n++){
						point[(axis+1)%3] = n;
						point[(axis+2)%3] = m;

						point[axis] = 0;
						HierarchyCell* fcell = (HierarchyCell*) fhcell->getSubCell(point);
						point[axis] = 1;
						HierarchyCell* bcell = (HierarchyCell*) hcell->getSubCell(point);

						bindSides(bcell, fcell, axis, fcell->getSide(axis, Cell::BACKWARD_SIDE));
					}
			}
			else {
				HierarchySide* hside = (HierarchySide*) hcell->getSide(axis,Cell::FORWARD_SIDE);

				int j = 0;
				for(int i=0; i<8; i++){
					HierarchyCell* bcell = (HierarchyCell*) ((HierarchyCell*) hcell)->getSubCell(i);
					if(bcell->getPoint(axis) == 1){
						HierarchySide* side = (HierarchySide*) defineHierarchySide(hcell, bcell, axis, Cell::FORWARD_SIDE);
						side->setParent(hside);
						hside->setSubSide(j++, side);
					}
				}
//				hside->setIsSplitted(true);
			}

			if(hcell->getCell(axis,Cell::BACKWARD_SIDE)!=NULL && ((HierarchyCell*) hcell->getCell(axis,Cell::BACKWARD_SIDE))->isSplitted()){
				HierarchyCell* bhcell = (HierarchyCell*) hcell->getCell(axis, Cell::BACKWARD_SIDE);
				int point[3] = {0,0,0};
				for(int m=0; m<2; m++)
					for(int n=0; n<2; n++){
						point[(axis+1)%3] = n;
						point[(axis+2)%3] = m;

						point[axis] = 1;
						HierarchyCell* bcell = (HierarchyCell*) bhcell->getSubCell(point);
						point[axis] = 0;
						HierarchyCell* fcell = (HierarchyCell*) ((HierarchyCell*) hcell)->getSubCell(point);

						bindSides(bcell, fcell, axis, bcell->getSide(axis, Cell::FORWARD_SIDE));
					}
			}
			else {
				HierarchySide* hside = (HierarchySide*) hcell->getSide(axis,Cell::BACKWARD_SIDE);

				int j = 0;
				for(int i=0; i<8; i++){
					HierarchyCell* bcell = (HierarchyCell*) ((HierarchyCell*) hcell)->getSubCell(i);
					if(bcell->getPoint(axis) == 0){
						HierarchySide* side = (HierarchySide*) defineHierarchySide(hcell, bcell, axis, Cell::BACKWARD_SIDE);
						side->setParent(hside);
						hside->setSubSide(j++, side);
					}
				}
//				hside->setIsSplitted(true);
			}
		}

		for(int axis=0; axis<3; axis++){
			if(hcell->getCell(axis,Cell::FORWARD_SIDE) != NULL && ((HierarchyCell*) hcell->getCell(axis,Cell::FORWARD_SIDE))->isSplitted()){
				HierarchyCell* fhcell = (HierarchyCell*) hcell->getCell(axis, Cell::FORWARD_SIDE);
				int point[3] = {0,0,0};
				for(int i=0; i<2; i++){
					point[(axis+1)%3] = i;
					point[(axis+2)%3] = 0;
					point[axis] = 0;
					Cell* drcell = fhcell->getSubCell(point);
					point[axis] = 1;
					Cell* dlcell =  hcell->getSubCell(point);
					Cell* ulcell = dlcell->getCell((axis+2)%3, Cell::FORWARD_SIDE);
					Cell* urcell = drcell->getCell((axis+2)%3, Cell::FORWARD_SIDE);

					defineRibOnFourCells((axis+1)%3, urcell, ulcell, drcell, dlcell, 
						dlcell->getSide(axis, Cell::FORWARD_SIDE),
						ulcell->getSide(axis, Cell::FORWARD_SIDE),
						dlcell->getSide((axis+2)%3, Cell::FORWARD_SIDE),
						drcell->getSide((axis+2)%3, Cell::FORWARD_SIDE));

					point[(axis+2)%3] = i;
					point[(axis+1)%3] = 0;
					point[axis] = 0;
					ulcell = fhcell->getSubCell(point);
					point[axis] = 1;
					dlcell =  hcell->getSubCell(point);
					urcell = ulcell->getCell((axis+1)%3, Cell::FORWARD_SIDE);
					drcell = dlcell->getCell((axis+1)%3, Cell::FORWARD_SIDE);

					defineRibOnFourCells((axis+2)%3, urcell, ulcell, drcell, dlcell, 
						dlcell->getSide((axis+1)%3, Cell::FORWARD_SIDE),
						ulcell->getSide((axis+1)%3, Cell::FORWARD_SIDE),
						dlcell->getSide(axis, Cell::FORWARD_SIDE),
						drcell->getSide(axis, Cell::FORWARD_SIDE));
				}
			}
			else {
				for(int i=0; i<2; i++){
					int point[3] = {0,0,0};

					point[axis] = 1;
					point[(axis+1)%3] = i;
					Cell* dlcell = hcell->getSubCell(point);
					point[(axis+2)%3] = 1;
					Cell* ulcell = hcell->getSubCell(point);

					defineRibOnFourCells((axis+1)%3, NULL, ulcell, NULL, dlcell, 
						dlcell->getSide(axis, Cell::FORWARD_SIDE),
						ulcell->getSide(axis, Cell::FORWARD_SIDE),
						dlcell->getSide((axis+2)%3, Cell::FORWARD_SIDE),
						NULL);

					point[axis] = 1;
					point[(axis+2)%3] = i;
					point[(axis+1)%3] = 0;
					dlcell = hcell->getSubCell(point);
					point[(axis+1)%3] = 1;
					Cell* drcell = hcell->getSubCell(point);

					defineRibOnFourCells((axis+2)%3, NULL, NULL, drcell, dlcell, 
						dlcell->getSide((axis+1)%3, Cell::FORWARD_SIDE),
						NULL,
						dlcell->getSide(axis, Cell::FORWARD_SIDE),
						drcell->getSide(axis, Cell::FORWARD_SIDE));
				}
			}

			if(hcell->getCell(axis,Cell::BACKWARD_SIDE)!=NULL && ((HierarchyCell*) hcell->getCell(axis,Cell::BACKWARD_SIDE))->isSplitted()){
				HierarchyCell* bhcell = (HierarchyCell*) hcell->getCell(axis, Cell::BACKWARD_SIDE);

				int point[3] = {0,0,0};
				for(int i=0; i<2; i++){
					point[(axis+1)%3] = i;
					point[(axis+2)%3] = 0;
					point[axis] = 0;
					Cell* drcell =  hcell->getSubCell(point);
					point[axis] = 1;
					Cell* dlcell =  bhcell->getSubCell(point);
					Cell* ulcell = dlcell->getCell((axis+2)%3, Cell::FORWARD_SIDE);
					Cell* urcell = drcell->getCell((axis+2)%3, Cell::FORWARD_SIDE);

					defineRibOnFourCells((axis+1)%3, urcell, ulcell, drcell, dlcell, 
						dlcell->getSide(axis, Cell::FORWARD_SIDE),
						ulcell->getSide(axis, Cell::FORWARD_SIDE),
						dlcell->getSide((axis+2)%3, Cell::FORWARD_SIDE),
						drcell->getSide((axis+2)%3, Cell::FORWARD_SIDE));

					point[(axis+2)%3] = i;
					point[(axis+1)%3] = 0;
					point[axis] = 0;
					ulcell =  hcell->getSubCell(point);
					point[axis] = 1;
					dlcell = bhcell->getSubCell(point);
					urcell = ulcell->getCell((axis+1)%3, Cell::FORWARD_SIDE);
					drcell = dlcell->getCell((axis+1)%3, Cell::FORWARD_SIDE);

					defineRibOnFourCells((axis+2)%3, urcell, ulcell, drcell, dlcell, 
						dlcell->getSide((axis+1)%3, Cell::FORWARD_SIDE),
						ulcell->getSide((axis+1)%3, Cell::FORWARD_SIDE),
						dlcell->getSide(axis, Cell::FORWARD_SIDE),
						drcell->getSide(axis, Cell::FORWARD_SIDE));
				}
			}
			else {
				for(int i=0; i<2; i++){
					int point[3] = {0,0,0};

					point[axis] = 0;
					point[(axis+1)%3] = i;
					Cell* drcell = hcell->getSubCell(point);
					point[(axis+2)%3] = 1;
					Cell* urcell = hcell->getSubCell(point);

					defineRibOnFourCells((axis+1)%3, urcell, NULL, drcell, NULL,
						drcell->getSide(axis, Cell::BACKWARD_SIDE),
						urcell->getSide(axis, Cell::BACKWARD_SIDE),
						NULL,
						urcell->getSide((axis+2)%3, Cell::BACKWARD_SIDE));

					point[axis] = 0;
					point[(axis+2)%3] = i;
					point[(axis+1)%3] = 0;
					Cell* ulcell = hcell->getSubCell(point);
					point[(axis+1)%3] = 1;
					urcell = hcell->getSubCell(point);

					defineRibOnFourCells((axis+2)%3, urcell, ulcell, NULL, NULL, 
						NULL,
						ulcell->getSide((axis+1)%3, Cell::FORWARD_SIDE),
						ulcell->getSide(axis, Cell::BACKWARD_SIDE),
						urcell->getSide(axis, Cell::BACKWARD_SIDE));
				}
			}
		}
	}



	private: void splitBorderRibsOnSubRibs(HierarchyCell* hcell){
		for(int axis=0; axis<3; axis++){
			for(int i=0;i<4;i++){
				HierarchyRib* rib = (HierarchyRib*) hcell->getRib(axis, i);
				bool wasSplitted = false;

				if(!rib->isSplitted()){
					rib->setSubRib(Cell::BACKWARD_SIDE, new HierarchyRib(axis));
					rib->setSubRib(Cell::FORWARD_SIDE, new HierarchyRib(axis));

					((HierarchyRib*) rib->getSubRib(Cell::BACKWARD_SIDE))->setParent(rib);
					((HierarchyRib*) rib->getSubRib(Cell::FORWARD_SIDE))->setParent(rib);

//					rib->setIsSplitted(true);
				}
				else wasSplitted = true;

				int point[3];
				for(int j=0; j<2; j++){
					point[axis]=j;
					Cell* dlcell = NULL;
					Cell* urcell = NULL;
					Cell* ulcell = NULL;
					Cell* drcell = NULL;

					if(i==Cell::ur){
						point[(axis+1)%3]=1; point[(axis+2)%3]=1;
						dlcell = ((HierarchyCell*) hcell)->getSubCell(point);
						if(wasSplitted){
							ulcell = rib->getSubRib(j)->getCell(Cell::ul);
							drcell = rib->getSubRib(j)->getCell(Cell::dr);
							urcell = rib->getSubRib(j)->getCell(Cell::ur);
						} else {
							ulcell = dlcell->getCell((axis+1)%3, Cell::FORWARD_SIDE);
							drcell = dlcell->getCell((axis+2)%3, Cell::FORWARD_SIDE);
							if(drcell != NULL) urcell = drcell->getCell((axis+1)%3, Cell::FORWARD_SIDE);
						}
						dlcell->setRib(axis, Cell::ur, rib->getSubRib(j));

						defineRibOnFourCells(axis, urcell, ulcell, drcell, dlcell,
							dlcell->getSide((axis+2)%3, Cell::FORWARD_SIDE),
							NULL,
							dlcell->getSide((axis+1)%3, Cell::FORWARD_SIDE),
							NULL);
					} else	if(i==Cell::dr){
						point[(axis+1)%3]=0; point[(axis+2)%3]=1;
						ulcell = ((HierarchyCell*) hcell)->getSubCell(point);
						if(wasSplitted){
							dlcell = rib->getSubRib(j)->getCell(Cell::dl);
							urcell = rib->getSubRib(j)->getCell(Cell::ur);
							drcell = rib->getSubRib(j)->getCell(Cell::dr);
						} else {
							dlcell = ulcell->getCell((axis+1)%3, Cell::BACKWARD_SIDE);
							urcell = ulcell->getCell((axis+2)%3, Cell::FORWARD_SIDE);
							if(urcell != NULL) drcell = urcell->getCell((axis+1)%3, Cell::BACKWARD_SIDE);
						}
						ulcell->setRib(axis, Cell::dr, rib->getSubRib(j));

						defineRibOnFourCells(axis, urcell, ulcell, drcell, dlcell,
							NULL,
							ulcell->getSide((axis+2)%3, Cell::FORWARD_SIDE),
							ulcell->getSide((axis+1)%3, Cell::BACKWARD_SIDE),
							NULL);
					} else	if(i==Cell::ul){
						point[(axis+1)%3]=1; point[(axis+2)%3]=0;
						drcell = ((HierarchyCell*) hcell)->getSubCell(point);
						if(wasSplitted){
							urcell = rib->getSubRib(j)->getCell(Cell::ur);
							dlcell = rib->getSubRib(j)->getCell(Cell::dl);
							ulcell = rib->getSubRib(j)->getCell(Cell::ul);
						} else {
							urcell = drcell->getCell((axis+1)%3, Cell::FORWARD_SIDE);
							dlcell = drcell->getCell((axis+2)%3, Cell::BACKWARD_SIDE);
							if(dlcell != NULL) ulcell = dlcell->getCell((axis+1)%3, Cell::FORWARD_SIDE);
						}
						drcell->setRib(axis, Cell::ul, rib->getSubRib(j));

						defineRibOnFourCells(axis, urcell, ulcell, drcell, dlcell,
							drcell->getSide((axis+2)%3, Cell::BACKWARD_SIDE),
							NULL,
							NULL,
							drcell->getSide((axis+1)%3, Cell::FORWARD_SIDE));
					} else	if(i==Cell::dl){
						point[(axis+1)%3]=0; point[(axis+2)%3]=0;
						urcell = ((HierarchyCell*) hcell)->getSubCell(point);
						if(wasSplitted){
							drcell = rib->getSubRib(j)->getCell(Cell::dr);
							ulcell = rib->getSubRib(j)->getCell(Cell::ul);
							dlcell = rib->getSubRib(j)->getCell(Cell::dl);
						} else {
							drcell = urcell->getCell((axis+1)%3, Cell::BACKWARD_SIDE);
							ulcell = urcell->getCell((axis+2)%3, Cell::BACKWARD_SIDE);
							if(ulcell != NULL) dlcell = ulcell->getCell((axis+1)%3, Cell::BACKWARD_SIDE);
						}
						urcell->setRib(axis, Cell::dl, rib->getSubRib(j));

						defineRibOnFourCells(axis, urcell, ulcell, drcell, dlcell,
							NULL,
							urcell->getSide((axis+2)%3, Cell::BACKWARD_SIDE),
							NULL,
							urcell->getSide((axis+1)%3, Cell::BACKWARD_SIDE));
					}
				}
			}
		}
	}

	private: HierarchyCell* makeSubCell(Cell* rCell, int point[], int ind){
		double r[3];
		double h[3];

		for(int axis=0; axis<3; axis++){
			r[axis] = rCell->getR(axis);
			h[axis] = rCell->getH(axis);

			r[axis] = r[axis] + (point[axis]*2 - 1)*h[axis]/4;
			h[axis] = h[axis] / 2;
		}
		int deep = ((HierarchyCell*) rCell)->getDeep() + 1;
		HierarchyCell* subcell = new HierarchyCell(r,point,h, ind, deep);
		return subcell;
	}

	protected: Side* defineHierarchySide(HierarchyCell* hcell, Cell* subcell, int axis, int direction){
		Side* side = NULL;

		if(direction == Cell::FORWARD_SIDE)
			side = new HierarchySide(axis, subcell, hcell->getCell(axis, direction));
		else if(direction == Cell::BACKWARD_SIDE) 
			side = new HierarchySide(axis, hcell->getCell(axis, direction), subcell);

		subcell->setSide(axis, direction, side);
		return side;
	}

	protected: void bindSides(Cell* bcell, Cell* fcell, int axis, Side* side){
		HierarchyCell* hf = (HierarchyCell*) fcell;
		HierarchyCell* hb = (HierarchyCell*) bcell;

		bcell->setSide(axis, Cell::FORWARD_SIDE, side);
		fcell->setSide(axis, Cell::BACKWARD_SIDE, side);

		side->setCell(Cell::FORWARD_SIDE, fcell);
		side->setCell(Cell::BACKWARD_SIDE, bcell);
	}

	private: void defineRibOnFourCells(int ribAxis,Cell* urcell,Cell* ulcell,Cell* drcell,Cell* dlcell, 
				Side* upSide, Side* downSide, Side* rightSide, Side* leftSide){
		Rib* rib = NULL;
		int maxdeep = 0;

		if(urcell != NULL && ((HierarchyCell*) urcell)->getDeep() > maxdeep) maxdeep = ((HierarchyCell*) urcell)->getDeep();
		if(ulcell != NULL && ((HierarchyCell*) ulcell)->getDeep() > maxdeep) maxdeep = ((HierarchyCell*) ulcell)->getDeep();
		if(drcell != NULL && ((HierarchyCell*) drcell)->getDeep() > maxdeep) maxdeep = ((HierarchyCell*) drcell)->getDeep();
		if(dlcell != NULL && ((HierarchyCell*) dlcell)->getDeep() > maxdeep) maxdeep = ((HierarchyCell*) dlcell)->getDeep();

		if(urcell != NULL && urcell->getRib(ribAxis, Cell::dl) != NULL && ((HierarchyCell*) urcell)->getDeep() == maxdeep)
			rib = urcell->getRib(ribAxis, Cell::dl);
		else if(ulcell != NULL && ulcell->getRib(ribAxis, Cell::dr) != NULL && ((HierarchyCell*) ulcell)->getDeep() == maxdeep)
			rib = ulcell->getRib(ribAxis, Cell::dr);
		else if(drcell != NULL && drcell->getRib(ribAxis, Cell::ul) != NULL && ((HierarchyCell*) drcell)->getDeep() == maxdeep)
			rib = drcell->getRib(ribAxis, Cell::ul);
		else if(dlcell != NULL && dlcell->getRib(ribAxis, Cell::ur) != NULL && ((HierarchyCell*) dlcell)->getDeep() == maxdeep)
			rib = dlcell->getRib(ribAxis, Cell::ur);
		else rib = new HierarchyRib(ribAxis, ulcell, urcell, dlcell, drcell);

		if(urcell != NULL && ((HierarchyCell*) urcell)->getDeep() == maxdeep) urcell->setRib(ribAxis, Cell::dl, rib);
		if(ulcell != NULL && ((HierarchyCell*) ulcell)->getDeep() == maxdeep) ulcell->setRib(ribAxis, Cell::dr, rib);
		if(drcell != NULL && ((HierarchyCell*) drcell)->getDeep() == maxdeep) drcell->setRib(ribAxis, Cell::ul, rib);
		if(dlcell != NULL && ((HierarchyCell*) dlcell)->getDeep() == maxdeep) dlcell->setRib(ribAxis, Cell::ur, rib);

		if(urcell != NULL) rib->setCell(Cell::ur, urcell);
		if(ulcell != NULL) rib->setCell(Cell::ul, ulcell);
		if(drcell != NULL) rib->setCell(Cell::dr, drcell);
		if(dlcell != NULL) rib->setCell(Cell::dl, dlcell);

		if(rightSide != NULL) rightSide->setRib(rib, Side::RIGHT_RIB);
		if(leftSide != NULL) leftSide->setRib(rib, Side::LEFT_RIB);
		if(upSide != NULL) upSide->setRib(rib, Side::UP_RIB);
		if(downSide != NULL) downSide->setRib(rib, Side::DOWN_RIB);

		if(rightSide != NULL) rib->setSide(Rib::DOWN_SIDE, rightSide);
		if(leftSide != NULL) rib->setSide(Rib::UP_SIDE, leftSide);
		if(upSide != NULL) rib->setSide(Rib::LEFT_SIDE, upSide);
		if(downSide != NULL) rib->setSide(Rib::RIGHT_SIDE, downSide);
	}

	private: void interpolateValuesInsideCell(HierarchyCell* hcell){
		Interpolater::interpolateFieldsOnSubCells(hcell);
		Interpolater::interpolateInsideGDparameters(hcell);
	}

	private: void markSubCells(HierarchyCell* hcell){
		for(int i=0; i<8; i++){
			Cell* cell = hcell->getSubCell(i);
			cell->setMark(Cell::INTERNAL_CELL_MARK);
			for(int axis=0; axis<3; axis++){
				for(int dir=0; dir<2; dir++){
					cell->getSide(axis, dir)->setMark(Side::INTERNAL_SIDE_MARK);
				}
				for(int i=0; i<4; i++){
					cell->getRib(axis, i)->setMark(Rib::INTERNAL_RIB_MARK);
				}
			}
		}

		for(int axis=0; axis<3; axis++){
			for(int dir=0; dir<2; dir++){
				HierarchySide* side = (HierarchySide*) hcell->getSide(axis, dir);
				if(side->getMark() != Side::INTERNAL_SIDE_MARK){
					for(int i=0; i<4; i++){
						Side* subside = side->getSubSide(i);
						subside->setMark(side->getMark());

						subside->getRib(Side::UP_RIB)->setMark(side->getRib(Side::UP_RIB)->getMark());
						subside->getRib(Side::DOWN_RIB)->setMark(side->getRib(Side::DOWN_RIB)->getMark());
						subside->getRib(Side::RIGHT_RIB)->setMark(side->getRib(Side::RIGHT_RIB)->getMark());
						subside->getRib(Side::LEFT_RIB)->setMark(side->getRib(Side::LEFT_RIB)->getMark());
					}

					int point[3] = {0,0,0};
					point[axis] = dir;
					for(int i=0; i<2; i++){
						for(int j=0; j<2; j++){
							point[(axis+1)%3] = i; point[(axis+1)%3] = j;
							Cell* subcell = hcell->getSubCell(point);
							subcell->setMark(hcell->getMark());
						}
					}
				}
			}
		}
	}

	public: void calculateIncrements(){
cout << "calculateIncrements";
clock_t start = clock();

		for(HierarchyCellIterator cit = HierarchyCellIterator(this); cit.hasNext();){
			HierarchyCell* cell = (HierarchyCell*) cit.next();
			if(cell->getMark() != Cell::INTERNAL_CELL_MARK || cell->isSplitted()) continue;

			double dRho = 0;
			double dMx = 0;
			double dMy = 0;
			double dMz = 0;
			double dBx = 0;
			double dBy = 0;
			double dBz = 0;
			double de = 0;

			for(int axis=0;axis<3;axis++){
				dRho+= (cell->getSide(axis,Cell::FORWARD_SIDE)->getFlow(0) 
					- cell->getSide(axis,Cell::BACKWARD_SIDE)->getFlow(0))/cell->getH(axis);
				dMx += (cell->getSide(axis,Cell::FORWARD_SIDE)->getFlow(1) 
					- cell->getSide(axis,Cell::BACKWARD_SIDE)->getFlow(1))/cell->getH(axis);
				dMy += (cell->getSide(axis,Cell::FORWARD_SIDE)->getFlow(2) 
					- cell->getSide(axis,Cell::BACKWARD_SIDE)->getFlow(2))/cell->getH(axis);
				dMz += (cell->getSide(axis,Cell::FORWARD_SIDE)->getFlow(3) 
					- cell->getSide(axis,Cell::BACKWARD_SIDE)->getFlow(3))/cell->getH(axis);
				de  += (cell->getSide(axis,Cell::FORWARD_SIDE)->getFlow(4) 
					- cell->getSide(axis,Cell::BACKWARD_SIDE)->getFlow(4))/cell->getH(axis);

				dBx +=  (cell->getSide(axis,Cell::FORWARD_SIDE)->getFlow(5) 
					- cell->getSide(axis,Cell::BACKWARD_SIDE)->getFlow(5))/cell->getH(axis);
				dBy +=  (cell->getSide(axis,Cell::FORWARD_SIDE)->getFlow(6) 
					- cell->getSide(axis,Cell::BACKWARD_SIDE)->getFlow(6))/cell->getH(axis);
				dBz +=  (cell->getSide(axis,Cell::FORWARD_SIDE)->getFlow(7) 
					- cell->getSide(axis,Cell::BACKWARD_SIDE)->getFlow(7))/cell->getH(axis);

				cell->getSide(axis, Cell::FORWARD_SIDE)->setIsFlowCalculated(false);
				cell->getSide(axis, Cell::BACKWARD_SIDE)->setIsFlowCalculated(false);
			}

			dRho = -dRho * dt;
			dMx = -dMx * dt;
			dMy = -dMy * dt;
			dMz = -dMz * dt;
			dBx = -dBx * dt;
			dBy = -dBy * dt;
			dBz = -dBz * dt;
			de = -de * dt;

			double dEk =
				(
					pow(cell->getMomentum(0) + dMx,2) 
					+ pow(cell->getMomentum(1) + dMy,2) 
					+ pow(cell->getMomentum(2) + dMz,2)
				) / (cell->getDensity() + dRho) / 2.0
				-
				(
					pow(cell->getMomentum(0),2) 
					+ pow(cell->getMomentum(1),2) 
					+ pow(cell->getMomentum(2),2)
				) / cell->getDensity() / 2.0;

			double dEm = 
				(
					(
						  pow(0.5*cell->getSide(0, 0)->getB() + 0.5*cell->getSide(0, 1)->getB() + dBx, 2)
						+ pow(0.5*cell->getSide(1, 0)->getB() + 0.5*cell->getSide(1, 1)->getB() + dBy, 2)
						+ pow(0.5*cell->getSide(2, 0)->getB() + 0.5*cell->getSide(2, 1)->getB() + dBz, 2)
					)
					-
					(
						  pow(0.5*cell->getSide(0, 0)->getB() + 0.5*cell->getSide(0, 1)->getB(), 2)
						+ pow(0.5*cell->getSide(1, 0)->getB() + 0.5*cell->getSide(1, 1)->getB(), 2)
						+ pow(0.5*cell->getSide(2, 0)->getB() + 0.5*cell->getSide(2, 1)->getB(), 2)
					)
				
				) / Constants::Pi / 8.0;

			cell->setdDensity(dRho);
			cell->setdMomentum(dMx, 0);
			cell->setdMomentum(dMy, 1);
			cell->setdMomentum(dMz, 2);
			cell->setdTermEnergy(de - dEk - dEm);
		}		

cout.setf(ios::showpoint,ios::floatfield); cout.precision(3);
cout << "\t" << ((double)(clock()-start))/CLOCKS_PER_SEC << endl;
cout.setf(ios::scientific,ios::floatfield); cout.precision(10);	
	}


	public: void update(){
cout << "update";
clock_t start = clock();

		for(HierarchyCellIterator cit = HierarchyCellIterator(this); cit.hasNext();){
			HierarchyCell* cell = (HierarchyCell*) cit.next();

double oldT = cell->getT();

			if(cell->getMark() == Cell::INTERNAL_CELL_MARK && !cell->isSplitted()){
				cell->setDensity(cell->getDensity() + cell->getdDensity());
				cell->setTermEnergy(cell->getTermEnergy() + cell->getdTermEnergy());
			}
			cell->setdDensity(0);
			cell->setdTermEnergy(0);

			for(int axis=0; axis<3; axis++){
				if(cell->getMark() == Cell::INTERNAL_CELL_MARK && !cell->isSplitted())
					cell->setMomentum(cell->getMomentum(axis) + cell->getdMomentum(axis), axis);
				cell->setdMomentum(0, axis);

				for(int dir = 0; dir<2; dir++){
					Side* side = cell->getSide(axis, dir);
					if(side->getMark() == Side::INTERNAL_SIDE_MARK 
					&& !((HierarchySide*) side)->isSplitted())
						side->setB(side->getB() + side->getdB());
					side->setdB(0);
				}

				for(int irib = 0; irib < 4; irib++){
					Rib* rib = cell->getRib(axis, irib);
					if(rib->getMark() == Rib::INTERNAL_RIB_MARK 
					&& !((HierarchyRib*) rib)->isSplitted())
						rib->setE(rib->getE() + rib->getdE());
					rib->setdE(0);
				}
			}

double newT = cell->getT();
double dT = newT - oldT;
if(cell->getRadius() < 4 * Constants::Re && dT < -10000) 
	cout << "dT = " << dT << " ::\t" << cell->getPath() << "\t||\t[" << cell->getR(0)/Constants::Re << ", " << cell->getR(1)/Constants::Re << ", " << cell->getR(2)/Constants::Re << "]" << endl;
		}

		isMaxSpeedCalculated = false;

cout.setf(ios::showpoint,ios::floatfield); cout.precision(3);
cout << "\t" << ((double)(clock()-start))/CLOCKS_PER_SEC << endl;
cout.setf(ios::scientific,ios::floatfield); cout.precision(10);	
	}

	protected: void markBorderCells(){
		for(HierarchyCellIterator cit = HierarchyCellIterator(this); cit.hasNext();){
			HierarchyCell* cell = (HierarchyCell*) cit.next();
			for(int axis=0; axis<3; axis++){
				cell->setMark(Cell::INTERNAL_CELL_MARK);

				cell->getSide(axis, Cell::FORWARD_SIDE)->setMark(Side::INTERNAL_SIDE_MARK);
				cell->getSide(axis, Cell::BACKWARD_SIDE)->setMark(Side::INTERNAL_SIDE_MARK);

				cell->getRib(axis, Cell::ul)->setMark(Rib::INTERNAL_RIB_MARK);
				cell->getRib(axis, Cell::ur)->setMark(Rib::INTERNAL_RIB_MARK);
				cell->getRib(axis, Cell::dl)->setMark(Rib::INTERNAL_RIB_MARK);
				cell->getRib(axis, Cell::dr)->setMark(Rib::INTERNAL_RIB_MARK);
			}
		}

		for(HierarchyCellIterator cit = HierarchyCellIterator(this); cit.hasNext();){
			HierarchyCell* cell = (HierarchyCell*) cit.next();
			for(int axis=0; axis<3; axis++){
				if(cell->getCell(0, Cell::FORWARD_SIDE) == NULL || cell->getCell(0, Cell::BACKWARD_SIDE) == NULL ||
					cell->getCell(1, Cell::FORWARD_SIDE) == NULL || cell->getCell(1, Cell::BACKWARD_SIDE) == NULL ||
					cell->getCell(2, Cell::FORWARD_SIDE) == NULL || cell->getCell(2, Cell::BACKWARD_SIDE) == NULL
				  )
					cell->setMark(Cell::FREE_BORDER_CELL_MARK);
			}
		}

		for(HierarchyCellIterator cit = HierarchyCellIterator(this); cit.hasNext();){
			HierarchyCell* cell = (HierarchyCell*) cit.next();
			if(cell->getMark() != Cell::FREE_BORDER_CELL_MARK) continue;

			for(int axis=0; axis<3; axis++){
				for(int dir=0; dir<2; dir++){
					if(cell->getCell(axis, dir) == NULL){
						cell->getSide(axis, dir)->setMark(Side::FREE_BORDER_SIDE_MARK);

						cell->getSide((axis+1)%3, Cell::FORWARD_SIDE)->setMark(Side::FREE_BORDER_SIDE_MARK);
						cell->getSide((axis+1)%3, Cell::BACKWARD_SIDE)->setMark(Side::FREE_BORDER_SIDE_MARK);
						cell->getSide((axis+2)%3, Cell::FORWARD_SIDE)->setMark(Side::FREE_BORDER_SIDE_MARK);
						cell->getSide((axis+2)%3, Cell::BACKWARD_SIDE)->setMark(Side::FREE_BORDER_SIDE_MARK);
					}
				}
			}
		}

		for(HierarchyCellIterator cit = HierarchyCellIterator(this); cit.hasNext();){
			HierarchyCell* cell = (HierarchyCell*) cit.next();
			if(cell->getMark() != Cell::FREE_BORDER_CELL_MARK) continue;

			for(int axis=0; axis<3; axis++){
				for(int irib = 0; irib<4; irib++){
					Rib* rib = cell->getRib(axis, irib);
					if(
						(rib->getSide(Rib::RIGHT_SIDE) == NULL 
							|| rib->getSide(Rib::RIGHT_SIDE)->getMark() == Side::FREE_BORDER_SIDE_MARK) &&
						(rib->getSide(Rib::LEFT_SIDE) == NULL 
							|| rib->getSide(Rib::LEFT_SIDE)->getMark() == Side::FREE_BORDER_SIDE_MARK) &&
						(rib->getSide(Rib::UP_SIDE) == NULL 
							|| rib->getSide(Rib::UP_SIDE)->getMark() == Side::FREE_BORDER_SIDE_MARK) &&
						(rib->getSide(Rib::DOWN_SIDE) == NULL 
							|| rib->getSide(Rib::DOWN_SIDE)->getMark() == Side::FREE_BORDER_SIDE_MARK)
					)
						rib->setMark(Rib::FREE_BORDER_RIB_MARK);
				}
			}
		}
	}


	public: void updateBorderCells(){
cout << "updateBorderCells";
clock_t start = clock();

		for(HierarchyCellIterator cit = HierarchyCellIterator(this); cit.hasNext();){
			HierarchyCell* cell = (HierarchyCell*) cit.next();
			if(cell->getMark() != Cell::FREE_BORDER_CELL_MARK) continue;

			int ind = 0;
			int daxis = 0;
			int direction = 0;

			for(int axis=0; axis<3; axis++){
				if(cell->getCell(axis, Cell::FORWARD_SIDE) == NULL) {
					daxis = axis;
					direction = Cell::FORWARD_SIDE;
					ind++;
				}
				if(cell->getCell(axis, Cell::BACKWARD_SIDE) == NULL) {
					daxis = axis;
					direction = Cell::BACKWARD_SIDE;
					ind++;
				}
			}

			if(ind == 1){
				Cell* nearCell = cell->getCell(daxis, (direction+1)%2);

				cell->setDensity(nearCell->getDensity());
				cell->setMomentum(nearCell->getMomentum(0),0);
				cell->setMomentum(nearCell->getMomentum(1),1);
				cell->setMomentum(nearCell->getMomentum(2),2);
				cell->setTermEnergy(nearCell->getTermEnergy());

				// div(B) = 0 => поле на грани daxis по направлению direction
				double Bdirection =
					cell->getSide(daxis, (direction+1)%2)->getB()
					+
					(1-2*direction) *
					(cell->getSide((daxis+1)%3,Cell::FORWARD_SIDE)->getB() - cell->getSide((daxis+1)%3,Cell::BACKWARD_SIDE)->getB())
					+
					(1-2*direction) *
					(cell->getSide((daxis+2)%3,Cell::FORWARD_SIDE)->getB() - cell->getSide((daxis+2)%3,Cell::BACKWARD_SIDE)->getB());
				cell->getSide(daxis, direction)->setB(Bdirection);


			}
		}

		for(HierarchyCellIterator cit = HierarchyCellIterator(this); cit.hasNext();){
			HierarchyCell* cell = (HierarchyCell*) cit.next();
			if(cell->getMark() != Cell::FREE_BORDER_CELL_MARK) continue;

			int ind = 0;
			int daxis[3] = {0,0,0};
			int direction[3] = {0,0,0};

			for(int axis=0; axis<3; axis++){
				if(cell->getCell(axis, Cell::FORWARD_SIDE) == NULL) {
					daxis[ind] = axis;
					direction[ind] = Cell::FORWARD_SIDE;
					ind++;
				}
				if(cell->getCell(axis, Cell::BACKWARD_SIDE) == NULL) {
					daxis[ind] = axis;
					direction[ind] = Cell::BACKWARD_SIDE;
					ind++;
				}
			}

			if(ind ==2){
				cell->getSide(daxis[0], direction[0])->setB( cell->getSide(daxis[0], (direction[0]+1)%2)->getB() );

				double Bdirection =
					cell->getSide(daxis[1], (direction[1]+1)%2)->getB()
					+
					(1-2*direction[1]) *
					(cell->getSide((daxis[1]+1)%3,1)->getB() - cell->getSide((daxis[1]+1)%3,0)->getB())
					+
					(1-2*direction[1]) *
					(cell->getSide((daxis[1]+2)%3,1)->getB() - cell->getSide((daxis[1]+2)%3,0)->getB());
				cell->getSide(daxis[1], direction[1])->setB( Bdirection );

				Cell* cell0 = cell->getCell(daxis[0], (direction[0]+1)%2);
				Cell* cell1 = cell->getCell(daxis[1], (direction[1]+1)%2);

				double rho = (cell0->getDensity() + cell1->getDensity())/2.0;
				double velocity[3];
				velocity[0] = (cell0->getMomentum(0)/cell0->getDensity() + cell1->getMomentum(0)/cell1->getDensity())/2.0;
				velocity[1] = (cell0->getMomentum(1)/cell0->getDensity() + cell1->getMomentum(1)/cell1->getDensity())/2.0;
				velocity[2] = (cell0->getMomentum(2)/cell0->getDensity() + cell1->getMomentum(2)/cell1->getDensity())/2.0;

				cell->setDensity(rho);
				cell->setMomentum(rho * velocity[0], 0);
				cell->setMomentum(rho * velocity[1], 1);
				cell->setMomentum(rho * velocity[2], 2);

				cell->setTermEnergy((cell0->getTermEnergy() + cell1->getTermEnergy()) / 2.0);
			}

			if(ind ==3 and cell->getMark() == Cell::FREE_BORDER_CELL_MARK){
				cell->getSide(daxis[0], direction[0])->setB( cell->getSide(daxis[0], (direction[0]+1)%2)->getB() );
				cell->getSide(daxis[1], direction[1])->setB( cell->getSide(daxis[1], (direction[1]+1)%2)->getB() );
				cell->getSide(daxis[2], direction[2])->setB( cell->getSide(daxis[2], (direction[2]+1)%2)->getB() );


				cell->setTermEnergy(cell->getTermEnergy());
			}
		}
cout.setf(ios::showpoint,ios::floatfield); cout.precision(3);
cout << "\t" << ((double)(clock()-start))/CLOCKS_PER_SEC << endl;
cout.setf(ios::scientific,ios::floatfield); cout.precision(10);
	}

	public: void updateBorderSides(){
cout << "updateBorderSides";
clock_t start = clock();

		for(HierarchyCellIterator cit = HierarchyCellIterator(this); cit.hasNext();){
			HierarchyCell* cell = (HierarchyCell*) cit.next();
			if(cell->getMark() != Cell::FREE_BORDER_CELL_MARK) continue;

			for(int axis = 0; axis<3; axis++){
				for(int dir=0; dir<2; dir++){
					Side* side = cell->getSide(axis, dir);
					if(side->getMark() == Side::FREE_BORDER_SIDE_MARK && 
						(
							side->getRib(Side::RIGHT_RIB)->getMark() == Rib::INTERNAL_RIB_MARK ||
							side->getRib(Side::LEFT_RIB)->getMark() == Rib::INTERNAL_RIB_MARK ||
							side->getRib(Side::UP_RIB)->getMark() == Rib::INTERNAL_RIB_MARK ||
							side->getRib(Side::DOWN_RIB)->getMark() == Rib::INTERNAL_RIB_MARK
						)
					){
						for(int baxis=1; baxis <= 2; baxis++){
							for(int bdir=0; bdir<2; bdir++){
								Cell* bcell = cell->getCell((axis + baxis)%3, bdir);
								if(bcell != NULL 
										&& bcell->getSide(axis, dir)->getMark() == Side::INTERNAL_SIDE_MARK){
									double* srcF = bcell->getSide(axis, dir)->getFlow();
									double* trgF =  side->getFlow();
									for(int j=0; j<8; j++) trgF[j]=srcF[j];
									break;
								}
							}
						}
					}
				}
			}
		}

cout.setf(ios::showpoint,ios::floatfield); cout.precision(3);
cout << "\t" << ((double)(clock()-start))/CLOCKS_PER_SEC << endl;
cout.setf(ios::scientific,ios::floatfield); cout.precision(10);
	}

	public: void updateBorderCurrents(){
cout << "updateBorderCurrents" << endl;
		for(int axis=0; axis<3; axis++){
			for(HierarchyCellIterator cit = HierarchyCellIterator(this); cit.hasNext();){
				HierarchyCell* cell = (HierarchyCell*) cit.next();
				if(cell->getMark() != Cell::FREE_BORDER_CELL_MARK) continue;

				Side* borderSide = NULL;
				int direction = 0;

				if(cell->getSide(axis, Cell::FORWARD_SIDE)->getMark() > 0) {
					direction = Cell::FORWARD_SIDE;
					borderSide = cell->getSide(axis, Cell::FORWARD_SIDE);
				}
				if(cell->getSide(axis, Cell::BACKWARD_SIDE)->getMark() > 0) {
					direction = Cell::BACKWARD_SIDE;
					borderSide = cell->getSide(axis, Cell::BACKWARD_SIDE);
				}

				if(borderSide != NULL){
					for(int irib=0;irib<4;irib++){
						Rib* rib = borderSide->getRib(irib);
						Rib* prib = cell->getSide(axis, (direction+1)%2)->getRib(irib);
						if(prib->getMark() == 0) rib->setE(prib->getE() + prib->getdE());

						if(rib->getCell(Cell::ul)==0 && rib->getCell(Cell::dl)==0 && rib->getCell(Cell::dr)==0)
							rib->setE(rib->getCell(Cell::ur)->getRib(rib->getAxis(),Cell::ur)->getE()+
								rib->getCell(Cell::ur)->getRib(rib->getAxis(),Cell::ur)->getdE());
						if(rib->getCell(Cell::ur)==0 && rib->getCell(Cell::dl)==0 && rib->getCell(Cell::dr)==0)
							rib->setE(rib->getCell(Cell::ul)->getRib(rib->getAxis(),Cell::ul)->getE()+
								rib->getCell(Cell::ul)->getRib(rib->getAxis(),Cell::ul)->getdE());
						if(rib->getCell(Cell::ul)==0 && rib->getCell(Cell::dl)==0 && rib->getCell(Cell::ur)==0)
							rib->setE(rib->getCell(Cell::dr)->getRib(rib->getAxis(),Cell::dr)->getE()+
								rib->getCell(Cell::dr)->getRib(rib->getAxis(),Cell::dr)->getdE());
						if(rib->getCell(Cell::ur)==0 && rib->getCell(Cell::ul)==0 && rib->getCell(Cell::dr)==0)
							rib->setE(rib->getCell(Cell::dl)->getRib(rib->getAxis(),Cell::dl)->getE()+
								rib->getCell(Cell::dl)->getRib(rib->getAxis(),Cell::dl)->getdE());
					}
				}
			}
		}
	}

	public: void showLine(CalculaterInt* calc, int itime){
		double max_divB = 0;
		HierarchyCell* max_divB_cell = NULL;
		int count = 0;
		double minT = 1.0e+20;

		for(HierarchyCellIterator cit = HierarchyCellIterator(this); cit.hasNext();){
			HierarchyCell* lc = (HierarchyCell*) cit.next();
			if(lc->getT() < minT) minT = lc->getT();
			count++;

			if(lc->getMark() == Cell::INTERNAL_CELL_MARK){
				double divB = abs(
					(lc->getSide(0,Cell::FORWARD_SIDE)->getB() - lc->getSide(0,Cell::BACKWARD_SIDE)->getB())
					+
					(lc->getSide(1,Cell::FORWARD_SIDE)->getB() - lc->getSide(1,Cell::BACKWARD_SIDE)->getB())
					+
					(lc->getSide(2,Cell::FORWARD_SIDE)->getB() - lc->getSide(2,Cell::BACKWARD_SIDE)->getB())
					);
				if(max_divB <= divB){
					max_divB = divB;
					max_divB_cell = lc;
				}
			}
		}

cout.setf(ios::showpoint,ios::floatfield); cout.precision(3);
		cout 	
			<< itime << ":: "
			<< "time = " << time 
			<< " | "
			<< "maxs = " << getMaxSpeed() / 1.0e+5 << " [km/s]"
			<<"\tmin_dt = " << dt << " [s]"
//			<< " in " << max_speed_cell->getMark()
			<< " | "
			<< "max_div(B) =" << max_divB 
			<< " in " << max_divB_cell->getPath()
			<< " | "
			<< "active cells = " << count
			<< " | "
			<< "minT = " << minT
			<< endl;
cout.setf(ios::scientific,ios::floatfield); cout.precision(10);	

		CellArray::time += dt;
		CellArray::dt = dt;
	}
};
