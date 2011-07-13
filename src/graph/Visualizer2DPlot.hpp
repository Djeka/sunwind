#include <iostream>
#include <fstream>

using namespace std;

class Visualizer2DPlot: public Visualizer{
	private: CellArray* ca;	
	private: TaskData* td;

	private: double xnorm;
	private: double ynorm;

	private: bool m[16][3];

	public: Visualizer2DPlot(CellArray* ca, TaskData* td, double xnorm, double ynorm){
		Visualizer2DPlot::ca = ca;
		Visualizer2DPlot::td = td;
		Visualizer2DPlot::xnorm = xnorm;
		Visualizer2DPlot::ynorm = ynorm;
	}

	public: int visualize(string dataFileName, string resdir){
		loadVisualizeData(dataFileName);

		for(int type=0;type<16;type++){
			for(int axis=0;axis<3;axis++){
				if(m[type][axis]){visualizePlot(type, axis, resdir);}
			}
		}

		return 1;
	}

	private: void loadVisualizeData(string dataFileName){
		ifstream f(dataFileName.c_str());
		while(!f.eof()){
			string str;
			getline(f, str);			

			string name = str.substr(0,str.find("\t"));
			string yz = str.substr(str.find("\t",0)+1, 1);
			string xz = str.substr( str.find("\t", str.find("\t",0)+1)+1, 1);
			string xy = str.substr( str.find("\t", str.find("\t", str.find("\t",0)+1)+1)+1, 1);

			int type = getTypeByName(name);

			if(type>=0){
				if(yz.compare("Y")==0) m[type][0] = true;
				else m[type][0] = false;
				if(xz.compare("Y")==0) m[type][1] = true;
				else m[type][1] = false;
				if(xy.compare("Y")==0) m[type][2] = true;
				else m[type][2] = false;
			}
		}
		f.close();
	}

	public: void visualizePlot(int type, int axis, string resdir){
		ofstream f((	resdir + "/plot/" + 
				Utils::doubleToString(ca->getTime()) 
				+ "_" 
				+ getName(type) 
				+ "_" 
				+ Utils::getSndAxisNames(axis) 
				+ ".plot").c_str());
		cout << (	"\t" + 
				resdir + "/plot/" + 
				Utils::doubleToString(ca->getTime()) 
				+ "_" 
				+ getName(type) 
				+ "_" 
				+ Utils::getSndAxisNames(axis) 
				+ ".plot") << endl;

		f << " load \"gnuplot/head.gnuplot\"" << endl;
		f << " set output \"" <<resdir<<"/"<<getName(type)<<"/"<<Utils::getSndAxisNames(axis)<<"/"<<ca->getTime()<<"_"<<getName(type)<<"_"
			<<Utils::getSndAxisNames(axis) <<".png\"" << endl;
		f << " set title \"t=" << ca->getTime() << "[sec] ,"<<getName(type)<<"[" << getDimention(type) << "]\"" << endl;
		f << " set xlabel \""<<Utils::getAxisName((1+axis)%3)<<"[Rz]\"" << endl;
		f << " set ylabel \""<<Utils::getAxisName((2+axis)%3)<<"[Rz]\"" << endl;
		f << " splot \\" << endl;

		for(Iterator* cit = ca->getIterator(); cit->hasNext();){
			Cell* cell = cit->next();
			if( (
				(abs(cell->getR(axis) - td->getSlice(axis)) < cell->getH(axis)/2.0) ||
				(abs(cell->getR(axis) - td->getSlice(axis)) == cell->getH(axis)/2.0 && (cell->getR(axis) - td->getSlice(axis)) > 0 )
			    )
				&& !((HierarchyCell*) cell)->isSplitted()
				&& cell->getMark() == Cell::INTERNAL_CELL_MARK
			  )
				f << "\"-\" notitle ,\\" << endl;
		}
		f << "\"-\" notitle" << endl;

		int color = 0;
		for(Iterator* cit = ca->getIterator(); cit->hasNext();){
			Cell* cell = cit->next();

			if( (
				(abs(cell->getR(axis) - td->getSlice(axis)) < cell->getH(axis)/2.0) ||
				(abs(cell->getR(axis) - td->getSlice(axis)) == cell->getH(axis)/2.0 && (cell->getR(axis) - td->getSlice(axis)) > 0 )
			    )
				&& !((HierarchyCell*) cell)->isSplitted()
				&& cell->getMark() == Cell::INTERNAL_CELL_MARK)
			{
				double value = getValue(type, cell);
//				double value = color;
//				double value = cell->getMark();
//if(value != value) cout << "\t\t\t" << ((HierarchyCell*) cell)->getPath() << "\t" << cell->getDensity() << endl;

				if(type >= 7 && type<=9 && axis != (type-7)){
					double faxis = type - 7;
					double ratio = 2.0;
					double* r = cell->getR();
					double h1;
					double h2;

					h1 = cell->getH(faxis);
					if(faxis == (axis+1)%3) h2 = cell->getH((axis+2)%3)/ratio;
					if(faxis == (axis+2)%3) h2 = cell->getH((axis+1)%3)/ratio;

					value = cell->getSide(faxis, Cell::FORWARD_SIDE)->getB();
					f << " " << r[(axis+1)%3] / xnorm << "\t" << (r[(axis+2)%3] - h2) / ynorm << "\t" << value << endl;
					f << " " << r[(axis+1)%3] / xnorm << "\t" << (r[(axis+2)%3] + h2) / ynorm << "\t" << value << endl << endl;
					f << " " << (r[(axis+1)%3] + h1) / xnorm << "\t" << (r[(axis+2)%3] - h2) / ynorm << "\t" << value << endl;
					f << " " << (r[(axis+1)%3] + h1) / xnorm << "\t" << (r[(axis+2)%3] + h2) / ynorm << "\t" << value << endl << endl;

					value = cell->getSide(faxis, Cell::BACKWARD_SIDE)->getB();
					f << " " << (r[(axis+1)%3] - h1)/ xnorm << "\t" << (r[(axis+2)%3] - h2) / ynorm << "\t" << value << endl;
					f << " " << (r[(axis+1)%3] - h1)/ xnorm << "\t" << (r[(axis+2)%3] + h2) / ynorm << "\t" << value << endl << endl;
					f << " " << r[(axis+1)%3] / xnorm << "\t" << (r[(axis+2)%3] - h2) / ynorm << "\t" << value << endl;
					f << " " << r[(axis+1)%3] / xnorm << "\t" << (r[(axis+2)%3] + h2) / ynorm << "\t" << value << endl << endl;
				} else {

					f << " " << (cell->getR((axis+1)%3) - cell->getH((axis+1)%3)/2) / xnorm << "\t" << 
						(cell->getR((axis+2)%3) - cell->getH((axis+2)%3)/2) / ynorm << "\t" <<
						value << endl;

					f << " " << (cell->getR((axis+1)%3) - cell->getH((axis+1)%3)/2) / xnorm << "\t" << 
						(cell->getR((axis+2)%3) + cell->getH((axis+2)%3)/2) / ynorm << "\t" <<
						value << endl << endl;

					f << " " << (cell->getR((axis+1)%3) + cell->getH((axis+1)%3)/2) / xnorm << "\t" << 
						(cell->getR((axis+2)%3) - cell->getH((axis+2)%3)/2) / ynorm << "\t" <<
						value << endl;

					f << " " << (cell->getR((axis+1)%3) + cell->getH((axis+1)%3)/2) / xnorm << "\t" << 
						(cell->getR((axis+2)%3) + cell->getH((axis+2)%3)/2) / ynorm << "\t" <<
						value << endl << endl;
				}

				f << " e" << endl;

				if(color == 20) color = 0;
				else color++;
			}
		}
		f.close();
	}

	private: double getValue(int type,Cell* cell){
		if(type == 0) return cell->getDensity() / Constants::mp;;
		if(type == 1) return cell->getP();
		if(type == 2) return cell->getT();
		if(type == 3) return cell->getV(0)*1.0e-5;
		if(type == 4) return cell->getV(1)*1.0e-5;
		if(type == 5) return cell->getV(2)*1.0e-5;
		if(type == 6) return cell->getV()*1.0e-5;
		if(type >= 7 and type<=9) {
//			return ( cell->getSide(type-7, Cell::FORWARD_SIDE)->getB() + cell->getSide(type-7, Cell::BACKWARD_SIDE)->getB() ) / 2.0;
			return cell->getSide(type-7, Cell::FORWARD_SIDE)->getB();
		}
		if(type == 10) {
			double B = 0;
			for(int axis=0;axis<3;axis++){
				B += pow(cell->getSide(axis, Cell::FORWARD_SIDE)->getB() + cell->getSide(axis, Cell::BACKWARD_SIDE)->getB(), 2) / 4.0;
			}
			return sqrt(B);
		}
		if(type >= 11 and type<=13) {
			double E = 0;
			for(int i=0;i<4;i++){
				E+=cell->getRib(type-11,i)->getE()/4.0;
			}
			return E;
		}
		if(type == 14) {
			double E = 0;
			for(int axis=0;axis<3;axis++){
				for(int i=0;i<4;i++){
					E+=cell->getRib(axis,i)->getE()/4.0/3.0;
				}
			}
			return E;
		}
		if(type == 15) {
			double divB = 0;
			for(int axis=0;axis<3;axis++){
				if(cell->getCell(axis, Cell::FORWARD_SIDE) == NULL || cell->getCell(axis, Cell::BACKWARD_SIDE) == NULL) {
					divB = 0;
					break;
				}
				divB+= (cell->getSide(axis, Cell::FORWARD_SIDE)->getB()-cell->getSide(axis, Cell::BACKWARD_SIDE)->getB()) /cell->getH(axis);
			}
			return divB;
		}
		if(type == 16) {
			return cell->getFullEnergy();
		}
	}
};	
