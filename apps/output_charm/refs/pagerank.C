#include "pagerank.decl.h"
// these readonly variables should initialized in mainchare constructor
CProxy_pagerank_master master_proxy;
CProxy_pagerank_vertex vertex_proxy;
CmiUInt8 vertex_number;

struct pagerank_edge {
    uint64_t v;
    struct edge_properties {
        void pup(PUP::er & p) {}
    };
    struct edge_properties props;
    pagerank_edge () {} 
    pagerank_edge (uint64_t v) : v(v) {} 
    pagerank_edge (uint64_t v, const struct edge_properties &props) : 			v(v), props(props) {} 
    void pup(PUP::er & p) { p | v; p | props; }
};

class pagerank_vertex : public CBase_pagerank_vertex {
	public:
		struct vertex_properties {
			double pg_rank;
			double val;
		};
	public:
		pagerank_vertex() { }
		pagerank_vertex(CkMigrateMessage *m) {}
		void add_edge(const pagerank_edge &e) { edges.push_back(e); } 
		void init(double d) {
			this->props.val = 0;
			this->props.pg_rank = 1.0 / vertex_number;
		}
		void do_pagerank_step_update() {
			typedef std::list<pagerank_edge>::iterator Iterator;
			for(Iterator i = edges.begin(); i != edges.end(); i++)
				thisProxy[i->v].update(this->props.pg_rank / edges.size());
		}
		void update(double pr) {
			this->props.val += pr;
		}
		void do_pagerank_step_complete(double d) {
			this->props.val = d * this->props.val + (1.0 - d) / vertex_number;
			double diff = std::abs(this->props.pg_rank - this->props.val);
			contribute(sizeof(double), &diff, CkReduction::sum_double,
					CkCallback(CkReductionTarget(pagerank_master, reduction_diff), master_proxy));
			this->props.pg_rank = this->props.val;
			this->props.val = 0;
		}

	private:
		std::list<pagerank_edge> edges;
		vertex_properties props;
		CmiUInt8 parent;
    #ifdef HOOK_ENABLE
       #include "pagerank_hook.C"
    #endif
};
class pagerank_master : public CBase_pagerank_master {
	public:
		pagerank_master(const CkCallback & cb) {
			diff = 1.0;
			done_callback = cb;
		}
		void do_pagerank(int max, double e, double d) {
			vertex_proxy.init(d);
			CkStartQD(CkCallbackResumeThread());
			for (int cnt = 0; diff > e && cnt < max; cnt++) {
				vertex_proxy.do_pagerank_step_update();
				CkStartQD(CkCallbackResumeThread());
				vertex_proxy.do_pagerank_step_complete(d);
				CkStartQD(CkCallbackResumeThread());
				CkPrintf("cnt = %d (%d), diff = %f (%f) \n", cnt, max, diff, e);
			}
			CkStartQD(done_callback);
		}
		void reduction_diff(double d) { 
			diff = d; 
		}

	private:
		CkCallback done_callback;
		volatile double diff;
};
#include "pagerank.def.h"
