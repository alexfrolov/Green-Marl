#include "gm_backend_charm.h"
#include "gm_backend_charm_gen_steps.h"
#include "gm_charm_beinfo.h"
#include "gm_backend_gps_gen_steps.h"
#include "gm_error.h"
#include "gm_frontend.h"
#include "gm_gps_basicblock.h"
#include "gm_code_writer.h"
#include "gm_backend_gps.h"

extern void gm_redirect_reproduce(FILE *f);
extern void gm_baseindent_reproduce(int i);
extern void gm_flush_reproduce();

static bool is_symbol_defined_in_bb(gm_gps_basic_block* b, gm_symtab_entry *e) {
	std::map<gm_symtab_entry*, gps_syminfo*>& SYMS = b->get_symbols();
	if (SYMS.find(e) == SYMS.end())
		return false;
	else
		return true;
}

void gm_charm_gen::generate_vertex_entry_method_decls_aux() {
	char temp[1024];
  ast_procdef* proc = FE.get_current_proc();
	sprintf(temp, "entry void add_edge(const %s_edge &e);", 
			proc->get_procname()->get_genname());
	Body_ci.pushln(temp);
}

void gm_charm_gen::generate_vertex_entry_method_decls() {
	gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();
	std::list<gm_gps_basic_block*>& bb_blocks = info->get_basic_blocks();
	std::list<gm_gps_basic_block*>::iterator I;
	gm_redirect_reproduce(f_body);
	for (I = bb_blocks.begin(); I != bb_blocks.end(); I++) {
		gm_gps_basic_block* b = *I;
		if ((!b->is_prepare()) && (!b->is_vertex())) continue;
		generate_vertex_entry_method_decl(b, true);
	}
}

void gm_charm_gen::generate_vertex_entry_method_decl(gm_gps_basic_block* b, bool with_entry) {
	int id = b->get_id();
	int type = b->get_type();
	char temp[1024];
	char *entry_name = get_lib()->generate_vertex_entry_method_name(b);

	if (b->has_receiver()) {
		sprintf(temp, "entry void %s_recv(%s_recv_msg *msg);", entry_name, entry_name);
		Body_ci.pushln(temp);
	}

	if (b->get_scalar_rhs_count() > 0) {
		sprintf(temp, "entry void %s(%s_msg *msg);", entry_name, entry_name);
		Body_ci.pushln(temp);
	} else {
		sprintf(temp, "entry void %s();", entry_name);
		Body_ci.pushln(temp);
	}

	delete [] entry_name;
}

void gm_charm_gen::generate_edge() {
	char temp[1024];
  ast_procdef* proc = FE.get_current_proc();
	generate_edge_properties_type();
}

void gm_charm_gen::generate_vertex() {
	char name[1024];
	char temp[1024];
  ast_procdef* proc = FE.get_current_proc();
	sprintf(name, "%s_vertex", proc->get_procname()->get_genname());

	// generate chare class declaration in .ci file
	begin_chare_array(name, 1);
	generate_vertex_default_ctor_decl(name);
	generate_vertex_entry_method_decls();
	generate_vertex_entry_method_decls_aux();

	Body_ci.pushln("#ifdef HOOK_ENABLE");
	sprintf(temp, "   #include \"%s_hook.ci\"", proc->get_procname()->get_genname());
	Body_ci.pushln(temp);
	Body_ci.pushln("#endif");
	end_chare_array(name);

	// generate chare class implementation in .C file
	begin_class(name);
	Body.pushln("public:");
	Body.push_indent();
	generate_vertex_properties_type();

	Body.pop_indent();
	Body.pushln("public:");
	Body.push_indent();
	generate_vertex_default_ctor_def(name);
	generate_vertex_entry_methods_aux();
	generate_vertex_entry_methods();
	Body.pop_indent();

	Body.pushln("private:");
	Body.push_indent();
	generate_vertex_all_properties();
	Body.pop_indent();

	// add user hook
	Body.pushln("#ifdef HOOK_ENABLE");
	sprintf(temp, "   #include \"%s_hook.C\"", proc->get_procname()->get_genname());
	Body.pushln(temp);
	Body.pushln("#endif");

	end_class(name);
}

void gm_charm_gen::generate_vertex_all_properties() {
	char temp[1024];
  ast_procdef* proc = FE.get_current_proc();
	sprintf(temp, "std::vector<struct %s_edge> edges;", proc->get_procname()->get_genname());
	Body.pushln(temp);
	Body.pushln("struct vertex_properties props;");
}

void gm_charm_gen::generate_vertex_default_ctor_decl(char *name) {
	char temp[1024];
	sprintf(temp, "entry %s();", name); 
	Body_ci.pushln(temp);
}

void gm_charm_gen::generate_vertex_default_ctor_def(char *name) {
	char temp[1024];
	sprintf(temp, "%s() {}", name); 
	Body.pushln(temp);
	sprintf(temp, "%s(CkMigrateMessage* m) {}", name); 
	Body.pushln(temp);
}

void gm_charm_gen::generate_vertex_messages() {
	Body.pushln("// vertex messages");
	gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();
	std::list<gm_gps_basic_block*>& bb_blocks = info->get_basic_blocks();
	std::list<gm_gps_basic_block*>::iterator I;
	gm_redirect_reproduce(f_body);
	for (I = bb_blocks.begin(); I != bb_blocks.end(); I++) {
		gm_gps_basic_block* b = *I;
		if ((!b->is_prepare()) && (!b->is_vertex())) continue;
		generate_vertex_message_decl_ci(b);
		generate_vertex_message_def(b);
	}
}

void gm_charm_gen::generate_vertex_message_def(gm_gps_basic_block *b) {
	char *entry_name = get_lib()->generate_vertex_entry_method_name(b);
	//---------------------------------------------------
	// generate struct for *_recv method
	//---------------------------------------------------
	if (b->has_receiver()) {
		sprintf(temp, "struct %s_recv_msg : public CMessage_%s_recv_msg {", 
				entry_name, entry_name);
		Body.pushln(temp);
		generate_vertex_entry_method_args_recv(b, false);
		Body.pushln("};");
	}

	//---------------------------------------------------
	// generate struct for *_recv method
	//---------------------------------------------------

	if (b->get_scalar_rhs_count() > 0) {
		sprintf(temp, "struct %s_msg : public CMessage_%s_msg {", 
				entry_name, entry_name);
		Body.pushln(temp);
		generate_vertex_entry_method_args_scala(b, false);
		Body.pushln("};");
	}
	delete [] entry_name;
}

void gm_charm_gen::generate_vertex_message_decl_ci(gm_gps_basic_block *b) {
	char *entry_name = get_lib()->generate_vertex_entry_method_name(b);
	//---------------------------------------------------
	// generate struct for *_recv method
	//---------------------------------------------------
	if (b->has_receiver()) {
		sprintf(temp, "message %s_recv_msg;", entry_name);
		Body_ci.pushln(temp);
	}

	//---------------------------------------------------
	// generate struct for *_recv method
	//---------------------------------------------------
	if (b->get_scalar_rhs_count() > 0) {
		sprintf(temp, "message %s_msg;", entry_name);
		Body_ci.pushln(temp);
	}
	delete [] entry_name;
}

void gm_charm_gen::generate_vertex_entry_methods_aux() {
	char temp[1024];
  ast_procdef* proc = FE.get_current_proc();
	sprintf(temp, "void add_edge(const %s_edge &e) { edges.push_back(e); } ", 
			proc->get_procname()->get_genname());
	Body.pushln(temp);
	sprintf(temp, "int getNeighborsSize() { return edges.size(); } ");
	Body.pushln(temp);
}

void gm_charm_gen::generate_vertex_entry_methods() {
	gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();
	std::list<gm_gps_basic_block*>& bb_blocks = info->get_basic_blocks();
	std::list<gm_gps_basic_block*>::iterator I;
	gm_redirect_reproduce(f_body);
	for (I = bb_blocks.begin(); I != bb_blocks.end(); I++) {
		gm_gps_basic_block* b = *I;
		if ((!b->is_prepare()) && (!b->is_vertex())) continue;
		generate_vertex_entry_method(b);
	}
	gm_redirect_reproduce(stdout);
}

void gm_charm_gen::generate_vertex_entry_method(gm_gps_basic_block *b) {
	int id = b->get_id();
	int type = b->get_type();
	char temp[1024];
	char *entry_name = get_lib()->generate_vertex_entry_method_name(b);

	//---------------------------------------------------
	// Generate recieve entry method if needed
	//---------------------------------------------------
	if (b->has_receiver()) {

		sprintf(temp, "void %s_recv (%s_recv_msg *msg) {", entry_name, entry_name);
		Body.pushln(temp);
		// load entry method parameters
		generate_vertex_entry_method_args_recv(b, true);
		Body.pushln("delete msg;");

		// add stack variables (only for inner loops)
		generate_vertex_scalar_with_scope(b, GPS_SCOPE_INNER);

		set_receiver_generate(true); 

		std::list<gm_gps_comm_unit>& R = b->get_receivers();
		std::list<gm_gps_comm_unit>::iterator I;
		for (I = R.begin(); I != R.end(); I++) {
			gm_gps_comm_unit U = *I;
			if (U.get_type() == GPS_COMM_NESTED) {
				ast_foreach* fe = U.fe;
				assert(fe!=NULL);

				Body.pushln("/*------");
				Body.pushln("(Nested Loop)");
				Body.flush();

				//if (is_conditional)
				//	gm_baseindent_reproduce(5);
				//else
					gm_baseindent_reproduce(3);

					fe->reproduce(0);
					gm_flush_reproduce();
					Body.pushln("-----*/");

					//get_lib()->generate_message_receive_begin(fe, Body, b, R.size() == 1);

					if (fe->get_body()->get_nodetype() == AST_SENTBLOCK) {
						generate_sent_block((ast_sentblock*) fe->get_body(), false);
					} else {
						generate_sent(fe->get_body());
					}

					//get_lib()->generate_message_receive_end(Body, R.size() == 1);

			} else {
				ast_sentblock* sb = U.sb;
				assert(sb!=NULL); 
				Body.pushln("/*------");
				Body.pushln("(Random Write)");
				Body.pushln("{");
				Body.flush();
				//if (is_conditional)
				//	gm_baseindent_reproduce(6);
				//else
					gm_baseindent_reproduce(5);
				std::list<ast_sent*>& sents = sb->get_sents();
				std::list<ast_sent*>::iterator I;
				for (I = sents.begin(); I != sents.end(); I++) {
					ast_sent* s = *I;
					if (s->find_info_ptr(GPS_FLAG_SENT_BLOCK_FOR_RANDOM_WRITE_ASSIGN) == sb) s->reproduce(0);
				}
				gm_flush_reproduce();
				Body.pushln("}");
				Body.pushln("-----*/");
				assert(false);
			}
		}

		set_receiver_generate(false); 

		Body.pushln("}");
	}

	//---------------------------------------------------
	// Generate main entry method 
	//---------------------------------------------------

	if (b->get_scalar_rhs_count() > 0) {
		sprintf(temp, "void %s (%s_msg *msg) {", entry_name, entry_name);
		Body.pushln(temp);
		// load entry method parameters
		generate_vertex_entry_method_args_scala(b, true);
		Body.pushln("delete msg;");
	}
	else {
		sprintf(temp, "void %s () {", entry_name);
		Body.pushln(temp);
		//generate_vertex_entry_method_args_scala(b, true);
	}

	// Generate stack variables
	if (b->has_receiver())
		generate_vertex_entry_method_args_recv(b, false);

	// add a local copy of scalar variables stack (will be used as a source for reduction)
	generate_vertex_scalar_with_scope(b, GPS_SCOPE_GLOBAL);
	generate_vertex_scalar_with_scope(b, GPS_SCOPE_OUTER);

	assert(type == GM_GPS_BBTYPE_BEGIN_VERTEX);
	bool is_conditional = b->find_info_bool(GPS_FLAG_IS_INTRA_MERGED_CONDITIONAL);
	char cond_var[128];
	if (is_conditional) sprintf(cond_var, "%s%d", GPS_INTRA_MERGE_IS_FIRST, b->find_info_int(GPS_INT_INTRA_MERGED_CONDITIONAL_NO));

	if (b->get_num_sents() > 0) {
		//assert (b->get_num_sents() == 1);
		gm_baseindent_reproduce(3);
		Body.pushln("/*------");
		Body.flush();
		b->reproduce_sents(false);
		Body.pushln("-----*/");
		Body.NL();

		std::list<ast_sent*>& sents = b->get_sents();
		std::list<ast_sent*>::iterator I;
		int cnt = 0;
		for (I = sents.begin(); I != sents.end(); I++) {
			ast_sent* s = *I;
			assert(s->get_nodetype() == AST_FOREACH);
			ast_foreach* fe = (ast_foreach*) s;
			ast_sent* body = fe->get_body();
			if (cnt != 0) Body.NL();
			cnt++;
			if (fe->find_info_bool(GPS_FLAG_IS_INTRA_MERGED_CONDITIONAL)) {
				sprintf(temp, "/*if (!%s)", cond_var); //FIXME
				Body.push(temp);
				if (body->get_nodetype() != AST_SENTBLOCK)
					Body.pushln(" {*/");
				else {
					Body.pushln("*/");
					//Body.NL();
				}
			}

			generate_sent(body);

			if (fe->find_info_bool(GPS_FLAG_IS_INTRA_MERGED_CONDITIONAL)) {
				if (body->get_nodetype() != AST_SENTBLOCK) Body.pushln("/*}*/");
			}

		}
	}

	Body.pushln("}");
	delete [] entry_name;
}

void gm_charm_gen::generate_vertex_scalar_with_scope(gm_gps_basic_block *b, int scope) {
	char temp[1024];

	bool with_comm_sym = false;

	// load scalar variable
	std::map<gm_symtab_entry*, gps_syminfo*>& symbols = b->get_symbols();
	std::map<gm_symtab_entry*, gps_syminfo*>::iterator I;

	for (I = symbols.begin(); I != symbols.end(); I++) {
		gm_symtab_entry *sym = I->first;
		gps_syminfo* local_info = I->second;
		if (!local_info->is_scalar()) continue;

		gps_syminfo* global_info = (gps_syminfo*) sym->find_info(GPS_TAG_BB_USAGE);
		assert(global_info!=NULL);

		if (sym->getType()->is_node_iterator()) {
			// do nothing
		} else if (global_info->get_scope() == scope && (with_comm_sym || !find_communication_symbol_info(b, sym))) {
			if (local_info->is_used_as_lhs() || local_info->is_used_as_reduce()) {
				generate_scalar_var_def(sym, false);
				Body.pushln(";");
			}
		} 
	}

	Body.NL();
}

void gm_charm_gen::generate_vertex_entry_method_args_scala(gm_gps_basic_block *b, bool with_assign) {
	char temp[1024];
	// load scalar variable
	std::map<gm_symtab_entry*, gps_syminfo*>& symbols = b->get_symbols();
	std::map<gm_symtab_entry*, gps_syminfo*>::iterator I;
	//Body.push("\n/*scalars:*/ ");
	for (I = symbols.begin(); I != symbols.end(); I++) {
		gm_symtab_entry* sym = I->first;
		gps_syminfo* local_info = I->second;

		if (!local_info->is_scalar()) continue;

		gps_syminfo* global_info = (gps_syminfo*) sym->find_info(GPS_TAG_BB_USAGE);
		assert(global_info!=NULL);

		if (sym->getType()->is_node_iterator()) {
			// do nothing
		} else if (global_info->is_scoped_global()) {
			if (local_info->is_used_as_rhs()) {
				generate_scalar_var_def(sym, false);
				if (with_assign) {
					Body.push(" = ");
					get_lib()->generate_broadcast_receive_vertex(sym->getId(), Body);
				}
				Body.pushln(";");
			} else {
				//sprintf(temp, "omitting  %s\n", sym->getId()->get_genname());
				//Body.push(temp);
			}
		} 
	}
}

void gm_charm_gen::generate_vertex_entry_method_args_recv(gm_gps_basic_block *b, bool with_assign) {
	assert(b->has_receiver());
	int cnt = 0;
	// generate parameters from communicating symbols
	gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();

	std::list<gm_gps_comm_unit>& R = b->get_receivers();
	std::list<gm_gps_comm_unit>::iterator I;
	for (I = R.begin(); I != R.end(); I++) {
		gm_gps_comm_unit U = *I;
		std::list<gm_gps_communication_symbol_info>& LIST = info->get_all_communication_symbols(U);

		if (U.get_type() == GPS_COMM_NESTED) {
			ast_foreach* fe = U.fe;
			assert(fe!=NULL);

			std::list<gm_gps_communication_symbol_info>::iterator J;
			for (J = LIST.begin(); J != LIST.end(); J++) {
				gm_gps_communication_symbol_info& SYM = *J;
				gm_symtab_entry * e = SYM.symbol;

				// check it once again later
				//if (e->getType()->is_property() || e->getType()->is_node_compatible() || e->getType()->is_edge_compatible() || !is_symbol_defined_in_bb(b, e)) {
				//const char* str = get_type_string(SYM.gm_type);
				//Body.push(str);
				//Body.SPC();
				//}
				if (e->getType()->is_property()) {
					assert(false);
					//Body.pushln("generate_vertex_prop_access_remote_lhs(e->getId(), Body);");
				} /*else {
					Body.push(e->getId()->get_genname());
				}*/

				generate_scalar_var_def(e, false);
				if (with_assign) {
					Body.push(" = ");
					get_lib()->generate_broadcast_receive_vertex(e->getId(), Body);
				}
				Body.pushln(";");
			}

		} else {
			assert(false);
		}
	}
}

gm_gps_communication_symbol_info *gm_charm_gen::find_communication_symbol_info(gm_gps_basic_block *b, gm_symtab_entry *e) {
	if (!b->has_receiver())
		return NULL;

	int cnt = 0;
	gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();
	std::list<gm_gps_comm_unit>& R = b->get_receivers();
	std::list<gm_gps_comm_unit>::iterator I;
	for (I = R.begin(); I != R.end(); I++) {
		gm_gps_comm_unit U = *I;
		if (gm_gps_communication_symbol_info *S = info->find_communication_symbol_info(U, e))
			return S;
	}
	return NULL;
}

void gm_charm_gen::generate_edge_properties_type() {
	char temp[1024];
  ast_procdef* proc = FE.get_current_proc();
	sprintf(temp, "%s_edge", proc->get_procname()->get_genname());
	begin_struct(temp);
	Body.pushln("uint64_t v;");
	begin_struct("edge_properties");

	gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();
  std::set<gm_symtab_entry*>& prop = info->get_edge_prop_symbols();
	std::set<gm_symtab_entry*>::iterator I;
	for (I = prop.begin(); I != prop.end(); I++) {
			gm_symtab_entry* sym = *I;
			sprintf(temp, "%s %s;", 
					get_type_string(sym->getType()->get_target_type()->get_typeid() /*, is_master_generate()*/), 
					sym->getId()->get_genname());
			Body.pushln(temp);
	}
	Body.pushln("void pup(PUP::er & p) {");
	for (I = prop.begin(); I != prop.end(); I++) {
			gm_symtab_entry* sym = *I;
			sprintf(temp, "p | %s;", sym->getId()->get_genname());
			Body.pushln(temp);
	}
	Body.pushln("}");
	end_struct("edge_properties");
	Body.pushln("struct edge_properties props;");
	// ctors
	sprintf(temp, "%s_edge () {} ", proc->get_procname()->get_genname());
	Body.pushln(temp);
	sprintf(temp, "%s_edge (uint64_t v) : v(v) {} ", proc->get_procname()->get_genname());
	Body.pushln(temp);
	sprintf(temp, "%s_edge (uint64_t v, const struct edge_properties &props) : \
			v(v), props(props) {} ", proc->get_procname()->get_genname());
	Body.pushln(temp);

	Body.pushln("void pup(PUP::er & p) { p | v; p | props; }");
	end_struct("edge");
}

void gm_charm_gen::generate_vertex_properties_type() {
	char temp[1024];
	begin_struct("vertex_properties");
	gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();
  std::set<gm_symtab_entry*>& prop = info->get_node_prop_symbols();
	std::set<gm_symtab_entry*>::iterator I;
	for (I = prop.begin(); I != prop.end(); I++) {
			gm_symtab_entry* sym = *I;
			sprintf(temp, "%s %s;", 
					get_type_string(sym->getType()->get_target_type()->get_typeid() /*, is_master_generate()*/), 
					sym->getId()->get_genname());
			Body.pushln(temp);
	}
	end_struct("vertex_properties");
}

void gm_charm_gen::generate_scalar_var_def(gm_symtab_entry* sym, bool finish_sent) {
    if (sym->find_info_bool(GPS_FLAG_EDGE_DEFINED_INNER)) return; // skip edge iteration

    assert(sym->getType()->is_primitive() || sym->getType()->is_node_compatible() || sym->getType()->is_collection());

    char temp[1024];
    sprintf(temp, "%s %s", get_type_string(sym->getType(), is_master_generate()), sym->getId()->get_genname());
    Body.push(temp);

    if (finish_sent) Body.pushln(";");
}

