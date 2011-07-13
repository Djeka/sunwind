#include <iostream>
#include <fstream>

using namespace std;

class Visualizer1DPlot: public Visualizer{
	private: CellArray* ca;	
	private: TaskData* td;

	private: double xnorm;
	private: double ynorm;

	private: bool m[16][3];
	private: vector<double> time_drops;
	private: string resdir;
	

	public: Visualizer1DPlot(CellArray* ca, TaskData* td, double xnorm, string resdir){
		Visualizer1DPlot::ca = ca;
		Visualizer1DPlot::td = td;
		Visualizer1DPlot::xnorm = xnorm;
		Visualizer1DPlot::ynorm = ynorm;
		Visualizer1DPlot::resdir = resdir;
	}

	public: int visualize(string dataFileName){
		loadVisualizeData(dataFileName);

		dropCurrentData();

		for(int type=0;type<16;type++){
			for(int axis=0;axis<3;axis++){
				if(m[type][axis]){visualizePlot(type, axis);}
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

	public: void visualizePlot(int type, int axis){
		ofstream f((	resdir + "/plot/" + 
				Utils::doubleToString(ca->getTime()) 
				+ "_" 
				+ getName(type) 
				+ "_" 
				+ Utils::getAxisName(axis) 
				+ ".plot").c_str());
		cout << (	resdir + "/plot/" + 
				Utils::doubleToString(ca->getTime()) 
				+ "_" 
				+ getName(type) 
				+ "_" 
				+ Utils::getAxisName(axis)
				+ ".plot") << endl;

		f << " set term gif" << endl;
		f << " set autoscale x" << endl;
		f << " set autoscale y" << endl;
		f << " set output \"" + resdir + "/"<<getName(type)<<"/"<<Utils::getAxisName(axis)<<"/"<<ca->getTime()<<"_"<<getName(type)<<"_"
			<<Utils::getAxisName(axis) <<".gif\"" << endl;
		f << " set title \"t=" << ca->getTime() << " [sec], "<<getName(type)<<" [" << getDimention(type) << "]\"" << endl;
		f << " set xlabel \""<<Utils::getAxisName(axis)<<" [Rz]\"" << endl;

		f << " plot ";
		for(int i=0;i<time_drops.size();i++){
			f << "'"<<(resdir + "/plot/" + Utils::doubleToString(time_drops[i]) + "_" + getName(type) 
				+ "_" + Utils::getAxisName(axis) + ".data")<<"' "<<
				"with linespoints pt " << i+1;
			if(i+1 != time_drops.size()) f << ", ";
		}
		f << endl;

		f.close();
	}

	public: void dropCurrentData(){
		double time = ca->getTime();
		time_drops.push_back(time);

		for(int type=0;type<16;type++){
			for(int axis=0;axis<3;axis++){
				if(m[type][axis]){dropCurrentData(time, type, axis);}
			}
		}
	}

	public: void dropCurrentData(double time, int type, int axis){
		ofstream f((resdir + "/plot/" + Utils::doubleToString(time) + "_" + getName(type) + "_" + Utils::getAxisName(axis) + ".data").c_str());
		for(Iterator* cit = ca->getIterator(); cit->hasNext();){
			Cell* cell = cit->next();
			if(abs(cell->getR((axis+1)%3) - td->getSlice((axis+1)%3)) < cell->getH((axis+1)%3)/2.0
				&& abs(cell->getR((axis+2)%3) - td->getSlice((axis+2)%3)) < cell->getH((axis+2)%3)/2.0
				&& !((HierarchyCell*) cell)->isSplitted()
//				&& cell->getMark() == Cell::INTERNAL_CELL_MARK
			)
			{
				double value = getValue(type, cell);
				f << " " << cell->getR(axis) / xnorm << "\t" << value << endl;
			}
		}
		f.close();
	}
};
