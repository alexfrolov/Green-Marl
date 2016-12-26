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
		CkPrintf("%d: dist = %f\n", thisIndex, this->props.pg_rank);
	}
