#include "color_scc.decl.h"
// these readonly variables should initialized in mainchare constructor
CProxy_color_scc_master master_proxy;
CProxy_color_scc_vertex vertex_proxy;

struct color_scc_edge {
    uint64_t v;
    struct edge_properties {
        void pup(PUP::er & p) {}
    };
    struct edge_properties props;
    color_scc_edge () {} 
    color_scc_edge (uint64_t v) : v(v) {} 
    color_scc_edge (uint64_t v, const struct edge_properties &props) : 			v(v), props(props) {} 
    void pup(PUP::er & p) { p | v; p | props; }
};

class color_scc_vertex : public CBase_color_scc_vertex {
	public:
		struct vertex_properties {
			unsigned long SCC;
			unsigned long color;
			vertex_properties() : SCC(-1), color(-1) {}
		};
	public:
		color_scc_vertex() {}
		color_scc_vertex(CkMigrateMessage *m) {}
		void add_edge(const color_scc_edge &e) { edges.push_back(e); } 
		void init() {
			if (this->props.SCC == -1) { 
				this->trans_edges.clear();
				this->props.color = thisIndex;
			}
		}
		void transpose() { 
			if (this->props.SCC == -1)
				for(Iterator i = edges.begin(); i != edges.end(); i++)
					thisProxy[i->v].transpose_update(thisIndex);
		}
		void transpose_update(const CmiUInt8& v) {
			trans_edges.push_back(color_scc_edge(v));
		}
		void trim() { 
			if (this->props.SCC == -1 && 
					(this->edges.size() == 0 || this->trans_edges.size() == 0)) {
				this->props.SCC = thisIndex;
				//CkPrintf("%d: trimmed (Out-degree %d, In(parent)-degree %d)\n", 
				//		thisIndex, this->edges.size(), this->trans_edges.size());
			}
		}
		void forward() {
			if (this->props.SCC == -1) 
				for(Iterator i = edges.begin(); i != edges.end(); i++)
					thisProxy[i->v].forward_update(this->props.color);
		}
		void forward_update(const unsigned long & color) {
			if (this->props.SCC == -1 && color < this->props.color) {
				this->props.color = color;
				for(Iterator i = edges.begin(); i != edges.end(); i++)
					thisProxy[i->v].forward_update(this->props.color);
			}
		}
		void backward() {
			if (this->props.SCC == -1 && this->props.color == thisIndex) { 
				//CkPrintf("%d: backward started\n", thisIndex);
				this->props.SCC = this->props.color;
				for(Iterator i = trans_edges.begin(); i != trans_edges.end(); i++)
					thisProxy[i->v].backward_update(this->props.color, thisIndex);
			}
		}
		void backward_update(const unsigned long & color, const CmiUInt8 & source) {
			//CkPrintf("%d: backward_update this->props.color = %lld, color = %lld, source = %lld\n", 
			//		thisIndex, this->props.color, color, source);
			if (this->props.SCC == -1 && this->props.color == color) { 
				this->props.SCC = color;
				for(Iterator i = trans_edges.begin(); i != trans_edges.end(); i++)
					thisProxy[i->v].backward_update(this->props.color, thisIndex);
			}
		}
		void test_completion() {
			bool fin = (this->props.SCC != -1);
			//if (!fin) CkPrintf("%d:--> SCC = %lld\n", thisIndex, this->props.SCC); 
			contribute(sizeof(bool), &fin, CkReduction::logical_and,
					CkCallback(CkReductionTarget(color_scc_master, report_completion), master_proxy));
			CmiUInt8 c = (this->props.SCC != -1) ? 1 : 0;
			contribute(sizeof(CmiUInt8), &c, CkReduction::sum_long,
					CkCallback(CkReductionTarget(color_scc_master, report_completion1), master_proxy));
		}
	private:
		typedef std::vector<color_scc_edge>::iterator Iterator;
		std::vector<color_scc_edge> edges;
		std::vector<color_scc_edge> trans_edges;
		vertex_properties props;
		CmiUInt8 parent;
    #ifdef HOOK_ENABLE
       #include "color_scc_hook.C"
    #endif
};
class color_scc_master : public CBase_color_scc_master {
	public:
		color_scc_master(const CkCallback & cb) {
			done_callback = cb;
		}
		void do_color_scc() {
			CkPrintf("do_color_scc: start\n");

			fin = false;
			while (!fin) {
				CkPrintf("do_color_scc: while-loop iteration start\n");
				
				// initialize new step
				vertex_proxy.init();
				CkWaitQD();

				// transpose graph (only for unlabeled vertices)
				vertex_proxy.transpose();
				CkWaitQD();

				// trim dangling vertices
				vertex_proxy.trim();
				CkWaitQD();

				// forward coloring
				vertex_proxy.forward();
				CkWaitQD();

				//vertex_proxy.print();
				//CkWaitQD();

				// backward coloring
				vertex_proxy.backward();
				CkWaitQD();

				vertex_proxy.test_completion();
				CkWaitQD();
			}

			CkStartQD(done_callback);
		}
		void report_completion(const bool & fin) {
			//this->fin = fin;
		}
		void report_completion1(const CmiUInt8 & tot) {
			CkPrintf("tot = %lld\n", tot);
			if (tot == N)
				this->fin = true;
		}
	private:
		volatile bool fin;
		CkCallback done_callback;
};
#include "color_scc.def.h"
