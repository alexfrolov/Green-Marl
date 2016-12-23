#include "gm_backend_charm.h"
#include "gm_backend_charm_gen_steps.h"
#include "gm_charm_beinfo.h"
#include "gm_backend_gps.h"
#include "gm_backend_gps_gen_steps.h"
#include "gm_error.h"
#include "gm_argopts.h"
#include "gm_frontend.h"
#include "gm_builtin.h"

extern bool gm_read_setup_file(std::map<std::string, std::string>& setup, bool export_env);

const char* gm_charm_gen::get_unbox_method_string(int gm_type) {
	return "get";
}     

const char* gm_charm_gen::get_box_type_string(int gm_type) {
    switch(gm_type) {
        case GMTYPE_NODE:
					return "uint64_t";
        case GMTYPE_EDGE:
					return "uint64_t";
        default:
            assert(false);
    }
}

const char* gm_charm_gen::get_type_string(ast_typedecl* T, bool is_master) {
	if (T->is_primitive() || T->is_node()) {
		return (get_type_string(T->get_typeid()));
	} else if (T->is_node_compatible()) {
		return (get_type_string(GMTYPE_NODE));
	} else if (T->is_collection()) {
		assert(false);
		//return get_collection_type_string(T);
	} else {
		assert(false);
		// to be done
	}
	return "???";
}


const char* gm_charm_gen::get_type_string(int gm_type) {
	switch (gm_type) {
		case GMTYPE_INT:
			return "int";
		case GMTYPE_LONG:
			return "long";
		case GMTYPE_FLOAT:
			return "float";
		case GMTYPE_DOUBLE:
			return "double";
		case GMTYPE_BOOL:
			return "bool";
		case GMTYPE_NODE:
			//if (get_lib()->is_node_type_int())
			//	return "int";
			//else
				return "long";
		case GMTYPE_NSEQ:
			//if (get_lib()->is_node_type_int())
			//	return "int";
			//else
				return "std::vector<long>";
		case GMTYPE_NSET:
			//if (get_lib()->is_node_type_int())
			//	return "int";
			//else
				return "std::vector<long>";
		default:
			assert(false);
			return "ERROR";
	}
}

bool gm_charm_gen::open_output_files() {
    char temp[1024];
    assert(dname != NULL);
    assert(fname != NULL);

    sprintf(temp, "%s/%s.ci", dname, fname);
    f_body_ci = fopen(temp, "w");
    if (f_body_ci == NULL) {
        gm_backend_error(GM_ERROR_FILEWRITE_ERROR, temp);
        return false;
    }
    Body_ci.set_output_file(f_body_ci);

    sprintf(temp, "%s/%s.C", dname, fname);
    f_body = fopen(temp, "w");
    if (f_body == NULL) {
        gm_backend_error(GM_ERROR_FILEWRITE_ERROR, temp);
        return false;
    }
    Body.set_output_file(f_body);

    get_lib()->set_code_writer(&Body);

		sprintf(temp, "%s/%s_compile.mk", dname, fname);
		f_shell = fopen(temp, "w");
		if (f_shell == NULL) {
			gm_backend_error(GM_ERROR_FILEWRITE_ERROR, temp);
			return false;
		}
    Makefile.set_output_file(f_shell);

    return true;
}

void gm_charm_gen::close_output_files(bool remove_files) {
    char temp[1024];
    if (f_body_ci != NULL) {
        Body_ci.flush();
        fclose(f_body_ci);
        if (remove_files) {
            sprintf(temp, "rm %s/%s.h", dname, fname);
            system(temp);
        }
        f_body_ci = NULL;
    }
    if (f_body != NULL) {
        Body.flush();
        fclose(f_body);
        if (remove_files) {
            sprintf(temp, "rm %s/%s.cc", dname, fname);
            system(temp);
        }
        f_body = NULL;
    }

    if (f_shell != NULL) {
        fclose(f_shell);
        f_shell = NULL;
        if (remove_files) {
            sprintf(temp, "rm %s/%s_compile.mk", dname, fname);
            system(temp);
        }
    }
}

//--------------------------------------------------------------
// A Back-End for Charm++ generation
//--------------------------------------------------------------

void gm_charm_gen::init_gen_steps() {
	std::list<gm_compile_step*>& l = get_gen_steps();
	l.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_analyze_symbol_usage)); // gps!!
	l.push_back(GM_COMPILE_STEP_FACTORY(gm_charm_analyse_symbols));
	l.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_find_reachable));
	l.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_find_congruent_message));    // Find congruent message

	l.push_back(GM_COMPILE_STEP_FACTORY(gm_charm_gen_class));
	//assert(false);
}

bool gm_charm_gen::do_generate() {
	if (!open_output_files()) return false;
	bool b = gm_apply_compiler_stage(get_gen_steps());

	// generate makefile
	std::map<std::string, std::string> setup;
	gm_read_setup_file(setup, false);
	generate_makefile(setup);

	close_output_files();
	return b;
}

void gm_charm_gen::generate_makefile(std::map<std::string,std::string>& setup) {
	const char* gm_top = setup["GM_TOP"].c_str();

	fprintf(f_shell, "GM_TOP= %s\n",gm_top);
	fprintf(f_shell, "GM_GRAPH= ${GM_TOP}/apps/output_charm/gm_graph\n",gm_top);
	fprintf(f_shell, "CC= g++\n");
	fprintf(f_shell, "CFLAGS = -g -O3 -I${GM_GRAPH}/inc -I. \n", gm_top);

	fprintf(f_shell, "LFLAGS = -L${GM_GRAPH}/lib \n");
	fprintf(f_shell, "include ${GM_TOP}/setup.mk\n");
	//fprintf(f_shell, "include ${GM_TOP}/apps/output_cpp/common.mk\n");

	fprintf(f_shell, "\n");
	fprintf(f_shell, "%s.decl.h %s.def.h : %s.ci\n", fname, fname, fname);
	fprintf(f_shell, "\t$(CHARMC) $<\n");

	fprintf(f_shell, "\n");
	fprintf(f_shell, "%s : %s.C %s.decl.h %s.def.h\n", fname, fname, fname, fname);
	fprintf(f_shell, "\t$(CHARMC) ${CFLAGS} $< ${LFLAGS} -o $@", fname, fname);
}

void gm_charm_gen::generate_lhs_id(ast_id *id) {
	Body.push(id->get_genname());
}
void gm_charm_gen::generate_rhs_id(ast_id *i) {
	if (i->getSymInfo()->getType()->is_node_iterator()) {
		/*
			 if (i->getSymInfo()->find_info_bool(GPS_FLAG_COMM_SYMBOL)) { // transmitted information
			 if (!this->is_receiver_generate()) { // master generate
			 get_lib()->generate_node_iterator_rhs(i, Body);  // this.getId()
			 } else {
			 generate_lhs_id(i); // normal symbol name
			 }
			 } else {
			 get_lib()->generate_node_iterator_rhs(i, Body);     // this.getId()
			 }
		 */
		if (i->getSymInfo()->find_info_bool(GPS_FLAG_IS_OUTER_LOOP)) {
			if (this->is_receiver_generate()) {
				generate_lhs_id(i); // normal symbol name. 
			}
			else {
				get_lib()->generate_node_iterator_rhs(i, Body);     // this.getId()
			}
		} else if (i->getSymInfo()->find_info_bool(GPS_FLAG_IS_INNER_LOOP)) {
			if (this->is_receiver_generate()) {
				get_lib()->generate_node_iterator_rhs(i, Body);     // this.getId()
			}
			else {
				generate_lhs_id(i); // normal symbol name. 
			}
		} else {
			generate_lhs_id(i); // normal symbol name
		}

	} else {
		// normal symbol name
		generate_lhs_id(i);
	}
}

void gm_charm_gen::generate_lhs_field(ast_field *f) {
    ast_id* prop = f->get_second();
    if (is_master_generate()) {
        assert(false);
    } else if (is_receiver_generate()) {

        if (f->get_first()->getTypeInfo()->is_node_iterator()){
            int iter_type = f->get_first()->getTypeInfo()->get_defined_iteration_from_iterator();
            if (gm_is_all_graph_node_iteration(iter_type)) {
                get_lib()->generate_vertex_prop_access_remote_lhs(prop, Body);
            } else if (gm_is_out_nbr_node_iteration(iter_type) || gm_is_in_nbr_node_iteration(iter_type)) {
                get_lib()->generate_vertex_prop_access_lhs(prop, Body);
            } else {
                assert(false);
            }
        } else if (f->get_first()->getTypeInfo()->is_edge()) {
            get_lib()->generate_vertex_prop_access_remote_lhs_edge(prop, Body);
        } else {
            if (f->get_first()->getTypeInfo()->is_node_compatible()) { // random node
                printf("get_lib()->generate_vertex_prop_access_lhs(prop, Body);\n");
								assert(false);
            } else {
                assert(false);
            }
        }
    } else { // vertex generate;
             //assert(f->getSourceTypeSummary() == GMTYPE_NODEITER_ALL);
        if (f->get_first()->getTypeInfo()->is_edge()) {
            get_lib()->generate_vertex_prop_access_lhs_edge(prop, Body);
        } else {
            assert(f->get_first()->getTypeInfo()->is_node_compatible());
            get_lib()->generate_vertex_prop_access_lhs(prop, Body);
        }
    }

	//assert(false);
}
void gm_charm_gen::generate_rhs_field(ast_field *f) {
	generate_lhs_field(f);
}
void gm_charm_gen::generate_expr_builtin(ast_expr* e) {
	ast_expr_builtin * be = (ast_expr_builtin*) e;
	gm_builtin_def* def = be->get_builtin_def();
	std::list<ast_expr*>& ARGS = be->get_args();

	switch (def->get_method_id()) {
		case GM_BLTIN_TOP_DRAND:         // rand function
		case GM_BLTIN_TOP_IRAND:         // rand function
		case GM_BLTIN_GRAPH_NUM_NODES:
		case GM_BLTIN_GRAPH_RAND_NODE:
		case GM_BLTIN_NODE_DEGREE:
		case GM_BLTIN_NODE_IN_DEGREE:
		case GM_BLTIN_NODE_HAS_EDGE_TO:
		case GM_BLTIN_NODE_IS_NBR_FROM:
		case GM_BLTIN_NODE_RAND_NBR:
			get_lib()->generate_expr_builtin(be, Body, is_master_generate());
			break;

		case GM_BLTIN_TOP_LOG:           // log function
			Body.push("Math.log(");
			assert(ARGS.front()!=NULL);
			generate_expr(ARGS.front());
			Body.push(")");
			break;
		case GM_BLTIN_TOP_EXP:           // exp function
			Body.push("Math.exp(");
			assert(ARGS.front()!=NULL);
			generate_expr(ARGS.front());
			Body.push(")");
			break;
		case GM_BLTIN_TOP_POW:           // pow function
			Body.push("Math.pow(");
			assert(ARGS.front()!=NULL);
			generate_expr(ARGS.front());
			Body.push(",");
			assert(ARGS.back()!=NULL);
			generate_expr(ARGS.back());
			Body.push(")");
			break;

		default:
			get_lib()->generate_expr_builtin(be, Body, is_master_generate());
			break;
	}
}
void gm_charm_gen::generate_expr_minmax(ast_expr* e) {
	assert(false);
}
void gm_charm_gen::generate_expr_abs(ast_expr* e) {
	Body.push(" std::abs(");
	generate_expr(e->get_left_op());
	Body.push(") ");
}
void gm_charm_gen::generate_expr_nil(ast_expr* e) {
	get_lib()->generate_expr_nil(e, Body);
}

void gm_charm_gen::generate_sent_return(ast_return *r) {
}

void gm_charm_gen::generate_sent_reduce_assign(ast_assign *a) {
	if (is_master_generate()) {
		// [to be done]
		assert(false);
	}

	if (a->find_info_ptr(GPS_FLAG_SENT_BLOCK_FOR_RANDOM_WRITE_ASSIGN) != NULL) {
		if (!is_receiver_generate()) {
			// generate random write messaging
			printf("get_lib()->generate_message_payload_packing_for_random_write(a, Body);");
			assert(false);
			return;
		}
	}

	if (a->is_target_scalar()) {
		// check target is global
		{
			get_lib()->generate_reduce_assign_vertex(a, Body, a->get_reduce_type());
			//assert(false);
		}
		return;
	}

	//--------------------------------------------------
	// target is vertex-driven
	// reduction now became normal read/write
	//--------------------------------------------------
	if (a->is_argminmax_assign()) {
		assert((a->get_reduce_type() == GMREDUCE_MIN) || (a->get_reduce_type() == GMREDUCE_MAX));

		Body.push("if (");
		if (a->is_target_scalar())
			generate_rhs_id(a->get_lhs_scala());
		else
			generate_rhs_field(a->get_lhs_field());

		if (a->get_reduce_type() == GMREDUCE_MIN)
			Body.push(" > ");
		else
			Body.push(" < ");

		generate_expr(a->get_rhs());
		Body.pushln(") {");
		if (a->is_target_scalar())
			generate_lhs_id(a->get_lhs_scala());
		else
			generate_lhs_field(a->get_lhs_field());
		Body.push(" = ");
		generate_expr(a->get_rhs());
		Body.pushln(";");

		std::list<ast_node*>& lhs_list = a->get_lhs_list();
		std::list<ast_expr*>& rhs_list = a->get_rhs_list();
		std::list<ast_node*>::iterator I;
		std::list<ast_expr*>::iterator J;
		J = rhs_list.begin();
		for (I = lhs_list.begin(); I != lhs_list.end(); I++, J++) {
			ast_node* n = *I;
			if (n->get_nodetype() == AST_ID) {
				generate_lhs_id((ast_id*) n);
			} else {
				generate_lhs_field((ast_field*) n);
			}
			Body.push(" = ");
			generate_expr(*J);
			Body.pushln(";");
		}

		Body.pushln("}");
	} else {
		if (a->is_target_scalar()) {
			generate_lhs_id(a->get_lhs_scala());
		} else {
			generate_lhs_field(a->get_lhs_field());
		}

		Body.push(" = ");

		if ((a->get_reduce_type() == GMREDUCE_PLUS) || (a->get_reduce_type() == GMREDUCE_MULT) || (a->get_reduce_type() == GMREDUCE_AND)
				|| (a->get_reduce_type() == GMREDUCE_OR)) {
			if (a->is_target_scalar())
				generate_rhs_id(a->get_lhs_scala());
			else
				generate_rhs_field(a->get_lhs_field());

			switch (a->get_reduce_type()) {
				case GMREDUCE_PLUS:
					Body.push(" + (");
					break;
				case GMREDUCE_MULT:
					Body.push(" * (");
					break;
				case GMREDUCE_AND:
					Body.push(" && (");
					break;
				case GMREDUCE_OR:
					Body.push(" || (");
					break;
				default:
					assert(false);
					break;
			}
			generate_expr(a->get_rhs());
			Body.pushln(");");
		} else if ((a->get_reduce_type() == GMREDUCE_MIN) || (a->get_reduce_type() == GMREDUCE_MAX)) {
			if (a->get_reduce_type() == GMREDUCE_MIN)
				Body.push("java.lang.Min(");
			else
				Body.push("java.lang.Max(");

			if (a->is_target_scalar())
				generate_rhs_id(a->get_lhs_scala());
			else
				generate_rhs_field(a->get_lhs_field());
			Body.push(",");
			generate_expr(a->get_rhs());

			Body.pushln(");");
		} else {
			assert(false);
		}
	}
}
void gm_charm_gen::generate_sent_foreach(ast_foreach* fe) {
	// must be a sending foreach
	Body.pushln("// generate_sent_foreach");
	if (fe->find_info_bool(GPS_FLAG_IS_INNER_LOOP)) {

		assert(gm_is_out_nbr_node_iteration(fe->get_iter_type()) || 
				gm_is_in_nbr_node_iteration(fe->get_iter_type())); 
		get_lib()->generate_message_send(fe, Body);
		//assert(false);
	}
	else {
		//assert (!fe->find_info_bool(GPS_FLAG_IS_OUTER_LOOP));
		if (fe->is_source_field())
		{
			ast_field * f = fe->get_source_field();

			char iterator_name[256];
			char temp[2048];
			sprintf(iterator_name, "__%s_iter", fe->get_iterator()->get_genname());
			int base_type = f->get_second()->getTypeInfo()->get_target_type()->is_node_collection() ? GMTYPE_NODE : GMTYPE_EDGE;
			const char* unbox_name = get_unbox_method_string(base_type);

			sprintf(temp, "typedef std::vector<long>::iterator Iterator;");
			Body.pushln(temp);

			sprintf(temp, "for (Iterator %s = this->props.%s.begin(); %s != this->props.%s.end(); %s++) {",
					//get_box_type_string(base_type),
					iterator_name, 
					f->get_second()->get_genname(),
					iterator_name, 
					f->get_second()->get_genname(),
					iterator_name);
			Body.pushln(temp);

			sprintf(temp,"%s %s = *%s;",
					get_type_string(base_type), 
					fe->get_iterator()->get_genname(), 
					iterator_name);
			Body.pushln(temp);

			generate_sent(fe->get_body());
			Body.pushln("}");
		}
		else 
		{
			bool need_close_block;
			printf("get_lib()->generate_benign_feloop_header(fe, need_close_block, Body);\n");
			//assert(false);

			generate_sent(fe->get_body());
			if (need_close_block)
				Body.pushln("}");

		}
	}
}

void gm_charm_gen::generate_sent_assign(ast_assign *a) {
    // normal assign
    if (is_master_generate()) {
        this->gm_code_generator::generate_sent_assign(a);
        return;
    }

    // edge defining write
    if (a->find_info_bool(GPS_FLAG_EDGE_DEFINING_WRITE)) {
        return;
    }

    if (is_receiver_generate()) {
        if (!a->is_target_scalar() && a->get_lhs_field()->get_first()->getSymInfo()->find_info_bool(GPS_FLAG_EDGE_DEFINED_INNER)) {
            return;
        }

        if (a->find_info_bool(GPS_FLAG_COMM_DEF_ASSIGN)) { // defined in re-write rhs
            return;
        }
    }
		
    // vertex or receiver generate
    if (a->find_info_ptr(GPS_FLAG_SENT_BLOCK_FOR_RANDOM_WRITE_ASSIGN) != NULL) {
        if (!is_receiver_generate()) {
            // generate random write messaging
            //get_lib()->generate_message_payload_packing_for_random_write(a, Body);
					assert(false);
            return;
        }
    }

    if (a->is_target_scalar()) {
        ast_id* i = a->get_lhs_scala();

        gps_syminfo* syminfo = (gps_syminfo*) i->getSymInfo()->find_info(GPS_TAG_BB_USAGE);

        // normal assign
        if (!syminfo->is_scoped_global()) {
            this->gm_code_generator::generate_sent_assign(a);
            return;
        } else {
            // write to global scalar
            get_lib()->generate_reduce_assign_vertex(a, Body, GMREDUCE_NULL);
            return;

            // [TO BE DONE]
            //printf("need to implement: write to global %s\n", i->get_genname());
            //assert(false);
        }
    } else {
        ast_field* f = a->get_lhs_field();

        this->gm_code_generator::generate_sent_assign(a);
    }
}



void gm_charm_gen::setTargetDir(const char* d) {
	assert(d != NULL);
	if (dname != NULL) delete[] dname;
	dname = new char[strlen(d) + 1];
	strcpy(dname, d);
}           

void gm_charm_gen::setFileName(const char* f) {
	assert(f != NULL);
	if (fname != NULL) delete[] fname;
	fname = new char[strlen(f) + 1];
	strcpy(fname, f);
}

void gm_charm_gen::write_headers() {
	assert(false);
    //printf("lib = %p\n", get_giraph_lib());
    //get_giraph_lib()->generate_headers_vertex(Body);
    //get_giraph_lib()->generate_headers_main(Body_main);
    //get_giraph_lib()->generate_headers_input(Body_input);
    //get_giraph_lib()->generate_headers_output(Body_output);
}

void gm_charm_gen::generate_class(ast_procdef* proc) {
	//write_headers();
	begin_module(proc->get_procname()->get_genname(), false);
	generate_pre_include_section();
	//generate_master_messages();
	generate_vertex_messages();
	generate_readonly_vars();
	generate_master();
	generate_edge();
	generate_vertex();
	generate_post_include_section();
	end_module(proc->get_procname()->get_genname());
	//do_generate_job_configuration();
}

void gm_charm_gen::generate_readonly_vars() {
	char temp[1024];
  ast_procdef* proc = FE.get_current_proc();

	Body.pushln("// these readonly variables should initialized in mainchare constructor");
	sprintf(temp, "CProxy_%s_master master_proxy;", proc->get_procname()->get_genname());
	Body.pushln(temp);
	sprintf(temp, "CProxy_%s_vertex vertex_proxy;", proc->get_procname()->get_genname());
	Body.pushln(temp);
	Body.pushln("CmiUInt8 vertex_number;");
	Body.NL();

	sprintf(temp, "readonly CProxy_%s_master master_proxy;", proc->get_procname()->get_genname());
	Body_ci.pushln(temp);
	sprintf(temp, "readonly CProxy_%s_vertex vertex_proxy;", proc->get_procname()->get_genname());
	Body_ci.pushln(temp);
	Body.pushln("readonly CmiUInt8 vertex_number;");
	Body_ci.NL();
}

void gm_charm_gen::generate_pre_include_section() {
	char temp[1024];
  ast_procdef* proc = FE.get_current_proc();
	sprintf(temp, "struct %s_edge;", proc->get_procname()->get_genname());
	Body.pushln(temp);

	Body.pushln("#include <limits>");
	sprintf(temp, "#include \"%s.decl.h\"", proc->get_procname()->get_genname());
	Body.pushln(temp);
//	Body.pushln("#include \"gm_graph.h\"");
	Body.NL();
}

void gm_charm_gen::generate_post_include_section() {
	char temp[1024];
  ast_procdef* proc = FE.get_current_proc();
	sprintf(temp, "#include \"%s.def.h\"", proc->get_procname()->get_genname());
	Body.pushln(temp);
}

void gm_charm_gen::generate_expr_inf(ast_expr *e) {
    char* temp = temp_str;
    assert(e->get_opclass() == GMEXPR_INF);
    int t = e->get_type_summary();
    switch (t) {
        case GMTYPE_INF:
        case GMTYPE_INF_INT:
            sprintf(temp, "%s", e->is_plus_inf() ? 
								"std::numeric_limits<int>::max()" : 
								"std::numeric_limits<int>::min()");
            break;
        case GMTYPE_INF_LONG:
            sprintf(temp, "%s", e->is_plus_inf() ? 
								"std::numeric_limits<long>::max()" : 
								"std::numeric_limits<long>::min()");
            break;
        case GMTYPE_INF_FLOAT:
            sprintf(temp, "%s", e->is_plus_inf() ? 
								"std::numeric_limits<float>::max()" : 
								"std::numeric_limits<float>::min()");
            break;
        case GMTYPE_INF_DOUBLE:
            sprintf(temp, "%s", e->is_plus_inf() ? 
								"std::numeric_limits<double>::max()" : 
								"std::numeric_limits<double>::min()");
            break;
        default:
            sprintf(temp, "%s", e->is_plus_inf() ? 
								"std::numeric_limits<int>::max()" : 
								"std::numeric_limits<int>::min()");
            break;
    }
    _Body.push(temp);
    return;
}

void gm_charm_gen::generate_sent_call(ast_call *c)
{
    ast_expr_builtin *b = c->get_builtin();
    get_lib()->generate_expr_builtin(b, Body, false);
    Body.pushln(";");
}


void gm_charm_gen::analyse_symbols(ast_procdef* proc) { 
	assert(false);
}

