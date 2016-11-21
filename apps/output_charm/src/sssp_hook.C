public:
	void verify() {
		if (this->props.updated) {
				thisProxy[props.parent].check(this->props.dist);
		}
	}
	void check(int dist) {
		CmiAssert(this->props.dist <= dist);
	}
	void print() {
		CkPrintf("%d: dist = %d\n", thisIndex, this->props.dist);
	}
