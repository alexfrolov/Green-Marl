#include "sssp.decl.h"
// these readonly variables should initialized in mainchare constructor
CProxy_sssp_master master_proxy;
CProxy_sssp_vertex vertex_proxy;

struct sssp_edge {
    uint64_t v;
    struct edge_properties {
        int len;
        void pup(PUP::er & p) {
            p | len;
        }
    };
    struct edge_properties props;
    sssp_edge () {} 
    sssp_edge (uint64_t v) : v(v) {} 
    sssp_edge (uint64_t v, const struct edge_properties &props) : 			v(v), props(props) {} 
    void pup(PUP::er & p) { p | v; p | props; }
};

class sssp_vertex : public CBase_sssp_vertex {
	public:
		struct vertex_properties {
			bool updated;
			long parent;
			int dist;
		};
	public:
		sssp_vertex() {}
		sssp_vertex(CkMigrateMessage *m) {}
		void add_edge(const sssp_edge &e) { edges.push_back(e); } 
		void init() {
			this->props.updated = false;
			this->props.parent = -1;
			this->props.dist = std::numeric_limits<int>::max();
		}
		void make_root() {
			this->props.parent = thisIndex;
			this->props.dist = 0;
			this->props.updated = true;
			typedef std::list<sssp_edge>::iterator Iterator;
			for(Iterator i = edges.begin(); i != edges.end(); i++)
				thisProxy[i->v].update(this->props.dist + i->props.len, thisIndex);
		}
		void update(int l, CmiUInt8 p) {
			if (l < this->props.dist) {
				this->props.dist = l;
				this->props.parent = p;
				this->props.updated = true;
				typedef std::list<sssp_edge>::iterator Iterator;
				for(Iterator i = edges.begin(); i != edges.end(); i++)
					thisProxy[i->v].update(this->props.dist + i->props.len, thisIndex);
			}
		}
	private:
		std::list<sssp_edge> edges;
		vertex_properties props;
		CmiUInt8 parent;
    #ifdef HOOK_ENABLE
       #include "sssp_hook.C"
    #endif
};
class sssp_master : public CBase_sssp_master {
	public:
		sssp_master(const CkCallback & cb) {
			done_callback = cb;
		}
		void do_sssp(long root) {
			vertex_proxy.init();
			CkStartQD(CkCallbackResumeThread());
			vertex_proxy[root].make_root();
			CkStartQD(done_callback);
		}
	private:
		CkCallback done_callback;
};
#include "sssp.def.h"
