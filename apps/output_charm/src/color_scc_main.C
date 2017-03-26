#include "gm_graph.h"

class color_scc_graph;
class color_scc_edge;
class color_scc_vertex;

#include "color_scc_main.decl.h"
#include "common.h"

CProxy_Main main_proxy;
extern CProxy_color_scc_master master_proxy;
extern CProxy_color_scc_vertex vertex_proxy;
CmiUInt8 N;

#include "color_scc.C"

class color_scc_graph {
	public:
		typedef color_scc_vertex 				vertex;
		typedef color_scc_edge					 	edge;
		typedef CProxy_color_scc_vertex 	proxy;
		color_scc_graph () {}
		color_scc_graph (const proxy & vertex_proxy) :
			vertex_proxy(vertex_proxy) {}
		const proxy & get_proxy() { return vertex_proxy; } 
		void pup(PUP::er &p) {
			p | vertex_proxy;
		}
		static color_scc_edge::edge_properties generate_edge_properties() {
			color_scc_edge::edge_properties props;
			return props;
		}
	private:
		proxy vertex_proxy;
};

bool verify = true;

class Main : public CBase_Main {
	public:
		Main(CkArgMsg *m) : totaltime(0), sccCounter(0) {
			parse_options(m, &opts);
			main_proxy = thishandle;
			CkPrintf("Hello.\n");
			N = opts.N;
			vertex_proxy = CProxy_color_scc_vertex::ckNew(opts.N);
			master_proxy = CProxy_color_scc_master::ckNew(CkCallback(CkIndex_Main::done(), thisProxy));
			if (opts.graph_type == RMAT)
				graphlib::create_rmat_graph<color_scc_graph>(color_scc_graph(vertex_proxy), opts.rmat_cfg);
			else if (opts.graph_type == Random)
				graphlib::create_random_graph<color_scc_graph>(color_scc_graph(vertex_proxy), opts.random_cfg);
			else
				CkAbort("Unexpected graph type");

			srandom(123);
			CkStartQD(CkCallback(CkIndex_Main::do_color_scc(), thisProxy));
		}
		void do_color_scc() {
			starttime = CkWallTimer();
			master_proxy.do_color_scc();
		}
		void done() {
			CkPrintf("color_scc done.\n");
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
			if (verify) {
				CkPrintf("total SCC count is %lld\n", sccCounter);
			}
			CkPrintf("done.\n");
			CkExit();
		}
		void sccCounter_inc() { sccCounter++; }
	private:
		Options opts;
		double starttime, totaltime;
		unsigned long sccCounter;
};
#include "color_scc_main.def.h"
