class DataInitiaterTestRaspadRazriva: public DataInitiater{
	private: CellArray* ca;
	private: TaskData* td;

	public: DataInitiaterTestRaspadRazriva(CellArray* ca, TaskData* td){
		DataInitiaterTestRaspadRazriva::ca = ca;
		DataInitiaterTestRaspadRazriva::td = td;
	}

	public: virtual void initiate(){
		for(int i=0; i<ca->getSize();i++){
			Cell* cell = ca->getCell(i);
			double r = Utils::sqrtAverage(cell->getR(),3);

//			double dens = Constants::mp * Constants::No * exp(8.765 * ((Constants::Re + 200e+5)/r - 1.0) );
//			cell->setDensity(dens);
//			cell->setFullEnergy(dens*Constants::Cv*600);

			double Ul[8] = {0.18405, 0.3541, 3.8964, 0.5361, 2.4866, 4.0, 2.394, 1.197}; 	// {rho, P, u, v, w, Bx, By, Bz} on the Left
			double Ur[8] = {0.1, 0.1, -5.5, 0, 0, 4, 2, 1}; 					// {rho, P, u, v, w, Bx, By, Bz} on the Right

			int axis = 0;

			if(cell->getR(axis) <= 0){
				cell->setDensity(Ul[0]);
				cell->setMomentum(Ul[0]*Ul[2], axis);
				cell->setMomentum(Ul[0]*Ul[3], (axis+1)%3);
				cell->setMomentum(Ul[0]*Ul[4], (axis+2)%3);
				cell->getSide(axis, Cell::FORWARD_SIDE)->setB(Ul[5]);
				cell->getSide(axis, Cell::BACKWARD_SIDE)->setB(Ul[5]);
				cell->getSide((axis+1)%3, Cell::FORWARD_SIDE)->setB(Ul[6]);
				cell->getSide((axis+1)%3, Cell::BACKWARD_SIDE)->setB(Ul[6]);
				cell->getSide((axis+2)%3, Cell::FORWARD_SIDE)->setB(Ul[7]);
				cell->getSide((axis+2)%3, Cell::BACKWARD_SIDE)->setB(Ul[7]);
				cell->setP(Ul[1]);
			} else{
				cell->setDensity(Ur[0]);
				cell->setMomentum(Ur[0]*Ur[2], axis);
				cell->setMomentum(Ur[0]*Ur[3], (axis+1)%3);
				cell->setMomentum(Ur[0]*Ur[4], (axis+2)%3);
				cell->getSide(axis, Cell::FORWARD_SIDE)->setB(Ur[5]);
				cell->getSide(axis, Cell::BACKWARD_SIDE)->setB(Ur[5]);
				cell->getSide((axis+1)%3, Cell::FORWARD_SIDE)->setB(Ur[6]);
				cell->getSide((axis+1)%3, Cell::BACKWARD_SIDE)->setB(Ur[6]);
				cell->getSide((axis+2)%3, Cell::FORWARD_SIDE)->setB(Ur[7]);
				cell->getSide((axis+2)%3, Cell::BACKWARD_SIDE)->setB(Ur[7]);
				cell->setP(Ur[1]);
			}
		}
	}
};
