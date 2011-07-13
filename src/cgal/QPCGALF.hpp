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


class QPCGALF{
	public: static double** calculateQP(double** outB){
		double** inB = new double*[3];
		for(int dir=0; dir<3; dir++){
			inB[dir] = new double[12];
		}

		// by default, we have a nonnegative QP with Ax <= b
		Program qp (CGAL::SMALLER, false, 0, false, 0); 

		const int x1 = 0; 
		const int x2 = 1; 
		const int x3 = 2; 
		const int x4 = 3; 
		const int x5 = 4;
		const int x6 = 5;
		const int x7 = 6;
		const int x8 = 7;
		const int x9 = 8;
		const int x10 = 9;
		const int x11 = 10;

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
		double Byb1f=outB[0][6];
		double Byb1b=outB[0][7];
		double Byb2f=outB[0][8];
		double Byb2b=outB[0][9];
		double Tyf = (Byf1f - Byf1b) / 8;
		double Kyf = (Byf2f - Byf2b) / 8;
		double Tyb = (Byb1f - Byb1b) / 8;
		double Kyb = (Byb2f - Byb2b) / 8;

		double Bzf = outB[2][0];
		double Bzb = outB[2][1];
		double Bzf1f=outB[0][2];
		double Bzf1b=outB[0][3];
		double Bzf2f=outB[0][4];
		double Bzf2b=outB[0][5];
		double Bzb1f=outB[0][6];
		double Bzb1b=outB[0][7];
		double Bzb2f=outB[0][8];
		double Bzb2b=outB[0][9];
		double Tzf = (Bzf1f - Bzf1b) / 8;
		double Kzf = (Bzf2f - Bzf2b) / 8;
		double Tzb = (Bzb1f - Bzb1b) / 8;
		double Kzb = (Bzb2f - Bzb2b) / 8;

		double C1 = Bxf + Txf + Kxf + Tzf-Tyf+Txb-Kzf+Kyf+Kxb-Bzf+Bxb-Byf;
		double C2 = Tzf-Tyf+Txb-Kzf+Kyf+Kxb-Bzf+Bxb-Byf + Bxb+ Txb + Kxb;
		double C3 = Byf + Tyf + Kyf+ 2*Tyb+Txf-Txb+2*Kzf-Kxf+Kxb-2*Bzf+2*Byb-Bxf+Bxb;
		double C4 = 2*Tyb+Txf-Txb+2*Kzf-Kxf+Kxb-2*Bzf+2*Byb-Bxf+Bxb + Byb+ Tyb + Kyb;
		double C5 = Bzf+ Tzf + Kzf+ 2*Tyf-2*Tyb+2*Kxf-2*Kxb+4*Bzf+2*Byf-2*Byb+2*Bxf-2*Bxb;
		double C6 = 2*Tyf-2*Tyb+2*Kxf-2*Kxb+4*Bzf+2*Byf-2*Byb+2*Bxf-2*Bxb +  Bzb+ Tzb + Kzb;
		double C7 = Bxf - Txf + Kxf+ Tyf-Tzb+Txb+Kzb+Kyf-Kxb+Bzb-Byf+Bxb;
		double C8 = Tyf-Tzb+Txb+Kzb+Kyf-Kxb+Bzb-Byf+Bxb + Bxb+ Txb - Kxb;
		double C9 = Byf- Tyf + Kyf;
		double C10 = Byb+ Tyb - Kyb;
		double C11 = Bzf- Tzf + Kzf;
		double C12 = Bzb + Tzb - Kzb;
		double C13 = Bxb+ Txb + Kxb+ Tzf+Tyb-Txb+Kzf-Kyb+Kxb-Bzf+Byb+Bxb;
		double C14 = Tzf+Tyb-Txb+Kzf-Kyb+Kxb-Bzf+Byb+Bxb + Bxb- Txb + Kxb;
		double C15 = Byb+ Tyb + Kyb+ 2*Byb-Bxf+Bxb+2*Bzb-2*Tyb+Txf-Txb-2*Kzb+Kxf-Kxb;
		double C16 = 2*Byb-Bxf+Bxb+2*Bzb-2*Tyb+Txf-Txb-2*Kzb+Kxf-Kxb + Byb- Tyb + Kyb;
		double C17 = Bzb + Tzb + Kzb;
		double C18 = Bzb - Tzb + Kzb;
		double C19 = Bxb- Txb + Kxb+ Bzb+Byb+Bxb-Tzb-Tyb-Txb-Kzb-Kyb-Kxb;
		double C20 = Bzb+Byb+Bxb-Tzb-Tyb-Txb-Kzb-Kyb-Kxb + Bxb- Txb - Kxb;
		double C21 = Byb- Tyb + Kyb;
		double C22 = Byb- Tyb - Kyb;
		double C23 = Bzb- Tzb + Kzb;
		double C24 = Bzb- Tzb - Kzb;

/*
		x1 - Bycfb
		x2 - Bycbb
		x3 - Bzcfb
		x4 - Bzcbf
		x5 - Bzcbb
		x6 - Axf
		x7 - Axb
		x8 - Ayf
		x9 - Ayb
		x10 - Azf
		x11 - Azb
*/
		int ind = 0;
//		Bxcff : Bycfb + Bzcbf + Axb - Ayf - Azf   + Tzf-Tyf+Txb-Kzf+Kyf+Kxb-Bzf+Bxb-Byf;

		qp.set_a(x1,ind,1);
		qp.set_a(x4,ind,1);
		qp.set_a(x7,ind,1);
		qp.set_a(x8,ind,-1);
		qp.set_a(x10,ind,-1);
		qp.set_r(ind, CGAL::SMALLER);
		qp.set_b(ind, std::max(Bxf, Bxb) - (Tzf-Tyf+Txb-Kzf+Kyf+Kxb-Bzf+Bxb-Byf));

		ind++;
		qp.set_a(x1, ind,1);
		qp.set_a(x4, ind,1);
		qp.set_a(x7, ind,1);
		qp.set_a(x8, ind,-1);
		qp.set_a(x10,ind,-1);
		qp.set_r(ind, CGAL::LARGER);
		qp.set_b(ind, std::min(Bxf, Bxb) - (Tzf-Tyf+Txb-Kzf+Kyf+Kxb-Bzf+Bxb-Byf)); 


//		Bxcfb : Bycbb - Bzcbf + Axb - Ayf + Azb   + Tyf-Tzb+Txb+Kzb+Kyf-Kxb+Bzb-Byf+Bxb;
		ind++;
		qp.set_a(x2,ind,1);
		qp.set_a(x4,ind,-1);
		qp.set_a(x7,ind,1);
		qp.set_a(x8,ind,-1);
		qp.set_a(x11,ind,1);
		qp.set_r(ind, CGAL::SMALLER);
		qp.set_b(ind, std::max(Bxf, Bxb) - (Tyf-Tzb+Txb+Kzb+Kyf-Kxb+Bzb-Byf+Bxb));

		ind++;
		qp.set_a(x2,ind,1);
		qp.set_a(x4,ind,-1);
		qp.set_a(x7,ind,1);
		qp.set_a(x8,ind,-1);
		qp.set_a(x11,ind,1);
		qp.set_r(ind, CGAL::LARGER);
		qp.set_b(ind, std::min(Bxf, Bxb) - (Tyf-Tzb+Txb+Kzb+Kyf-Kxb+Bzb-Byf+Bxb));


//		Bxcbf :-Bycfb + Bzcbb + Axb + Ayb - Azf   + Tzf+Tyb-Txb+Kzf-Kyb+Kxb-Bzf+Byb+Bxb;
		ind++;
		qp.set_a(x1,ind,-1);
		qp.set_a(x5,ind,1);
		qp.set_a(x7,ind,1);
		qp.set_a(x9,ind,1);
		qp.set_a(x10,ind,-1);
		qp.set_r(ind, CGAL::SMALLER);
		qp.set_b(ind, std::max(Bxf, Bxb) - (Tzf+Tyb-Txb+Kzf-Kyb+Kxb-Bzf+Byb+Bxb));

		ind++;
		qp.set_a(x1,ind,-1);
		qp.set_a(x5,ind,1);
		qp.set_a(x7,ind,1);
		qp.set_a(x9,ind,1);
		qp.set_a(x10,ind,-1);
		qp.set_r(ind, CGAL::LARGER);
		qp.set_b(ind, std::min(Bxf, Bxb) - (Tzf+Tyb-Txb+Kzf-Kyb+Kxb-Bzf+Byb+Bxb));


//		Bxcbb :-Bycbb - Bzcbb + Axb + Ayb + Azb   + Bzb+Byb+Bxb-Tzb-Tyb-Txb-Kzb-Kyb-Kxb;
		ind++;
		qp.set_a(x2,ind,-1);
		qp.set_a(x5,ind,-1);
		qp.set_a(x7,ind,1);
		qp.set_a(x9,ind,1);
		qp.set_a(x11,ind,1);
		qp.set_r(ind, CGAL::SMALLER);
		qp.set_b(ind, std::max(Bxf, Bxb) - (Bzb+Byb+Bxb-Tzb-Tyb-Txb-Kzb-Kyb-Kxb));

		ind++;
		qp.set_a(x2,ind,-1);
		qp.set_a(x5,ind,-1);
		qp.set_a(x7,ind,1);
		qp.set_a(x9,ind,1);
		qp.set_a(x11,ind,1);
		qp.set_r(ind, CGAL::LARGER);
		qp.set_b(ind, std::min(Bxf, Bxb) - (Bzb+Byb+Bxb-Tzb-Tyb-Txb-Kzb-Kyb-Kxb));


//		Bycff :-Bycfb + Bzcfb + Bzcbb - Axf + Axb + 2*Ayb - 2*Azf   + 2*Tyb+Txf-Txb+2*Kzf-Kxf+Kxb-2*Bzf+2*Byb-Bxf+Bxb;
		ind++;
		qp.set_a(x1,ind,-1);
		qp.set_a(x3,ind,1);
		qp.set_a(x5,ind,1);
		qp.set_a(x6,ind,-1);
		qp.set_a(x7,ind,1);
		qp.set_a(x9,ind,-2);
		qp.set_a(x10,ind,-2);
		qp.set_r(ind, CGAL::SMALLER);
		qp.set_b(ind, std::max(Byf, Byb) - (2*Tyb+Txf-Txb+2*Kzf-Kxf+Kxb-2*Bzf+2*Byb-Bxf+Bxb));

		ind++;
		qp.set_a(x1,ind,-1);
		qp.set_a(x3,ind,1);
		qp.set_a(x5,ind,1);
		qp.set_a(x6,ind,-1);
		qp.set_a(x7,ind,1);
		qp.set_a(x9,ind,-2);
		qp.set_a(x10,ind,-2);
		qp.set_r(ind, CGAL::LARGER);
		qp.set_b(ind, std::min(Byf, Byb) - (2*Tyb+Txf-Txb+2*Kzf-Kxf+Kxb-2*Bzf+2*Byb-Bxf+Bxb));


//		Bycbf :-Bycbb - Bzcfb - Bzcbb - Axf + Axb + 2*Ayb + 2*Azb   + 2*Byb-Bxf+Bxb+2*Bzb-2*Tyb+Txf-Txb-2*Kzb+Kxf-Kxb;
		ind++;
		qp.set_a(x2,ind,-1);
		qp.set_a(x3,ind,-1);
		qp.set_a(x5,ind,-1);
		qp.set_a(x6,ind,-1);
		qp.set_a(x7,ind,1);
		qp.set_a(x9,ind,2);
		qp.set_a(x11,ind,2);
		qp.set_r(ind, CGAL::SMALLER);
		qp.set_b(ind, std::max(Byf, Byb) - (2*Byb-Bxf+Bxb+2*Bzb-2*Tyb+Txf-Txb-2*Kzb+Kxf-Kxb));

		ind++;
		qp.set_a(x2,ind,-1);
		qp.set_a(x3,ind,-1);
		qp.set_a(x5,ind,-1);
		qp.set_a(x6,ind,-1);
		qp.set_a(x7,ind,1);
		qp.set_a(x9,ind,2);
		qp.set_a(x11,ind,2);
		qp.set_r(ind, CGAL::LARGER);
		qp.set_b(ind, std::min(Byf, Byb) - (2*Byb-Bxf+Bxb+2*Bzb-2*Tyb+Txf-Txb-2*Kzb+Kxf-Kxb));


//		Bzcff :-Bzcfb - Bzcbf - Bzcbb + 2*Axf - 2*Axb + 2*Ayf - 2*Ayb + 4*Azf   + 2*Tyf-2*Tyb+2*Kxf-2*Kxb+4*Bzf+2*Byf-2*Byb+2*Bxf-2*Bxb;
		ind++;
		qp.set_a(x3,ind,-1);
		qp.set_a(x4,ind,-1);
		qp.set_a(x5,ind,-1);
		qp.set_a(x6,ind,2);
		qp.set_a(x7,ind,-2);
		qp.set_a(x8,ind,2);
		qp.set_a(x9,ind,-2);
		qp.set_a(x10,ind,4);
		qp.set_r(ind, CGAL::SMALLER);
		qp.set_b(ind, std::max(Bzf, Bzb) - (2*Tyf-2*Tyb+2*Kxf-2*Kxb+4*Bzf+2*Byf-2*Byb+2*Bxf-2*Bxb));

		ind++;
		qp.set_a(x3,ind,-1);
		qp.set_a(x4,ind,-1);
		qp.set_a(x5,ind,-1);
		qp.set_a(x6,ind,2);
		qp.set_a(x7,ind,-2);
		qp.set_a(x8,ind,2);
		qp.set_a(x9,ind,-2);
		qp.set_a(x10,ind,4);
		qp.set_r(ind, CGAL::LARGER);
		qp.set_b(ind, std::min(Bzf, Bzb) - (2*Tyf-2*Tyb+2*Kxf-2*Kxb+4*Bzf+2*Byf-2*Byb+2*Bxf-2*Bxb));


		ind++;
		qp.set_a(x1,ind,1);
		qp.set_r(ind, CGAL::SMALLER);
		qp.set_b(ind, std::max(Byf, Byb));
		ind++;
		qp.set_a(x1,ind,1);
		qp.set_r(ind, CGAL::LARGER);
		qp.set_b(ind, std::min(Byf, Byb));

		ind++;
		qp.set_a(x2,ind,1);
		qp.set_r(ind, CGAL::SMALLER);
		qp.set_b(ind, std::max(Byf, Byb));
		ind++;
		qp.set_a(x2,ind,1);
		qp.set_r(ind, CGAL::LARGER);
		qp.set_b(ind, std::min(Byf, Byb));

		ind++;
		qp.set_a(x3,ind,1);
		qp.set_r(ind, CGAL::SMALLER);
		qp.set_b(ind, std::max(Bzf, Bzb));
		ind++;
		qp.set_a(x3,ind,1);
		qp.set_r(ind, CGAL::LARGER);
		qp.set_b(ind, std::min(Bzf, Bzb));

		ind++;
		qp.set_a(x4,ind,1);
		qp.set_r(ind, CGAL::SMALLER);
		qp.set_b(ind, std::max(Bzf, Bzb));
		ind++;
		qp.set_a(x4,ind,1);
		qp.set_r(ind, CGAL::LARGER);
		qp.set_b(ind, std::min(Bzf, Bzb));

		ind++;
		qp.set_a(x5,ind,1);
		qp.set_r(ind, CGAL::SMALLER);
		qp.set_b(ind, std::max(Bzf, Bzb));
		ind++;
		qp.set_a(x5,ind,1);
		qp.set_r(ind, CGAL::LARGER);
		qp.set_b(ind, std::min(Bzf, Bzb));


		double min = 0;
		double max = 0;
		double Cff = 0;
		double Cfb = 0;
		double Cbf = 0;
		double Cbb = 0;
///X

		max = std::max(std::max(std::max(Bxf1f, Bxf1b), std::max(Bxf2f, Bxf2b)), Bxf);
		min = std::min(std::min(std::min(Bxf1f, Bxf1b), std::min(Bxf2f, Bxf2b)), Bxf);
		Cff = Bxf/4.0 + Txf + Kxf;
		Cfb = Bxf/4.0 + Txf - Kxf;
		Cbf = Bxf/4.0 - Txf + Kxf;
		Cbb = Bxf/4.0 - Txf - Kxf;

		ind++;
		qp.set_a(x1,ind,1);
		qp.set_r(ind, CGAL::SMALLER);
		qp.set_b(ind, std::min(std::min(max - Cff, max - Cbb), std::min(-min + Cfb, -min + Cbf)) );
		ind++;
		qp.set_a(x1,ind,1);
		qp.set_r(ind, CGAL::LARGER);
		qp.set_b(ind, std::max(std::max(min - Cff, min - Cbb), std::max(-max + Cfb, -max + Cbf)) );


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


		qp.set_d(x9,x8, -12);
		qp.set_d(x9,x7, 42);
		qp.set_d(x9,x6, -34);
		qp.set_d(x9,x5, 10);
		qp.set_d(x9,x4, 8);
		qp.set_d(x9,x3, 10);
		qp.set_d(x9,x2, -12);
		qp.set_d(x11,x9, 16);
		qp.set_d(x10,x9, -60);
		qp.set_d(x9,x1, -16);
		qp.set_d(x8,x7, -22);
		qp.set_d(x8,x6, 14);
		qp.set_d(x8,x5, -6);
		qp.set_d(x8,x4, -8);
		qp.set_d(x8,x3, -6);
		qp.set_d(x8,x2, -4);
		qp.set_d(x10,x8, 36);
		qp.set_d(x8,x1, -8);
		qp.set_d(x7,x6, -24);
		qp.set_d(x7,x5, 8);
		qp.set_d(x7,x4, 12);
		qp.set_d(x7,x3, 8);
		qp.set_d(x7,x2, -6);
		qp.set_d(x11,x7, 10);
		qp.set_d(x10,x7, -54);
		qp.set_d(x7,x1, -2);
		qp.set_d(x6,x5, -8);
		qp.set_d(x6,x4, -4);
		qp.set_d(x6,x3, -8);
		qp.set_d(x6,x2, 2);
		qp.set_d(x11,x6, -6);
		qp.set_d(x10,x6, 42);
		qp.set_d(x6,x1, 6);
		qp.set_d(x5,x4, 4);
		qp.set_d(x5,x3, 12);
		qp.set_d(x5,x2, 8);
		qp.set_d(x11,x5, -14);
		qp.set_d(x10,x5, -30);
		qp.set_d(x5,x1, -8);
		qp.set_d(x4,x3, 4);
		qp.set_d(x4,x2, -4);
		qp.set_d(x11,x4, -6);
		qp.set_d(x10,x4, -22);
		qp.set_d(x4,x1, 4);
		qp.set_d(x3,x2, 4);
		qp.set_d(x11,x3, -12);
		qp.set_d(x10,x3, -28);
		qp.set_d(x3,x1, -4);
		qp.set_d(x11,x2, -8);
		qp.set_d(x11,x10, 8);
		qp.set_d(x10,x1, 8);


		qp.set_c(x1, 2*C9-2*C4-2*C3+2*C2-2*C14-2*C13+2*C10+2*C1);
		qp.set_c(x2, 2*C8+2*C7+2*C22+2*C21-2*C20-2*C19-2*C16-2*C15);
		qp.set_c(x3, -2*C6-2*C5+2*C4+2*C3-2*C16-2*C15+2*C12+2*C11);
		qp.set_c(x4, -2*C8-2*C7-2*C6-2*C5+2*C2+2*C18+2*C17+2*C1);
		qp.set_c(x5, -2*C6-2*C5+2*C4+2*C3+2*C24+2*C23-2*C20-2*C19-2*C16-2*C15+2*C14 + 2*C13);
		qp.set_c(x6, -2*C7+4*C6+4*C5-2*C4-2*C3-2*C16-2*C15+2*C1);
		qp.set_c(x7, 2*C7-4*C6-4*C5+2*C4+2*C3+4*C20+4*C2+2*C16+4*C13+2*C15+2*C1);
		qp.set_c(x8, -2*C9-2*C8-2*C7+4*C6+4*C5+2*C3-2*C2-2*C1);
		qp.set_c(x9, -4*C6-4*C5+6*C4+4*C3+2*C22-2*C21+2*C20+2*C19+2*C16+6*C15+2*C14+2*C13-2*C10);
		qp.set_c(x10, 8*C6-4*C3+10*C5-4*C4-2*C2-2*C13-2*C14-2*C11-2*C1);
		qp.set_c(x11, 2*C8+2*C7+2*C6-2*C23+2*C24+2*C20+2*C19-2*C18+2*C17+4*C16+4*C15-2*C12);

		qp.set_d(x1, x1, 8);
		qp.set_d(x2, x2, 8);
		qp.set_d(x3, x3, 8);
		qp.set_d(x4, x4, 8);
		qp.set_d(x5, x5, 12);
		qp.set_d(x6, x6, 14);
		qp.set_d(x7, x7, 26);
		qp.set_d(x8, x8, 14);
		qp.set_d(x9, x9, 38);
		qp.set_d(x10, x10, 54);
		qp.set_d(x11, x11, 18);

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
		double v7 = to_double(*(opt+6));
		double v8 = to_double(*(opt+7));
		double v9 = to_double(*(opt+8));
		double v10 = to_double(*(opt+9));
		double v11 = to_double(*(opt+10));

		inB[0][0] = Bxf + v6 + Txf + Kxf;
		inB[0][1] = Bxf - v6 + Txf - Kxf;
		inB[0][2] = Bxf - v6 - Txf + Kxf;
		inB[0][3] = Bxf + v6 - Txf - Kxf;

		inB[0][4] = v1 + v4 + v7 - v8 - v10   + Tzf-Tyf+Txb-Kzf+Kyf+Kxb-Bzf+Bxb-Byf;
		inB[0][5] = v2 - v4 + v7 - v8 + v11   + Tyf-Tzb+Txb+Kzb+Kyf-Kxb+Bzb-Byf+Bxb;
		inB[0][6] = -v1 + v5 + v7 + v9 - v10   + Tzf+Tyb-Txb+Kzf-Kyb+Kxb-Bzf+Byb+Bxb;
		inB[0][7] = -v2 - v5 + v7 + v9 + v11   + Bzb+Byb+Bxb-Tzb-Tyb-Txb-Kzb-Kyb-Kxb;

		inB[0][8] = Bxb + v7 + Txb + Kxb;
		inB[0][9] = Bxb - v7 + Txb - Kxb;
		inB[0][10] = Bxb - v7 - Txb + Kxb;
		inB[0][11] = Bxb + v7 - Txb - Kxb;


		inB[1][0] = Byf + v8 + Tyf + Kyf;
		inB[1][1] = Byf - v8 + Tyf - Kyf;
		inB[1][2] = Byf - v8 - Tyf + Kyf;
		inB[1][3] = Byf + v8 - Tyf - Kyf;

		inB[1][4] = -v1 + v3 + v5 - v6 + v7 + 2*v9 - 2*v10   + 2*Tyb+Txf-Txb+2*Kzf-Kxf+Kxb-2*Bzf+2*Byb-Bxf+Bxb;
		inB[1][5] = v1;
		inB[1][6] = -v2 - v3 - v5 - v6 + v7 + 2*v9 + 2*v11   + 2*Byb-Bxf+Bxb+2*Bzb-2*Tyb+Txf-Txb-2*Kzb+Kxf-Kxb;
		inB[1][7] = v2;

		inB[1][8] = Byb + v9 + Tyb + Kyb;
		inB[1][9] = Byb - v9 + Tyb - Kyb;
		inB[1][10] = Byb - v9 - Tyb + Kyb;
		inB[1][11] = Byb + v9 - Tyb - Kyb;


		inB[2][0] = Bzf + v10 + Tzf + Kzf;
		inB[2][1] = Bzf - v10 + Tzf - Kzf;
		inB[2][2] = Bzf - v10 - Tzf + Kzf;
		inB[2][3] = Bzf + v10 - Tzf - Kzf;

		inB[2][4] = -v3 - v4 - v5 + 2*v6 - 2*v7 + 2*v8 - 2*v9 + 4*v10   + 2*Tyf-2*Tyb+2*Kxf-2*Kxb+4*Bzf+2*Byf-2*Byb+2*Bxf-2*Bxb;
		inB[2][5] = v3;
		inB[2][6] = v4;
		inB[2][7] = v5;

		inB[2][8] = Bzb + v11 + Tzb + Kzb;
		inB[2][9] = Bzb - v11 + Tzb - Kzb;
		inB[2][10] = Bzb - v11 - Tzb + Kzb;
		inB[2][11] = Bzb + v11 - Tzb - Kzb;

		return inB;
	}
};
