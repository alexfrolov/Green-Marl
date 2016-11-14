#include "gm_backend_charm.h"
#include "gm_backend_charm_gen_steps.h"
#include "gm_charm_beinfo.h"
#include "gm_backend_gps_gen_steps.h"
#include "gm_error.h"
#include "gm_frontend.h"

void gm_charm_gen::generate_master() {
	char temp[1024];
  ast_procdef* proc = FE.get_current_proc();
	sprintf(temp, "%s_main_chare", proc->get_procname()->get_genname());

	// generate chare class declaration in .ci file
	begin_chare(temp, true);
	end_chare(temp);

	// generate chare class implementation in .C file
	begin_class(temp);
	end_class(temp);
}
