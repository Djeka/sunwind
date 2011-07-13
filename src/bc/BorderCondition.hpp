class BorderCondition {
	public: virtual void markCell(Cell* cell){};
	public: virtual bool isBorder(Cell* cell){};
	public: virtual int getBorderMark(Cell* cell){};
	public: virtual int getMark(){};
	public: virtual double getBorderU(int axis, int dir, Cell* cell, double* inU, int ind){};
	public: virtual double getBorderFlowOnSide(int axis, Rib* rib, int sideIndex){};
};
