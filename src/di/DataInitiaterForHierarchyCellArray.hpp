class DataInitiaterForHierarchyCellArray: public DataInitiater{
	private: CellArray* ca;
	private: TaskData* td;
	private: InitialParametersFunction* initParams;

	public: DataInitiaterForHierarchyCellArray(CellArray* ca, TaskData* td, InitialParametersFunction* initParams){
		DataInitiaterForHierarchyCellArray::ca = ca;
		DataInitiaterForHierarchyCellArray::td = td;
		DataInitiaterForHierarchyCellArray::initParams = initParams;
	}

	public: void initiate(){
		for(HierarchyCellIterator cit = HierarchyCellIterator(ca); cit.hasNext();){
			Cell* cell = cit.next();

			for(int axis = 0; axis<3; axis++){
				for(int dir=0; dir<2; dir++){
					cell->getSide(axis, dir)->setB(initParams->getB(cell->getSide(axis, dir)));
					cell->getSide(axis, dir)->setBbg(initParams->getBackGroundField(cell->getSide(axis, dir)));
				}
				cell->setMomentum(initParams->getMomentum(axis, cell), axis);
			}

			cell->setDensity(initParams->getDensity(cell));
			cell->setP(initParams->getPressure(cell));
		}
	}
};
