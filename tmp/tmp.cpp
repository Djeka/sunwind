for(HierarchyCellIterator cit = HierarchyCellIterator(this); cit.hasNext();){
	HierarchyCell* cell = (HierarchyCell*) cit.next()

	int axis_X = 0;
	double fside = cell->getSide(axis_X, Cell::FORWARD_SIDE);
	double fRoter = (fside->getRib((axis_X+2)%3, Cell::FORWARD_SIDE)->getE() 
				- fside->getRib((axis_X+2)%3, Cell::BACKWARD_SIDE)->getE())
					/ cell->getH((axis_X+1)%3);
	fRoter += - (fside->getRib((axis_X+1)%3, Cell::FORWARD_SIDE)->getE() 
				- fside->getRib((axis_X+1)%3, Cell::BACKWARD_SIDE)->getE())
					/ cell->getH((axis_X+2)%3);

	double bside = cell->getSide(axis_X, Cell::FORWARD_SIDE);
	double bRoter = (fside->getRib((axis_X+2)%3, Cell::FORWARD_SIDE)->getE() 
				- fside->getRib((axis_X+2)%3, Cell::BACKWARD_SIDE)->getE())
					/ cell->getH((axis_X+1)%3);
	bRoter += - (fside->getRib((axis_X+1)%3, Cell::FORWARD_SIDE)->getE() 
				- fside->getRib((axis_X+1)%3, Cell::BACKWARD_SIDE)->getE())
					/ cell->getH((axis_X+2)%3);

	double roter = 0.5 * (fRoter + bRoter);
}

