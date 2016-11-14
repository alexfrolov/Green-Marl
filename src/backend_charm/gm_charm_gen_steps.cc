#include "gm_backend_charm.h"
#include "gm_backend_charm_gen_steps.h"

void gm_charm_gen_class::process(ast_procdef* proc) {
	CHARM_BE.generate_class(proc);
}

