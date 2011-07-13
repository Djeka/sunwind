class BorderConditionEarth: public BorderCondition {
	private: TaskData* td;
	private: static const int EARTH_BORDER = 101;
	private: double R;

	public: BorderConditionEarth(TaskData* td){
		BorderConditionEarth::td = td;
		R = Constants::Re * 3;
	}

	public: virtual int getMark(){
		return EARTH_BORDER;
	};

	public: void markCell(Cell* cell){
		if(!isBorder(cell)) return;

		cell->setMark(getMark());
		for(int axis=0; axis<3; axis++){
			for(int dir=0; dir<2; dir++){
				cell->getSide(axis, dir)->setMark(getMark());
			}

			for(int irib=0; irib<4; irib++){
				Rib* rib = cell->getRib(axis, irib);
				if(
				   (rib->getSide(Rib::RIGHT_SIDE) == NULL || rib->getSide(Rib::RIGHT_SIDE)->getMark() == getMark()) &&
				   (rib->getSide(Rib::LEFT_SIDE) == NULL || rib->getSide(Rib::LEFT_SIDE)->getMark() == getMark()) &&
				   (rib->getSide(Rib::UP_SIDE) == NULL || rib->getSide(Rib::UP_SIDE)->getMark() == getMark()) &&
				   (rib->getSide(Rib::DOWN_SIDE) == NULL || rib->getSide(Rib::DOWN_SIDE)->getMark() == getMark())
				  )
					rib->setMark(getMark());
			}
		}
	}

	public: bool isBorder(Cell* cell){
		int ind = 0;
		double r[3];

		if( ((HierarchyCell*) cell)->isSplitted()) return false;

		for(int axis=0; axis<3; axis++){
			for(int dir=0; dir<2; dir++){
				r[axis] = cell->getR(axis) + (2*dir-1) * cell->getH(axis) / 2.0;

				r[(axis+1)%3] = cell->getR((axis+1)%3) + cell->getH((axis+1)%3) / 2.0;
				r[(axis+2)%3] = cell->getR((axis+2)%3) + cell->getH((axis+2)%3) / 2.0;
				if(pow(r[0]*r[0] + r[1]*r[1] + r[2]*r[2], 0.5)  <= R) ind++;

				r[(axis+1)%3] = cell->getR((axis+1)%3) - cell->getH((axis+1)%3) / 2.0;
				r[(axis+2)%3] = cell->getR((axis+2)%3) + cell->getH((axis+2)%3) / 2.0;
				if(pow(r[0]*r[0] + r[1]*r[1] + r[2]*r[2], 0.5)  <= R) ind++;

				r[(axis+1)%3] = cell->getR((axis+1)%3) + cell->getH((axis+1)%3) / 2.0;
				r[(axis+2)%3] = cell->getR((axis+2)%3) - cell->getH((axis+2)%3) / 2.0;
				if(pow(r[0]*r[0] + r[1]*r[1] + r[2]*r[2], 0.5)  <= R) ind++;

				r[(axis+1)%3] = cell->getR((axis+1)%3) - cell->getH((axis+1)%3) / 2.0;
				r[(axis+2)%3] = cell->getR((axis+2)%3) - cell->getH((axis+2)%3) / 2.0;
				if(pow(r[0]*r[0] + r[1]*r[1] + r[2]*r[2], 0.5)  <= R) ind++;
			}
		}
		if(ind > 0) return true;

		return false;
	};
	public: int getBorderMark(Cell* cell){
		if(!isBorder(cell)) return NULL;
		return getMark();
	};

	public: double getBorderFlowOnSide(int axis, Rib* rib, int sideIndex){
		int vIndex = 0;
		if(sideIndex == Rib::LEFT_SIDE) vIndex = Rib::RIGHT_SIDE;
		else if(sideIndex == Rib::RIGHT_SIDE) vIndex = Rib::LEFT_SIDE;
		else if(sideIndex == Rib::UP_SIDE) vIndex = Rib::DOWN_SIDE;
		else if(sideIndex == Rib::DOWN_SIDE) vIndex = Rib::UP_SIDE;

		Side* vside = rib->getSide(vIndex);

		if(sideIndex == Rib::RIGHT_SIDE) return vside->getFlow(5+(axis+1)%3);
		else if(sideIndex == Rib::LEFT_SIDE) return vside->getFlow(5+(axis+1)%3);
		else if(sideIndex == Rib::UP_SIDE) return vside->getFlow(5+(axis+2)%3);
		else if(sideIndex == Rib::DOWN_SIDE) return vside->getFlow(5+(axis+2)%3);

		return NULL;
	};

	public: double getBorderU(int axis, int dir, Cell* cell, double* inU, int ind){
		double U = inU[ind];
		if(ind == 1+axis) U = - inU[ind];

		return U;
	};
};
