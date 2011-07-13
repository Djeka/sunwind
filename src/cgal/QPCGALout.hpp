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


class QPCGALout{
	public: static double** calculateQP(double** outB){
		double** inB = new double*[3];
		for(int dir=0; dir<3; dir++){
			inB[dir] = new double[8];
		}

		// by default, we have a nonnegative QP with Ax <= b
		Program qp (CGAL::SMALLER, false, 0, false, 0); 

		const int x1 = 0; 
		const int x2 = 1; 
		const int x3 = 2; 
		const int x4 = 3; 
		const int x5 = 4;
		const int x6 = 5;

		double Bxf = outB[0][0];
		double Bxb = outB[0][1];
		double Bxf1f=outB[0][2];
		double Bxf1b=outB[0][3];
		double Bxf2f=outB[0][4];
		double Bxf2b=outB[0][5];
		double Bxb1f=outB[0][6];
		double Bxb1b=outB[0][7];
		double Bxb2f=outB[0][8];
		double Bxb2b=outB[0][9];
		double Txf = (Bxf1f - Bxf1b) / 8;
		double Kxf = (Bxf2f - Bxf2b) / 8;
		double Txb = (Bxb1f - Bxb1b) / 8;
		double Kxb = (Bxb2f - Bxb2b) / 8;

		double Byf = outB[1][0];
		double Byb = outB[1][1];
		double Byf1f=outB[1][2];
		double Byf1b=outB[1][3];
		double Byf2f=outB[1][4];
		double Byf2b=outB[1][5];
		double Byb1f=outB[1][6];
		double Byb1b=outB[1][7];
		double Byb2f=outB[1][8];
		double Byb2b=outB[1][9];
		double Tyf = (Byf1f - Byf1b) / 8;
		double Kyf = (Byf2f - Byf2b) / 8;
		double Tyb = (Byb1f - Byb1b) / 8;
		double Kyb = (Byb2f - Byb2b) / 8;

		double Bzf = outB[2][0];
		double Bzb = outB[2][1];
		double Bzf1f=outB[2][2];
		double Bzf1b=outB[2][3];
		double Bzf2f=outB[2][4];
		double Bzf2b=outB[2][5];
		double Bzb1f=outB[2][6];
		double Bzb1b=outB[2][7];
		double Bzb2f=outB[2][8];
		double Bzb2b=outB[2][9];
		double Tzf = (Bzf1f - Bzf1b) / 8;
		double Kzf = (Bzf2f - Bzf2b) / 8;
		double Tzb = (Bzb1f - Bzb1b) / 8;
		double Kzb = (Bzb2f - Bzb2b) / 8;

/*
		x1 - Axf
		x2 - Axb
		x3 - Ayf
		x4 - Ayb
		x5 - Azf
		x6 - Azb
*/
		double min = 0;
		double max = 0;
		double Cff = 0;
		double Cfb = 0;
		double Cbf = 0;
		double Cbb = 0;
		int ind = 0;

///X

		max = std::max(std::max(std::max(Bxf1f, Bxf1b), std::max(Bxf2f, Bxf2b)), Bxf);
		min = std::min(std::min(std::min(Bxf1f, Bxf1b), std::min(Bxf2f, Bxf2b)), Bxf);
		Cff = Bxf/4.0 + Txf + Kxf;
		Cfb = Bxf/4.0 + Txf - Kxf;
		Cbf = Bxf/4.0 - Txf + Kxf;
		Cbb = Bxf/4.0 - Txf - Kxf;

		qp.set_a(x1,ind,1);
		qp.set_r(ind, CGAL::SMALLER);
		qp.set_b(ind, std::min(std::min(max - Cff, max - Cbb), std::min(-min + Cfb, -min + Cbf)) );
		ind++;
		qp.set_a(x1,ind,1);
		qp.set_r(ind, CGAL::LARGER);
		qp.set_b(ind, std::max(std::max(min - Cff, min - Cbb), std::max(-max + Cfb, -max + Cbf)) );

cout << 
std::max(
	std::max(std::min(std::min(Bxf1f, Bxf2f), Bxf) - Cff, -std::max(std::max(Bxf1f, Bxf2b), Bxf) + Cfb),
	std::max(std::max(std::max(Bxf1b, Bxf2f), Bxf) + Cbf,  std::min(std::min(Bxf1b, Bxf2b), Bxf) - Cbb)
)
<< "\t\t" <<
std::min(
	std::min(std::max(std::max(Bxf1f, Bxf2f), Bxf) - Cff, -std::min(std::min(Bxf1f, Bxf2b), Bxf) + Cfb),
	std::min(std::min(std::min(Bxf1b, Bxf2f), Bxf) + Cbf,  std::max(std::max(Bxf1b, Bxf2b), Bxf) - Cbb)
)
<< endl;

cout << std::min(std::min(max - Cff, max - Cbb), std::min(-min + Cfb, -min + Cbf)) << "\t" <<
std::max(std::max(min - Cff, min - Cbb), std::max(-max + Cfb, -max + Cbf)) << endl; 

		max = std::max(std::max(std::max(Bxb1f, Bxb1b), std::max(Bxb2f, Bxb2b)), Bxb);
		min = std::min(std::min(std::min(Bxb1f, Bxb1b), std::min(Bxb2f, Bxb2b)), Bxb);
		Cff = Bxb/4.0 + Txb + Kxb;
		Cfb = Bxb/4.0 + Txb - Kxb;
		Cbf = Bxb/4.0 - Txb + Kxb;
		Cbb = Bxb/4.0 - Txb - Kxb;

		ind++;
		qp.set_a(x2,ind,1);
		qp.set_r(ind, CGAL::SMALLER);
		qp.set_b(ind, std::min(std::min(max - Cff, max - Cbb), std::min(-min + Cfb, -min + Cbf)) );
		ind++;
		qp.set_a(x2,ind,1);
		qp.set_r(ind, CGAL::LARGER);
		qp.set_b(ind, std::max(std::max(min - Cff, min - Cbb), std::max(-max + Cfb, -max + Cbf)) );

///Y

		max = std::max(std::max(std::max(Byf1f, Byf1b), std::max(Byf2f, Byf2b)), Byf);
		min = std::min(std::min(std::min(Byf1f, Byf1b), std::min(Byf2f, Byf2b)), Byf);
		Cff = Byf/4.0 + Tyf + Kyf;
		Cfb = Byf/4.0 + Tyf - Kyf;
		Cbf = Byf/4.0 - Tyf + Kyf;
		Cbb = Byf/4.0 - Tyf - Kyf;

		ind++;
		qp.set_a(x3,ind,1);
		qp.set_r(ind, CGAL::SMALLER);
		qp.set_b(ind, std::min(std::min(max - Cff, max - Cbb), std::min(-min + Cfb, -min + Cbf)) );
		ind++;
		qp.set_a(x3,ind,1);
		qp.set_r(ind, CGAL::LARGER);
		qp.set_b(ind, std::max(std::max(min - Cff, min - Cbb), std::max(-max + Cfb, -max + Cbf)) );


		max = std::max(std::max(std::max(Byb1f, Byb1b), std::max(Byb2f, Byb2b)), Byb);
		min = std::min(std::min(std::min(Byb1f, Byb1b), std::min(Byb2f, Byb2b)), Byb);
		Cff = Byb/4.0 + Tyb + Kyb;
		Cfb = Byb/4.0 + Tyb - Kyb;
		Cbf = Byb/4.0 - Tyb + Kyb;
		Cbb = Byb/4.0 - Tyb - Kyb;

		ind++;
		qp.set_a(x4,ind,1);
		qp.set_r(ind, CGAL::SMALLER);
		qp.set_b(ind, std::min(std::min(max - Cff, max - Cbb), std::min(-min + Cfb, -min + Cbf)) );
		ind++;
		qp.set_a(x4,ind,1);
		qp.set_r(ind, CGAL::LARGER);
		qp.set_b(ind, std::max(std::max(min - Cff, min - Cbb), std::max(-max + Cfb, -max + Cbf)) );

///Z

		max = std::max(std::max(std::max(Bzf1f, Bzf1b), std::max(Bzf2f, Bzf2b)), Bzf);
		min = std::min(std::min(std::min(Bzf1f, Bzf1b), std::min(Bzf2f, Bzf2b)), Bzf);
		Cff = Bzf/4.0 + Tzf + Kzf;
		Cfb = Bzf/4.0 + Tzf - Kzf;
		Cbf = Bzf/4.0 - Tzf + Kzf;
		Cbb = Bzf/4.0 - Tzf - Kzf;

		ind++;
		qp.set_a(x5,ind,1);
		qp.set_r(ind, CGAL::SMALLER);
		qp.set_b(ind, std::min(std::min(max - Cff, max - Cbb), std::min(-min + Cfb, -min + Cbf)) );
		ind++;
		qp.set_a(x5,ind,1);
		qp.set_r(ind, CGAL::LARGER);
		qp.set_b(ind, std::max(std::max(min - Cff, min - Cbb), std::max(-max + Cfb, -max + Cbf)) );


		max = std::max(std::max(std::max(Bzb1f, Bzb1b), std::max(Bzb2f, Bzb2b)), Bzb);
		min = std::min(std::min(std::min(Bzb1f, Bzb1b), std::min(Bzb2f, Bzb2b)), Bzb);
		Cff = Bzb/4.0 + Tzb + Kzb;
		Cfb = Bzb/4.0 + Tzb - Kzb;
		Cbf = Bzb/4.0 - Tzb + Kzb;
		Cbb = Bzb/4.0 - Tzb - Kzb;

		ind++;
		qp.set_a(x6,ind,1);
		qp.set_r(ind, CGAL::SMALLER);
		qp.set_b(ind, std::min(std::min(max - Cff, max - Cbb), std::min(-min + Cfb, -min + Cbf)) );
		ind++;
		qp.set_a(x6,ind,1);
		qp.set_r(ind, CGAL::LARGER);
		qp.set_b(ind, std::max(std::max(min - Cff, min - Cbb), std::max(-max + Cfb, -max + Cbf)) );


		qp.set_d(x6,x5, -16);
		qp.set_d(x6,x4, 24);
		qp.set_d(x6,x3, -8);
		qp.set_d(x6,x2, 24);
		qp.set_d(x6,x1, -8);
		qp.set_d(x5,x4, -40);
		qp.set_d(x5,x3, 24);
		qp.set_d(x5,x2, -40);
		qp.set_d(x5,x1, 24);
		qp.set_d(x4,x3, -16);
		qp.set_d(x4,x2, 40);
		qp.set_d(x4,x1, -24);
		qp.set_d(x3,x2, -24);
		qp.set_d(x3,x1, 8);
		qp.set_d(x2,x1, -16);

		qp.set_d(x1, x1, 8);
		qp.set_c(x1, -4*(2*Txf-2*Txb+2*Kzf-2*Kzb-2*Bzf/4.0+2*Bzb/4.0+4*Byb/4.0-2*Bxf/4.0+2*Bxb/4.0)+4*(2*Tyf-2*Tyb+2*Kxf-2*Kxb+4*Bzf/4.0+2*Byf/4.0-2*Byb/4.0+2*Bxf/4.0-2*Bxb/4.0));

		qp.set_d(x2, x2, 24);
		qp.set_c(x2, 8*(2*Tzf-2*Tzb+2*Kyf-2*Kyb-2*Bzf/4.0+2*Bzb/4.0-2*Byf/4.0+2*Byb/4.0+4*Bxb/4.0)+4*(2*Txf-2*Txb+2*Kzf-2*Kzb-2*Bzf/4.0+2*Bzb/4.0+4*Byb/4.0-2*Bxf/4.0+2*Bxb/4.0)-4*(2*Tyf-2*Tyb+2*Kxf-2*Kxb+4*Bzf/4.0+2*Byf/4.0-2*Byb/4.0+2*Bxf/4.0-2*Bxb/4.0));

		qp.set_d(x3, x3, 8);
		qp.set_c(x3, -4*(2*Tzf-2*Tzb+2*Kyf-2*Kyb-2*Bzf/4.0+2*Bzb/4.0-2*Byf/4.0+2*Byb/4.0+4*Bxb/4.0)+4*(2*Tyf-2*Tyb+2*Kxf-2*Kxb+4*Bzf/4.0+2*Byf/4.0-2*Byb/4.0+2*Bxf/4.0-2*Bxb/4.0));

		qp.set_d(x4, x4, 24);
		qp.set_c(x4, 4*(2*Tzf-2*Tzb+2*Kyf-2*Kyb-2*Bzf/4.0+2*Bzb/4.0-2*Byf/4.0+2*Byb/4.0+4*Bxb/4.0)-4*(2*Tyf-2*Tyb+2*Kxf-2*Kxb+4*Bzf/4.0+2*Byf/4.0-2*Byb/4.0+2*Bxf/4.0-2*Bxb/4.0)+8*(2*Txf-2*Txb+2*Kzf-2*Kzb-2*Bzf/4.0+2*Bzb/4.0+4*Byb/4.0-2*Bxf/4.0+2*Bxb/4.0));

		qp.set_d(x5, x5, 24);
		qp.set_c(x5, -4*(2*Txf-2*Txb+2*Kzf-2*Kzb-2*Bzf/4.0+2*Bzb/4.0+4*Byb/4.0-2*Bxf/4.0+2*Bxb/4.0) + 8*(2*Tyf-2*Tyb+2*Kxf-2*Kxb+4*Bzf/4.0+2*Byf/4.0-2*Byb/4.0+2*Bxf/4.0-2*Bxb/4.0)-4*(2*Tzf-2*Tzb+2*Kyf-2*Kyb-2*Bzf/4.0+2*Bzb/4.0-2*Byf/4.0+2*Byb/4.0+4*Bxb/4.0));

		qp.set_d(x6, x6, 8);
		qp.set_c(x6, 4*(2*Tzf-2*Tzb+2*Kyf-2*Kyb-2*Bzf/4.0+2*Bzb/4.0-2*Byf/4.0+2*Byb/4.0+4*Bxb/4.0)+4*(2*Txf-2*Txb+2*Kzf-2*Kzb-2*Bzf/4.0+2*Bzb/4.0+4*Byb/4.0-2*Bxf/4.0+2*Bxb/4.0));


		// solve the program, using ET as the exact type
		Solution s = CGAL::solve_quadratic_program(qp, ET());
		assert (s.solves_quadratic_program(qp));

 std::cout << s << endl;

		// output solution
		CGAL::Quadratic_program_solution<ET>::Variable_value_iterator opt = s.variable_values_begin();
		double v1 = to_double(*opt);
		double v2 = to_double(*(opt+1));
		double v3 = to_double(*(opt+2));
		double v4 = to_double(*(opt+3));
		double v5 = to_double(*(opt+4));
		double v6 = to_double(*(opt+5));

		inB[0][0] = Bxb/4.0 + v2 + Txb + Kxb;
		inB[0][1] = Bxb/4.0 - v2 + Txb - Kxb;
		inB[0][2] = Bxb/4.0 - v2 - Txb + Kxb;
		inB[0][3] = Bxb/4.0 + v2 - Txb - Kxb;
		inB[0][4] = Bxf/4.0 + v1 + Txf + Kxf;
		inB[0][5] = Bxf/4.0 - v1 + Txf - Kxf;
		inB[0][6] = Bxf/4.0 - v1 - Txf + Kxf;
		inB[0][7] = Bxf/4.0 + v1 - Txf - Kxf;


		inB[1][0] = Byb/4.0 + v4 + Tyb + Kyb;
		inB[1][1] = Byb/4.0 - v4 + Tyb - Kyb;
		inB[1][2] = Byb/4.0 - v4 - Tyb + Kyb;
		inB[1][3] = Byb/4.0 + v4 - Tyb - Kyb;
		inB[1][4] = Byf/4.0 + v3 + Tyf + Kyf;
		inB[1][5] = Byf/4.0 - v3 + Tyf - Kyf;
		inB[1][6] = Byf/4.0 - v3 - Tyf + Kyf;
		inB[1][7] = Byf/4.0 + v3 - Tyf - Kyf;


		inB[2][0] = Bzb/4.0 + v6 + Tzb + Kzb;
		inB[2][1] = Bzb/4.0 - v6 + Tzb - Kzb;
		inB[2][2] = Bzb/4.0 - v6 - Tzb + Kzb;
		inB[2][3] = Bzb/4.0 + v6 - Tzb - Kzb;
		inB[2][4] = Bzf/4.0 + v5 + Tzf + Kzf;
		inB[2][5] = Bzf/4.0 - v5 + Tzf - Kzf;
		inB[2][6] = Bzf/4.0 - v5 - Tzf + Kzf;
		inB[2][7] = Bzf/4.0 + v5 - Tzf - Kzf;

		return inB;
	}
};
