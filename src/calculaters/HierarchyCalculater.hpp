#include <iostream>
using namespace std;

class HierarchyCalculater: public Calculater{

	public: HierarchyCalculater(CellArray* ca, Method* method){
		Calculater::ca = ca;
		Calculater::method = method;
	}

	public: HierarchyCalculater(CellArray* ca, Method* method, Method* bgmethod){
		Calculater::ca = ca;
		Calculater::method = method;
		Calculater::bgmethod = bgmethod;
	}

	public: void calculateFlows(){
cout << "calculateFlows";
clock_t start = clock();
		for(HierarchyCellIterator cit = HierarchyCellIterator(ca); cit.hasNext();){
			HierarchyCell* cell = (HierarchyCell*) cit.next();
			for(int axis=0; axis<3; axis++){
				HierarchySide* side = (HierarchySide*) cell->getSide(axis, Cell::FORWARD_SIDE);
				if(!cell->isSplitted() && side->getFCell() != NULL && !side->isSplitted() && !side->isFlowCalculated()){
					side->setFlow(calculateFlow(side, false));
					side->setIsFlowCalculated(true);
				}

				side = (HierarchySide*) cell->getSide(axis, Cell::BACKWARD_SIDE);
				if(!cell->isSplitted() && side->getBCell() != NULL && !side->isSplitted()
					&& cell->getDeep() > ((HierarchyCell*) side->getBCell())->getDeep() && !side->isFlowCalculated()){
					side->setFlow(calculateFlow(side, false));
					side->setIsFlowCalculated(true);
				}
			}
		}
cout.setf(ios::showpoint,ios::floatfield); cout.precision(3);
cout << "\t" << ((double)(clock()-start))/CLOCKS_PER_SEC << endl;
cout.setf(ios::scientific,ios::floatfield); cout.precision(10);
	}

	private: vector<double> calculateFlow(Side* side, bool debug){
		HierarchyCell* lc = (HierarchyCell*) side->getBCell();
		HierarchyCell* rc = (HierarchyCell*) side->getFCell();
		int axis = side->getAxis();
		vector<double> F(8, 0);
		double Ul[8];
		double Ur[8];
		double Bdl[3];
		double Bdr[3];
		double B = side->getB();

		Ul[0] = lc->getDensity();
		Ul[1] = lc->getMomentum(axis);
		Ul[2] = lc->getMomentum((axis+1)%3);
		Ul[3] = lc->getMomentum((axis+2)%3);
		Ul[4] = lc->getFullEnergy();
		Ul[5] = (lc->getSide(axis,Cell::FORWARD_SIDE)->getB() + lc->getSide(axis,Cell::BACKWARD_SIDE)->getB() ) / 2.0;
		Ul[6] = (lc->getSide((axis+1)%3,Cell::FORWARD_SIDE)->getB() + lc->getSide((axis+1)%3,Cell::BACKWARD_SIDE)->getB() ) / 2.0;
		Ul[7] = (lc->getSide((axis+2)%3,Cell::FORWARD_SIDE)->getB() + lc->getSide((axis+2)%3,Cell::BACKWARD_SIDE)->getB() ) / 2.0;

		Ur[0] = rc->getDensity();
		Ur[1] = rc->getMomentum(axis);
		Ur[2] = rc->getMomentum((axis+1)%3);
		Ur[3] = rc->getMomentum((axis+2)%3);
		Ur[4] = rc->getFullEnergy();
		Ur[5] = (rc->getSide(axis,Cell::FORWARD_SIDE)->getB() + rc->getSide(axis,Cell::BACKWARD_SIDE)->getB() ) / 2.0;
		Ur[6] = (rc->getSide((axis+1)%3,Cell::FORWARD_SIDE)->getB() + rc->getSide((axis+1)%3,Cell::BACKWARD_SIDE)->getB() ) / 2.0;
		Ur[7] = (rc->getSide((axis+2)%3,Cell::FORWARD_SIDE)->getB() + rc->getSide((axis+2)%3,Cell::BACKWARD_SIDE)->getB() ) / 2.0;

		Bdl[0] =  (lc->getSide(axis,Cell::FORWARD_SIDE)->getBbg() 	+ lc->getSide(axis,Cell::BACKWARD_SIDE)->getBbg() ) / 2.0;
		Bdl[1] =  (lc->getSide((axis+1)%3,Cell::FORWARD_SIDE)->getBbg() + lc->getSide((axis+1)%3,Cell::BACKWARD_SIDE)->getBbg() ) / 2.0;
		Bdl[2] =  (lc->getSide((axis+2)%3,Cell::FORWARD_SIDE)->getBbg() + lc->getSide((axis+2)%3,Cell::BACKWARD_SIDE)->getBbg() ) / 2.0;

		Bdr[0] =  (rc->getSide(axis,Cell::FORWARD_SIDE)->getBbg() 	+ rc->getSide(axis,Cell::BACKWARD_SIDE)->getBbg() ) / 2.0;
		Bdr[1] =  (rc->getSide((axis+1)%3,Cell::FORWARD_SIDE)->getBbg() + rc->getSide((axis+1)%3,Cell::BACKWARD_SIDE)->getBbg() ) / 2.0;
		Bdr[2] =  (rc->getSide((axis+2)%3,Cell::FORWARD_SIDE)->getBbg() + rc->getSide((axis+2)%3,Cell::BACKWARD_SIDE)->getBbg() ) / 2.0;

		if(lc->getMark() != Cell::INTERNAL_CELL_MARK || rc->getMark() != Cell::INTERNAL_CELL_MARK){
			if(lc->getMark() == Cell::FREE_BORDER_CELL_MARK && rc->getMark() == Cell::INTERNAL_CELL_MARK)
				for(int i=0; i<8; i++) Ul[i] = Ur[i];
			else if(rc->getMark() == Cell::FREE_BORDER_CELL_MARK && lc->getMark() == Cell::INTERNAL_CELL_MARK)
				for(int i=0; i<8; i++) Ur[i] = Ul[i];
			else{
				vector<BorderCondition*> borders = ca->getBorderConditions();
				for(int ind=0; ind < borders.size(); ind++){
					BorderCondition* bc = borders.at(ind);
					if(lc->getMark() == bc->getMark() && rc->getMark() == Cell::INTERNAL_CELL_MARK)
						for(int i=0; i<8; i++) Ul[i] = bc->getBorderU(axis, Cell::BACKWARD_SIDE, rc, Ur, i);
					else if(rc->getMark() == bc->getMark() && lc->getMark() == Cell::INTERNAL_CELL_MARK)
						for(int i=0; i<8; i++) Ur[i] = bc->getBorderU(axis, Cell::FORWARD_SIDE, lc, Ul, i);
				}
			}
		}

		double schemaMaxSpeed = method->getMaxSpeed(Ul, Ur);
		double bgMaxSpeed = bgmethod->getMaxSpeed(Ul, Ur, Bdl, Bdr);
		double maxspeed = max(schemaMaxSpeed, bgMaxSpeed);

		vector<double> inF = method->calculateFlow(Ul, Ur, schemaMaxSpeed);
		vector<double> bgF = bgmethod->calculateFlow(Ul, Ur, Bdl, Bdr, bgMaxSpeed);

//		double maxspeed = method->getMaxSpeed(Ul, Ur);
//		vector<double> inF = method->calculateFlow(Ul, Ur, maxspeed);
//		vector<double> bgF(8,0);

		F[0] 		= inF[0] + bgF[0];
		F[1+axis] 	= inF[1] + bgF[1];
		F[1+(axis+1)%3] = inF[2] + bgF[2];
		F[1+(axis+2)%3] = inF[3] + bgF[3];
		F[4] 		= inF[4] + bgF[4];
		F[5+axis] 	= inF[5] + bgF[5];
		F[5+(axis+1)%3] = inF[6] + bgF[6];
		F[5+(axis+2)%3] = inF[7] + bgF[7];

		double dt = Constants::curant * (lc->getH(axis)/2.0 + rc->getH(axis)/2.0) / maxspeed;
		if(ca->getTimeStep() > dt){
			ca->setTimeStep(dt);
			ca->setMaxSpeed(maxspeed);
		}

		return F;
	}

	public: void calculateFields(){
		calculateFieldE();
		calculateFieldB();
	}

	public: void calculateFieldB(){
cout << "calculateFieldB";
clock_t start = clock();

		for(int axis=0; axis<3; axis++){
			for(HierarchyCellIterator cit = HierarchyCellIterator(ca); cit.hasNext();){
				HierarchyCell* cell = (HierarchyCell*) cit.next();
				HierarchySide* side = NULL;

				side = (HierarchySide*) cell->getSide(axis, Cell::BACKWARD_SIDE);
				if(side->getMark() == Side::INTERNAL_SIDE_MARK && !side->isSplitted() && side->getdB() == 0)
					calculateFieldB(side, false);

				side = (HierarchySide*) cell->getSide(axis, Cell::FORWARD_SIDE);
				if(side->getMark() == Side::INTERNAL_SIDE_MARK && !side->isSplitted() && side->getdB() == 0)
					calculateFieldB(side, false);
			}
		}

cout.setf(ios::showpoint,ios::floatfield); cout.precision(3);
cout << "\t" << ((double)(clock()-start))/CLOCKS_PER_SEC << endl;
cout.setf(ios::scientific,ios::floatfield); cout.precision(10);	
	}

	private: void calculateFieldB(Side* side, bool debug){
		int axis = side->getAxis();

		double fE2 = side->getRib(Side::RIGHT_RIB)->getE() + side->getRib(Side::RIGHT_RIB)->getdE();
		double bE2 = side->getRib(Side::LEFT_RIB)->getE() + side->getRib(Side::LEFT_RIB)->getdE();
		double fE1 = side->getRib(Side::UP_RIB)->getE() + side->getRib(Side::UP_RIB)->getdE();
		double bE1 = side->getRib(Side::DOWN_RIB)->getE() + side->getRib(Side::DOWN_RIB)->getdE();

		double d1 = 0;
		double d2 = 0;

		if(((HierarchyCell*) side->getFCell())->getDeep() > ((HierarchyCell*) side->getBCell())->getDeep()){
			d1 = side->getFCell()->getH((axis+1)%3);
			d2 = side->getFCell()->getH((axis+2)%3);
		}
		else{
			d1 = side->getBCell()->getH((axis+1)%3);
			d2 = side->getBCell()->getH((axis+2)%3);
		}

		double dE1 = fE1 - bE1;
		double dE2 = fE2 - bE2;

		double dB = -1.0 *  Constants::c * ca->getTimeStep() * (dE2/d1 - dE1/d2);

		side->setdB(dB);
	}

	public: void calculateFieldE(){
cout << "calculateFieldE";
clock_t start = clock();

		for(HierarchyCellIterator cit = HierarchyCellIterator(ca); cit.hasNext();){
			HierarchyCell* cell = (HierarchyCell*) cit.next();
			for(int axis=0; axis<3; axis++){
				for(int irib=0; irib<4; irib++){
					calculateFieldE(cell->getRib(axis, irib), false);
				}
			}
		}

cout.setf(ios::showpoint,ios::floatfield); cout.precision(3);
cout << "\t" << ((double)(clock()-start))/CLOCKS_PER_SEC << endl;
cout.setf(ios::scientific,ios::floatfield); cout.precision(10);
	}

	private: void calculateFieldE(Rib* rib, bool debug){
		if(!(rib->getMark() == Rib::INTERNAL_RIB_MARK && !((HierarchyRib*)rib)->isSplitted() && rib->getdE() == 0)) return;

		int axis = rib->getAxis();
		double leftF;
		double rightF;
		double upF;
		double downF;

		Side* upSide  = rib->getSide(Rib::UP_SIDE);
		Side* downSide  = rib->getSide(Rib::DOWN_SIDE);
		Side* rightSide  = rib->getSide(Rib::RIGHT_SIDE);
		Side* leftSide  = rib->getSide(Rib::LEFT_SIDE);

		if(rightSide != NULL) rightF = rightSide->getFlow(5 + (axis+1)%3 );
		if(leftSide != NULL) leftF = leftSide->getFlow(5 + (axis+1)%3 );
		if(upSide != NULL) upF = upSide->getFlow(5 + (axis+2)%3 );
		if(downSide != NULL) downF = downSide->getFlow(5 + (axis+2)%3 );

		if(rightSide == NULL) rightF = extrapolateFlowOnSide(Rib::RIGHT_SIDE, rib, Cell::dl, Cell::dr, (axis+1)%3, debug);
		if(leftSide == NULL) leftF = extrapolateFlowOnSide(Rib::LEFT_SIDE, rib, Cell::ul, Cell::ur, (axis+1)%3, debug);
		if(upSide == NULL) upF = extrapolateFlowOnSide(Rib::UP_SIDE, rib, Cell::dl, Cell::ul, (axis+2)%3, debug);
		if(downSide == NULL) downF = extrapolateFlowOnSide(Rib::DOWN_SIDE, rib, Cell::dr, Cell::ur, (axis+2)%3, debug);

		if(rightSide != NULL) rightF = correctFlowIfSideIsBorder(rightF, rightSide, rib, axis, Rib::RIGHT_SIDE);
		if(leftSide != NULL) leftF = correctFlowIfSideIsBorder(leftF, leftSide, rib, axis, Rib::LEFT_SIDE);
		if(upSide != NULL) upF = correctFlowIfSideIsBorder(upF, upSide, rib, axis, Rib::UP_SIDE);
		if(downSide != NULL) downF = correctFlowIfSideIsBorder(downF, downSide, rib, axis, Rib::DOWN_SIDE);

		double leftE = 	1.0 / Constants::c * leftF;
		double rightE = 1.0 / Constants::c * rightF;
		double upE =  - 1.0 / Constants::c * upF;
		double downE= -	1.0 / Constants::c * downF;

		double dE = 0.25 * (leftE + rightE + upE + downE) - rib->getE();

		rib->setdE(dE);
	}

	private: double correctFlowIfSideIsBorder(double defaultF, Side* side, Rib* rib, int axis, int sideIndex){
		double F = defaultF;
		if(side->getMark() != Side::INTERNAL_SIDE_MARK && side->getMark() != Side::FREE_BORDER_SIDE_MARK){
			int vIndex = 0;
			if(sideIndex == Rib::LEFT_SIDE) vIndex = Rib::RIGHT_SIDE;
			else if(sideIndex == Rib::RIGHT_SIDE) vIndex = Rib::LEFT_SIDE;
			else if(sideIndex == Rib::UP_SIDE) vIndex = Rib::DOWN_SIDE;
			else if(sideIndex == Rib::DOWN_SIDE) vIndex = Rib::UP_SIDE;

			if(rib->getSide(vIndex) != NULL && rib->getSide(vIndex)->getMark() != Side::INTERNAL_SIDE_MARK){
				vector<BorderCondition*> borders = ca->getBorderConditions();
				for(int ind=0; ind < borders.size(); ind++){
					BorderCondition* bc = borders.at(ind);
					if(side->getMark() == bc->getMark()) F = bc->getBorderFlowOnSide(axis, rib, sideIndex);
				}
			}
		}

		return F;
	}

	//TODO: сделать грамотноую экстраполяцию поля на грани в случае когда одна ячейка разсплитована, а другая нет
	private: double extrapolateFlowOnSide(int sideInd, Rib* rib, int ind0, int ind1, int Faxis, bool debug){
		double flow = 0;
		int axis = rib->getAxis();

		HierarchyCell* cell0 = ((HierarchyCell*) rib->getCell(ind0));
		HierarchyCell* cell1 = ((HierarchyCell*) rib->getCell(ind1));

		int vind0;
		int vind1;
		int vSideInd;
		if(ind0 == Cell::ur) vind0 = Cell::dl;
		else if(ind0 == Cell::ul) vind0 = Cell::dr;
		else if(ind0 == Cell::dr) vind0 = Cell::ul;
		else if(ind0 == Cell::dl) vind0 = Cell::ur;

		if(ind1 == Cell::ur) vind1 = Cell::dl;
		else if(ind1 == Cell::ul) vind1 = Cell::dr;
		else if(ind1 == Cell::dr) vind1 = Cell::ul;
		else if(ind1 == Cell::dl) vind1 = Cell::ur;

		if(sideInd == Rib::RIGHT_SIDE) vSideInd = Rib::LEFT_SIDE;
		else if(sideInd == Rib::LEFT_SIDE) vSideInd = Rib::RIGHT_SIDE;
		else if(sideInd == Rib::UP_SIDE) vSideInd = Rib::DOWN_SIDE;
		else if(sideInd == Rib::DOWN_SIDE) vSideInd = Rib::UP_SIDE;

		if(cell1->getDeep() > cell0->getDeep()){
			if(cell0->getRib(axis, vind0)->getSide(sideInd)->getMark() == Side::FREE_BORDER_SIDE_MARK)
				return rib->getSide(vSideInd)->getFlow(5 + Faxis);
			else
				return cell0->getRib(axis, vind0)->getSide(sideInd)->getFlow(5 + Faxis);
		} else if(cell1->getDeep() < cell0->getDeep()){
			if(cell1->getRib(axis, vind1)->getSide(sideInd)->getMark() == Side::FREE_BORDER_SIDE_MARK)
				return rib->getSide(vSideInd)->getFlow(5 + Faxis);
			else
				return cell1->getRib(axis, vind1)->getSide(sideInd)->getFlow(5 + Faxis);
		} else if(cell1->getParent() != cell0->getParent()){
			if(cell1->getParent()->getRib(axis, vind1)->getSide(sideInd)->getMark() == Side::FREE_BORDER_SIDE_MARK)
				return rib->getSide(vSideInd)->getFlow(5 + Faxis);
			else
				return cell1->getParent()->getRib(axis, vind1)->getSide(sideInd)->getFlow(5 + Faxis);
		} else{
			if(cell1->getParent()->getRib(axis, vind0)->getSide(sideInd)->getMark() == Side::FREE_BORDER_SIDE_MARK ||
			cell1->getParent()->getRib(axis, vind1)->getSide(sideInd)->getMark() == Side::FREE_BORDER_SIDE_MARK)
				return rib->getSide(vSideInd)->getFlow(5 + Faxis);
			else
				return 0.5 * (cell1->getParent()->getRib(axis, vind0)->getSide(sideInd)->getFlow(5 + Faxis) 
				     + cell1->getParent()->getRib(axis, vind1)->getSide(sideInd)->getFlow(5 + Faxis));
		}

	}
};
