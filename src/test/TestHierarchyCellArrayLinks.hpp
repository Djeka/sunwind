#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>

class TestHierarchyCellArrayLinks: public TestObject{
	private: CellArray* ca;
	private: string testName;

	public: TestHierarchyCellArrayLinks(CellArray* ca){
		TestHierarchyCellArrayLinks::ca = ca;
		testName = "TestHierarchyCellArrayLinks";
	}

	public: int test(){
		return firstTest() + secondTest();
	}

	public: string getTestName(){
		return testName;
	}

	private: int firstTest(){
		int ind = 0;
		for(Iterator* cit = ca->getIterator(); cit->hasNext();){
			HierarchyCell* hcell = (HierarchyCell*) cit->next();

			if(hcell->isSplitted() 
	// 			&& hcell->getPoint(0) == 1 && hcell->getPoint(1) == 1 && hcell->getPoint(2) == 1
			){
				for(int axis=0; axis<3; axis++){
					for(int i=0; i<8; i++){
						HierarchyCell* cell = (HierarchyCell*) hcell->getSubCell(i);

						if(axis == 1){
							if(cell->getRib(axis, Cell::ur) != cell->getSide((axis+2)%3, Cell::FORWARD_SIDE)->getRib(Side::UP_RIB)){
								ind++;
								cout <<"d"<< hcell->getDeep() <<"::\t" << cell->getPath() << endl;
		 						cout <<"\t"<<axis<<" forw up\t";
								cout << cell->getPoint(0) << " " << cell->getPoint(1) << " " << cell->getPoint(2)<< "\t";
								cout << cell->getRib(axis, Cell::ur) << " ";
								cout << cell->getSide((axis+2)%3, Cell::FORWARD_SIDE)->getRib(Side::UP_RIB);
									cout << "\tbad";
								cout << endl;
							}

							if(cell->getRib(axis, Cell::ul) !=cell->getSide((axis+2)%3, Cell::FORWARD_SIDE)->getRib(Side::DOWN_RIB)){
								ind++;
								cout <<"d"<< hcell->getDeep() <<"::\t" << cell->getPath() << endl;
		 						cout <<"\t"<<axis<<" forw down\t";
								cout << cell->getPoint(0) << " " << cell->getPoint(1) << " " << cell->getPoint(2)<< "\t";
								cout << cell->getRib(axis, Cell::ul) << " ";
								cout << cell->getSide((axis+2)%3, Cell::FORWARD_SIDE)->getRib(Side::DOWN_RIB);
									cout << "\tbad";
								cout << endl;
							}

							if(cell->getRib((axis+1)%3, Cell::ur) !=cell->getSide((axis+2)%3, Cell::FORWARD_SIDE)->getRib(Side::RIGHT_RIB)){
								ind++;
								cout <<"d"<< hcell->getDeep() <<"::\t" << cell->getPath() << endl;
		 						cout <<"\t"<<axis<<" forw right\t";
								cout << cell->getPoint(0) << " " << cell->getPoint(1) << " " << cell->getPoint(2)<< "\t";
								cout << cell->getRib((axis+1)%3, Cell::ur) << " ";
								cout << cell->getSide((axis+2)%3, Cell::FORWARD_SIDE)->getRib(Side::RIGHT_RIB);
									cout << "\tbad";
								cout << endl;
							}

							if(cell->getRib((axis+1)%3, Cell::dr) !=cell->getSide((axis+2)%3, Cell::FORWARD_SIDE)->getRib(Side::LEFT_RIB)){
								ind++;
								cout <<"d"<< hcell->getDeep() <<"::\t" << cell->getPath() << endl;
		 						cout <<"\t"<<axis<<" forw left\t";
								cout << cell->getPoint(0) << " " << cell->getPoint(1) << " " << cell->getPoint(2)<< "\t";
								cout << cell->getRib((axis+1)%3, Cell::dr) << " ";
								cout << cell->getSide((axis+2)%3, Cell::FORWARD_SIDE)->getRib(Side::LEFT_RIB);
									cout << "\tbad";
								cout << endl;
							}

							if(cell->getRib(axis, Cell::dr) != cell->getSide((axis+2)%3, Cell::BACKWARD_SIDE)->getRib(Side::UP_RIB)){
								ind++;
								cout <<"d"<< hcell->getDeep() <<"::\t" << cell->getPath() << endl;
		 						cout <<"\t"<<axis<<" back up\t";
								cout << cell->getPoint(0) << " " << cell->getPoint(1) << " " << cell->getPoint(2)<< "\t";
								cout << cell->getRib(axis, Cell::dr) << " ";
								cout << cell->getSide((axis+2)%3, Cell::BACKWARD_SIDE)->getRib(Side::UP_RIB);
									cout << "\tbad";
								cout << endl;
							}

							if(cell->getRib(axis, Cell::dl) !=cell->getSide((axis+2)%3, Cell::BACKWARD_SIDE)->getRib(Side::DOWN_RIB)){
								ind++;
								cout <<"d"<< hcell->getDeep() <<"::\t" << cell->getPath() << endl;
		 						cout <<"\t"<<axis<<" back down\t";
								cout << cell->getPoint(0) << " " << cell->getPoint(1) << " " << cell->getPoint(2)<< "\t";
								cout << cell->getRib(axis, Cell::dl) << " ";
								cout << cell->getSide((axis+2)%3, Cell::BACKWARD_SIDE)->getRib(Side::DOWN_RIB);
									cout << "\tbad";
								cout << endl;
							}

							if(cell->getRib((axis+1)%3, Cell::ul) !=cell->getSide((axis+2)%3, Cell::BACKWARD_SIDE)->getRib(Side::RIGHT_RIB)){
								ind++;
								cout <<"d"<< hcell->getDeep() <<"::\t" << cell->getPath() << endl;
		 						cout <<"\t"<<axis<<" back right\t";
								cout << cell->getPoint(0) << " " << cell->getPoint(1) << " " << cell->getPoint(2)<< "\t";
								cout << cell->getRib((axis+1)%3, Cell::ul) << " ";
								cout << cell->getSide((axis+2)%3, Cell::BACKWARD_SIDE)->getRib(Side::RIGHT_RIB);
									cout << "\tbad";
								cout << endl;
							}

							if(cell->getRib((axis+1)%3, Cell::dl) !=cell->getSide((axis+2)%3, Cell::BACKWARD_SIDE)->getRib(Side::LEFT_RIB)){
								ind++;
								cout <<"d"<< hcell->getDeep() <<"::\t" << cell->getPath() << endl;
		 						cout <<"\t"<<axis<<" back left\t";
								cout << cell->getPoint(0) << " " << cell->getPoint(1) << " " << cell->getPoint(2)<< "\t";
								cout << cell->getRib((axis+1)%3, Cell::dl) << " ";
								cout << cell->getSide((axis+2)%3, Cell::BACKWARD_SIDE)->getRib(Side::LEFT_RIB);
									cout << "\tbad";
								cout << endl;
							}
						}



						if(
							cell->getRib(axis, Cell::ur) == NULL || cell != cell->getRib(axis, Cell::ur)->getCell(Cell::dl)
							||
							cell->getRib(axis, Cell::ul) == NULL || cell != cell->getRib(axis, Cell::ul)->getCell(Cell::dr)
							||
							cell->getRib(axis, Cell::dr) == NULL || cell != cell->getRib(axis, Cell::dr)->getCell(Cell::ul)
							||
							cell->getRib(axis, Cell::dl) == NULL || cell != cell->getRib(axis, Cell::dl)->getCell(Cell::ur)
						){

							if(cell->getRib(axis, Cell::ur) == NULL || cell != cell->getRib(axis, Cell::ur)->getCell(Cell::dl)){
								ind++;
								cout <<"\t"<<axis<<":\t"<< cell->getPath() <<endl;
								cout << "\t\tur\t";
								if(cell->getRib(axis, Cell::ur) != NULL)
									cout << cell->getRib(axis, Cell::ur) << " " << cell->getRib(axis, Cell::ur)->getCell(Cell::dl);
								else
									cout << cell->getRib(axis, Cell::ur) << " " << NULL;
								cout << "\tbad link";
								cout << endl;
							}

							if(cell->getRib(axis, Cell::ul) == NULL || cell != cell->getRib(axis, Cell::ul)->getCell(Cell::dr)){
								ind++;
								cout << "\t\tul\t";
								cout <<"\t"<<axis<<":\t"<< cell->getPath() <<endl;
								if(cell->getRib(axis, Cell::ul) != NULL)
									cout << cell->getRib(axis, Cell::ul) << " " << cell->getRib(axis, Cell::ul)->getCell(Cell::dr);
								else
									cout << cell->getRib(axis, Cell::ur) << " " << NULL;
									cout << "\tbad link";
								cout << endl;
							}

							if(cell->getRib(axis, Cell::dr) == NULL || cell != cell->getRib(axis, Cell::dr)->getCell(Cell::ul)){
								ind++;
								cout << "\t\tdr\t";
								cout <<"\t"<<axis<<":\t"<< cell->getPath() <<endl;
								if(cell->getRib(axis, Cell::dr) != NULL)
									cout << cell->getRib(axis, Cell::dr) << " " << cell->getRib(axis, Cell::dr)->getCell(Cell::ul);
								else
									cout << cell->getRib(axis, Cell::ur) << " " << NULL;
								cout << "\tbad link";
								cout << endl;
							}

							if(cell->getRib(axis, Cell::dl) == NULL || cell != cell->getRib(axis, Cell::dl)->getCell(Cell::ur)){
								ind++;
								cout << "\t\tdl\t";
								cout <<"\t"<<axis<<":\t"<< cell->getPath() <<endl;
								if(cell->getRib(axis, Cell::dl) != NULL)
									cout << cell->getRib(axis, Cell::dl) << " " << cell->getRib(axis, Cell::dl)->getCell(Cell::ur);
								else
									cout << cell->getRib(axis, Cell::ur) << " " << NULL;
								cout << "\tbad link";
								cout << endl;
							}
						}
					}
				}
			}
		}

		return ind;
	}

	private: int secondTest(){
		int ind = 0;
		for(Iterator* cit = ca->getIterator(); cit->hasNext();){
			HierarchyCell* hcell = (HierarchyCell*) cit->next();
			HierarchyCell* hucell = (HierarchyCell*) hcell->getCell(2, Cell::FORWARD_SIDE);
			HierarchyCell* fcell = (HierarchyCell*) hcell->getCell(0, Cell::FORWARD_SIDE);
			HierarchyCell* fucell = NULL; if(fcell != NULL) fucell = (HierarchyCell*) fcell->getCell(2, Cell::FORWARD_SIDE);

			Rib* urRib = NULL;
			Rib* ur = NULL;
			Cell* invHcell = NULL;
			Rib* drRib = NULL;
			Rib* dr = NULL;
			Cell* invFcell = NULL;
			Rib* dlRib = NULL;
			Rib* dl = NULL;
			Cell* invFucell = NULL;
			Rib* ulRib = NULL;
			Rib* ul = NULL;
			Cell* invHucell = NULL;

			int hdeep = -1;
			int fdeep = -1;
			int hudeep = -1;
			int fudeep = -1;

			hdeep = hcell->getDeep();
			if(fcell != NULL) fdeep = fcell->getDeep();
			if(fucell != NULL) fudeep = fucell->getDeep();
			if(hucell != NULL) hudeep = hucell->getDeep();

			if(hdeep == hudeep && hudeep == fdeep && fdeep == fudeep){

				urRib = hcell->getSide(0, Cell::FORWARD_SIDE)->getRib(Side::UP_RIB);
				ur = hcell->getRib(1, Cell::ur);
				invHcell = ur->getCell(Cell::dl);

				if(fcell != NULL) {
					drRib = fcell->getSide(0, Cell::BACKWARD_SIDE)->getRib(Side::UP_RIB);
					dr = fcell->getRib(1, Cell::dr);
					invFcell = dr->getCell(Cell::ul);
				}
				if(fucell != NULL) {
					dlRib = fucell->getSide(0, Cell::BACKWARD_SIDE)->getRib(Side::DOWN_RIB);
					dl = fucell->getRib(1, Cell::dl);
					invFucell = dl->getCell(Cell::ur);
				}
				if(hucell != NULL) {
					ulRib = hucell->getSide(0, Cell::FORWARD_SIDE)->getRib(Side::DOWN_RIB);
					ul = hucell->getRib(1, Cell::ul);
					invHucell = ul->getCell(Cell::dr);
				}

				if( 
					(hucell != NULL && fcell != NULL && fucell != NULL 
						&& !(urRib == drRib && drRib == dlRib && dlRib == ulRib && ulRib == ur && ur == ul && ul == dl && dl == dr))
					|| (hucell != NULL && fcell == NULL && fucell == NULL && !(urRib == ulRib && ulRib == ur && ur == ul))
					|| (hucell == NULL && fcell != NULL && fucell == NULL && !(urRib == drRib && drRib == ur && ur == dr))
					|| (hucell == NULL && fcell != NULL && fucell != NULL && !(urRib == drRib && drRib == dlRib && dlRib == ur && ur == dl && dl == dr))
					|| (hucell == NULL && fcell == NULL && fucell == NULL && !(urRib == ur))
				){
					ind++;
					cout << hcell->getPath()  << "\t";

					cout << "dl: d" << hcell->getDeep() << " " << urRib << "\t";
					if(fcell  != NULL) cout << "ul: d" << fcell->getDeep()  << " " << drRib << "\t";
					if(hucell != NULL) cout << "dr: d" << hucell->getDeep() << " " << ulRib << "\t";
					if(fucell != NULL) cout << "ur: d" << fucell->getDeep() << " " << dlRib << "\t";
					cout << endl;

					cout << "\t\t\t\t\t\t   " << ur << "\t   ";
					if(fcell  != NULL) cout << dr << "\t   ";
					if(hucell != NULL) cout << ul << "\t   ";
					if(fucell != NULL) cout << dl;
					cout << "\tbad :(" << endl;
				}

				if((hcell != NULL && hcell != invHcell) || (fcell != NULL && fcell != invFcell) 
						|| (hucell != NULL && hucell != invHucell) || (fucell != NULL && fucell != invFucell)){
					ind++;
					cout << "---------------" << endl;
					if(hucell != NULL) cout << hucell->getPath() << endl;
					cout << hcell->getPath() << endl;
					cout << hucell->getRib(1, Cell::ul)->getCell(Cell::dr) << endl;
					cout << ul->getCell(Cell::dr) << endl;
					cout << invHucell << endl;


					cout << "dr: " << hucell << " <-> " << invHucell << "\t||\t" << "ur: " << fucell << " <-> " << invFucell << endl;
					cout << "dl: " << hcell << " <-> " << invHcell << "\t||\t" << "ul: " << fcell << " <-> " << invFcell << endl;
					cout << "---------------" << endl;
				}
			}
		}

		return ind;
	}
};
