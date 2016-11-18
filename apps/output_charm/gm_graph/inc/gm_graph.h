#ifndef __gm_graph_h__
#define __gm_graph_h__

namespace graphlib {

struct Options {
	int N;
};

void parse_options(CkArgMsg *m, Options *opts) {
}

template <typename CProxy_T_vertex, typename Options>
void create_graph(CProxy_T_vertex & g, Options & opts) {
}

};


#endif
