#include "gm_backend_charm.h"
#include "gm_backend_charm_gen_steps.h"
#include "gm_charm_beinfo.h"
#include "gm_backend_gps_gen_steps.h"
#include "gm_error.h"
#include "gm_frontend.h"
#include "gm_backend_gps.h" 

extern void gm_redirect_reproduce(FILE *f);
extern void gm_baseindent_reproduce(int i);
extern void gm_flush_reproduce();

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
	gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();

	std::list<gm_gps_basic_block*>& bb_blocks = info->get_basic_blocks();
	std::list<gm_gps_basic_block*>::iterator I;

	for (I = bb_blocks.begin(); I != bb_blocks.end(); I++) {
		gm_gps_basic_block* b = *I;
		generate_master_entry_method_decl(b, true);
	}

}
void gm_charm_gen::generate_master_entry_method_decl(gm_gps_basic_block *b, bool with_entry) {
  ast_procdef* proc = FE.get_current_proc();
	char *name = proc->get_procname()->get_genname();
	int id = b->get_id();
	int type = b->get_type();
	char temp[1024];
	char *entry_name = get_lib()->generate_master_entry_method_name(b);
	
	if ((type == GM_GPS_BBTYPE_SEQ) && (b->is_after_vertex()))  {
		sprintf(temp, "entry void %s(", entry_name);
		Body_ci.push(temp);
		do_generate_scalar_broadcast_receive(b, Body_ci);
		Body_ci.pushln(");");
	} else {
		sprintf(temp, "entry void %s();", entry_name);
		Body_ci.pushln(temp);
	}
	delete [] entry_name;
}
void gm_charm_gen::generate_master_entry_methods() {
	gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();

	std::list<gm_gps_basic_block*>& bb_blocks = info->get_basic_blocks();
	std::list<gm_gps_basic_block*>::iterator I;

	for (I = bb_blocks.begin(); I != bb_blocks.end(); I++) {
		gm_gps_basic_block* b = *I;
		generate_master_entry_method(b);
	}

}
void gm_charm_gen::generate_master_entry_method(gm_gps_basic_block *b) {
  ast_procdef* proc = FE.get_current_proc();
	char *name = proc->get_procname()->get_genname();
	int id = b->get_id();
	int type = b->get_type();
	char temp[1024];
	char *entry_name = get_lib()->generate_master_entry_method_name(b);

	char cond_var[128];
	sprintf(temp, "void %s(", entry_name);
	Body.push(temp);

	if (type == GM_GPS_BBTYPE_BEGIN_VERTEX) {
		Body.pushln(") {");

		Body.pushln("/*------");
		Body.flush();
		gm_baseindent_reproduce(3);
		b->reproduce_sents(); 
		Body.pushln("-----*/");

		Body.pushln("/*****");
		Body.pushln(" * GM_GPS_BBTYPE_BEGIN_VERTEX");
		Body.pushln(" *****/");

		// generate Broadcast
		do_generate_scalar_broadcast_send(b);

		/*get_lib()->generate_broadcast_state_master("_master_state", Body);
		if (b->find_info_bool(GPS_FLAG_IS_INTRA_MERGED_CONDITIONAL)) {
			int cond_bb_no = b->find_info_int(GPS_INT_INTRA_MERGED_CONDITIONAL_NO);
			sprintf(temp, "%s%d", GPS_INTRA_MERGE_IS_FIRST, cond_bb_no);
			get_lib()->generate_broadcast_isFirst_master(temp, Body);
		}*/
		Body.NL();

		// generate next statement
		
		assert(b->get_num_exits() == 1);
		gm_gps_basic_block* next = b->get_nth_exit(0);
		char *next_entry_name = get_lib()->generate_master_entry_method_name(b->get_nth_exit(0));

		if (next->get_scalar_args_count() > 0 ) {
			Body.pushln("// Wait for contribute");
			sprintf(temp, "// thisProxy.%s(...) will be called by contribute", next_entry_name);
			Body.pushln(temp);

		} else {
			Body.pushln("// Wait for quiescence detection");
			sprintf(temp, "CkStartQD(CkIndex_%s_main_chare::%s(), &thishandle);", name, next_entry_name);
			Body.pushln(temp);
		}

		delete [] next_entry_name;

	} else if (type == GM_GPS_BBTYPE_SEQ) {

		if (b->is_after_vertex()) {
			//Body.push("/*entries-->*/");
			assert(b->get_num_entries() == 1);
			do_generate_scalar_broadcast_receive(b, Body);
			//assert(false);
		}

		Body.pushln(") {");

		Body.pushln("/*------");
		Body.flush();
		gm_baseindent_reproduce(3);
		b->reproduce_sents(); 
		Body.pushln("-----*/");

		Body.pushln("/*****");
		Body.pushln(" * GM_GPS_BBTYPE_SEQ");
		Body.pushln(" *****/");

		// define local variables 
		std::map<gm_symtab_entry*, gps_syminfo*>& symbols = b->get_symbols();
		std::map<gm_symtab_entry*, gps_syminfo*>::iterator I;
		for (I = symbols.begin(); I != symbols.end(); I++) {
			gm_symtab_entry* sym = I->first;
			gps_syminfo* local_info = I->second;
			if (!local_info->is_scalar() || sym->isArgument()) continue; //TODO: why is sym->isArgument() != local_info->is_argument() ?
			gps_syminfo* global_info = (gps_syminfo*) sym->find_info(GPS_TAG_BB_USAGE);

			if (!global_info->is_used_in_multiple_BB()) {
				assert(false); // check it later
				generate_scalar_var_def(sym, true);
			}
		}
		Body.NL();

		int cond_bb_no = b->find_info_int(GPS_INT_INTRA_MERGED_CONDITIONAL_NO);

		// generate sequential sentences
		b->prepare_iter();
		ast_sent* s = b->get_next();
		while (s != NULL) {
			if (s->find_info_bool(GPS_FLAG_IS_INTRA_MERGED_CONDITIONAL)) {
				sprintf(temp, "if (!%s%d) {", GPS_INTRA_MERGE_IS_FIRST, cond_bb_no);
				Body.pushln(temp);
			}

			generate_sent(s);

			if (s->find_info_bool(GPS_FLAG_IS_INTRA_MERGED_CONDITIONAL)) {
				Body.pushln("}");
			}

			s = b->get_next();
		}

		if (b->get_num_exits() == 0) {
			Body.pushln("CkExit();");
		} else {
			char *next_entry_name = get_lib()->generate_master_entry_method_name(b->get_nth_exit(0));
			int n = b->get_nth_exit(0)->get_id();
			assert(n > 1);
			sprintf(temp, "thisProxy.%s();", next_entry_name);
			Body.pushln(temp);
			delete [] next_entry_name;
		}
	} else if (type == GM_GPS_BBTYPE_IF_COND) {

		Body.pushln(") {");
		assert(false);

		Body.pushln("/*------");
		Body.flush();
		gm_baseindent_reproduce(3);
		b->reproduce_sents(); 
		Body.pushln("-----*/");

		Body.pushln("GM_GPS_BBTYPE_IF_COND");
		assert(false);

		Body.push("boolean _expression_result = ");

		// generate sentences
		ast_sent* s = b->get_1st_sent();
		assert(s!= NULL);
		assert(s->get_nodetype() == AST_IF);
		ast_if* i = (ast_if*) s;
		generate_expr(i->get_cond());
		Body.pushln(";");

		sprintf(temp, "if (_expression_result) _master_state_nxt = %d;\nelse _master_state_nxt = %d;", b->get_nth_exit(0)->get_id(),
				b->get_nth_exit(1)->get_id());
		Body.pushln(temp);
	} else if (type == GM_GPS_BBTYPE_WHILE_COND) {

		Body.pushln(") {");

		Body.pushln("/*------");
		Body.flush();
		gm_baseindent_reproduce(3);
		b->reproduce_sents(); 
		Body.pushln("-----*/");

		Body.pushln("/*****");
		Body.pushln(" * GM_GPS_BBTYPE_WHILE_COND");
		Body.pushln(" *****/");

		ast_sent* s = b->get_1st_sent();
		assert(s!= NULL);
		assert(s->get_nodetype() == AST_WHILE);
		ast_while* i = (ast_while*) s;
		if (i->is_do_while())
			Body.pushln("// Do-While(...)");
		else
			Body.pushln("// While (...)");

		Body.NL();
		Body.push("boolean _expression_result = ");
		generate_expr(i->get_cond());
		Body.pushln(";");

		char *next_entry_name_true = get_lib()->generate_master_entry_method_name(b->get_nth_exit(0));
		char *next_entry_name_false = get_lib()->generate_master_entry_method_name(b->get_nth_exit(1));
		sprintf(temp, "if (_expression_result) thisProxy.%s;\nelse thisProxy.%s();\n", 
				next_entry_name_true, // continue while
				next_entry_name_false); // exit
		Body.pushln(temp);

		if (b->find_info_bool(GPS_FLAG_IS_INTRA_MERGED_CONDITIONAL)) {
			sprintf(temp, "if (!_expression_result) %s%d=true; // reset is_first\n\n", GPS_INTRA_MERGE_IS_FIRST, b->get_id());
			Body.pushln(temp);
		}


	} else if ((type == GM_GPS_BBTYPE_PREPARE1) || (type == GM_GPS_BBTYPE_PREPARE2)) {

		assert(false);

		Body.pushln(") {");

		Body.pushln("/*------");
		Body.flush();
		gm_baseindent_reproduce(3);
		b->reproduce_sents(); 
		Body.pushln("-----*/");

		Body.pushln("GM_GPS_BBTYPE_PREPARE1 || GM_GPS_BBTYPE_PREPARE2");
		assert(false);

		// generate Broadcast
		Body.pushln("do_generate_scalar_broadcast_send(b);");
		//get_lib()->generate_broadcast_state_master("_master_state", Body);

		Body.pushln("// Preparation Step;");
		assert(b->get_num_exits() == 1);
		int n = b->get_nth_exit(0)->get_id();
		sprintf(temp, "_master_state_nxt = %d;", n);
		Body.pushln(temp);
		Body.pushln("_master_should_start_workers = true;");
	} else if (type == GM_GPS_BBTYPE_MERGED_TAIL) {

		assert(false);

		Body.pushln(") {");

		Body.pushln("/*------");
		Body.flush();
		gm_baseindent_reproduce(3);
		b->reproduce_sents(); 
		Body.pushln("-----*/");

		Body.pushln("GM_GPS_BBTYPE_MERGED_TAIL");
		assert(false);


		Body.pushln("// Intra-Loop Merged (tail)");
		int source_id = b->find_info_int(GPS_INT_INTRA_MERGED_CONDITIONAL_NO);
		sprintf(temp, "if (%s%d) _master_state_nxt = %d;", GPS_INTRA_MERGE_IS_FIRST, source_id, b->get_nth_exit(0)->get_id());
		Body.pushln(temp);
		sprintf(temp, "else _master_state_nxt = %d;", b->get_nth_exit(1)->get_id());
		Body.pushln(temp);
		sprintf(temp, "%s%d = false;\n", GPS_INTRA_MERGE_IS_FIRST, source_id);
		Body.pushln(temp);
	} else if (type == GM_GPS_BBTYPE_MERGED_IF) {
		assert(false);

		Body.pushln(") {");

		Body.pushln("/*------");
		Body.flush();
		gm_baseindent_reproduce(3);
		b->reproduce_sents(); 
		Body.pushln("-----*/");

		Body.pushln("GM_GPS_BBTYPE_MERGED_IF");
		assert(false);

		Body.pushln("// Intra-Loop Merged (head)");
		int source_id = b->find_info_int(GPS_INT_INTRA_MERGED_CONDITIONAL_NO);
		sprintf(temp, "if (%s%d) _master_state_nxt = %d;", GPS_INTRA_MERGE_IS_FIRST, source_id, b->get_nth_exit(0)->get_id());
		Body.pushln(temp);
		sprintf(temp, "else _master_state_nxt = %d;", b->get_nth_exit(1)->get_id());
		Body.pushln(temp);
		sprintf(temp, "%s%d = false;\n", GPS_INTRA_MERGE_IS_FIRST, source_id);
		Body.pushln(temp);
	} else {
		assert(false);
	}

	Body.pushln("}");

	delete [] entry_name;
	return;

}

void gm_charm_gen::do_generate_scalar_broadcast_send(gm_gps_basic_block *b) {
	char temp[1024];
	int id = b->get_id();
	char *v_ep_name = get_lib()->generate_vertex_entry_method_name(b);
	sprintf(temp, "// call %s in each chare of g", v_ep_name);
	Body.pushln(temp);

	if (b->get_scalar_args_count()) {
		sprintf(temp, "%s_msg *_msg = new %s_msg();", v_ep_name, v_ep_name);
		Body.pushln(temp);
		// check if scalar variable is used inside the block
		std::map<gm_symtab_entry*, gps_syminfo*>& syms = b->get_symbols();
		std::map<gm_symtab_entry*, gps_syminfo*>::iterator I;
		for (I = syms.begin(); I != syms.end(); I++) {
			gps_syminfo* local_info = I->second;
			gps_syminfo* global_info = (gps_syminfo*) I->first->find_info(GPS_TAG_BB_USAGE);
			if (!global_info->is_scalar()) continue;
			if (local_info->is_used_as_reduce()) {
				int reduce_type = local_info->get_reduce_type();

				//printf("being used as reduce :%s\n", I->first->getId()->get_genname());
				//Body.pushln("get_lib()->generate_broadcast_reduce_initialize_master(I->first->getId(), Body, reduce_type, \
				get_reduce_base_value(reduce_type, I->first->getType()->getTypeSummary()));");
				//assert(false);
				// [TODO] global argmax
				continue;
			}
			if (!global_info->is_used_in_master() && !global_info->is_argument()) continue;
			if (local_info->is_used_as_rhs()) {
				// create a broad cast variable
				//get_lib()->generate_broadcast_send_master(I->first->getId(), Body);
				sprintf(temp, "_msg.%s = %s;", I->first->getId()->get_genname(), I->first->getId()->get_genname());
				Body.pushln(temp);
			}
		}
		sprintf(temp, "g.%s(_msg);", v_ep_name);
		Body.pushln(temp);
	} else {
		sprintf(temp, "g.%s();", v_ep_name);
		Body.pushln(temp);
	}



	delete [] v_ep_name;

}

void gm_charm_gen::do_generate_scalar_broadcast_receive(gm_gps_basic_block *b, gm_code_writer & Body) {
	assert(b->get_num_entries() ==1);
	gm_gps_basic_block* pred = b->get_nth_entry(0);
	assert(pred->is_vertex());

	// check if scalar variable is modified inside the block
	std::map<gm_symtab_entry*, gps_syminfo*>& syms = pred->get_symbols();
	std::map<gm_symtab_entry*, gps_syminfo*>::iterator I;
	for (I = syms.begin(); I != syms.end(); I++) {
		gps_syminfo* local_info = I->second;
		gps_syminfo* global_info = (gps_syminfo*) I->first->find_info(GPS_TAG_BB_USAGE);
		if (!global_info->is_scalar()) continue;
		if (!global_info->is_used_in_master()) continue;
		if (local_info->is_used_as_lhs() || local_info->is_used_as_reduce()) {
			gm_symtab_entry *sym = I->first;
			// create a broad cast variable
			//get_lib()->generate_broadcast_receive_master(I->first->getId(), Body, local_info->get_reduce_type());
			assert(sym->getType()->is_primitive() || sym->getType()->is_node_compatible() || sym->getType()->is_collection());
			sprintf(temp, "%s %s", 
					get_type_string(sym->getType(), is_master_generate()),
					sym->getId()->get_genname());
			Body.push(temp);
		}
	}
}
