class Cell;
class Rib;

class Side: public SideInt
{
	public: static const signed char INTERNAL_SIDE_MARK = 0;
	public: static const signed char FREE_BORDER_SIDE_MARK = 1;
	public: static const signed char SW_BORDER_SIDE_MARK = 2;
	public: static const signed char MPI_SIDE_MARK = 3;

	public: static const int LEFT_RIB = 0;
	public: static const int UP_RIB = 1;
	public: static const int RIGHT_RIB = 2;
	public: static const int DOWN_RIB = 3;

	protected: int axis;
	protected: int mark; //0 - internal side; 1 - border free side; 2 - border sw side
	protected: double Bbg;
	protected: double B;
	protected: double dB;
	protected: double* F; //8 elements
	protected: Cell* fcell; //Forward cell
	protected: Cell* bcell; //Backward cell
	protected: Rib* ribs[4];
	protected: bool flowCalculated;

	public: Side(){}

	public: Side(int axis, double B, Cell* bcell, Cell* fcell){
		Side::axis = axis;
		Side::B = B;
		Side::dB = 0;
		Side::Bbg = 0;
		Side::fcell = fcell;
		Side::bcell = bcell;
		Side::flowCalculated = false;

		for(int i=0; i<4; i++) ribs[i] = NULL;
		F = new double[8];
	}

	public: Side(int axis, Cell* bcell, Cell* fcell){
		Side::axis = axis;
		Side::B = 0;
		Side::dB = 0;
		Side::Bbg = 0;
		Side::fcell = fcell;
		Side::bcell = bcell;
		Side::flowCalculated = false;

		for(int i=0; i<4; i++) ribs[i] = NULL;
		F = new double[8];
	}

	public: ~Side(){}

	public: void free(){
	}

	public: int getAxis(){
		return axis;
	}
	public: void setB(double B){
		Side::B = B;
	}
	public: double getB(){
		return B;
	}
	public: void setdB(double dB){
		Side::dB = dB;
	}
	public: double getdB(){
		return dB;
	}
	public: double getBbg(){
		return Bbg;
	}
	public: double getBplusBbg(){
		return B + Bbg;
	}
	public: void setBbg(double Bbg){
		Side::Bbg = Bbg;
	}

	public: Cell* getFCell(){
		return fcell;
	}
	public: Cell* getBCell(){
		return bcell;
	}

	public: void setCell(int dir, Cell* cell){
		if(dir == 1) fcell = cell;
		else if(dir == 0) bcell = cell;
	}

	public: Cell* getCell(int ind){
		if(ind == 1) return fcell;
		if(ind == 0) return bcell;

		return NULL;
	}
	public: void setMark(int mark){
		Side::mark = mark;
	}
	public: int getMark(){
		return mark;
	}
	public: void setFlow(vector<double> inF){
		for(int i=0; i<8; i++) F[i] = inF[i];
	}
	public: double* getFlow(){
		return F;
	}
	public: double getFlow(int ind){
		return F[ind];
	}
	public: void setRib(Rib* rib, int ind){
		ribs[ind] = rib;
	}
	public: Rib* getRib(int ind){
		return ribs[ind];
	}

	public: Rib* getRib(int inaxis, int dir){
		if(inaxis == (axis+2)%3 && dir == 1) return ribs[RIGHT_RIB];
		if(inaxis == (axis+2)%3 && dir == 0) return ribs[LEFT_RIB];
		if(inaxis == (axis+1)%3 && dir == 1) return ribs[UP_RIB];
		if(inaxis == (axis+1)%3 && dir == 0) return ribs[DOWN_RIB];

		return NULL;
	}

	public: bool isFlowCalculated(){
		return flowCalculated;
	}

	public: void setIsFlowCalculated(bool is){
		flowCalculated = is;
	}

//	public: double getH(int axis){
//		return fcell->getH(axis);
//	}
};
