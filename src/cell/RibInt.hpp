/*
                          2
                          |
                          |        
                          |       0
                          |      /
                 ur       |     /       dr
                          |    /
                          |   /
                          |  /
                          | /
            1_____________|/_______________
                         /|
                 ul     / |             dl
                       /  |
                      /   |
                     /    |
                    /     |
*/

class RibInt{
	public: virtual int getAxis(){}; // ось по которой направлено ребро
	public: virtual Cell* getCell(int ind){}; // получить соседнюю с ребром ячейку по индексу ind (см. картинку сверху). ind = Cell::ur; Cell::ul; Cell::dr; Cell::dl
	public: virtual void setCell(int ind, CellInt* cell){};
	public: virtual double getE(){}; // Электрическое поле на ребре
	public: virtual void setE(double E){};
	public: virtual double getdE(){}; // приращение эл. поля
	public: virtual void setdE(double dE){};
	public: virtual int getMark(){}; // маркировка ребра: внутренняя (Cell::INTERNAL_CELL_MARK), граничная (Cell::FREE_BORDER_CELL_MARK), солнечный ветер (SW_BORDER_CELL_MARK)
	public: virtual void setMark(int mark){};
	public: virtual double getJ(){}; // Ток на ребре
	public: virtual void free(){};
};
