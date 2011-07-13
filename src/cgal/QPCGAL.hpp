#include <iostream>
#include <cassert>
#include <CGAL/basic.h>
#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>

// choose exact integral type
#ifdef CGAL_USE_GMP
#include <CGAL/Gmpzf.h>
typedef CGAL::Gmpzf ET;
#else
#include <CGAL/MP_Float.h>
typedef CGAL::MP_Float ET;
#endif

// program and solution types
typedef CGAL::Quadratic_program<double> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;


class QPCGAL{
	public: static double** calculateQP(double** outB){
		double** inB = new double*[3];
		for(int i=0; i<3; i++) inB[i] = new double[4];

		// by default, we have a nonnegative QP with Ax <= b
		Program qp (CGAL::SMALLER, false, 0, false, 0); 

		const int x1 = 0; 
		const int x2 = 1; 
		const int x3 = 2; 
		const int x4 = 3; 
		const int x5 = 4; 

		double Bxbff = outB[0][0];
		double Bxbfb = outB[0][1];
		double Bxbbf = outB[0][2];
		double Bxbbb = outB[0][3];
		double Bxfff = outB[0][4];
		double Bxffb = outB[0][5];
		double Bxfbf = outB[0][6];
		double Bxfbb = outB[0][7];

		double Bybff = outB[1][0];
		double Bybfb = outB[1][1];
		double Bybbf = outB[1][2];
		double Bybbb = outB[1][3];
		double Byfff = outB[1][4];
		double Byffb = outB[1][5];
		double Byfbf = outB[1][6];
		double Byfbb = outB[1][7];

		double Bzbff = outB[2][0];
		double Bzbfb = outB[2][1];
		double Bzbbf = outB[2][2];
		double Bzbbb = outB[2][3];
		double Bzfff = outB[2][4];
		double Bzffb = outB[2][5];
		double Bzfbf = outB[2][6];
		double Bzfbb = outB[2][7];

		double C1 = -Bzfbf-Bzfbb-Byffb+Bybfb+Bxfff+Bxbff+Bxbbf;
		double C2 = -Bzfbf-Bzfbb-Byffb+Bybfb+2*Bxbff+Bxbbf;
		double C3 = +Bzbbf+Bzbbb-Byfbb+Bybbb+2*Bxbfb+Bxbbb;
		double C4 = +2*Bzfff+Bzffb+Bzfbf+Bzfbb+Byfff+Byffb-Bybff-Bybfb+Bxfff+Bxfbf-Bxbff-Bxbbf;
		double C5 = +Bzbfb+2*Bybbf-Bxfbb;
		double C6 = Bzbbb+Byfbb+Bybbb+Bxbbb;
		double C7 = Bzbbb+2*Bybbb+Bxbbb;
		double C8 = -Bzffb+Byfff+Bybff-Bxfbf;
		double C9 = -Bzffb+2*Bybff-Bxfbf;
		double C10 = -Bzfbb+Byffb+Bybfb+Bxbbf;
		double C11 = -Bzfbb+2*Bybfb+Bxbbf;


		qp.set_a(x1, 0,  -1); qp.set_a(x2, 0, 1); qp.set_a(x5, 0, 1); qp.set_r(0, CGAL::SMALLER); 
			qp.set_b(0, std::max(Bxfff, Bxbff) - ( - Bzfbf - Bzfbb - Byffb + Bybfb + Bxbff + Bxbbf)); 
		qp.set_a(x1, 1,  -1); qp.set_a(x2, 1, 1); qp.set_a(x5, 1, 1); qp.set_r(1, CGAL::LARGER);
			qp.set_b(1, -(- Bzfbf - Bzfbb - Byffb + Bybfb + Bxbff + Bxbbf) + std::min(Bxfff, Bxbff)); 

		qp.set_a(x2, 2,  -1); qp.set_a(x4, 2, -1); qp.set_a(x5, 2, -1); qp.set_r(2, CGAL::SMALLER); 
			qp.set_b(2, std::max(Bxffb, Bxbfb) - ( Bzbbf + Bzbbb - Byfbb + Bybbb + Bxbfb + Bxbbb)); 
		qp.set_a(x2, 3,  -1); qp.set_a(x4, 3, -1); qp.set_a(x5, 3, -1); qp.set_r(3, CGAL::LARGER); 
			qp.set_b(3, -(+ Bzbbf + Bzbbb - Byfbb + Bybbb + Bxbfb + Bxbbb) + std::min(Bxffb, Bxbfb)); 

		qp.set_a(x1, 4,  1); qp.set_a(x3, 4, 1); qp.set_r(4, CGAL::SMALLER); qp.set_b(4, std::max(Byfff, Bybff) -(- Bzffb + Bybff - Bxfbf)); 
		qp.set_a(x1, 5,  1); qp.set_a(x3, 5, 1); qp.set_r(5, CGAL::LARGER); qp.set_b(5, -(- Bzffb + Bybff - Bxfbf) + std::min(Byfff, Bybff)); 

		qp.set_a(x1, 6,  -1); qp.set_a(x2, 6, 1); qp.set_r(6, CGAL::SMALLER); qp.set_b(6, std::max(Byffb, Bybfb) - (- Bzfbb + Bybfb + Bxbbf)); 
		qp.set_a(x1, 7,  -1); qp.set_a(x2, 7, 1); qp.set_r(7, CGAL::LARGER); qp.set_b(7, -(- Bzfbb + Bybfb + Bxbbf) + std::min(Byffb, Bybfb));

		qp.set_a(x3, 8,  -1); qp.set_a(x4, 8, 1); qp.set_r(8, CGAL::SMALLER); qp.set_b(8, std::max(Byfbf, Bybbf) - (+ Bzbfb + Bybbf - Bxfbb)); 
		qp.set_a(x3, 9,  -1); qp.set_a(x4, 9, 1); qp.set_r(9, CGAL::LARGER); qp.set_b(9, -(+ Bzbfb + Bybbf - Bxfbb) + std::min(Byfbf, Bybbf));

		qp.set_a(x2, 10,  -1); qp.set_a(x4, 10, -1); qp.set_r(10, CGAL::SMALLER); 
			qp.set_b(10, std::max(Byfbb, Bybbb) -(+ Bzbbb + Bybbb + Bxbbb)); 
		qp.set_a(x2, 11,  -1); qp.set_a(x4, 11, -1); qp.set_r(11, CGAL::LARGER); qp.set_b(11, -(+ Bzbbb + Bybbb + Bxbbb) + std::min(Byfbb, Bybbb));

		qp.set_a(x2, 12,  -1); qp.set_a(x3, 12, -1); qp.set_a(x5, 12, -1); qp.set_r(12, CGAL::SMALLER); 
		qp.set_b(12, std::max(Bzfff, Bzbff) - (+ Bzfff + Bzffb + Bzfbf + Bzfbb + Byfff + Byffb - Bybff - Bybfb + Bxfff + Bxfbf - Bxbff - Bxbbf)); 
		qp.set_a(x2, 13,  -1); qp.set_a(x3, 13, -1); qp.set_a(x5, 13, -1); qp.set_r(13, CGAL::LARGER); 
		qp.set_b(13, -(+ Bzfff + Bzffb + Bzfbf + Bzfbb + Byfff + Byffb - Bybff - Bybfb + Bxfff + Bxfbf - Bxbff - Bxbbf) + std::min(Bzfff, Bzbff)); 



		qp.set_a(x1, 14,  1); qp.set_r(14, CGAL::SMALLER); qp.set_b(14, std::max(Bxfbf, Bxbbf)); 
		qp.set_a(x1, 15,  1); qp.set_r(15, CGAL::LARGER); qp.set_b(15, std::min(Bxfbf, Bxbbf)); 

		qp.set_a(x2, 16,  1); qp.set_r(16, CGAL::SMALLER); qp.set_b(16, std::max(Bzfbb, Bzbbb)); 
		qp.set_a(x2, 17,  1); qp.set_r(17, CGAL::LARGER); qp.set_b(17, std::min(Bzfbb, Bzbbb)); 

		qp.set_a(x3, 18,  1); qp.set_r(18, CGAL::SMALLER); qp.set_b(18, std::max(Bzffb, Bzbfb)); 
		qp.set_a(x3, 19,  1); qp.set_r(19, CGAL::LARGER); qp.set_b(19, std::min(Bzffb, Bzbfb)); 

		qp.set_a(x4, 20,  1); qp.set_r(20, CGAL::SMALLER); qp.set_b(20, std::max(Bxfbb, Bxbbb)); 
		qp.set_a(x4, 21,  1); qp.set_r(21, CGAL::LARGER); qp.set_b(21, std::min(Bxfbb, Bxbbb)); 

		qp.set_a(x5, 22,  1); qp.set_r(22, CGAL::SMALLER); qp.set_b(22,  std::max(Bzfbf, Bzbbf)); 
		qp.set_a(x5, 23,  1); qp.set_r(23, CGAL::LARGER); qp.set_b(23, std::min(Bzfbf, Bzbbf)); 


		qp.set_d(x1,x1, 16);
		qp.set_d(x2,x2, 20);
		qp.set_d(x3,x3, 12);
		qp.set_d(x4,x4, 12);
		qp.set_d(x5,x5, 12);

		qp.set_d(x2,x1, -16);
		qp.set_d(x3,x1, 8);
		qp.set_d(x5,x1, -8);
		qp.set_d(x3,x2, 4);
		qp.set_d(x4,x2, 12);
		qp.set_d(x5,x2, 16);
		qp.set_d(x4,x3, -4);
		qp.set_d(x5,x3, 4);
		qp.set_d(x5,x4, 4);

		qp.set_c(x1, 2*C9+2*C8-2*C11-2*C10-2*C1+2*Bxfbf+2*Bxbbf);
		qp.set_c(x2, 2*C11+2*C10-2*C3+2*C2+2*C1-2*C7-2*C6-2*C4+2*Bzfbb+2*Bzbbb);
		qp.set_c(x3, 2*C9+2*C8-2*C5-2*C4+2*Bzffb+2*Bzbfb);
		qp.set_c(x4, -2*C7-2*C6+2*C5-2*C3+2*Bxfbb+2*Bxbbb);
		qp.set_c(x5, -2*C4-2*C3+2*C2-2*C2+2*C1+2*Bzfbf+2*Bzbbf);


		qp.set_c0(C9*C9*2 + C7*C7*2 + C4*C4*2 + C8*C8*2+ C6*C6*2+C5*C5*2+C3*C3*2+C2*C2*2+C11*C11*2+C1*C1*2
			+C10*C10*2+Bzffb*Bzffb*2+Bzfbf*Bzfbf*2+Bzfbb*Bzfbb*2+Bzbfb*Bzbfb*2+Bzbbf*Bzbbf*2
			+Bzbbb*Bzbbb*2+Bxfbf*Bxfbf*2+Bxfbb*Bxfbb*2+Bxbbf*Bxbbf*2+Bxbbb*Bxbbb*2);


		// solve the program, using ET as the exact type
		Solution s = CGAL::solve_quadratic_program(qp, ET());
		assert (s.solves_quadratic_program(qp));

		// output solution
		CGAL::Quadratic_program_solution<ET>::Variable_value_iterator opt = s.variable_values_begin();
		double v1 = to_double(*opt);
		double v2 = to_double(*(opt+1));
		double v3 = to_double(*(opt+2));
		double v4 = to_double(*(opt+3));
		double v5 = to_double(*(opt+4));

		inB[0][0] = v5 - Bzfbf - Bzfbb - Byffb + Bybfb + Bxbff + Bxbbf + v2 - v1;
		inB[0][1] = - v5 - v4 + Bzbbf + Bzbbb - Byfbb + Bybbb + Bxbfb + Bxbbb - v2;
		inB[0][2] = v1;
		inB[0][3] = v4;

		inB[1][0] =  v3 - Bzffb + Bybff - Bxfbf + v1;
		inB[1][1] = - Bzfbb + Bybfb + Bxbbf + v2 - v1;
		inB[1][2] = v4 -  v3 + Bzbfb + Bybbf - Bxfbb;
		inB[1][3] = - v4 + Bzbbb + Bybbb + Bxbbb - v2;

		inB[2][0] = - v5 -  v3 + Bzfff + Bzffb + Bzfbf + Bzfbb + Byfff + Byffb - Bybff - Bybfb + Bxfff + Bxfbf - Bxbff - Bxbbf - v2;
		inB[2][1] = v3;
		inB[2][2] = v5;
		inB[2][3] = v2;

		return inB;
	}
};
