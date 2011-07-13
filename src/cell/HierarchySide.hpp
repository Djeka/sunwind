class Cell;
class Rib;

class HierarchySide : public Side
{
	protected: HierarchySide* parent;
	protected: Side* subsides[4];
	protected: bool splitted;

	public: HierarchySide(int axis, Cell* bcell, Cell* fcell){
		HierarchySide::axis = axis;
		HierarchySide::B = 0;
		HierarchySide::dB = 0;
		HierarchySide::Bbg = 0;
		HierarchySide::fcell = fcell;
		HierarchySide::bcell = bcell;

		F = new double[8];

		for(int i=0; i<8; i++) F[i] = 0;
		for(int i=0; i<4; i++) ribs[i] = NULL;
		for(int i=0; i<4; i++) subsides[i] = NULL;

		parent = NULL;
		HierarchySide::flowCalculated = false;
		splitted = false;
	}

	public: ~HierarchySide() {
		delete[] F;
	}

	public: void free(){
		for(int irib=0; irib<4; irib++){
			Rib* rib = getRib(irib);
			for(int jside=0; jside<4; jside++){
				if(rib != NULL && this == rib->getSide(jside)){
					rib->setSide(jside, NULL);
					break;
				}
			}
			setRib(NULL, irib);
		}

		if(fcell != NULL && fcell->getSide(axis, 0) == this) fcell->setSide(axis, 0, NULL);
		if(bcell != NULL && bcell->getSide(axis, 0) == this) bcell->setSide(axis, 1, NULL);

		setCell(1, NULL);
		setCell(0, NULL);

		for(int i=0; i<4; i++){
			setRib(NULL, i);
		}

		delete this;
	}

	public: bool isSplitted(){
		return splitted;
	}

	public: void setIsSplitted(bool is){
		splitted = is;
	}

	public: HierarchySide* getParent(){
		return parent;
	}

	public: void setParent(HierarchySide* side){
		parent = side;
	}

	public: HierarchySide* getSubSide(int ind){
		return (HierarchySide*) subsides[ind];
	}

	public: HierarchySide* getSubSide(int ind1, int ind2){
		if(ind1 == 0 && ind2 == 0) return (HierarchySide*) subsides[0];
		else if(ind1 == 0 && ind2 == 1 ) return (HierarchySide*) subsides[1];
		else if(ind1 == 1 && ind2 == 0 ) return (HierarchySide*) subsides[2];
		else if(ind1 == 1 && ind2 == 1 ) return (HierarchySide*) subsides[3];
	}

	public: void deleteSubsides(){
		delete[] subsides;
	}

	public: void setSubSide(int ind, Side* side){
		subsides[ind] = side;
	}

	public: double getB(){
		if(isSplitted()){
			double avB = 0;
			for(int i=0; i<4; i++) avB += 0.25 * subsides[i]->getB();
			return  avB;
		} else {
			return B;
		}
	}

	public: double getdB(){
		if(isSplitted()){
			double avdB = 0;
			for(int i=0; i<4; i++) avdB += 0.25 * subsides[i]->getdB();
			return  avdB;
		} else {
			return dB;
		}
	}

	public: double getFlow(int ind){
		if(isSplitted()){
			F[ind] = 0;
			for(int i=0; i<4; i++)	F[ind] += 0.25 * subsides[i]->getFlow(ind);
			return  F[ind];
		} else{
			return F[ind];
		}
	}
};
