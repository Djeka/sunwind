#include <iostream>
using namespace std;

class Calculater: public CalculaterInt{
	protected: CellArray* ca;
	protected: Method* method;
	protected: Method* bgmethod;

	public: Calculater(){}

	public: Calculater(CellArray* ca, Method* method){
		Calculater::ca = ca;
		Calculater::method = method;
	}

	public: void calculateFlows(){
		cout << "test" << endl;

		CellArray ca = *Calculater::ca;

		for(int axis=0; axis<3; axis++){
			for(int i=0;i<ca.getSize();i++){
				Cell lc = *ca.getCell(i);
				if(lc.getCell(axis, Cell::FORWARD_SIDE) == NULL) continue;
				Cell rc = *lc.getCell(axis, Cell::FORWARD_SIDE);
				lc.getSide(axis, Cell::FORWARD_SIDE)->setFlow(calculateFlow(axis, lc, rc));
			}
		}
	}

	private: vector<double> calculateFlow(int axis, Cell lc, Cell rc){
		vector<double> F(8, 0);
		double Ul[8];
		double Ur[8];

		Ul[0] = lc.getDensity();
		Ul[1] = lc.getMomentum(axis);
		Ul[2] = lc.getMomentum((axis+1)%3);
		Ul[3] = lc.getMomentum((axis+2)%3);
		Ul[4] = lc.getFullEnergy();
		Ul[5] = (lc.getSide(axis,Cell::FORWARD_SIDE)->getB() + lc.getSide(axis,Cell::BACKWARD_SIDE)->getB() ) / 2.0;
		Ul[6] = (lc.getSide((axis+1)%3,Cell::FORWARD_SIDE)->getB() + lc.getSide((axis+1)%3,Cell::BACKWARD_SIDE)->getB() ) / 2.0;
		Ul[7] = (lc.getSide((axis+2)%3,Cell::FORWARD_SIDE)->getB() + lc.getSide((axis+2)%3,Cell::BACKWARD_SIDE)->getB() ) / 2.0;

		Ur[0] = rc.getDensity();
		Ur[1] = rc.getMomentum(axis);
		Ur[2] = rc.getMomentum((axis+1)%3);
		Ur[3] = rc.getMomentum((axis+2)%3);
		Ur[4] = rc.getFullEnergy();
		Ur[5] = (rc.getSide(axis,Cell::FORWARD_SIDE)->getB() + rc.getSide(axis,Cell::BACKWARD_SIDE)->getB() ) / 2.0;
		Ur[6] = (rc.getSide((axis+1)%3,Cell::FORWARD_SIDE)->getB() + rc.getSide((axis+1)%3,Cell::BACKWARD_SIDE)->getB() ) / 2.0;
		Ur[7] = (rc.getSide((axis+2)%3,Cell::FORWARD_SIDE)->getB() + rc.getSide((axis+2)%3,Cell::BACKWARD_SIDE)->getB() ) / 2.0;

		vector<double> inF = method->calculateFlow(Ul, Ur, true);

		F[0] = inF[0];
		F[1+axis] = inF[1];
		F[1+(axis+1)%3] =inF[2];
		F[1+(axis+2)%3] =inF[3];
		F[4] = inF[4];
		F[5+axis] = inF[5];
		F[5+(axis+1)%3] =inF[6];
		F[5+(axis+2)%3] =inF[7];

		return F;
	}

	public: double calculateMaxSpeed(int axis, Cell lc, Cell rc){
		double Ul[8];
		double Ur[8];

		Ul[0] = lc.getDensity();
		Ul[1] = lc.getMomentum(axis);
		Ul[2] = lc.getMomentum((axis+1)%3);
		Ul[3] = lc.getMomentum((axis+2)%3);
		Ul[4] = lc.getFullEnergy();
		Ul[5] = (lc.getSide(axis,Cell::FORWARD_SIDE)->getB() + lc.getSide(axis,Cell::BACKWARD_SIDE)->getB() ) / 2.0;
		Ul[6] = (lc.getSide((axis+1)%3,Cell::FORWARD_SIDE)->getB() + lc.getSide((axis+1)%3,Cell::BACKWARD_SIDE)->getB() ) / 2.0;
		Ul[7] = (lc.getSide((axis+2)%3,Cell::FORWARD_SIDE)->getB() + lc.getSide((axis+2)%3,Cell::BACKWARD_SIDE)->getB() ) / 2.0;

		Ur[0] = rc.getDensity();
		Ur[1] = rc.getMomentum(axis);
		Ur[2] = rc.getMomentum((axis+1)%3);
		Ur[3] = rc.getMomentum((axis+2)%3);
		Ur[4] = rc.getFullEnergy();
		Ur[5] = (rc.getSide(axis,Cell::FORWARD_SIDE)->getB() + rc.getSide(axis,Cell::BACKWARD_SIDE)->getB() ) / 2.0;
		Ur[6] = (rc.getSide((axis+1)%3,Cell::FORWARD_SIDE)->getB() + rc.getSide((axis+1)%3,Cell::BACKWARD_SIDE)->getB() ) / 2.0;
		Ur[7] = (rc.getSide((axis+2)%3,Cell::FORWARD_SIDE)->getB() + rc.getSide((axis+2)%3,Cell::BACKWARD_SIDE)->getB() ) / 2.0;

		double maxs = method->getMaxSpeed(Ul, Ur);

		return maxs;
	}

	public: void calculateFields(){
		calculateFieldE();
		calculateFieldB();
	}

	public: void calculateFieldB(){
		CellArray ca = *Calculater::ca;

		for(int axis=0; axis<3; axis++){
			for(int i=0;i<ca.getSize();i++){
				Cell cell = *ca.getCell(i);
				if(cell.getSide(axis, Cell::BACKWARD_SIDE)->getMark() == Side::INTERNAL_SIDE_MARK) 
						calculateFieldB(cell.getSide(axis, Cell::BACKWARD_SIDE));
			}
		}		
	}

	private: void calculateFieldB(Side* side){
		int axis = side->getAxis();

		double fEx = side->getRib(Side::UP_RIB)->getE() + side->getRib(Side::UP_RIB)->getdE();
		double bEx = side->getRib(Side::DOWN_RIB)->getE() + side->getRib(Side::DOWN_RIB)->getdE();
		double fEy = side->getRib(Side::RIGHT_RIB)->getE() + side->getRib(Side::RIGHT_RIB)->getdE();
		double bEy = side->getRib(Side::LEFT_RIB)->getE() + side->getRib(Side::LEFT_RIB)->getdE();

		double dx = side->getBCell()->getH((axis+1)%3);
		double dy = side->getBCell()->getH((axis+2)%3);

		double dEx = fEx - bEx;
		double dEy = fEy - bEy;

		double dB = -1.0 *  Constants::c * ca->getTimeStep() * (dEy/dx - dEx/dy);

		side->setdB(dB);
	}

	public: void calculateFieldE(){
		CellArray ca = *Calculater::ca;

		for(int axis=0; axis<3; axis++){
			for(int i=0;i<ca.getSize();i++){
				Cell cell = *ca.getCell(i);
				if(cell.getRib(axis, Cell::dr)->getMark() == Rib::INTERNAL_RIB_MARK) calculateFieldE(cell.getRib(axis, Cell::dr));
			}
		}		
	}

	private: void calculateFieldE(Rib* rib){
		int axis = rib->getAxis();

		Side* upSide  = rib->getCell(Cell::ur)->getSide((axis+1)%3, Cell::BACKWARD_SIDE);
		Side* downSide = rib->getCell(Cell::dr)->getSide((axis+1)%3, Cell::BACKWARD_SIDE);
		Side* rightSide  = rib->getCell(Cell::ur)->getSide((axis+2)%3, Cell::BACKWARD_SIDE);
		Side* leftSide  = rib->getCell(Cell::ul)->getSide((axis+2)%3, Cell::BACKWARD_SIDE);

		double leftE = 	1.0 / Constants::c * leftSide->getFlow(5 + (axis+1)%3 );
		double rightE = 1.0 / Constants::c * rightSide->getFlow(5 + (axis+1)%3 );
		double upE = 	-1.0 / Constants::c * upSide->getFlow(5 + (axis+2)%3 );
		double downE = 	-1.0 / Constants::c * downSide->getFlow(5 + (axis+2)%3 );

		double dE = 0.25 * (leftE + rightE + upE + downE) - rib->getE();

		rib->setdE(dE);
	}

};
