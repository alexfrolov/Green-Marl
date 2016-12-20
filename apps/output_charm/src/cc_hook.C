public:
	void verify() {
		typedef std::list<cc_edge>::iterator Iterator;
		for (Iterator i = edges.begin(); i != edges.end(); i++)
			thisProxy[i->v].check(this->props.CC);
	}
	void check(CmiUInt8 cc) {
		CmiAssert(this->props.CC == cc);
	}
	void print() {
		CkPrintf("%d: CC = %d\n", thisIndex, this->props.CC);
	}
