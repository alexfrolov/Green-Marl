#include "sssp_main.decl.h"
#include "common.h"
#include "gm_graph.h"

CProxy_Main main_proxy;
extern CProxy_sssp_master master_proxy;
extern CProxy_sssp_vertex vertex_proxy;

class Main : public CBase_Main {
	private:
		Options opts;
	public:
		Main(CkArgMsg *m) {
			parse_options(m, &opts);
			main_proxy = thishandle;
			vertex_proxy = CProxy_sssp_vertex::ckNew(opts.N);
			master_proxy = CProxy_sssp_master::ckNew(CkCallback(CkIndex_Main::done(), thisProxy));
			graphlib::create_graph<CProxy_sssp_vertex>(vertex_proxy, opts);
			CkStartQD(CkCallback(CkIndex_Main::do_sssp(), thisProxy));
		}
		void do_sssp() {
			srandom(123);
			CmiUInt8 root = random() % opts.N;
			master_proxy.do_sssp(root);
		}
		void done() {
			CkPrintf("Done.\n");
			CkExit();
		}
};
#include "sssp_main.def.h"
