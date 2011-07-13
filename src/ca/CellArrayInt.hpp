class Iterator;

class CellArrayInt{

	public: virtual Iterator* getIterator(){};
	public: void virtual updateBorderCells(){};
	public: virtual void updateBorderCurrents(){}
	protected: virtual void markBorderCells(){};
	public: virtual void updateBorderSides(){};

	public: void virtual calculateMinTimeStep(CalculaterInt* calc, int itime){};
	public: void virtual calculateIncrements(){};
	public: void virtual update(){};
	public: void virtual defineInternalBorders(){};

	protected: virtual Side* defineSideType(int axis, Cell* bcell, Cell* fcell){};
	protected: virtual Rib* defineRibType(int axis,Cell* ulcell,Cell* urcell,Cell* dlcell,Cell* drcell){};

	public: virtual double getTimeStep(){};
	public: virtual void setTimeStep(double step){};
	public: virtual double getMaxSpeed(){};
	public: virtual void setMaxSpeed(double maxs){};

/*	public: int virtual getSize(){};
	public: int virtual getBorderSiz(){};
	public: Cell* getCell(int ind){};
	public: Cell* getBorderCell(int ind){};
	public: void virtual addTime(double dt){};
	public: double virtual getTime(){};
	public: void virtual setTimeStep(double dt){};
	public: double virtual getTimeStep(){};
	public: Cell* getCell(int x[3]){};
	public: Cell* getCell(int x, int y, int z){};


	public: void virtual updateBorderSides(){};
	public: void virtual updateBorderCurrents(){};

*/
};
