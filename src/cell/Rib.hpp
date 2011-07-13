class Cell;

/*
                          2
                          |
                          |        
                          |       0
                          |      /
                 ur       |     /       dr
                          |    /
                          |   /
                          |  /
                          | /
            1_____________|/_______________
                         /|
                 ul     / |             dl
                       /  |
                      /   |
                     /    |
                    /     |
*/

class Rib: public RibInt
{
	public: static const signed char INTERNAL_RIB_MARK = 0;
	public: static const signed char FREE_BORDER_RIB_MARK = 1;
	public: static const signed char SW_BORDER_RIB_MARK = 2;
	public: static const signed char MPI_RIB_MARK = 3;

	public: static const int LEFT_SIDE = 0;
	public: static const int UP_SIDE = 1;
	public: static const int RIGHT_SIDE = 2;
	public: static const int DOWN_SIDE = 3;

	protected: int mark; //0 - internal rib; 1 - border free rib; 2 - border sw rib
	protected: int axis;
	protected: double E;
	protected: double dE;
	protected: Cell* ulcell;
	protected: Cell* urcell;
	protected: Cell* dlcell;
	protected: Cell* drcell;
	protected: Side* nearSides[4];

	public: Rib(){}

	public: Rib(int axis, double E){
		Rib::axis = axis;
		Rib::E = E;
		Rib::dE = 0;

		Rib::ulcell = NULL;
		Rib::urcell = NULL;
		Rib::dlcell = NULL;
		Rib::drcell = NULL;

//		nearSides = new Side*[4];
		nearSides[0] = NULL;
		nearSides[1] = NULL;
		nearSides[2] = NULL;
		nearSides[3] = NULL;
	}

	public: Rib(int axis){
		Rib::axis = axis;
		Rib::E = 0;
		Rib::dE = 0;

		Rib::ulcell = NULL;
		Rib::urcell = NULL;
		Rib::dlcell = NULL;
		Rib::drcell = NULL;

//		nearSides = new Side*[4];
		nearSides[0] = NULL;
		nearSides[1] = NULL;
		nearSides[2] = NULL;
		nearSides[3] = NULL;
	}

	public: Rib(int axis, double E, Cell* ulcell, Cell* urcell, Cell* dlcell, Cell* drcell){
		Rib::axis = axis;
		Rib::E = E;
		Rib::dE = 0;

		Rib::ulcell = ulcell;
		Rib::urcell = urcell;
		Rib::dlcell = dlcell;
		Rib::drcell = drcell;

//		nearSides = new Side*[4];
		nearSides[0] = NULL;
		nearSides[1] = NULL;
		nearSides[2] = NULL;
		nearSides[3] = NULL;
	}

	public: ~Rib() {}

	public: void free(){
	}

	public: void setCell(int ind, Cell* cell){
		if(ind == Cell::ul)	 ulcell = cell;
		else if(ind == Cell::ur) urcell = cell;
		else if(ind == Cell::dl) dlcell = cell;
		else if(ind == Cell::dr) drcell = cell;
	}

	public: int getAxis(){
		return axis;
	}
	public: void setE(double E){
		Rib::E = E;
	}
	public: double getE(){
		return E;
	}
	public: void setdE(double dE){
		Rib::dE = dE;
	}
	public: double getdE(){
		return dE;
	}

	public: Cell* getCell(int ind){
		if(ind == Cell::ul) return ulcell;
		if(ind == Cell::ur) return urcell;
		if(ind == Cell::dl) return dlcell;
		if(ind == Cell::dr) return drcell;

		return NULL;
	}
	public: void setMark(int mark){
		Rib::mark = mark;
	}
	public: int getMark(){
		return mark;
	}
/*
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
*/

	public: void setSide(int ind, Side* side){
		nearSides[ind] = side;
	}
//	public: Side** getNearSides(){
//		nearSides;
//	}
	public: Side* getSide(int ind){
		return nearSides[ind];
	}
	public: Side* getSide(int axis, int dir){
		if(axis == (getAxis()+1)%3 && dir == Cell::FORWARD_SIDE) return nearSides[RIGHT_SIDE];
		else if(axis == (getAxis()+1)%3 && dir == Cell::BACKWARD_SIDE) return nearSides[LEFT_SIDE];
		else if(axis == (getAxis()+2)%3 && dir == Cell::FORWARD_SIDE) return nearSides[UP_SIDE];
		else if(axis == (getAxis()+2)%3 && dir == Cell::BACKWARD_SIDE) return nearSides[DOWN_SIDE];
		else return NULL;
	}
};
