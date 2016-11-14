#include <set>
#include "gm_backend_charm.h"
#include "gm_backend_charm_gen_steps.h"
#include "gm_charm_beinfo.h"
#include "gps_syminfo.h"
#include "gm_typecheck.h"
#include "gm_frontend.h"
#include "gm_charm_basicblock.h"

void gm_charm_analyse_symbols::process(ast_procdef* p) {
	/*std::set<gm_symtab_entry*>::iterator J;
	std::set<gm_symtab_entry*>& args = proc->get_symtab_field()->get_entries();
	for (J = args.begin(); J != args.end(); J++) {
		gm_symtab_entry *sym = *J;
		gps_syminfo* syminfo = (gps_syminfo*) sym->find_info(GPS_TAG_BB_USAGE);
		assert(syminfo!=NULL);
		syminfo->set_is_argument(true);
	}*/

	gm_charm_beinfo * beinfo = (gm_charm_beinfo *) FE.get_backend_info(p);

	std::set<gm_symtab_entry*>& scalars = beinfo->get_scalar_symbols();
	std::set<gm_symtab_entry*>& prop = beinfo->get_node_prop_symbols();
	std::set<gm_symtab_entry*>& e_prop = beinfo->get_edge_prop_symbols();
	gps_flat_symbol_t T(scalars, prop, e_prop);
	p->traverse(&T, false, true);
	//assert(false);
}

