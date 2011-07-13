class Side;
class Rib;

/*

      	        _____________________________
	      /            ur              /|
	     / |                          / |
	    /                            /  |
	 ul/   |                      ur/   |
	  /                            /    |
	 /     |                      /     |
	/____________________________/      |
	|      |      dr             |      |
	|                            |      |ur
	|      |                     |      |
	|      dr                    |      |
	|      |                     |      |				axis
	|                            |      |                    2
      dl|      |                     |ul    |                    |    1
        |                            |      |                    |   /
	|      |_ _ _ _ _ _ _ _ _ _ _|_ _ _ |                    |  /
	|                  ul        |     /                     | /
	|    /                       |    /                      |/________`0
	|   dl                       |   /
	|  /                         |  /dr
	|                            | /
	|/                           |/
	|____________________________|
		      dl

*/


class CellInt{
	public: virtual int getIndex(){}; // возвращает индекс ячейки. Т.е. если это сабячейка, то индекс от 0 до 7. Если это ячейка верхнего уровня, то индекс от 0 и до количества ячекк верхнего уровня
	public: virtual void setIndex(int ind){}; 
	public: virtual double getDensity(){}; // плотность в ячейке. Если ячека содержит подячейки, то берется среднее по всем
	public: virtual void setDensity(double density){}; 
	public: virtual double getdDensity(){}; // приращение плотности за итерацию по времени
	public: virtual void setdDensity(double){};
	public: virtual double* getMomentum(){}; // импульс. Возвращается массив из 3-х элементов
	public: virtual double getMomentum(int axis){}; // импульс. Возвращается импульс по оси axis
	public: virtual void setMomentum(double m, int axis){};
	public: virtual double getdMomentum(int axis){}; // приращение импульса по оси axis
	public: virtual void setdMomentum(double, int){};
	public: virtual double getTermEnergy(){}; // тепловая энергия в ячейке. Та которая эпсилон.
	public: virtual void setTermEnergy(double){};
	public: virtual double getdTermEnergy(){}; // приращение тепловой энергии
	public: virtual void setdTermEnergy(double){};
	public: virtual double getFullEnergy(){}; // полная энергия (тепловая + кинетическая + магнитная)
	public: virtual void setFullEnergy(double){};

	public: virtual double* getR(){}; // массив радиус-вектора центра ячейки
	public: virtual double getR(int axis){}; // элемент радиус-вектора по оси axis
	public: virtual double getRadius(){}; // расстояние от начала координат - до центра ячейки
	public: virtual double* getH(){}; // массив размера ячейки
	public: virtual double getH(int axis){}; // размер ячейки по оси axis
	public: virtual int* getPoint(){}; // координата ячейки (целочисленная). Начинается с [0,0,0] и далее по соответствующей оси
	public: virtual int getPoint(int axis){};
	public: virtual CellInt* getCell(int axis, int direction){}; // Соседняя ячейка. Берется по оси axis в направлении direction (Cell::FORWARD_SIDE, Cell::BACKWARD_SIDE)
	public: virtual Side* getSide(int axis, int direction){}; // Грань ячейки. Берется по оси axis в направлении direction (Cell::FORWARD_SIDE, Cell::BACKWARD_SIDE)
	public: virtual void setSide(int axis, int direction, SideInt*){};
	public: virtual Rib* getRib(int axis, int ind){}; // Ребро ячейки. Берется по оси axis. ind - индекс ребра (см. картинку выше), возможные значения: Cell::ur, Cell::dr, Cell::ul, Cell::dl
	public: virtual void setRib(int, int, RibInt*){};
	public: virtual int getMark(){}; // маркировка ячейки. Нужна для определения типа ячейки: внутренняя (Cell::INTERNAL_CELL_MARK), граничная (Cell::FREE_BORDER_CELL_MARK), солнечный ветер (SW_BORDER_CELL_MARK)
	public: virtual void setMark(int){};

	public: virtual double getT(){}; // Возврашает температуру
	public: virtual double getV(){}; // Возврашает вектор скорости
	public: virtual double getV(int axis){}; // Возврашает скорость по оси axis
	public: virtual double getP(){}; // Возврашает давление
	public: virtual void setP(double P){};
	public: virtual double getMaxSpeed(){}; // Возврашает максимальную скоость (собственное число) в ячейке.
	public: virtual double getMaxSpeedOnSide(int axis, int direction){}; // Возврашает масимальную скорость на грани
	public: virtual double getMinTimeStep(){}; // Возврашает минимальный шаг по времени с учетом максимальной скорости и размера ячейки
	public: virtual string getPointString(){}; // Возврашает стрингу-идентификатор ячейки в дереве.
	public: virtual double getDivB(){}; // Возвращает дивергенцию магнитного поля в ячейке (в идеале должно быть нулем ;) )
};
