class Cell;
class Rib;
class CellInt;
class RibInt;

/*
		|*.     
		|    *.   RIGHT_RIB
		|        *.
		|	     *.	
		|		|			axis
     DOWN_RIB	|		|		1
		|		|		|           0
		|		|		|         /
		|		| UP_RIB	|      /
		*.		|		|   /
		    *.		|		|/
	    LEFT_RIB	*.	|		*.
		  	    *.  |		    *.
			        *		        *.
							    2
*/

class SideInt{
	public: virtual int getAxis(){}; // ось по которой направлен перпендикуляр к грани
	public: virtual double getB(){}; // магнитное поле в центре грани
	public: virtual void setB(double B){}; 
	public: virtual void setdB(double dB){};
	public: virtual double getdB(){}; // приращение магниного поля поля
	public: virtual double getBbg(){}; // фоновое магнитное поле
	public: virtual void setBbg(double Bbg){}; // фоновое магнитное поле
	public: virtual double getBplusBbg(){}; // полное магнитное поле
	public: virtual Cell* getFCell(){}; // forward cell (по оси axis)
	public: virtual Cell* getBCell(){}; // backward cell (против оси axis)
	public: virtual Cell* getCell(int ind){}; // получить соседнюю ячейку. ind = Cell::FORWARD_SIDE || Cell::BACKWARD_SIDE
	public: virtual void setCell(int dir, CellInt* cell){};
	public: virtual int getMark(){}; // маркировка грани. внутренняя (Cell::INTERNAL_CELL_MARK), граничная (Cell::FREE_BORDER_CELL_MARK), солнечный ветер (SW_BORDER_CELL_MARK)
	public: virtual void setMark(int mark){};
	public: virtual double* getFlow(){}; // поток на грани
	public: virtual double getFlow(int ind){}; // ind-ый элемент потока
	public: virtual void setFlow(vector<double> inf){};
	public: virtual Rib* getRib(int ind){}; // получить ребро на грани. Определяет в соответствии c ind и с картинкой на верху. ind = Rib::UP_RIB, Rib::DOWN_RIB, Rib:: LEFT_RIB, Rib::RIGHT_RIB
	public: virtual void setRib(RibInt* rib, int ind){};
	public: virtual Rib* getRib(int inaxis, int dir){}; // получить репорт на грани по оси и направлени. inaxis - ось, dir - направление. 
							       // Например Rib::UP_LEFT можно получить как inaxis = getRib( (axis+2)%3, Cell::FORWARD_SIDE)
	public: virtual void free(){};
	public: virtual bool isFlowCalculated(){};
	public: virtual void setIsFlowCalculated(bool is){};
//	public: virtual double getH(int axis){};
};
