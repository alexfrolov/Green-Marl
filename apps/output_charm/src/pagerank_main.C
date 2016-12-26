#include "gm_graph.h"

class pagerank_graph;
class pagerank_edge;
class pagerank_vertex;

#include "pagerank_main.decl.h"
#include "common.h"
#include "pagerank.C"

CProxy_Main main_proxy;
extern CProxy_pagerank_master master_proxy;
extern CProxy_pagerank_vertex vertex_proxy;

class pagerank_graph {
	public:
		typedef pagerank_vertex 				vertex;
		typedef pagerank_edge					 	edge;
		typedef CProxy_pagerank_vertex 	proxy;
		pagerank_graph () {}
		pagerank_graph (const proxy & vertex_proxy) :
			vertex_proxy(vertex_proxy) {}
		const proxy & get_proxy() { return vertex_proxy; } 
		void pup(PUP::er &p) {
			p | vertex_proxy;
		}
		static pagerank_edge::edge_properties generate_edge_properties() {
			pagerank_edge::edge_properties props;
			//props.len = 1 + random() % 10;
			return props;
		}
	private:
		proxy vertex_proxy;
};

bool verify = true;

class Main : public CBase_Main {
	public:
		Main(CkArgMsg *m) : totaltime(0), source_counter(0)  {
			parse_options(m, &opts);
			main_proxy = thishandle;
			CkPrintf("Hello.\n");
			vertex_proxy = CProxy_pagerank_vertex::ckNew(opts.N);
			master_proxy = CProxy_pagerank_master::ckNew(CkCallback(CkIndex_Main::done(), thisProxy));
			vertex_number = opts.N;
			if (opts.graph_type == RMAT)
				graphlib::create_rmat_graph<pagerank_graph>(pagerank_graph(vertex_proxy), opts.rmat_cfg);
			else if (opts.graph_type == Random)
				graphlib::create_random_graph<pagerank_graph>(pagerank_graph(vertex_proxy), opts.random_cfg);
			else
				CkAbort("Unexpected graph type");

			srandom(123);
			CkStartQD(CkCallback(CkIndex_Main::do_pagerank(), thisProxy));
		}
		void do_pagerank() {
			starttime = CkWallTimer();
			master_proxy.do_pagerank(10, 0.01, 0.85);
		}
		void done() {
			CkPrintf("pagerank done.\n");
			totaltime += CkWallTimer() - starttime;
			CkPrintf("[Final] CPU time used = %.6f seconds\n", totaltime / source_counter);
			if (verify) {
				CkPrintf("start verification...\n");
				vertex_proxy.verify();
				//vertex_proxy.print();
				CkStartQD(CkCallback(CkIndex_Main::exit(), thisProxy));
			} else
				CkExit();
		}
		void report_visited(CmiUInt8 tot) {
			CkPrintf("visited vertices = %lld\n", tot);
		}
		void exit() {
			CkPrintf("done.\n");
			CkExit();
		}
	private:
		Options opts;
		double starttime, totaltime;
		int source_counter;
};
#include "pagerank_main.def.h"
