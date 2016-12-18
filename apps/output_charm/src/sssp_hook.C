public:
	void verify() {
		//FIXME:!!!
		//if (this->props.updated) {
		if (this->props.parent != -1) {
				thisProxy[props.parent].check(this->props.dist);
		}
	}
	void check(int dist) {
		CmiAssert(this->props.dist <= dist);
	}
	void print() {
		CkPrintf("%d: dist = %d\n", thisIndex, this->props.dist);
	}
	void count_visited(const CkCallback &cb) {
		CmiUInt8 c = (props.parent == -1 ? 0 : 1);
		//contribute(sizeof(CmiUInt8), &c, CkReduction::sum_long,
		//		CkCallback(CkReductionTarget(TestDriver, done), driverProxy));
		contribute(sizeof(CmiUInt8), &c, CkReduction::sum_long, cb);
	}

