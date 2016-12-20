#include "cc.decl.h"
// these readonly variables should initialized in mainchare constructor
CProxy_cc_master master_proxy;
CProxy_cc_vertex vertex_proxy;

struct cc_edge {
    uint64_t v;
    struct edge_properties {
        void pup(PUP::er & p) {}
    };
    struct edge_properties props;
    cc_edge () {} 
    cc_edge (uint64_t v) : v(v) {} 
    cc_edge (uint64_t v, const struct edge_properties &props) : 			v(v), props(props) {} 
    void pup(PUP::er & p) { p | v; p | props; }
};

class cc_vertex : public CBase_cc_vertex {
	public:
		struct vertex_properties {
			long CC;
		};
	public:
		cc_vertex() {}
		cc_vertex(CkMigrateMessage *m) {}
		void add_edge(const cc_edge &e) { edges.push_back(e); } 
		void init() {
			this->props.CC = thisIndex;
		}
		void start() {
			typedef std::list<cc_edge>::iterator Iterator;
			for(Iterator i = edges.begin(); i != edges.end(); i++)
				thisProxy[i->v].update(this->props.CC);
		}
		void update(CmiUInt8 cc) {
			if (cc < this->props.CC) {
				this->props.CC = cc;
				typedef std::list<cc_edge>::iterator Iterator;
				for(Iterator i = edges.begin(); i != edges.end(); i++)
					thisProxy[i->v].update(this->props.CC);
			}
		}
	private:
		std::list<cc_edge> edges;
		vertex_properties props;
		CmiUInt8 parent;
    #ifdef HOOK_ENABLE
       #include "cc_hook.C"
    #endif
};
class cc_master : public CBase_cc_master {
	public:
		cc_master(const CkCallback & cb) {
			done_callback = cb;
		}
		void do_cc() {
			vertex_proxy.init();
			CkStartQD(CkCallbackResumeThread());
			vertex_proxy.start();
			CkStartQD(done_callback);
		}
	private:
		CkCallback done_callback;
};
#include "cc.def.h"
