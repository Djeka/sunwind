#include "iostream"
#include "fstream"
#include <sstream>
#include <cstdlib>

class TaskData{
	private: int n[3];
	private: double h[3];
	private: double Ll[3];
	private: double L[3];
	private: double slice[3];
	private: double splittingDeepMeasure;
	private: double dipole;

	public: TaskData(string name, double rnorm){
		ifstream f(name.c_str());
		int i = 0;
		while(!f.eof()){
			string str;
			getline(f, str);
			string svalue = str.substr(0,str.find("\t"));
			double fvalue;
			int ivalue;

			if(i>=0 and i<=2) {
				sscanf(svalue.c_str(), "%d", &ivalue);
				n[i] = ivalue;
			} else if(i>=3 and i<=5){
				std::istringstream stm;
				stm.str(svalue);
				stm >>fvalue;
//				sscanf(svalue.c_str(), "%fI", &fvalue);
//				fvalue = atof(svalue);
				Ll[i-3] = fvalue*rnorm;
			} else if(i>=6 and i<=8){
//				sscanf(svalue.c_str(), "%f", &fvalue);
				std::istringstream stm;
				stm.str(svalue);
				stm >>fvalue;
				L[i-6] = fvalue*rnorm;
			} else if(i>=9 and i<=11){
				std::istringstream stm;
				stm.str(svalue);
				stm >>fvalue;
				slice[i-9] = fvalue*rnorm;
			} else if(i==12){
				std::istringstream stm;
				stm.str(svalue);
				stm >>fvalue;
				splittingDeepMeasure = fvalue;
			} else if(i==13){
				std::istringstream stm;
				stm.str(svalue);
				stm >>fvalue;
				dipole = fvalue;
			}
			i++;
		}
		f.close();

		for(int i=0; i<3; i++){
			h[i] = L[i] / n[i];
		}
	}

	public: int getN(int ind){
		return n[ind];
	}
	public: double getSlice(int ind){
		return slice[ind];
	}
	public: double getH(int ind){
		return h[ind];
	}
	public: double getL(int ind){
		return L[ind];
	}
	public: double getLl(int ind){
		return Ll[ind];
	}
	public: double getSplittingDeepMeasure(){
		return splittingDeepMeasure;
	}
	public: double getEarthMagneticDipole(){
		return dipole;
	}
};
