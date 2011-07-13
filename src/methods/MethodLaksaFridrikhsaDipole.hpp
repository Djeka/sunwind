#include <iostream>
using namespace std;

class MethodLaksaFridrikhsaDipole: public Method{
	public: MethodLaksaFridrikhsaDipole(){}

	public: vector<double> calculateFlow(double Ul[], double Ur[], double Bdl[], double Bdr[]){
		return calculateFlow(Ul, Ur, Bdl, Bdr, -1);
	}

	public: vector<double> calculateFlow(double Ul[], double Ur[], double Bdl[], double Bdr[], double maxSpeed){
		vector<double> F(8, 0);
		vector<double> Fl = getF(Ul, Bdl);
		vector<double> Fr = getF(Ur, Bdr);

		double maxs;
		if(maxSpeed >= 0) maxs = maxSpeed;
		else maxs = getMaxSpeed(Ul, Ur, Bdl, Bdr);

		for(int i=0; i<8; i++){
			F[i] = 0.5*(Fl[i]+Fr[i]) + 0.5 * maxs * (Ul[i] - Ur[i]);
		}
		return F;
	}

	private: vector<double> getF(double U[], double Bd[]){
		vector<double> F(8, 0);
		vector<double> B(3,0);
		vector<double> V(3,0);

		for(int i=0; i<3; i++){
			B[i] = U[5+i];
			V[i] = U[1+i] / U[0];
		}

		double BBd = B[0]*Bd[0] + B[1]*Bd[1] + B[2]*Bd[2];
		double BV =  B[0]*V[0] + B[1]*V[1] + B[2]*V[2];

		for(int i=0; i<3; i++){
			if(i == 0){
				F[1+i] = F[1+i] + BBd / 4.0 / Constants::Pi;
			}
			F[1+i] = F[1+i] - (B[0]*Bd[i] + B[i]*Bd[0]) / 4.0 / Constants::Pi;

			F[5+i] = V[0]*Bd[i] - V[i]*Bd[0];
		}
		F[4] =  (BBd * V[0] - Bd[0] * BV) / 4.0 / Constants::Pi;

		return F;
	}

	public: double getMaxSpeed(double Ul[], double Ur[], double Bdl[], double Bdr[]){
		double sl = getMaxSpeed(Ul, Bdl);
		double sr = getMaxSpeed(Ur, Bdr);
		double maxs = max(sl, sr);

		return maxs;
	}

	private: double getMaxSpeed(double U[], double Bd[]){
		double Bd2 = pow(Bd[0],2) + pow(Bd[1],2) + pow(Bd[2],2);
		return sqrt(Bd2 / (4.0 * Constants::Pi * U[0]) );
	}
};
