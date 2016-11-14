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

void gm_charm_gen::generate_vertex() {
	char temp[1024];
  ast_procdef* proc = FE.get_current_proc();
	sprintf(temp, "%s_vertex", proc->get_procname()->get_genname());

	// generate chare class declaration in .ci file
	begin_chare(temp, false);
	end_chare(temp);

	// generate chare class implementation in .C file
	begin_class(temp);
	Body.pushln("private:");
	generate_edge_list(proc);
	generate_vertex_properties(proc);
	Body.pushln("public:");
	generate_default_ctor(temp);
	generate_vertex_entry_methods();
	end_class(temp);
}
void gm_charm_gen::generate_default_ctor(char *name) {
	char temp[1024];
	sprintf(temp, "%s() {}", name); 
	Body.pushln(temp);
}

void gm_charm_gen::generate_vertex_entry_methods() {
	char temp[1024];

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
	sprintf(temp, "// basic block id%d ", b->get_id()); 
	sprintf(temp, "void entry_method_bb%d (", b->get_id());
	Body.push(temp);
	// generate entry method parameters
	generate_vertex_entry_method_args(b);
	Body.pushln(") {");
	// generate entry method body

	assert(type == GM_GPS_BBTYPE_BEGIN_VERTEX);
	bool is_conditional = b->find_info_bool(GPS_FLAG_IS_INTRA_MERGED_CONDITIONAL);
	char cond_var[128];
	if (is_conditional) sprintf(cond_var, "%s%d", GPS_INTRA_MERGE_IS_FIRST, b->find_info_int(GPS_INT_INTRA_MERGED_CONDITIONAL_NO));

	if (b->has_receiver()) {
		Body.pushln("// receiver block");
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
					gm_baseindent_reproduce(2);

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
	}

	if (b->get_num_sents() > 0) {
		Body.pushln("// main block");
		//assert (b->get_num_sents() == 1);
		gm_baseindent_reproduce(2);
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
}

void gm_charm_gen::generate_vertex_entry_method_args(gm_gps_basic_block *b) {
	int cnt = 0;
	// load scalar variable
	std::map<gm_symtab_entry*, gps_syminfo*>& symbols = b->get_symbols();
	std::map<gm_symtab_entry*, gps_syminfo*>::iterator I;
	Body.push("\n/*scalars:*/ ");
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
				if (cnt == 1) 
					Body.pushln(",");
				generate_scalar_var_def(sym, false);
				cnt++;
			} else {
				//printf("omitting  %s\n", sym->getId()->get_genname());
			}
		} 
	}

	if (b->find_info_bool(GPS_FLAG_IS_INTRA_MERGED_CONDITIONAL)) {
		char temp[1024];
		sprintf(temp, "%s%d", GPS_INTRA_MERGE_IS_FIRST, b->find_info_int(GPS_INT_INTRA_MERGED_CONDITIONAL_NO));
		printf("get_lib()->generate_receive_isFirst_vertex(temp, Body);\n");
		//assert(false);
	}

	// generate parameters from communicating symbols
	if (b->has_receiver()) {
		Body.push("\n/*receivers:*/ ");
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

					if (cnt > 1) 
						Body.push(",");

					// check it once again later
					//if (e->getType()->is_property() || e->getType()->is_node_compatible() || e->getType()->is_edge_compatible() || !is_symbol_defined_in_bb(b, e)) {
						const char* str = get_type_string(SYM.gm_type);
						Body.push(str);
						Body.SPC();
					//}
					if (e->getType()->is_property()) {
						assert(false);
						//Body.pushln("generate_vertex_prop_access_remote_lhs(e->getId(), Body);");
					} else {
						Body.push(e->getId()->get_genname());
						cnt++;
					}
				}

			} else {
				assert(false);
			}
		}
	}

}

void gm_charm_gen::generate_edge_list(ast_procdef *proc) {
	char temp[1024];
	Body.pushln("// edge list");
	begin_struct("edge");
	Body.pushln("uint64_t v;");
	Body.pushln("// edge properties");
	begin_struct("edge_properties");
	gm_charm_beinfo * info = (gm_charm_beinfo *) FE.get_backend_info(proc);
  std::set<gm_symtab_entry*>& prop = info->get_edge_prop_symbols();
	std::set<gm_symtab_entry*>::iterator I;
	for (I = prop.begin(); I != prop.end(); I++) {
			gm_symtab_entry* sym = *I;
			sprintf(temp, "%s %s;", 
					get_type_string(sym->getType()->get_target_type()->get_typeid() /*, is_master_generate()*/), 
					sym->getId()->get_genname());
			Body.pushln(temp);
	}
	end_struct("edge_properties");
	Body.pushln("struct edge_properties props;");
	end_struct("edge");
	Body.pushln("std::list<struct edge> edges;");
}

void gm_charm_gen::generate_vertex_properties(ast_procdef *proc) {
	char temp[1024];
	Body.pushln("// vertex properties");
	begin_struct("vertex_properties");
	gm_charm_beinfo * info = (gm_charm_beinfo *) FE.get_backend_info(proc);
  std::set<gm_symtab_entry*>& prop = info->get_node_prop_symbols();
	std::set<gm_symtab_entry*>::iterator I;
	for (I = prop.begin(); I != prop.end(); I++) {
			gm_symtab_entry* sym = *I;
			sprintf(temp, "%s %s;", 
					get_type_string(sym->getType()->get_target_type()->get_typeid() /*, is_master_generate()*/), 
					sym->getId()->get_genname());
			Body.pushln(temp);
	}
	end_class("vertex_properties");
	Body.pushln("struct vertex_properties props;");
}

void gm_charm_gen::generate_scalar_var_def(gm_symtab_entry* sym, bool finish_sent) {
    if (sym->find_info_bool(GPS_FLAG_EDGE_DEFINED_INNER)) return; // skip edge iteration

    assert(sym->getType()->is_primitive() || sym->getType()->is_node_compatible() || sym->getType()->is_collection());

    char temp[1024];
    sprintf(temp, "%s %s", get_type_string(sym->getType(), is_master_generate()), sym->getId()->get_genname());
    Body.push(temp);

    if (finish_sent) Body.pushln(";");
}

