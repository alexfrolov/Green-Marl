#include "gm_backend_charm.h"
#include "gm_frontend.h"
#include "gm_error.h"
#include "gm_charm_beinfo.h"
#include "gm_backend_charm_gen_steps.h" 
#include "gm_backend_gps_gen_steps.h" 
#include "gm_backend_gps_opt_steps.h" 
#include "gm_backend_cpp_opt_steps.h" 
#include "gm_ind_opt_steps.h"

void gm_charm_gen::init_opt_steps() {
	std::list<gm_compile_step*>& l = get_opt_steps();
	//assert(false);

	l.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_opt_defer));                    // deferred assignment --> insert _next
	l.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_transform_bfs));            // transform bfs
	l.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_edge_iteration));           // expand edge iteration
	l.push_back(GM_COMPILE_STEP_FACTORY(gm_ind_opt_propagate_trivial_writes));
	l.push_back(GM_COMPILE_STEP_FACTORY(gm_ind_opt_remove_unused_scalar));
	l.push_back(GM_COMPILE_STEP_FACTORY(gm_ind_opt_move_propdecl));            // copied from from ind-opt

	l.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_insert_temp_property));      // replace scalar -> temp property
	l.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_split_loops_for_flipping));      // replace scalar -> temp property
	l.push_back(GM_COMPILE_STEP_FACTORY(gm_ind_opt_flip_edges));               // Flip Edges

	l.push_back(GM_COMPILE_STEP_FACTORY(gm_ind_opt_move_propdecl));            // Move property declarations
	l.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_remove_master_set_iteration));        // Master set iteration with random write ==> expand into foreach
	l.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_remove_master_random_write));          // Master random write ==> expand into foreach
	l.push_back(GM_COMPILE_STEP_FACTORY(gm_ind_opt_loop_merge));               // Merge Loops

	l.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_check_synthesizable));    

	l.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_check_reverse_edges));
	l.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_new_check_nested_loops));           // check if max two-depth and apply scope analysis 
	l.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_new_analyze_scope_sent_var));    // check scope of variable
	l.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_new_analyze_scope_rhs_lhs));     // check scope of variable
	l.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_new_check_pull_data));           // check if it contains data pulling
	l.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_new_check_random_read));         // check if it contains random access
	l.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_new_check_random_write));        // check if it contains random access
	l.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_check_edge_value));          //
	l.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_early_filter));          // find early filtered loops
	l.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_new_rewrite_rhs));               //


	l.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_create_ebb)); // from gps backend!!!
	l.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_split_comm_ebb));            // split communicating every BB into two
	//l.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_merge_ebb_again));           // Merging Ebbs
	//l.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_merge_ebb_intra_loop));      // Merging Ebbs Inside Loops
}

bool gm_charm_gen::do_local_optimize() {
	//-----------------------------------
	// [TODO]
	// currently, there should be one and only one top-level procedure
	//-----------------------------------
	if (FE.get_num_procs() != 1) {
			gm_backend_error(GM_ERROR_CHARM_NUM_PROCS, "");
			return false;
	}

	//-----------------------------------
	// prepare backend information struct
	//-----------------------------------
	FE.prepare_proc_iteration();
	ast_procdef* p;
	while ((p = FE.get_next_proc()) != NULL) {
			FE.get_proc_info(p)->set_be_info(new gm_charm_beinfo(p));
	}

	//-----------------------------------
	// Now apply all the steps 
	//-----------------------------------
	return gm_apply_compiler_stage(get_opt_steps());
}
bool gm_charm_gen::do_local_optimize_lib() {
	//assert(false);
	return true;
}
bool gm_charm_lib::do_local_optimize() {
	//assert(false);
	return true;
}
