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
	generate_master_entry_method_do_procname_decl();
	generate_master_entry_method_decls();
	end_chare(temp);

	// generate chare class implementation in .C file
	begin_class(temp);
	Body.pushln("private:");
	generate_master_properties();
	Body.pushln("public:");
	generate_master_default_ctor_def(temp);
	generate_master_entry_method_do_procname_def();
	generate_master_entry_methods();
	end_class(temp);
}

void gm_charm_gen::generate_master_messages() {
	Body.pushln("// master messages");
}

void gm_charm_gen::generate_master_properties() {
	char temp[1024];
  ast_procdef* proc = FE.get_current_proc();
	char *name = proc->get_procname()->get_genname();
	Body.pushln("   gm_charmlib_options opts;");
	sprintf(temp, "   CProxy_%s_vertex g", name);
	Body.pushln(temp);
}

void gm_charm_gen::generate_master_default_ctor_decl(char *name) {
	char temp[1024];
	sprintf(temp, "entry %s(CkArgMsg *m);", name); 
	Body_ci.pushln(temp);
}
void gm_charm_gen::generate_master_default_ctor_def(char *name) {
	char temp[1024];
  ast_procdef* proc = FE.get_current_proc();

	sprintf(temp, "%s(CkArgMsg *m) {", name); 
	Body.pushln(temp);

	sprintf(temp, "CkPrintf(\"Running %s: \\n\");", name);
	Body.pushln(temp);
	Body.NL();

	Body.pushln("graphlib::parse_options(m, &opts);");
	Body.NL();

	Body.pushln("// create chare array");
	sprintf(temp, "g = CProxy_%s::ckNew(opts.N);", proc->get_procname()->get_genname());
	Body.pushln(temp);
	Body.NL();

	Body.pushln("// create graph");
	sprintf(temp,"graphlib::create_graph<CProxy_%s_vertex>(g, opts);", proc->get_procname()->get_genname());
	Body.pushln(temp);
	Body.NL();

	// start computation
	sprintf(temp, "CkStartQD(CkIndex_%s_main_chare::do_%s(), &thishandle);", 
			proc->get_procname()->get_genname(),
			proc->get_procname()->get_genname());
	Body.pushln(temp);
	Body.pushln("}");
}

void gm_charm_gen::generate_master_entry_method_do_procname_decl() {
	char temp[1024];
  ast_procdef* proc = FE.get_current_proc();
	char *name = proc->get_procname()->get_genname();
	sprintf(temp, "entry void do_%s();", name); 
	Body_ci.pushln(temp);
}

void gm_charm_gen::generate_master_entry_method_do_procname_def() {
	char temp[1024];
  ast_procdef* proc = FE.get_current_proc();
	char *name = proc->get_procname()->get_genname();
	sprintf(temp, "void do_%s() {", name);
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
