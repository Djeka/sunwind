class Iterator{
	public: virtual void drop(){};
	public: virtual Cell* next(){};
	public: virtual bool hasNext(){ return false;};
};
