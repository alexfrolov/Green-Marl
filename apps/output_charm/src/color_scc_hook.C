public:
	void verify() {
		//typedef std::vector<color_scc_edge>::iterator Iterator;
		//for (Iterator i = edges.begin(); i != edges.end(); i++)
		//	thisProxy[i->v].check(this->props.SCC);

		if (this->props.SCC == thisIndex)
			main_proxy.sccCounter_inc();

	}
	void check(CmiUInt8 scc) {
		//CmiAssert(this->props.SCC == scc);
	}
	void print() {
		CkPrintf("%d: SCC = %lld, color = %lld\n", thisIndex, this->props.SCC, this->props.color);
	}
