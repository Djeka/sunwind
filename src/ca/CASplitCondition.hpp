class CASplitCondition{
	public: virtual bool doSplit(Cell* cell) {return NULL;};
//	public: virtual bool doSplit(Cell* cell, int iteration) {return NULL;}; // TODO: жуткий хак!! сплитование должно происходить отдельной ячейки сколь угодно раз, а не послойной!
};
