#include "gm_backend_charm.h"
#include "gm_backend_gps.h"
#include "gm_frontend.h"
#include "gps_comminfo.h"
#include "gm_builtin.h"
#include "gm_defs.h"

char *create_reduce_op_string(int reduce_op_type, ast_typedecl *type) {
	char temp [1024];
	switch(reduce_op_type) {
		case GMREDUCE_INVALID:
			assert(false);
			break;
		case GMREDUCE_PLUS:
			sprintf(temp, "CkReduction::sum_%s", CHARM_BE.get_type_string(type->get_typeid()));
			break;
		case GMREDUCE_MULT:
			assert(false);
			break;
		case GMREDUCE_MIN:
			assert(false);
			break;
		case GMREDUCE_MAX:
			assert(false);
			break;
		case GMREDUCE_AND:
			assert(false);
			break;
		case GMREDUCE_OR:
			sprintf(temp, "CkReduction::logical_or");
			break;
		case GMREDUCE_AVG:
			assert(false);
			break;
		case GMREDUCE_DEFER:
			assert(false);
			break;
		case GMREDUCE_NULL:
			assert(false);
			break;
		default:
			assert(false);
	}
	return gm_strdup(temp);
}

const char* create_key_string(ast_id* id) {
	//sprintf(str_buf, "KEY_%s", id->get_genname());
	//return str_buf;
	return id->get_genname();
}


void gm_charm_lib::generate_broadcast_send_master(ast_id* id, gm_code_writer& Body) {

	/*char temp[1024];
	sprintf(temp, "setAggregatedValue(%s, new ", create_key_string(id));
	Body.push(temp);
	generate_broadcast_writable_type(id->getTypeSummary(), Body);

	Body.push("(");
	get_main()->generate_rhs_id(id);
	Body.pushln("));");*/
	assert(false);

}

void gm_charm_lib::generate_broadcast_receive_master(ast_id* id, gm_code_writer& Body, int reduce_op) {
	get_main()->generate_lhs_id(id);
}

void gm_charm_lib::generate_reduce_assign_vertex(ast_assign* a, gm_code_writer& Body, int reduce_op_type) {
	char temp [1024];
	gm_gps_basic_block *b = (gm_gps_basic_block *)a->get_basic_block();
  ast_procdef* proc = FE.get_current_proc();

	assert(a->is_target_scalar());

	//assert(reduce_op_type == GMREDUCE_NULL);
	ast_id* id = a->get_lhs_scala();
	Body.push(id->get_genname());
	Body.push(" = ");
	get_main()->generate_expr(a->get_rhs());
	Body.pushln(";");

	if (!get_main()->is_master_generate()) {
		assert(b->get_num_exits() == 1);

		char *reduce_op_str = create_reduce_op_string(reduce_op_type, id->getTypeInfo());
		char *entry_method = generate_master_entry_method_name(b->get_nth_exit(0));

		sprintf(temp, "contribute(sizeof(%s), &%s, %s, \n\
			CkCallback(CkReductionTarget(%s_master, __reduction_%s), master_proxy));", 
				get_main()->get_type_string(id->getTypeInfo()->get_typeid()), id->get_genname(),
				reduce_op_str, proc->get_procname()->get_genname(), id->get_genname());
		Body.pushln(temp);
		delete [] reduce_op_str;
		delete [] entry_method;
	}
}

void gm_charm_lib::generate_vertex_prop_access_lhs(ast_id *id, gm_code_writer& Body) {
	char temp[1024];
	sprintf(temp, "%s.%s", "this->props", id->get_genname());
	Body.push(temp);
}
void gm_charm_lib::generate_vertex_prop_access_lhs_edge(ast_id *id, gm_code_writer& Body) {
	char temp[1024];
	sprintf(temp, "_e->props.%s", id->get_genname());
	Body.push(temp);
}
void gm_charm_lib::generate_vertex_prop_access_rhs(ast_id *id, gm_code_writer& Body) {
assert(false);
}
void gm_charm_lib::generate_vertex_prop_access_rhs_edge(ast_id *id, gm_code_writer& Body) {
assert(false);
}
void gm_charm_lib::generate_node_iterator_rhs(ast_id* id, gm_code_writer& Body) {
	if (id->getSymInfo()->find_info_bool(GPS_FLAG_IS_OUTER_LOOP) || id->getSymInfo()->find_info_bool(GPS_FLAG_IS_INNER_LOOP)) {
		Body.push("thisIndex");
	}
	else {
		Body.push(id->get_genname());
	}
}
int gm_charm_lib::get_type_size(ast_typedecl* t) {
assert(false);
}
int gm_charm_lib::get_type_size(int gm_type) {
assert(false);
}

void gm_charm_lib::generate_expr_nil(ast_expr* e, gm_code_writer& Body) {
    Body.push("(-1)");
}

static bool is_symbol_defined_in_bb(gm_gps_basic_block* b, gm_symtab_entry *e) {
	std::map<gm_symtab_entry*, gps_syminfo*>& SYMS = b->get_symbols();
	if (SYMS.find(e) == SYMS.end())
		return false;
	else
		return true;
}

void gm_charm_lib::generate_message_receive_begin(ast_foreach* fe, gm_code_writer& Body, gm_gps_basic_block* b, bool is_only_comm) {
	gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();
	int comm_type = (fe == NULL) ? GPS_COMM_INIT : GPS_COMM_NESTED;
	gm_gps_comm_unit U(comm_type, fe);
	generate_message_receive_begin(U, Body, b, is_only_comm);
}
void gm_charm_lib::generate_message_receive_begin(ast_sentblock* sb, gm_symtab_entry* drv, gm_code_writer& Body, gm_gps_basic_block* b, bool is_only_comm) {
	gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();
	int comm_type = GPS_COMM_RANDOM_WRITE;
	gm_gps_comm_unit U(comm_type, sb, drv);
	generate_message_receive_begin(U, Body, b, is_only_comm);
}
void gm_charm_lib::generate_message_receive_begin(gm_gps_comm_unit& U, gm_code_writer& Body, gm_gps_basic_block* b, bool is_only_comm) {
    gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();

    std::list<gm_gps_communication_symbol_info>& LIST = info->get_all_communication_symbols(U);
    //int comm_id = info->find_communication_size_info(fe).id;
    int comm_id = (info->find_communication_size_info(U))->msg_class->id;

    char temp[1024];
    if (!is_only_comm && !info->is_single_message()) {
        sprintf(temp, "if (_msg.m_type == %d) {", comm_id);
        Body.pushln(temp);
    }

    std::list<gm_gps_communication_symbol_info>::iterator I;
    for (I = LIST.begin(); I != LIST.end(); I++) {
        gm_gps_communication_symbol_info& SYM = *I;
        gm_symtab_entry * e = SYM.symbol;

        // check it once again later
        if (e->getType()->is_property() || e->getType()->is_node_compatible() || e->getType()->is_edge_compatible() || !is_symbol_defined_in_bb(b, e)) {
            const char* str = main->get_type_string(SYM.gm_type);
            Body.push(str);
            Body.SPC();
        }
        if (e->getType()->is_property()) {
            Body.pushln("generate_vertex_prop_access_remote_lhs(e->getId(), Body);");
        } else {
            Body.push(e->getId()->get_genname());
        }
        Body.push(" = ");
        Body.push("_msg.");
        const char* fname = get_message_field_var_name(SYM.gm_type, SYM.idx);
        Body.push(fname);
        delete[] fname;
        Body.pushln(";");
    }
}
void gm_charm_lib::generate_message_receive_end(gm_code_writer& Body, bool is_only_comm) {
	if (!is_only_comm) {
		Body.pushln("}");
	}
}

const char* gm_charm_lib::get_message_field_var_name(int gm_type, int idx) {
	char temp[1024];
	const char* str = main->get_type_string(gm_type);
	sprintf(temp, "%c%d", str[0], idx);
	return gm_strdup(temp);
}
void gm_charm_lib::generate_message_send(ast_foreach* fe, gm_code_writer& Body) {
	char temp[1024];
	char str_buf[1024 * 8];
	gm_gps_basic_block *b = (gm_gps_basic_block *)fe->get_basic_block();
	gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();

	int m_type = (fe == NULL) ? GPS_COMM_INIT : GPS_COMM_NESTED;

	gm_gps_comm_unit U(m_type, fe);

	std::list<gm_gps_communication_symbol_info>& LIST = info->get_all_communication_symbols(U);

	gm_gps_communication_size_info& SINFO = *(info->find_communication_size_info(U));

	assert((fe != NULL) && (gm_is_out_nbr_node_iteration(fe->get_iter_type())));
	ast_procdef* proc = FE.get_current_proc();
	sprintf(temp, "typedef std::vector<struct %s_edge>::iterator Iterator;", 
			proc->get_procname()->get_genname());
	Body.pushln(temp);
	Body.pushln("for (Iterator _e = edges.begin(); _e != edges.end(); _e++) {");
	Body.pushln("// Sending messages to each neighbor");

	// check if any edge updates that should be done before message sending
	std::list<ast_sent*> sents_after_message;

	if ((fe != NULL) && (fe->has_info_list(GPS_LIST_EDGE_PROP_WRITE))) {
		std::list<void*>& L = fe->get_info_list(GPS_LIST_EDGE_PROP_WRITE);

		std::list<void*>::iterator I;
		for (I = L.begin(); I != L.end(); I++) {
			ast_sent* s = (ast_sent*) *I;
			assert(s->get_nodetype() == AST_ASSIGN);
			ast_assign* a = (ast_assign*) s;
			assert(!a->is_target_scalar());
			gm_symtab_entry* e = a->get_lhs_field()->get_second()->getSymInfo();
			int* i = (int*) fe->find_info_map_value(GPS_MAP_EDGE_PROP_ACCESS, e);
			assert(i!= NULL);

			if (*i == GPS_ENUM_EDGE_VALUE_SENT_WRITE) {
				sents_after_message.push_back(s);
			} else {
				get_main()->generate_sent(s);
			}
		}
	}
	
	char *entry_name = generate_vertex_entry_method_name(b);
	sprintf(temp, "%s_recv_msg *_msg = new %s_recv_msg();", 
			entry_name, entry_name);
	Body.pushln(temp);

	//------------------------------------------------------------
	// create message variables 
	//------------------------------------------------------------
	if (fe != NULL) {
		assert(fe->get_body()->get_nodetype() == AST_SENTBLOCK);
		std::list<ast_sent*>::iterator J;
		ast_sentblock* sb = (ast_sentblock*) fe->get_body();
		for (J = sb->get_sents().begin(); J != sb->get_sents().end(); J++) {
			ast_sent* s = *J;
			if (s->find_info_bool(GPS_FLAG_COMM_DEF_ASSIGN)) {
				get_main()->generate_sent(s);
			}
		}
	}

	std::list<gm_gps_communication_symbol_info>::iterator I;
	for (I = LIST.begin(); I != LIST.end(); I++) {
		gm_gps_communication_symbol_info& SYM = *I;
		gm_symtab_entry * e = SYM.symbol;
		Body.push("_msg->");
		Body.push(create_key_string(e->getId()));
		Body.push(" = ");
		if (e->getType()->is_node_property()) {
			generate_vertex_prop_access_rhs(e->getId(), Body);
		} else if (e->getType()->is_edge_property()) {
			generate_vertex_prop_access_rhs_edge(e->getId(), Body);
		} else {
			get_main()->generate_rhs_id(e->getId());
		}
		Body.pushln(";");
	}

	sprintf(temp, "thisProxy[_e->v].%s_recv(_msg);", entry_name);
	Body.pushln(temp);
	if (sents_after_message.size() > 0) {
		Body.NL();
		std::list<ast_sent*>::iterator I;
		for (I = sents_after_message.begin(); I != sents_after_message.end(); I++) {
			ast_sent*s = *I;
			get_main()->generate_sent(s);
		}

		sents_after_message.clear();
	}
	Body.pushln("}");

	assert(sents_after_message.size() == 0);
	delete [] entry_name;
}

void gm_charm_lib::generate_expr_builtin_field(ast_expr_builtin_field* e, gm_code_writer& Body) {
	ast_field* driver = e->get_field_driver();
	gm_builtin_def* def = e->get_builtin_def();
	std::list<ast_expr*>& ARGS = e->get_args();
	char temp[1024];

	switch (def->get_method_id()) {
		case GM_BLTIN_SET_ADD:
			sprintf(temp, "this->props.%s.%s(", driver->get_second()->get_genname(), "push_back");
			Body.push(temp);
			get_main()->generate_expr(ARGS.front());
			Body.push(")");
			break;
		case GM_BLTIN_SET_SIZE:
			sprintf(temp, "this->props.%s.%s()", driver->get_second()->get_genname(), "size");
			Body.push(temp);
			break;
		default:
			Body.push("???");
			//assert(false);
			break;
	}
}

void gm_charm_lib::generate_expr_builtin(ast_expr_builtin* e, gm_code_writer& Body, bool is_master) {

	if (e->driver_is_field()) {
		//assert(false);
		generate_expr_builtin_field((ast_expr_builtin_field*) e, Body);
		return;
	}

	ast_id* driver = e->get_driver();
	gm_builtin_def* def = e->get_builtin_def();
	std::list<ast_expr*>& ARGS = e->get_args();
	char temp[1024];

	switch (def->get_method_id()) {
		case GM_BLTIN_TOP_DRAND:         // rand function
			//assert(false);
			Body.push("(drand48())");
			break;

		case GM_BLTIN_TOP_IRAND:         // rand function
			Body.push("(lrand48() % ");
			get_main()->generate_expr(ARGS.front());
			Body.push(")");
			break;

		case GM_BLTIN_GRAPH_RAND_NODE:         // random node function
			assert(false);
			Body.push("(new java.util.Random()).nextInt(");
			Body.push("getGraphSize()");
			Body.push(")");
			break;

		case GM_BLTIN_GRAPH_NUM_NODES:
			//Body.push("/*please check*/");
			Body.push("vertex_number");
			break;
		case GM_BLTIN_NODE_DEGREE:
			//Body.push("/*please check*/");
			Body.push("getNeighborsSize()");
			break;
		case GM_BLTIN_NODE_IN_DEGREE:
			assert(false);
			Body.push(STATE_SHORT_CUT);
			Body.push(".");
			Body.push(GPS_REV_NODE_ID);
			Body.push(".length");
			break;
		default:
			Body.push("???");
			//assert(false);
			break;
	}
}

void gm_charm_lib::generate_vertex_prop_access_remote_lhs(ast_id *id, gm_code_writer& Body) {
    char temp[1024];
    sprintf(temp, "_remote_%s", id->get_genname());
    Body.push(temp);
}
void gm_charm_lib::generate_vertex_prop_access_remote_lhs_edge(ast_id *id, gm_code_writer& Body) {
    char temp[1024];
    sprintf(temp, "_remote_%s", id->get_genname());
    Body.push(temp);
}
void gm_charm_lib::generate_vertex_prop_access_remote_rhs(ast_id *id, gm_code_writer& Body) {
    generate_vertex_prop_access_lhs(id, Body);
}
void gm_charm_lib::generate_vertex_prop_access_prepare(gm_code_writer& Body) {
	assert(false);
}
void gm_charm_lib::generate_broadcast_receive_vertex(ast_id* id, gm_code_writer& Body) {
    Body.push("msg->");
    Body.push(create_key_string(id));
}

char *gm_charm_lib::generate_vertex_entry_method_name(gm_gps_basic_block *b) {
	char temp [1024];
	sprintf(temp, "__vertex_ep_bb%d", b->get_id());
	return gm_strdup(temp);
}

char *gm_charm_lib::generate_master_entry_method_name(gm_gps_basic_block *b) {
	char temp [1024];
	sprintf(temp, "__master_ep_bb%d", b->get_id());
	return gm_strdup(temp);
}
