class InitialParametersFunction{
	public: virtual double getDensity(Cell* cell){};
	public: virtual double getDensity(double* r){};
	public: virtual double getMomentum(int axis, Cell* cell){};
	public: virtual double getMomentum(int axis, double* r){};
	public: virtual double getPressure(Cell* cell){};
	public: virtual double getPressure(double* r){};
	public: virtual double getB(Side* side){};
	public: virtual double getBackGroundField(Side* side){};
	public: virtual double getMaxSpeed(Cell* cell){};
	public: virtual double getMaxSpeed(double* r){};
	public: virtual double getMaxSpeedOnSide(Side* side){};
};
