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
	generate_master_default_ctor_decl(temp);
	generate_master_entry_method_decls();
	end_chare(temp);

	// generate chare class implementation in .C file
	begin_class(temp);
	Body.pushln("public:");
	generate_master_default_ctor_def(temp);
	generate_master_entry_methods();
	end_class(temp);
}

void gm_charm_gen::generate_master_messages() {
	Body.pushln("// master messages");
}
void gm_charm_gen::generate_master_default_ctor_decl(char *name) {
	char temp[1024];
	sprintf(temp, "entry %s(CkArgMsg *m);", name); 
	Body_ci.pushln(temp);
}
void gm_charm_gen::generate_master_default_ctor_def(char *name) {
	char temp[1024];
	sprintf(temp, "entry %s(CkArgMsg *m) {", name); 
	Body.pushln(temp);

	// start computation
  ast_procdef* proc = FE.get_current_proc();
	sprintf(temp, "do_%s();", proc->get_procname()->get_genname());
	Body.pushln(temp);
	Body.pushln("}");
}

void gm_charm_gen::generate_master_entry_method_decls() {
	Body.pushln("// entry method decls..");
}
void gm_charm_gen::generate_master_entry_method_decl(gm_gps_basic_block *b, bool with_entry) {
}
void gm_charm_gen::generate_master_entry_methods() {
	Body.pushln("// entry method defs..");
}
void gm_charm_gen::generate_master_entry_method(gm_gps_basic_block *b) {
}
