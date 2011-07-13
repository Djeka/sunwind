#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
//#include <boost/lexical_cast.hpp>

using namespace std;

class Utils{
	public: static string doubleToString(double flt){
		char szbuff[255];
		string Y;
		sprintf(szbuff, "%.7g", flt);
		Y = szbuff;
		return Y;
	}
	public: static string getAxisName(int axis){
		if(axis==0) return "x";
		else if(axis==1) return "y";
		else if(axis==2) return "z";
	}
	public: static string getSndAxisNames(int axis){
		if(axis==0) return "yz";
		else if(axis==1) return "xz";
		else if(axis==2) return "xy";
	}
	public: static double sqrtAverage(double* v,int size){
		double value = 0;
		for(int i=0; i<size;i++){
			value+=pow(v[i],2);
		}
		return sqrt(value);
	}
	public: static double sqrSum(vector<double> v){
		double value = 0;
		for(int i=0; i<v.size(); i++){
			value+=pow(v[i],2);
		}
		return value;
	}
	public: static double sqrSum(double* v, int size){
		double value = 0;
		for(int i=0; i<size; i++){
			value+=pow(v[i],2);
		}
		return value;
	}
	public: static void makeDirectories(){
/*		system("mkdir res");
		system("mkdir res/plot");
		makeResDirectoryStruct("N");
		makeResDirectoryStruct("P");
		makeResDirectoryStruct("T");
		makeResDirectoryStruct("Vx");
		makeResDirectoryStruct("Vy");
		makeResDirectoryStruct("Vz");
		makeResDirectoryStruct("V");
		makeResDirectoryStruct("Bx");
		makeResDirectoryStruct("By");
		makeResDirectoryStruct("Bz");
		makeResDirectoryStruct("B");
		makeResDirectoryStruct("Ex");
		makeResDirectoryStruct("Ey");
		makeResDirectoryStruct("Ez");
		makeResDirectoryStruct("E");
		makeResDirectoryStruct("divB");
*/
	}
	public: static void makeResDirectoryStruct(string name){
/*		system(("mkdir res/" + name).c_str());
		system(("mkdir res/" + name + "/xy").c_str());
		system(("mkdir res/" + name + "/xz").c_str());
		system(("mkdir res/" + name + "/yz").c_str());
*/
	}

	public: static string getIndentByDeep(int deep){
		string indent = "";
		for(int i=0; i<deep ; i++) indent += "\t";
		return indent;
	}

	public: static string intToStr(int val)
	{
		string str;
		char buff[32];
		sprintf(buff,"%d",val);
		return string(buff);
	}

	public: static int signum(double value){
		if(value > 0) return 1;
		else if(value < 0) return -1;
		else return 0;
	}

	public: static double minmod(double a, double b){
		return 0.5 * (signum(a) + signum(b)) * min(abs(a), abs(b));
	}
/*
	public: static int versusIndex(int ind){
		if(ind == Cell::ur) return Cell::dl;
		else if(ind == Cell::ul) return = Cell::dr;
		else if(ind == Cell::dr) return = Cell::ul;
		else if(ind == Cell::dl) return = Cell::ur;		
		else return NULL;
	}
*/
};
