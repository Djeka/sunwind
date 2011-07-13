#include <vector>
/*

      	        _____________________________
	      /            ur              /|
	     / |                          / |
	    /                            /  |
	 ul/   |                      ur/   |
	  /                            /    |
	 /     |                      /     |
	/____________________________/      |
	|      |      dr             |      |
	|                            |      |ur
	|      |                     |      |
	|      dr                    |      |
	|      |                     |      |				axis
	|                            |      |                    2
      dl|      |                     |ul    |                    |    1
        |                            |      |                    |   /
	|      |_ _ _ _ _ _ _ _ _ _ _|_ _ _ |                    |  /
	|                  ul        |     /                     | /
	|    /                       |    /                      |/________`0
	|   dl                       |   /
	|  /                         |  /dr
	|                            | /
	|/                           |/
	|____________________________|
		      dl

*/
class Rib;
class Side;

class Cell : public CellInt //Cell - Rectengular Parallepiped Cell
{
	public: static const signed int ul = 0; // Up-Left Rib
	public: static const signed int ur = 1; // Up-Right Rib
	public: static const signed int dl = 2; // Down-Left Rib
	public: static const signed int dr = 3; // Down-Right Rib

	public: static const signed char FORWARD_SIDE = 1;
	public: static const signed char BACKWARD_SIDE = 0;

	public: static const signed char INTERNAL_CELL_MARK = 0;
	public: static const signed char FREE_BORDER_CELL_MARK = 1;
	public: static const signed char SW_BORDER_CELL_MARK = 2;
	public: static const signed char MPI_CELL_MARK = 3;

	protected: int mark; //0 - internal cell; 1 - border free cell; 2 - border sw cell
	protected: double density;
	protected: double dDensity;
	protected: double* momentum;
	protected: double* dMomentum;
	protected: double termEnergy;
	protected: double dTermEnergy;
	protected: double* r;
	protected: double* h;
	protected: int* point;
	protected: Side* sides[3][2];
	protected: Rib* ribs[3][4];
	protected: int index;

	public: Cell(){}

	public: Cell(double density, double momentum[3], double termEnergy, double r[3],int point[3], double h[3], int index){
		Cell::index = index;
		Cell::density = density;
		setTermEnergy(termEnergy);
		Cell::dDensity = 0;
		setdTermEnergy(0);

		Cell::point = new int[3];
		Cell::momentum = new double[3];
		Cell::dMomentum = new double[3];
		Cell::r = new double[3];
		Cell::h = new double[3];

		for(int i=0; i<3; i++){
			Cell::momentum[i] = momentum[i];
			Cell::dMomentum[i] = 0;
			Cell::point[i] = point[i];
			Cell::r[i] = r[i];
			Cell::h[i] = h[i];
		}
	}

	public: void setIndex(int index){
		Cell::index = index;
	}
	public: int getIndex(){
		return index;
	}

	public: void setDensity(double density){
		Cell::density = density;
	}
	public: double getDensity(){
		return density;
	}

	public: void setMomentum(double momentum, int axis){
		Cell::momentum[axis] = momentum;
	}
	public: double getMomentum(int ind){
		return Cell::momentum[ind];
	}
	public: double* getMomentum(){
		return momentum;
	}
	public: void setTermEnergy(double termEnergy){
		Cell::termEnergy = termEnergy;
	}
	public: double getTermEnergy(){
		return termEnergy;
	}
	public: double getFullEnergy(){
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
	public: void setFullEnergy(double fullEnergy){
		double Ek = 0;
		double Em = 0;
		for(int axis=0; axis<3; axis++){
			Em += pow(0.5*(getSide(axis, Cell::FORWARD_SIDE)->getB() + getSide(axis, Cell::BACKWARD_SIDE)->getB()), 2);
			Ek += pow(getMomentum(axis), 2);
		}
		Em = Em / Constants::Pi / 8.0;
		Ek = Ek / getDensity() / 2.0;

		setTermEnergy( fullEnergy - Em - Ek );
	}

	public: double getR(int ind){
		return r[ind];
	}
	public: double* getR(){
		return r;
	}
	public: double getRadius(){
		return sqrt(pow(r[0],2) + pow(r[1],2) + pow(r[2],2));
	}
	public: double getH(int ind){
		return h[ind];
	}
	public: double* getH(){
		return h;
	}
	public: int getPoint(int ind){
		return point[ind];
	}
	public: int* getPoint(){
		return point;
	}
	public: void setSide(int axis, int dir, Side* side){
		sides[axis][dir] = side;
	}
	public: Side* getSide(int axis, int forward_or_backward_side){
		return sides[axis][forward_or_backward_side];
	}
	public: Cell* getCell(int axis, int forward_or_backward_side){
		return sides[axis][forward_or_backward_side]->getCell(forward_or_backward_side);
	}
	public: void setRib(int axis, int ind, Rib* rib){
		ribs[axis][ind] = rib;
	}
	public: Rib* getRib(int axis, int ind){
		return ribs[axis][ind];
	}
	public: void setMark(int mark){
		Cell::mark = mark;
	}
	public: int getMark(){
		return mark;
	}
	public: void setdDensity(double value){
		Cell::dDensity = value;
	}
	public: double getdDensity(){
		return dDensity;
	}
	public: void setdMomentum(double value, int ind){
		Cell::dMomentum[ind] = value;
	}
	public: double getdMomentum(int ind){
		return dMomentum[ind];
	}
	public: void setdTermEnergy(double value){
		Cell::dTermEnergy = value;
	}
	public: double getdTermEnergy(){
		return dTermEnergy;
	}
	public: double getT(){
		return getP() / density / (Constants::gamma-1.0) / Constants::Cv;
	}
	public: double getV(int ind){
		return momentum[ind]/density;
	}
	public: double getV(){
		return sqrt(pow(getV(0),2)+pow(getV(1),2)+pow(getV(2),2));
	}
	public: double getP(){
		return (Constants::gamma - 1.0)*getTermEnergy();
	}
	public: void setP(double P){
		setTermEnergy( P / (Constants::gamma - 1.0));
	}
	public: double getMaxSpeed(){
		double maxs = 0;
		for(int axis=0;axis<3;axis++){
			double u = getV(axis);

			Side* fside = getSide(axis,Cell::FORWARD_SIDE);
			Side* bside = getSide(axis,Cell::BACKWARD_SIDE);
			double Bn = (fside->getBplusBbg() + bside->getBplusBbg()) / 2.0;

			fside = getSide((axis+1)%3,Cell::FORWARD_SIDE);
			bside = getSide((axis+1)%3,Cell::BACKWARD_SIDE);
			double B1 = (fside->getBplusBbg() + bside->getBplusBbg()) / 2.0;

			fside = getSide((axis+2)%3,Cell::FORWARD_SIDE);
			bside = getSide((axis+2)%3,Cell::BACKWARD_SIDE);
			double B2 = (fside->getBplusBbg() + bside->getBplusBbg()) / 2.0;

			double ce =  sqrt(Constants::gamma*getP()/getDensity());

			double aa = abs(Bn) / sqrt(4*Constants::Pi*getDensity());
			double a1 = sqrt( pow(ce,2) + (pow(Bn,2) + pow(B1,2) + pow(B2,2))/4/Constants::Pi + 2*aa*ce);
			double a2 = sqrt( pow(ce,2) + (pow(Bn,2) + pow(B1,2) + pow(B2,2))/4/Constants::Pi - 2*aa*ce);
			double af = 0.5*(a1+a2);

			if(maxs < max(max(abs(u+aa),abs(u-aa)), max(abs(u+af),abs(u-af)))) maxs = max(max(abs(u+aa),abs(u-aa)), max(abs(u+af),abs(u-af)));
		}
		return maxs;
	}
	public: double getMaxSpeedOnSide(int axis, int direction){
		Cell* cell = getCell(axis, direction);
		double u = 0.5 * (getV(axis) + cell->getV(axis));
		double rho = 0.5 * (getDensity() + cell->getDensity());

		double Bn = getSide(axis,direction)->getBplusBbg();

		double B1 = 0.25 * (
				   getSide((axis+1)%3,Cell::FORWARD_SIDE)->getBplusBbg() 
				 + getSide((axis+1)%3,Cell::BACKWARD_SIDE)->getBplusBbg()
				 + cell->getSide((axis+1)%3,Cell::FORWARD_SIDE)->getBplusBbg() 
				 + cell->getSide((axis+1)%3,Cell::BACKWARD_SIDE)->getBplusBbg()
				   );

		double B2 = 0.25 * (
				   getSide((axis+2)%3,Cell::FORWARD_SIDE)->getBplusBbg() 
				 + getSide((axis+2)%3,Cell::BACKWARD_SIDE)->getBplusBbg()
				 + cell->getSide((axis+2)%3,Cell::FORWARD_SIDE)->getBplusBbg() 
				 + cell->getSide((axis+2)%3,Cell::BACKWARD_SIDE)->getBplusBbg()
				   );

		double ce =  sqrt(Constants::gamma*(getP() + cell->getP()) * 0.5 / rho);

		double aa = abs(Bn) / sqrt(4*Constants::Pi*rho);
		double a1 = sqrt( pow(ce,2) + (pow(Bn,2) + pow(B1,2) + pow(B2,2))/4/Constants::Pi + 2*aa*ce);
		double a2 = sqrt( pow(ce,2) + (pow(Bn,2) + pow(B1,2) + pow(B2,2))/4/Constants::Pi - 2*aa*ce);
		double af = 0.5*(a1+a2);

		return max(max(abs(u+aa),abs(u-aa)), max(abs(u+af),abs(u-af)));
	}
	public: double getMinTimeStep(){
		double dt = 1.0e+5;
		for(int axis=0;axis<3;axis++){
			double u = getV(axis);
			double Bn = (getSide(axis,Cell::FORWARD_SIDE)->getB() + getSide(axis,Cell::BACKWARD_SIDE)->getB()) / 2.0;
			double B1 = (getSide((axis+1)%3,Cell::FORWARD_SIDE)->getB() + getSide((axis+1)%3,Cell::BACKWARD_SIDE)->getB()) / 2.0;
			double B2 = (getSide((axis+2)%3,Cell::FORWARD_SIDE)->getB() + getSide((axis+2)%3,Cell::BACKWARD_SIDE)->getB()) / 2.0;
			double ce =  sqrt(Constants::gamma*getP()/getDensity());

			double aa = abs(Bn) / sqrt(4*Constants::Pi*getDensity());
			double a1 = sqrt( pow(ce,2) + (pow(Bn,2) + pow(B1,2) + pow(B2,2))/4/Constants::Pi + 2*aa*ce);
			double a2 = sqrt( pow(ce,2) + (pow(Bn,2) + pow(B1,2) + pow(B2,2))/4/Constants::Pi - 2*aa*ce);
			double af = 0.5*(a1+a2);

			if(Constants::curant * getH(axis) / max(max(abs(u+aa),abs(u-aa)), max(abs(u+af),abs(u-af))) < dt) 
					dt = Constants::curant * getH(axis) / max(max(abs(u+aa),abs(u-aa)), max(abs(u+af),abs(u-af)));
		}
		return dt;
	}
	public: bool isBorderCell(int axis, TaskData& td, int forward_or_backward){
		bool result = false;
		if(forward_or_backward==Cell::FORWARD_SIDE and getPoint(axis)+1 == td.getN(axis)) result = true;
		if(forward_or_backward==Cell::BACKWARD_SIDE and getPoint(axis) == 0) result = true;
		return result;
	}

	public: string getPointString(){
		string str = "[" + Utils::intToStr(getPoint(0)) + ","+ Utils::intToStr(getPoint(1)) + "," + Utils::intToStr(getPoint(2)) +  "]";
		return str;
	}

	public: double getDivB(){
		double divB = 0;
		for(int axis=0;axis<3;axis++){
			divB+= (getSide(axis, Cell::FORWARD_SIDE)->getB() - getSide(axis, Cell::BACKWARD_SIDE)->getB()) / getH(axis);
		}
		return divB;
	}
};
