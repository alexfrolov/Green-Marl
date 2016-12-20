#include "gm_graph.h"

class cc_graph;
class cc_edge;
class cc_vertex;

#include "cc_main.decl.h"
#include "common.h"
#include "cc.C"

CProxy_Main main_proxy;
extern CProxy_cc_master master_proxy;
extern CProxy_cc_vertex vertex_proxy;

class cc_graph {
	public:
		typedef cc_vertex 				vertex;
		typedef cc_edge					 	edge;
		typedef CProxy_cc_vertex 	proxy;
		cc_graph () {}
		cc_graph (const proxy & vertex_proxy) :
			vertex_proxy(vertex_proxy) {}
		const proxy & get_proxy() { return vertex_proxy; } 
		void pup(PUP::er &p) {
			p | vertex_proxy;
		}
		static cc_edge::edge_properties generate_edge_properties() {
			cc_edge::edge_properties props;
			return props;
		}
	private:
		proxy vertex_proxy;
};

bool verify = true;

class Main : public CBase_Main {
	public:
		Main(CkArgMsg *m) : totaltime(0) {
			parse_options(m, &opts);
			main_proxy = thishandle;
			CkPrintf("Hello.\n");
			vertex_proxy = CProxy_cc_vertex::ckNew(opts.N);
			master_proxy = CProxy_cc_master::ckNew(CkCallback(CkIndex_Main::done(), thisProxy));
			if (opts.graph_type == RMAT)
				graphlib::create_rmat_graph<cc_graph>(cc_graph(vertex_proxy), opts.rmat_cfg);
			else if (opts.graph_type == Random)
				graphlib::create_random_graph<cc_graph>(cc_graph(vertex_proxy), opts.random_cfg);
			else
				CkAbort("Unexpected graph type");

			srandom(123);
			CkStartQD(CkCallback(CkIndex_Main::do_cc(), thisProxy));
		}
		void do_cc() {
			starttime = CkWallTimer();
			master_proxy.do_cc();
		}
		void done() {
			CkPrintf("cc done.\n");
			totaltime += CkWallTimer() - starttime;
			CkPrintf("[Final] CPU time used = %.6f seconds\n", totaltime);
			if (verify) {
				CkPrintf("start verification...\n");
				vertex_proxy.verify();
				//vertex_proxy.print();
				CkStartQD(CkCallback(CkIndex_Main::exit(), thisProxy));
			} else
				CkExit();
		}
		void exit() {
			CkPrintf("done.\n");
			CkExit();
		}
	private:
		Options opts;
		double starttime, totaltime;
};
#include "cc_main.def.h"
