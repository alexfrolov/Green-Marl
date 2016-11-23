#include "gm_graph.h"

class avg_teen_cnt_graph;
class avg_teen_cnt_edge;
class avg_teen_cnt_vertex;

#include "avg_teen_cnt_main.decl.h"
#include "common.h"
#include "avg_teen_cnt.C"

CProxy_Main main_proxy;
extern CProxy_avg_teen_cnt_master master_proxy;
extern CProxy_avg_teen_cnt_vertex vertex_proxy;

class avg_teen_cnt_graph {
	public:
		typedef avg_teen_cnt_vertex 				vertex;
		typedef avg_teen_cnt_edge					 	edge;
		typedef CProxy_avg_teen_cnt_vertex 	proxy;
		avg_teen_cnt_graph () {}
		avg_teen_cnt_graph (const proxy & vertex_proxy) :
			vertex_proxy(vertex_proxy) {}
		const proxy & get_proxy() { return vertex_proxy; } 
		void pup(PUP::er &p) {
			p | vertex_proxy;
		}
		static avg_teen_cnt_edge::edge_properties generate_edge_properties() {
			avg_teen_cnt_edge::edge_properties props;
			return props;
		}
	private:
		proxy vertex_proxy;
};

bool verify = true;

class Main : public CBase_Main {
	public:
		Main(CkArgMsg *m) {
			parse_options(m, &opts);
			main_proxy = thishandle;
			CkPrintf("Hello.\n");
			vertex_proxy = CProxy_avg_teen_cnt_vertex::ckNew(opts.N);
			master_proxy = CProxy_avg_teen_cnt_master::ckNew(CkCallback(CkIndex_Main::done(), thisProxy));
			//graphlib::create_rmat_graph<avg_teen_cnt_graph>(avg_teen_cnt_graph(vertex_proxy), opts.rmat_cfg);
			graphlib::create_random_graph<avg_teen_cnt_graph>(avg_teen_cnt_graph(vertex_proxy), opts.random_cfg);
			CkStartQD(CkCallback(CkIndex_Main::do_avg_teen_cnt(), thisProxy));
		}
		void do_avg_teen_cnt() {
			srandom(123);
			root = random() % opts.N;
			master_proxy.do_avg_teen_cnt(root);
		}
		void done() {
			CkPrintf("avg_teen_cnt done.\n");
			if (verify) {
				CkPrintf("start verification...\n");
				vertex_proxy.verify();
				vertex_proxy.print();
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
		CmiUInt8 root;
};
#include "avg_teen_cnt_main.def.h"
