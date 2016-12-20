#ifndef GM_BACKEND_CHARM
#define GM_BACKEND_CHARM

#include "gm_backend.h"
#include "gm_misc.h"
#include "gm_code_writer.h"
#include "gm_compile_step.h"
#include "gm_gps_basicblock.h"

class gm_charm_gen;
extern gm_charm_gen CHARM_BE;

//-----------------------------------------------------------------
// interface for graph library Layer
//-----------------------------------------------------------------
class gm_charm_lib : public gm_graph_library {
	public:
		gm_charm_lib() {
			main = NULL;
		}
    gm_charm_lib(gm_charm_gen* gen) {
        set_main(gen);
    }
    virtual void set_main(gm_charm_gen* gen) {
        main = gen;
    }
    virtual gm_charm_gen* get_main() {
        return main;
    }
		bool do_local_optimize();

		void generate_expr_nil(ast_expr* e, gm_code_writer& Body);

    //virtual void generate_broadcast_reduce_initialize_master(ast_id* id, gm_code_writer& Body, int reduce_type, const char* base_value);
    //virtual void generate_broadcast_prepare(gm_code_writer& Body);
    //virtual void generate_broadcast_state_master(const char* state_var, gm_code_writer& Body);
    //virtual void generate_broadcast_isFirst_master(const char* var, gm_code_writer& Body);
    //virtual void generate_broadcast_variable_type(int gm_type_id, gm_code_writer& Body, int reduce_op = GMREDUCE_NULL);
    virtual void generate_broadcast_send_master(ast_id* id, gm_code_writer& Body);
    virtual void generate_broadcast_receive_master(ast_id* id, gm_code_writer& Body, int reduce_op = GMREDUCE_NULL);
    //virtual void generate_headers(gm_code_writer& Body);
		virtual void generate_reduce_assign_vertex(ast_assign* a, gm_code_writer& Body, int reduce_op_type = GMREDUCE_NULL);

    virtual void generate_broadcast_receive_vertex(ast_id* id, gm_code_writer& Body);

		virtual void generate_vertex_prop_access_lhs(ast_id *id, gm_code_writer& Body);
		virtual void generate_vertex_prop_access_lhs_edge(ast_id *id, gm_code_writer& Body);
		virtual void generate_vertex_prop_access_rhs(ast_id *id, gm_code_writer& Body);
		virtual void generate_vertex_prop_access_rhs_edge(ast_id *id, gm_code_writer& Body);

    virtual void generate_vertex_prop_access_remote_lhs(ast_id *id, gm_code_writer& Body);
    virtual void generate_vertex_prop_access_remote_lhs_edge(ast_id *id, gm_code_writer& Body);
    virtual void generate_vertex_prop_access_remote_rhs(ast_id *id, gm_code_writer& Body);
    virtual void generate_vertex_prop_access_prepare(gm_code_writer& Body);

    virtual void generate_node_iterator_rhs(ast_id* id, gm_code_writer& Body);
    virtual int get_type_size(ast_typedecl* t);
    virtual int get_type_size(int gm_type);

		// caller should delete var_name later
		const char* get_message_field_var_name(int gm_type, int idx);
		virtual void generate_message_send(ast_foreach* fe, gm_code_writer& Body);

    virtual void generate_message_receive_begin(ast_foreach* fe, gm_code_writer& Body, gm_gps_basic_block* b, bool is_only_comm);
    virtual void generate_message_receive_begin(ast_sentblock* sb, gm_symtab_entry* drv, gm_code_writer& Body, gm_gps_basic_block* b, bool is_only_comm);
    virtual void generate_message_receive_begin(gm_gps_comm_unit& U, gm_code_writer& Body, gm_gps_basic_block* b, bool is_only_comm);

    virtual void generate_message_receive_end(gm_code_writer& Body, bool is_only_comm);

    virtual void generate_expr_builtin_field(ast_expr_builtin_field* e, gm_code_writer& Body);
    virtual void generate_expr_builtin(ast_expr_builtin* e, gm_code_writer& Body, bool is_master);

		virtual char *generate_vertex_entry_method_name(gm_gps_basic_block *b);
		virtual char *generate_master_entry_method_name(gm_gps_basic_block *b);


	private:
		gm_charm_gen *main;
};

//-----------------------------------------------------------------
// interface for graph library Layer
//-----------------------------------------------------------------
class gm_charm_gen : public gm_backend, public gm_code_generator {
	public:
		gm_charm_gen() : 
			gm_code_generator(Body), dname(NULL), fname(NULL), f_body(NULL), f_body_ci(NULL), f_shell(NULL),  
			_is_master_gen(false), _is_receiver_gen(false) {
				set_lib(new gm_charm_lib(this));
				init_opt_steps();
				init_gen_steps();
		}

    virtual bool do_local_optimize_lib();
    virtual bool do_local_optimize();
    virtual bool do_generate();
    //virtual void do_generate_begin();
    //virtual void do_generate_end();

    virtual gm_charm_lib* get_lib() { return glib; }
    virtual void set_lib(gm_charm_lib* g) { glib = g; }

	protected:
    std::list<gm_compile_step*> opt_steps;
    std::list<gm_compile_step*> gen_steps;
		std::list<gm_compile_step*>& get_opt_steps() {
			return opt_steps;
		}
		std::list<gm_compile_step*>& get_gen_steps() {
			return gen_steps;
		}
    //virtual void build_up_language_voca();
    virtual void init_opt_steps();
    virtual void init_gen_steps();
    //----------------------------------
    // stages in backend gen
    //----------------------------------
    virtual void write_headers();

	public:

		virtual void setTargetDir(const char* dname);
		virtual void setFileName(const char* fname);
		const char* getFileName() {
			return fname;
		}


		virtual void analyse_symbols(ast_procdef* proc);

		virtual void generate_makefile(std::map<std::string,std::string>& setup);
		virtual void generate_readonly_vars();

		virtual void generate_class(ast_procdef* proc);
		virtual void generate_master();
		virtual void generate_master_messages();
		virtual void generate_master_properties();
		virtual void generate_master_scalar();
		virtual void generate_master_default_ctor_decl(char *name);
		virtual void generate_master_default_ctor_def(char *name);
		virtual void generate_master_entry_method_decls();
		virtual void generate_master_entry_method_decl(gm_gps_basic_block *b, bool with_entry);
		virtual void generate_master_entry_methods();
		virtual void generate_master_entry_method(gm_gps_basic_block *b);
		virtual void generate_master_entry_method_do_procname_decl();
		virtual void generate_master_entry_method_do_procname_def();
		virtual void generate_master_entry_method_reduction_vars();
		virtual void generate_master_entry_method_reduction_vars_decl();
		virtual void generate_master_entry_method_state_machine();
		virtual void generate_master_entry_method_state_machine_decl();

    virtual void do_generate_scalar_broadcast_send(gm_gps_basic_block* b);
    virtual void do_generate_scalar_broadcast_receive(gm_gps_basic_block *b, gm_code_writer & Body);
		virtual void do_generate_scalar_argument_list(gm_code_writer & Body, bool init_locals);

		virtual void generate_edge();

		virtual void generate_vertex();
		virtual void generate_vertex_messages();
		virtual void generate_vertex_message_def(gm_gps_basic_block *b);
		virtual void generate_vertex_message_decl_ci(gm_gps_basic_block *b);
		virtual void generate_vertex_entry_method_decls();
		virtual void generate_vertex_entry_method_decls_aux();
		virtual void generate_vertex_entry_method_decl(gm_gps_basic_block *b, bool with_entry);
		virtual void generate_vertex_entry_methods();
		virtual void generate_vertex_entry_methods_aux();
		virtual void generate_vertex_entry_method(gm_gps_basic_block *b);
		virtual void generate_vertex_entry_method_args_scala(gm_gps_basic_block *b, bool with_assign);
		virtual void generate_vertex_entry_method_args_recv(gm_gps_basic_block *b, bool with_assign);
		virtual void generate_vertex_default_ctor_decl(char *name);
		virtual void generate_vertex_default_ctor_def(char *name);
		virtual void generate_vertex_properties_type();
		virtual void generate_edge_properties_type();
		virtual void generate_vertex_all_properties();
		virtual void generate_vertex_scalar(gm_gps_basic_block *b);

		virtual void begin_module(char *name, bool is_mainmodule);
		virtual void end_module(char *name);
		virtual void begin_class(char *name);
		virtual void end_class(char *name);
		virtual void begin_struct(char *name);
		virtual void end_struct(char *name);
		virtual void begin_chare(char *name, bool is_mainchare);
		virtual void end_chare(char *name);
		virtual void begin_chare_array(char *name, int dim);
		virtual void end_chare_array(char *name);
		virtual void generate_pre_include_section();
		virtual void generate_post_include_section();

    virtual void generate_rhs_id(ast_id* i);
    virtual void generate_rhs_field(ast_field* i);
    virtual void generate_expr_foreign(ast_expr* e) { assert(false); }
    virtual void generate_expr_builtin(ast_expr* e);
    virtual void generate_expr_minmax(ast_expr* e);
    virtual void generate_expr_abs(ast_expr* e);
    virtual void generate_expr_nil(ast_expr* e);
    //virtual void generate_expr_type_conversion(ast_expr *e);
		virtual void generate_sent_return(ast_return *r);

    virtual void generate_scalar_var_def(gm_symtab_entry* sym, bool finish_sent);

    virtual const char* get_type_string(int prim_t); // returned string should be copied before usage.
		virtual const char* get_type_string(ast_typedecl* T, bool is_master);
    //virtual void generate_expr_list(std::list<ast_expr*>& L) { assert(false); }
    //virtual void generate_expr(ast_expr* e) { assert(false); }
    //virtual void generate_expr_val(ast_expr* e) { assert(false); }
    //virtual void generate_expr_inf(ast_expr* e) { assert(false); }
    //virtual void generate_expr_uop(ast_expr* e) { assert(false); }
    //virtual void generate_expr_ter(ast_expr* e) { assert(false); }
    //virtual void generate_expr_bin(ast_expr* e) { assert(false); }
    //virtual void generate_expr_comp(ast_expr* e) { assert(false); }
    virtual bool check_need_para(int optype, int up_optype, bool is_right) {
        return gm_need_paranthesis(optype, up_optype, is_right);
    }

    virtual void generate_mapaccess(ast_expr_mapaccess* e) { assert(false); }

    virtual void generate_lhs_id(ast_id* i);
    virtual void generate_lhs_field(ast_field* i);
    virtual void generate_sent_nop(ast_nop* n) { assert(false); }
    virtual void generate_sent_reduce_assign(ast_assign *a);
    virtual void generate_sent_defer_assign(ast_assign *a) { assert(false); }
    virtual void generate_sent_vardecl(ast_vardecl *a) { assert(false); }
    virtual void generate_sent_foreach(ast_foreach *a);
    virtual void generate_sent_bfs(ast_bfs* b) { assert(false); }

    //virtual void generate_sent(ast_sent*) { assert(false); }
    virtual void generate_sent_assign(ast_assign *a);
    //virtual void generate_sent_if(ast_if *a) { assert(false); }
    //virtual void generate_sent_while(ast_while *w) { assert(false); }
    //virtual void generate_sent_block(ast_sentblock *b, bool need_brace = true) { assert(false); }
    //virtual void generate_sent_return(ast_return *b) { assert(false); }
    virtual void generate_sent_call(ast_call *c);
    //virtual void generate_sent_foreign(ast_foreign *f) { assert(false); }

		virtual void generate_expr_inf(ast_expr *e);

		virtual bool has_reduction_target_in_input_args(gm_gps_basic_block *b);

		void set_master_generate(bool b) {
			_is_master_gen = b;
		}
		bool is_master_generate() {
			return _is_master_gen;
		} 
		void set_receiver_generate(bool b) {
			_is_receiver_gen = b;
		}
		bool is_receiver_generate() {
			return _is_receiver_gen;
		}

		const char* get_unbox_method_string(int gm_type);
		const char* get_box_type_string(int gm_type);

	protected:
    char* dname;
    char* fname;
    gm_code_writer Body, Body_ci, Makefile;
    FILE* f_body, *f_body_ci, *f_shell;
    char temp [1024];
    char* get_temp_buffer_member() {return temp;}
    bool open_output_files();
    void close_output_files(bool remove_files=false);
    void remove_output_files();

private:
    gm_charm_lib* glib; // graph library
		bool _is_master_gen;
		bool _is_receiver_gen;
};

#endif
