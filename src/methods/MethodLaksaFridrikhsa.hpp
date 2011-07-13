#include <iostream>
using namespace std;

class MethodLaksaFridrikhsa: public Method{
	public: MethodLaksaFridrikhsa(){}

	public: vector<double> calculateFlow(double Ul[], double Ur[]){
		return calculateFlow(Ul, Ur, -1);
	}

	public: vector<double> calculateFlow(double Ul[], double Ur[], double maxSpeed){
		vector<double> F(8, 0);
		vector<double> Fl = getF(Ul);
		vector<double> Fr = getF(Ur);

		double maxs;
		if(maxSpeed >= 0) maxs = maxSpeed;
		else maxs = getMaxSpeed(Ul, Ur);

		for(int i=0; i<8; i++){
			F[i] = 0.5*(Fl[i]+Fr[i]) + 0.5 * maxs * (Ul[i] - Ur[i]);
		}
		return F;
	}

	private: vector<double> getF(double U[]){
		vector<double> F(8, 0);

		double rho = U[0];
		double u = U[1] / rho;
		double v = U[2] / rho;
		double w = U[3] / rho;
		double Bx = U[5];
		double By = U[6];
		double Bz = U[7];
		double e = U[4];
		double Ek = rho*(pow(u,2) + pow(v,2) + pow(w,2))/2.0;
		double Em = (pow(Bx,2) + pow(By,2) + pow(Bz,2))/8.0/Constants::Pi;
		double P = (Constants::gamma - 1.0)*(e - Ek - Em);

		F[0] = rho*u;
		F[1] = rho*pow(u,2) + P + Em - pow(Bx,2)/4.0/Constants::Pi;
		F[2] = rho*u*v - Bx*By/4.0/Constants::Pi;
		F[3] = rho*u*w - Bx*Bz/4.0/Constants::Pi;
		F[4] = (e+P+Em)*u - Bx/4.0/Constants::Pi*(u*Bx+v*By+w*Bz);
		F[5] = 0;
		F[6] = u*By - v*Bx;
		F[7] = u*Bz - w*Bx;

		return F;
	}

	public: double getMaxSpeed(double Ul[], double Ur[]){
		double sl = getMaxSpeed(Ul);
		double sr = getMaxSpeed(Ur);
		double maxs = max(sl, sr);

		return maxs;
	}

	private: double getMaxSpeed(double U[]){
		double maxs = 0;
		double rho = U[0];
		double Ek = (pow(U[1],2) + pow(U[2],2) + pow(U[3],2)) / 2.0 / rho;
		double B2 = pow(U[5],2) + pow(U[6],2) + pow(U[7],2);
		double Em = B2 / 8.0 / Constants::Pi;
//		double ce =  sqrt(Constants::gamma*getP(U,Ek,Em) / rho);
		double ce2 =  Constants::gamma * getP(U,Ek,Em) / rho;
		double tmp2 = B2 / (4*Constants::Pi*rho);

////		for(int axis=0;axis<3;axis++){
		int axis = 0;
			double u = U[1+axis]/rho;
			double u2 = pow(u,2);
			double Bn = U[5+axis];

//			double aa = abs(Bn) / sqrt(4*Constants::Pi*rho);
//			double a1 = sqrt( pow(ce,2) + 2.0*Em/rho + 2*aa*ce);
//			double a2 = sqrt( pow(ce,2) + 2.0*Em/rho - 2*aa*ce);
//			double af = 0.5*(a1+a2);
//			double cur_maxs = max(max(abs(u+aa),abs(u-aa)), max(abs(u+af),abs(u-af)));

			double aA2 = pow(Bn,2) / (4*Constants::Pi*rho);
			double a1 = 0.5*(ce2 + tmp2);
			double a2 = sqrt(pow(a1,2) - aA2*ce2);
			double af2 = a1+a2;

//			double cur_maxs =  max(abs(u) + sqrt(af2), abs(u) + sqrt(aA2));
			maxs =  abs(u) + sqrt(af2);
////			if(maxs <cur_maxs) maxs = cur_maxs;
////		}
		return maxs;
	}

	private: double getP(double* U, double Ek, double Em){
		return  (Constants::gamma - 1.0) * (U[4] - Ek - Em);
	}
};
