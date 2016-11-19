#include "gm_graph.h"

class sssp_graph;
class sssp_edge;
class sssp_vertex;

#include "sssp_main.decl.h"
#include "common.h"
#include "sssp.C"

CProxy_Main main_proxy;
extern CProxy_sssp_master master_proxy;
extern CProxy_sssp_vertex vertex_proxy;

class sssp_graph {
	public:
		typedef sssp_vertex 				vertex;
		typedef sssp_edge					 	edge;
		typedef CProxy_sssp_vertex 	proxy;
		sssp_graph () {}
		sssp_graph (const proxy & vertex_proxy) :
			vertex_proxy(vertex_proxy) {}
		const proxy & get_proxy() { return vertex_proxy; } 
		void pup(PUP::er &p) {
			p | vertex_proxy;
		}
		static sssp_edge::edge_properties generate_edge_properties() {
			sssp_edge::edge_properties props;
			props.len = random() % 10;
			return props;
		}
	private:
		proxy vertex_proxy;
};

class Main : public CBase_Main {
	public:
		Main(CkArgMsg *m) {
			parse_options(m, &opts);
			main_proxy = thishandle;
			CkPrintf("Hello.\n");
			vertex_proxy = CProxy_sssp_vertex::ckNew(opts.N);
			master_proxy = CProxy_sssp_master::ckNew(CkCallback(CkIndex_Main::done(), thisProxy));
			graphlib::create_rmat_graph<sssp_graph>(sssp_graph(vertex_proxy), opts.rmat_cfg);
			CkStartQD(CkCallback(CkIndex_Main::done(), thisProxy));
		}
		void do_sssp() {
			srandom(123);
			root = random() % opts.N;
			master_proxy.do_sssp(root);
		}
		void done() {
			CkPrintf("Done.\n");
			CkExit();
		}
	private:
		Options opts;
		CmiUInt8 root;
};
#include "sssp_main.def.h"
