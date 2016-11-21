#ifndef __random_generator_h__
#define __random_generator_h__

namespace graphlib {

struct random_config {
	int scale;
	int K;
	void pup(PUP::er &p) {
		p | scale;
		p | K;
	}
};

template <typename Graph>
class random_generator : public CBase_random_generator <Graph> {
	public:
		random_generator() {}
		random_generator(Graph & g, struct random_config & cfg, bool strongscale) : 
			g(g), cfg(cfg), strongscale(strongscale) {}
		void generate();
		void addEdge(const std::pair<uint64_t, uint64_t> & e) {
			//CkPrintf("adding edge (%lld, %lld)\n", e.first, e.second);
			g.get_proxy()[e.first].add_edge(typename Graph::edge(e.second, 
						Graph::generate_edge_properties()));
		}
	private:
		Graph g;
		struct random_config cfg;
		bool strongscale;
};

template <typename Graph>
void random_generator<Graph>::generate() {
	CkPrintf("%d: generate\n", CmiMyPe());

	uint64_t N, M;
	if (strongscale)
		N = (1 << cfg.scale);
	else
		N = (1 << cfg.scale) * CkNumPes();
	M = N * cfg.K;

	uint64_t e_start = CkMyPe() * M / CkNumPes(); 
	//world.rank() * (m + world.size() - 1) / world.size(); // FIXME???
	uint64_t e_count = M / CkNumPes(); 
	//(std::min)((m + world.size() - 1) / world.size(), m - e_start);

	srandom(12345);
	for (uint64_t i = 0; i < e_start; i++)
		random();

	for (uint64_t i = 0; i < e_count; i++)
		addEdge(std::make_pair(random() % N, random() % N));

}

template <typename Graph>
void create_random_graph(const Graph & g, struct random_config & cfg) {
	typedef CProxy_random_generator<Graph> CProxy_Generator;
 	CProxy_Generator generator = CProxy_Generator::ckNew(g, cfg, true);
	generator.generate();
}

};


#endif
