#include <iostream>
using namespace std;

class CellArray: public CellArrayInt{
	protected: Cell** cells;
	protected: Cell** border_cells;
	protected: TaskData* td;
	protected: double dt;
	protected: bool isMaxSpeedCalculated;
	protected: int size;
	protected: int border_size;
	protected: double time;
	protected: Iterator* iterator;
	protected: std::vector<BorderCondition*> bcs;
	protected: double maxSpeed;
	protected: HierarchyCell* max_speed_cell;

	public: CellArray(){}

	public: CellArray(TaskData& td){
		CellArray::td = &td;
		Cell* cell;
		int ind = 0;
		time = 0;
		dt =0;
		isMaxSpeedCalculated = false;

		size = td.getN(0)*td.getN(1)*td.getN(2);
		cells = new Cell*[size];

		for(int ix=0; ix<td.getN(0); ix++){
			for(int iy=0; iy<td.getN(1); iy++){
				for(int iz=0; iz<td.getN(2); iz++){
					double r[3];
					r[0] = td.getLl(0) + (ix+0.5)*td.getH(0);
					r[1] = td.getLl(1) + (iy+0.5)*td.getH(1);
					r[2] = td.getLl(2) + (iz+0.5)*td.getH(2);

					int point[3] = {ix, iy, iz};
					double h[3] = {td.getH(0), td.getH(1), td.getH(2)};
					double momentum[3] = {0,0,0};

					cells[ind] = new Cell(0,momentum,0,r,point,h, ind);
					ind++;
				}
			}
		}

		bindSidesForRegulatCA();
		bindRibsForRegulatCA();
		markBorderCells();
	}

	protected: Side* defineSideType(int axis, Cell* bcell, Cell* fcell){
		return new Side(axis,0, bcell, fcell);
	}

	protected: Rib* defineRibType(int axis, Cell* ulcell, Cell* urcell, Cell* dlcell, Cell* drcell){
		return new Rib(axis,0,ulcell,urcell,dlcell,drcell);
	}

	public: Iterator* getIterator(){
		return NULL;
	}

	protected: void bindSidesForRegulatCA(){
		TaskData& td = *CellArray::td;
		Cell* bcell;
		Cell* fcell;
		for(int i=0; i<size; i++){
			bcell = getCell(i);
			int bx[3];
			bx[0] = bcell->getPoint(0);
			bx[1] = bcell->getPoint(1);
			bx[2] = bcell->getPoint(2);

			for(int axis=0; axis<3; axis++){
				if(!bcell->isBorderCell(axis, td, Cell::FORWARD_SIDE)){
					int fx[3];
					fx[0]=bx[0];fx[1]=bx[1];fx[2]=bx[2];
					fx[axis] = fx[axis]+1;

					fcell = getCell(fx);
					defineSide(bcell, fcell, axis);
				}
				if(bcell->isBorderCell(axis, td, Cell::FORWARD_SIDE)){
					defineBorderSide(bcell, axis, Cell::FORWARD_SIDE);
				}
				if(bcell->isBorderCell(axis, td, Cell::BACKWARD_SIDE)){
					defineBorderSide(bcell, axis, Cell::BACKWARD_SIDE);
				}
			}
		}
	}

	protected: void bindRibsForRegulatCA(){
		TaskData& td = *CellArray::td;
		Cell* bcell;
		for(int i=0; i<size; i++){
			bcell = getCell(i);
			int bx[3] = {bcell->getPoint(0), bcell->getPoint(1), bcell->getPoint(2)};

			for(int axis=0; axis<3; axis++){
				if(!bcell->isBorderCell((axis+1)%3, td, Cell::FORWARD_SIDE) 
						and !bcell->isBorderCell((axis+2)%3, td, Cell::FORWARD_SIDE))
					defineInternalRib(axis,bcell);
				else if(bcell->isBorderCell((axis+1)%3, td, Cell::FORWARD_SIDE) 
						and !bcell->isBorderCell((axis+2)%3, td, Cell::FORWARD_SIDE))
					defineBorderRib(axis, 
								bcell, 
								bcell->getCell((axis+2)%3, Cell::FORWARD_SIDE),
								Cell::dl, Cell::dr,
								NULL,
								bcell->getSide((axis+2)%3, Cell::FORWARD_SIDE),
								bcell->getCell((axis+2)%3, Cell::FORWARD_SIDE)->getSide((axis+1)%3, Cell::FORWARD_SIDE),
								bcell->getSide((axis+1)%3, Cell::FORWARD_SIDE)
							);
				else if(!bcell->isBorderCell((axis+1)%3, td, Cell::FORWARD_SIDE) 
						and bcell->isBorderCell((axis+2)%3, td, Cell::FORWARD_SIDE))
					defineBorderRib(axis, bcell, 
								bcell->getCell((axis+1)%3, Cell::FORWARD_SIDE), 
								Cell::dl, Cell::ul,
								bcell->getCell((axis+1)%3, Cell::FORWARD_SIDE)->getSide((axis+2)%3, Cell::FORWARD_SIDE),
								bcell->getSide((axis+2)%3, Cell::FORWARD_SIDE),
								NULL,
								bcell->getSide((axis+1)%3, Cell::FORWARD_SIDE)
							);

				if(bcell->isBorderCell((axis+1)%3, td, Cell::BACKWARD_SIDE) 
						and !bcell->isBorderCell((axis+2)%3, td, Cell::BACKWARD_SIDE))
					defineBorderRib(axis, bcell, 
								bcell->getCell((axis+2)%3, Cell::BACKWARD_SIDE), 
								Cell::ur, Cell::ul,
								bcell->getSide((axis+2)%3, Cell::BACKWARD_SIDE),
								NULL,
								bcell->getSide((axis+1)%3, Cell::BACKWARD_SIDE),
								bcell->getCell((axis+2)%3, Cell::BACKWARD_SIDE)->getSide((axis+1)%3, Cell::BACKWARD_SIDE)
							);
				else if(!bcell->isBorderCell((axis+1)%3, td, Cell::BACKWARD_SIDE) 
						and bcell->isBorderCell((axis+2)%3, td, Cell::BACKWARD_SIDE))
					defineBorderRib(axis, bcell, 
								bcell->getCell((axis+1)%3, Cell::BACKWARD_SIDE), 
								Cell::ur, Cell::dr,
								bcell->getSide((axis+2)%3, Cell::BACKWARD_SIDE),
								bcell->getCell((axis+1)%3, Cell::BACKWARD_SIDE)->getSide((axis+2)%3, Cell::BACKWARD_SIDE),
								bcell->getSide((axis+1)%3, Cell::BACKWARD_SIDE),
								NULL
							);

				if(bcell->isBorderCell((axis+1)%3, td, Cell::FORWARD_SIDE) 
						and bcell->isBorderCell((axis+2)%3, td, Cell::FORWARD_SIDE))
					defineBorderRib(axis, bcell, NULL, Cell::dl, NULL,
								NULL,
								bcell->getSide((axis+2)%3, Cell::FORWARD_SIDE),
								NULL,
								bcell->getSide((axis+1)%3, Cell::FORWARD_SIDE)
							);
				if(bcell->isBorderCell((axis+1)%3, td, Cell::FORWARD_SIDE) 
						and bcell->isBorderCell((axis+2)%3, td, Cell::BACKWARD_SIDE))
					defineBorderRib(axis, bcell, NULL, Cell::dr, NULL,
								NULL,
								bcell->getSide((axis+2)%3, Cell::BACKWARD_SIDE),
								bcell->getSide((axis+1)%3, Cell::FORWARD_SIDE),
								NULL
							);
				if(bcell->isBorderCell((axis+1)%3, td, Cell::BACKWARD_SIDE) 
						and bcell->isBorderCell((axis+2)%3, td, Cell::BACKWARD_SIDE))
					defineBorderRib(axis, bcell, NULL, Cell::ur, NULL,
								bcell->getSide((axis+2)%3, Cell::BACKWARD_SIDE),
								NULL,
								bcell->getSide((axis+1)%3, Cell::BACKWARD_SIDE),
								NULL
							);
				if(bcell->isBorderCell((axis+1)%3, td, Cell::BACKWARD_SIDE) 
						and bcell->isBorderCell((axis+2)%3, td, Cell::FORWARD_SIDE))
					defineBorderRib(axis, bcell, NULL, Cell::ul, NULL,
								bcell->getSide((axis+2)%3, Cell::FORWARD_SIDE),
								NULL,
								NULL,
								bcell->getSide((axis+1)%3, Cell::BACKWARD_SIDE)
							);
			}
		}

		Cell* cell;
		for(int i=0; i<size; i++){
			cell = getCell(i);
			for(int axis=0;axis<3;axis++){
				Rib* ul = cell->getRib(axis, Cell::ul);
				Rib* ur = cell->getRib(axis, Cell::ur);
				Rib* dr = cell->getRib(axis, Cell::dr);
				Rib* dl = cell->getRib(axis, Cell::dl);

/*
				Side* upSide = cell->getSide((axis+2)%3, Cell::FORWARD_SIDE);
				Side* downSide = cell->getSide((axis+2)%3, Cell::BACKWARD_SIDE);
				Side* rightSide = cell->getSide((axis+1)%3, Cell::FORWARD_SIDE);
				Side* leftSide = cell->getSide((axis+1)%3, Cell::BACKWARD_SIDE);

				upSide->setRib(ul, Side::DOWN_RIB);
				upSide->setRib(ur, Side::UP_RIB);
				downSide->setRib(dl, Side::DOWN_RIB);
				downSide->setRib(dr, Side::UP_RIB);
				leftSide->setRib(ul, Side::RIGHT_RIB);
				leftSide->setRib(dl, Side::LEFT_RIB);
				rightSide->setRib(ur, Side::RIGHT_RIB);
				rightSide->setRib(dr, Side::LEFT_RIB);
*/
			}

		}

	}

	public: int getSize(){
		return size;
	}

	public: int getBorderSize(){
		return border_size;
	}

	public: Cell* getCell(int ind){
		return cells[ind];
	}

	public: Cell* getBorderCell(int ind){
		return border_cells[ind];
	}

	public: void addTime(double dt){
		time += dt;
	}

	public: double getTime(){
		return time;
	}
	public: void setTimeStep(double dt){
		CellArray::dt = dt;
	}
	public: double getTimeStep(){
		return dt;
	}

	public: void setMaxSpeed(double maxs){
		CellArray::maxSpeed = maxs;
	}
	public: double getMaxSpeed(){
		return maxSpeed;
	}

	public: Cell* getCell(int x[3]){
		return cells[ x[0]*td->getN(1)*td->getN(2) + x[1]*td->getN(2) + x[2] ];
	}
	public: Cell* getCell(int x, int y, int z){
		return cells[ x*td->getN(1)*td->getN(2) + y*td->getN(2) + z ];
	}

	public: void addBorderCondition(BorderCondition* bc){
		bcs.push_back(bc);
	}

	public: vector<BorderCondition*> getBorderConditions(){
		return bcs;
	}

	protected: void defineSide(Cell* bcell, Cell* fcell, int axis){
		double rs[3];
		Side* side;

		side = defineSideType(axis, bcell, fcell);
		bcell->setSide(axis, Cell::FORWARD_SIDE, side);
		fcell->setSide(axis, Cell::BACKWARD_SIDE, side);
	}

	protected: void defineBorderSide(Cell* cell, int axis, int direction){
		double rs[3];
		Side* side;

		for(int i=0;i<3;i++) rs[i] = cell->getR(i);

		if(direction == Cell::FORWARD_SIDE) side = defineSideType(axis, cell, NULL);
		if(direction == Cell::BACKWARD_SIDE) side = defineSideType(axis, NULL, cell);
		cell->setSide(axis, direction, side);
	}

	protected: void defineInternalRib(int axis, Cell* dlcell){
		int bx[3] = {dlcell->getPoint(0), dlcell->getPoint(1), dlcell->getPoint(2)};
		int x[3];

		x[0]=bx[0];x[1]=bx[1];x[2]=bx[2];
		x[(axis+1)%3] = x[(axis+1)%3]+1;
		Cell* ulcell = getCell(x);

		x[0]=bx[0];x[1]=bx[1];x[2]=bx[2];
		x[(axis+2)%3] = x[(axis+2)%3]+1;
		Cell* drcell = getCell(x);

		x[0]=bx[0];x[1]=bx[1];x[2]=bx[2];
		x[(axis+1)%3] = x[(axis+1)%3]+1;
		x[(axis+2)%3] = x[(axis+2)%3]+1;
		Cell* urcell = getCell(x);

		Rib* rib = defineRibType(axis, ulcell, urcell, dlcell, drcell);

		ulcell->setRib(axis, Cell::dr, rib);
		urcell->setRib(axis, Cell::dl, rib);
		dlcell->setRib(axis, Cell::ur, rib);
		drcell->setRib(axis, Cell::ul, rib);

		Side* upSide = urcell->getSide((axis+1)%3, Cell::BACKWARD_SIDE);
		Side* downSide = ulcell->getSide((axis+1)%3, Cell::BACKWARD_SIDE);
		Side* rightSide = urcell->getSide((axis+2)%3, Cell::BACKWARD_SIDE);
		Side* leftSide = drcell->getSide((axis+2)%3, Cell::BACKWARD_SIDE);

		rightSide->setRib(rib, Side::DOWN_RIB);
		leftSide->setRib(rib, Side::UP_RIB);
		upSide->setRib(rib, Side::LEFT_RIB);
		downSide->setRib(rib, Side::RIGHT_RIB);

		rib->setSide(Rib::UP_SIDE, upSide);
		rib->setSide(Rib::DOWN_SIDE, downSide);
		rib->setSide(Rib::RIGHT_SIDE, rightSide);
		rib->setSide(Rib::LEFT_SIDE, leftSide);
	}

	protected: void defineBorderRib(int aaxis, Cell* acell, Cell* bcell, int aind, int bind, 
						Side* rightSide, Side* leftSide, Side* upSide, Side* downSide){
		int acind;
		int bcind;
		Rib* rib = defineRibType(aaxis, NULL, NULL, NULL, NULL);

		if(aind == Cell::ur) acind = Cell::dl;
		else if(aind == Cell::ul) acind = Cell::dr;
		else if(aind == Cell::dl) acind = Cell::ur;
		else if(aind == Cell::dr) acind = Cell::ul;

		if(bind == Cell::ur) bcind = Cell::dl;
		else if(bind == Cell::ul) bcind = Cell::dr;
		else if(bind == Cell::dl) bcind = Cell::ur;
		else if(bind == Cell::dr) bcind = Cell::ul;

		rib->setCell(aind, acell);
		acell->setRib(aaxis, acind, rib);
		if(bcell != NULL){
			rib->setCell(bind, bcell);
			bcell->setRib(aaxis, bcind, rib);
		}

		if(rightSide != NULL) rightSide->setRib(rib, Side::DOWN_RIB);
		if(leftSide != NULL) leftSide->setRib(rib, Side::UP_RIB);
		if(upSide != NULL) upSide->setRib(rib, Side::LEFT_RIB);
		if(downSide != NULL) downSide->setRib(rib, Side::RIGHT_RIB);

		if(rightSide != NULL) rib->setSide(Rib::RIGHT_SIDE, rightSide);
		if(leftSide != NULL) rib->setSide(Rib::LEFT_SIDE, leftSide);
		if(upSide != NULL) rib->setSide(Rib::UP_SIDE, upSide);
		if(downSide != NULL) rib->setSide(Rib::DOWN_SIDE, downSide);
	}

	protected: void markBorderCells(){
		border_size = 0;

		for(int i=0; i<size; i++){
			Cell* cell = getCell(i);
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

		for(int i=0; i<size; i++){
			Cell* cell = getCell(i);
			for(int axis=0; axis<3; axis++){
				if(cell->getPoint(axis) == 0 or cell->getPoint(axis)+1 == td->getN(axis)) cell->setMark(Cell::FREE_BORDER_CELL_MARK);
			}
			if(cell->getMark() != 0) border_size++;
		}

		border_cells = new Cell*[border_size];

		int ind = 0;
		for(int i=0; i<size; i++){
			Cell* cell = getCell(i);
			if(cell->getMark() != 0) border_cells[ind++] = cell;
		}

		for(int i=0; i<border_size; i++){
			Cell* cell = getBorderCell(i);
			for(int axis=0; axis<3; axis++){
				if(cell->getPoint(axis) == 0) 
					cell->getSide(axis, Cell::BACKWARD_SIDE)->setMark(Side::FREE_BORDER_SIDE_MARK);
				if(cell->getPoint(axis)+1 == td->getN(axis)) 
					cell->getSide(axis, Cell::FORWARD_SIDE)->setMark(Side::FREE_BORDER_SIDE_MARK);
			}
		}

		for(int i=0; i<border_size; i++){
			Cell* cell = getBorderCell(i);
			for(int axis=0; axis<3; axis++){
				if(cell->getSide(axis, Cell::FORWARD_SIDE)->getMark()>0){
					Side* side = cell->getSide(axis, Cell::FORWARD_SIDE);
					side->getRib(Side::UP_RIB)->setMark(Rib::FREE_BORDER_RIB_MARK);
					side->getRib(Side::DOWN_RIB)->setMark(Rib::FREE_BORDER_RIB_MARK);
					side->getRib(Side::LEFT_RIB)->setMark(Rib::FREE_BORDER_RIB_MARK);
					side->getRib(Side::RIGHT_RIB)->setMark(Rib::FREE_BORDER_RIB_MARK);
				}
				if(cell->getSide(axis, Cell::BACKWARD_SIDE)->getMark()>0){
					Side* side = cell->getSide(axis, Cell::BACKWARD_SIDE);
					side->getRib(Side::UP_RIB)->setMark(Rib::FREE_BORDER_RIB_MARK);
					side->getRib(Side::DOWN_RIB)->setMark(Rib::FREE_BORDER_RIB_MARK);
					side->getRib(Side::LEFT_RIB)->setMark(Rib::FREE_BORDER_RIB_MARK);
					side->getRib(Side::RIGHT_RIB)->setMark(Rib::FREE_BORDER_RIB_MARK);
				}
			}
		}
	}

	public: void updateBorderCells(){
		for(int i=0; i<border_size; i++){
			Cell* cell = getBorderCell(i);
			int ind = 0;
			int daxis = 0;
			int direction = 0;
			for(int axis=0; axis<3; axis++){
				if(cell->getSide(axis, Cell::FORWARD_SIDE)->getMark() > 0) {
					daxis = axis;
					direction = Cell::FORWARD_SIDE;
					ind++;
				}
				if(cell->getSide(axis, Cell::BACKWARD_SIDE)->getMark() > 0) {
					daxis = axis;
					direction = Cell::BACKWARD_SIDE;
					ind++;
				}
			}
			if(ind == 1 and cell->getMark() == Cell::FREE_BORDER_CELL_MARK){
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

/*				cell->getSide(daxis, direction)->getRib(Side::UP_RIB)->setE(
						cell->getSide(daxis, (direction+1)%2)->getRib(Side::UP_RIB)->getE());
				cell->getSide(daxis, direction)->getRib(Side::DOWN_RIB)->setE(
						cell->getSide(daxis, (direction+1)%2)->getRib(Side::DOWN_RIB)->getE());
				cell->getSide(daxis, direction)->getRib(Side::LEFT_RIB)->setE(
						cell->getSide(daxis, (direction+1)%2)->getRib(Side::LEFT_RIB)->getE());
				cell->getSide(daxis, direction)->getRib(Side::RIGHT_RIB)->setE(
						cell->getSide(daxis, (direction+1)%2)->getRib(Side::RIGHT_RIB)->getE());
*/
			}

		}

		for(int i=0; i<border_size; i++){
			Cell* cell = getBorderCell(i);
			int ind = 0;
			int daxis[3] = {0,0,0};
			int direction[3] = {0,0,0};
			for(int axis=0; axis<3; axis++){
				if(cell->getSide(axis, Cell::FORWARD_SIDE)->getMark() > 0) {
					daxis[ind] = axis;
					direction[ind] = Cell::FORWARD_SIDE;
					ind++;
				}
				if(cell->getSide(axis, Cell::BACKWARD_SIDE)->getMark() > 0) {
					daxis[ind] = axis;
					direction[ind] = Cell::BACKWARD_SIDE;
					ind++;
				}
			}
			if(ind ==2 and cell->getMark() == Cell::FREE_BORDER_CELL_MARK){
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

				cell->setP((cell0->getP()+cell1->getP())/2.0);

/*				cell->getSide(daxis[0], direction[0])->getRib(Side::UP_RIB)->setE(
						cell->getSide(daxis[0], (direction[0]+1)%2)->getRib(Side::UP_RIB)->getE());
				cell->getSide(daxis[0], direction[0])->getRib(Side::DOWN_RIB)->setE(
						cell->getSide(daxis[0], (direction[0]+1)%2)->getRib(Side::DOWN_RIB)->getE());
				cell->getSide(daxis[0], direction[0])->getRib(Side::LEFT_RIB)->setE(
						cell->getSide(daxis[0], (direction[0]+1)%2)->getRib(Side::LEFT_RIB)->getE());
				cell->getSide(daxis[0], direction[0])->getRib(Side::RIGHT_RIB)->setE(
						cell->getSide(daxis[0], (direction[0]+1)%2)->getRib(Side::RIGHT_RIB)->getE());

				cell->getSide(daxis[1], direction[1])->getRib(Side::UP_RIB)->setE(
						cell->getSide(daxis[1], (direction[1]+1)%2)->getRib(Side::UP_RIB)->getE());
				cell->getSide(daxis[1], direction[1])->getRib(Side::DOWN_RIB)->setE(
						cell->getSide(daxis[1], (direction[1]+1)%2)->getRib(Side::DOWN_RIB)->getE());
				cell->getSide(daxis[1], direction[1])->getRib(Side::LEFT_RIB)->setE(
						cell->getSide(daxis[1], (direction[1]+1)%2)->getRib(Side::LEFT_RIB)->getE());
				cell->getSide(daxis[1], direction[1])->getRib(Side::RIGHT_RIB)->setE(
						cell->getSide(daxis[1], (direction[1]+1)%2)->getRib(Side::RIGHT_RIB)->getE());
*/
			}

			if(ind ==3 and cell->getMark() == Cell::FREE_BORDER_CELL_MARK){
				cell->getSide(daxis[0], direction[0])->setB( cell->getSide(daxis[0], (direction[0]+1)%2)->getB() );
				cell->getSide(daxis[1], direction[1])->setB( cell->getSide(daxis[1], (direction[1]+1)%2)->getB() );
				cell->getSide(daxis[2], direction[2])->setB( cell->getSide(daxis[2], (direction[2]+1)%2)->getB() );


				cell->setP(cell->getP());
			}
		}
	}

	public: void updateBorderSides(){
		for(int axis=0; axis<3; axis++){
			for(int i=0; i<border_size; i++){
			  Cell* lc = getBorderCell(i);
	  		  if(lc->getCell(axis, Cell::FORWARD_SIDE) != NULL && lc->getCell(axis, Cell::FORWARD_SIDE)->getMark()>0){
			  	Side* pside = NULL;
				if(lc->getCell((axis+1)%3,Cell::BACKWARD_SIDE)!=NULL && lc->getCell((axis+1)%3,Cell::BACKWARD_SIDE)->getMark()==0){
					pside=lc->getCell((axis+1)%3, Cell::BACKWARD_SIDE)->getSide(axis,Cell::FORWARD_SIDE);
					double* srcF = pside->getFlow();
					double* trgF = lc->getSide(axis, Cell::FORWARD_SIDE)->getFlow();
					for(int j=0; j<8; j++) trgF[j]=srcF[j];
				}
				if(lc->getCell((axis+1)%3,Cell::FORWARD_SIDE)!=NULL && lc->getCell((axis+1)%3,Cell::FORWARD_SIDE)->getMark()==0){
					pside=lc->getCell((axis+1)%3, Cell::FORWARD_SIDE)->getSide(axis,Cell::FORWARD_SIDE);
					double* srcF = pside->getFlow();
					double* trgF = lc->getSide(axis, Cell::FORWARD_SIDE)->getFlow();
					for(int j=0; j<8; j++) trgF[j]=srcF[j];
				}
				if(lc->getCell((axis+2)%3, Cell::FORWARD_SIDE)!=NULL && lc->getCell((axis+2)%3, Cell::FORWARD_SIDE)->getMark()==0){
					pside=lc->getCell((axis+2)%3, Cell::FORWARD_SIDE)->getSide(axis,Cell::FORWARD_SIDE);
					double* srcF = pside->getFlow();
					double* trgF = lc->getSide(axis, Cell::FORWARD_SIDE)->getFlow();
					for(int j=0; j<8; j++) trgF[j]=srcF[j];
				}
				if(lc->getCell((axis+2)%3,Cell::BACKWARD_SIDE)!=NULL && lc->getCell((axis+2)%3,Cell::BACKWARD_SIDE)->getMark()==0){
					pside=lc->getCell((axis+2)%3, Cell::BACKWARD_SIDE)->getSide(axis,Cell::FORWARD_SIDE);
					double* srcF = pside->getFlow();
					double* trgF = lc->getSide(axis, Cell::FORWARD_SIDE)->getFlow();
					for(int j=0; j<8; j++) trgF[j]=srcF[j];
				}
			  }
			}
		}
	}

	public: void updateBorderCurrents(){
		for(int axis=0; axis<3; axis++){
			for(int i=0; i<border_size; i++){
				Cell* cell = getBorderCell(i);
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

	public: void calculateMinTimeStep(CalculaterInt* calc, int itime){
		double dt = 100;
		double max_divB = 0;
		int max_speed_cell = 0;
		int max_divB_cell = 0;

		for(int i=0;i<getSize();i++){
			Cell* lc = getCell(i);
			for(int axis=0; axis<3; axis++){
				if(lc->getCell(axis, Cell::FORWARD_SIDE) == NULL) continue;
				Cell* rc = lc->getCell(axis, Cell::FORWARD_SIDE);

				double maxs = calc->calculateMaxSpeed(axis, *lc, *rc);

				if(lc->getH(0)/maxs < dt) {dt = lc->getH(0)/maxs; max_speed_cell=i;}
				if(lc->getH(1)/maxs < dt) {dt = lc->getH(1)/maxs; max_speed_cell=i;}
				if(lc->getH(2)/maxs < dt) {dt = lc->getH(2)/maxs; max_speed_cell=i;}

				if(rc->getH(0)/maxs < dt) {dt = rc->getH(0)/maxs; max_speed_cell=i;}
				if(rc->getH(1)/maxs < dt) {dt = rc->getH(1)/maxs; max_speed_cell=i;}
				if(rc->getH(2)/maxs < dt) {dt = rc->getH(2)/maxs; max_speed_cell=i;}
			}

			double divB = abs(
				(lc->getSide(0,Cell::FORWARD_SIDE)->getB() - lc->getSide(0,Cell::BACKWARD_SIDE)->getB())
				+
				(lc->getSide(1,Cell::FORWARD_SIDE)->getB() - lc->getSide(1,Cell::BACKWARD_SIDE)->getB())
				+
				(lc->getSide(2,Cell::FORWARD_SIDE)->getB() - lc->getSide(2,Cell::BACKWARD_SIDE)->getB())
				);
			if(max_divB < divB){
				max_divB = divB;
				max_divB_cell = i;
			}
		}

		dt = dt * Constants::curant;

		cout 	
			<< itime << ":: "
			<< "time = " << time 
			<< " | "
			<< "maxs = " << getMaxSpeed() 
			<<"\tmin_dt = " << dt 
			<< " in [" << getCell(max_speed_cell)->getPoint(0)
			<< "," << getCell(max_speed_cell)->getPoint(1)
			<< "," << getCell(max_speed_cell)->getPoint(2)
			<< "]"
			<< "\t\t"
			<< "max_div(B) =" << max_divB 
			<< " in [" << getCell(max_divB_cell)->getPoint(0)
			<< "," << getCell(max_divB_cell)->getPoint(1)
			<< "," << getCell(max_divB_cell)->getPoint(2)
			<< "]"
			<< endl;

		CellArray::time += dt;
		CellArray::dt = dt;
	}

	public: void calculateIncrements(){
		for(int i=0;i<getSize();i++){
			Cell* cell = getCell(i);
/*
			if(cell->getPoint(0)==0 and cell->getPoint(1)==0 and cell->getPoint(2)==1){
				for(int axis=0; axis<3; axis++){
					cout.precision(3);

					Side* side = cell->getSide(axis, 0);
					cout<< axis<<" :back:\t"<<
						side->getRib(Side::UP_RIB)->getAxis()<<": "<<side->getRib(Side::UP_RIB)->getE()<<"\t"<<
						side->getRib(Side::DOWN_RIB)->getAxis()<<": "<<side->getRib(Side::DOWN_RIB)->getE()<<"\t|\t"<<
						side->getRib(Side::RIGHT_RIB)->getAxis()<<": "<<side->getRib(Side::RIGHT_RIB)->getE()<<"\t"<<
						side->getRib(Side::LEFT_RIB)->getAxis()<<": "<<side->getRib(Side::LEFT_RIB)->getE()<<endl;

					side= cell->getSide(axis, 1);
					cout<< axis<<" :forw:\t"<<
						side->getRib(Side::UP_RIB)->getAxis()<<": "<<side->getRib(Side::UP_RIB)->getE()<<"\t"<<
						side->getRib(Side::DOWN_RIB)->getAxis()<<": "<<side->getRib(Side::DOWN_RIB)->getE()<<"\t|\t"<<
						side->getRib(Side::RIGHT_RIB)->getAxis()<<": "<<side->getRib(Side::RIGHT_RIB)->getE()<<"\t"<<
						side->getRib(Side::LEFT_RIB)->getAxis()<<": "<<side->getRib(Side::LEFT_RIB)->getE()<<endl;

					cout.precision(10);
				}
			}
*/
			if(cell->getMark() != Cell::INTERNAL_CELL_MARK) continue;

			double dRho = 0;
			double dMx = 0;
			double dMy = 0;
			double dMz = 0;
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
			}

			dRho = -dRho * dt;
			dMx = -dMx * dt;
			dMy = -dMy * dt;
			dMz = -dMz * dt;
			de = -de * dt;

			cell->setdDensity(dRho);
			cell->setdMomentum(dMx, 0);
			cell->setdMomentum(dMy, 1);
			cell->setdMomentum(dMz, 2);
			cell->setdTermEnergy(de -
				 (
					(pow(cell->getMomentum(0) + dMx,2) 
						+ pow(cell->getMomentum(1) + dMy,2) 
						+ pow(cell->getMomentum(2) + dMz,2)) / (cell->getDensity() + dRho) / 2.0
					-
					(pow(cell->getMomentum(0),2) 
						+ pow(cell->getMomentum(1),2) 
						+ pow(cell->getMomentum(2),2)) / cell->getDensity() / 2.0
 				 )
				-
				(
					  pow(0.5*cell->getSide(0, 0)->getB() + 0.5*cell->getSide(0, 0)->getdB() 
						+ 0.5*cell->getSide(0, 1)->getB() + 0.5*cell->getSide(0, 1)->getdB(), 2)
					+ pow(0.5*cell->getSide(1, 0)->getB() + 0.5*cell->getSide(1, 0)->getdB() 
						+ 0.5*cell->getSide(1, 1)->getB() + 0.5*cell->getSide(1, 1)->getdB(), 2)
					+ pow(0.5*cell->getSide(2, 0)->getB() + 0.5*cell->getSide(2, 0)->getdB() 
						+ 0.5*cell->getSide(2, 1)->getB() + 0.5*cell->getSide(2, 1)->getdB(), 2)
					-
					  pow(0.5*cell->getSide(0, 0)->getB() + 0.5*cell->getSide(0, 1)->getB(), 2)
					+ pow(0.5*cell->getSide(0, 0)->getB() + 0.5*cell->getSide(0, 1)->getB(), 2)
					+ pow(0.5*cell->getSide(0, 0)->getB() + 0.5*cell->getSide(0, 1)->getB(), 2)
					
				)/ Constants::Pi / 8.0
				);
		}		
	}
	public: void update(){
		for(int i=0;i<getSize();i++){
			Cell* cell = getCell(i);

			if(cell->getMark() == 0){
				cell->setDensity(cell->getDensity() + cell->getdDensity());
				cell->setTermEnergy(cell->getTermEnergy() + cell->getdTermEnergy());
			}
			cell->setdDensity(0);
			cell->setdTermEnergy(0);
			for(int axis=0; axis<3; axis++){
				if(cell->getMark() == 0) cell->setMomentum(cell->getMomentum(axis) + cell->getdMomentum(axis), axis);
				cell->setdMomentum(0, axis);
			}

			for(int axis=0; axis<3; axis++){
				if(cell->getSide(axis, Cell::BACKWARD_SIDE)->getMark() == 0) {
					double B = cell->getSide(axis, Cell::BACKWARD_SIDE)->getB();
					double dB = cell->getSide(axis, Cell::BACKWARD_SIDE)->getdB();
					cell->getSide(axis, Cell::BACKWARD_SIDE)->setB(B+dB);
				}
				cell->getSide(axis, Cell::BACKWARD_SIDE)->setdB(0);

				if(cell->getRib(axis,Cell::dr)->getMark() == 0) {
					cell->getRib(axis,Cell::dr)->setE(cell->getRib(axis,Cell::dr)->getE() + cell->getRib(axis,Cell::dr)->getdE());
				}
				cell->getRib(axis, Cell::dr)->setdE(0);
				if(cell->getRib(axis,Cell::dl)->getMark() == 0) {
					cell->getRib(axis,Cell::dl)->setE(cell->getRib(axis,Cell::dl)->getE() + cell->getRib(axis,Cell::dl)->getdE());
				}
				cell->getRib(axis, Cell::dl)->setdE(0);
				if(cell->getRib(axis,Cell::ur)->getMark() == 0) {
					cell->getRib(axis,Cell::ur)->setE(cell->getRib(axis,Cell::ur)->getE() + cell->getRib(axis,Cell::ur)->getdE());
				}
				cell->getRib(axis, Cell::ur)->setdE(0);
				if(cell->getRib(axis,Cell::ul)->getMark() == 0) {
					cell->getRib(axis,Cell::ul)->setE(cell->getRib(axis,Cell::ul)->getE() + cell->getRib(axis,Cell::ul)->getdE());
				}
				cell->getRib(axis, Cell::ul)->setdE(0);
			}
		}
		isMaxSpeedCalculated = false;
	}

	public: void defineInternalBorders(){
		vector<BorderCondition*> borders = getBorderConditions();
		for(int ind=0; ind < borders.size(); ind++){
			BorderCondition* bc = borders.at(ind);

			for(int i=0;i<getSize();i++){
				Cell* cell = getCell(i);
				if(bc->isBorder(cell)) bc->markCell(cell);
			}
		}
	}
};
