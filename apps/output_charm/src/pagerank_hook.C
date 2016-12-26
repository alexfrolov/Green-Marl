public:
	void verify() {
		//FIXME:!!!
		//if (this->props.updated) {
		/*if (this->props.parent != -1) {
				thisProxy[props.parent].check(this->props.dist);
		}*/
	}
	void check(int dist) {
		//CmiAssert(this->props.dist <= dist);
	}
	void print() {
		CkPrintf("%d: dist = %f, ", thisIndex, this->props.pg_rank);
		/*typedef std::list<pagerank_edge>::iterator Iterator;
		for(Iterator i = edges.begin(); i != edges.end(); i++)
			CkPrintf("%d ", i->v);*/
		CkPrintf("\n");
	}
