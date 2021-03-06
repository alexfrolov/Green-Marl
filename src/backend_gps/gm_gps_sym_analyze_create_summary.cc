#include <stdio.h>
#include <set>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_typecheck.h"
#include "gps_syminfo.h"

//-----------------------------------------------------------------------------------
// Create a flat table, for the generation of fields in master/vertex class
//-----------------------------------------------------------------------------------


static int comp_start_byte(std::set<gm_symtab_entry*>& prop) {
    int byte_begin = 0;
    std::set<gm_symtab_entry*>::iterator I;
    for (I = prop.begin(); I != prop.end(); I++) {
        gm_symtab_entry * sym = *I;
        gps_syminfo* syminfo = (gps_syminfo*) sym->find_info(GPS_TAG_BB_USAGE);

        int size = PREGEL_BE->get_lib()->get_type_size(sym->getType()->get_target_type());
        syminfo->set_start_byte(byte_begin);
        byte_begin += size;
    }
    return byte_begin;
}

static void create_list_of_props_in_order(bool is_node_prop, bool is_input, std::list<gm_symtab_entry*>& L)
{
    gm_symtab* arg_fields = FE.get_current_proc()->get_symtab_field();
    if (arg_fields->get_entries().size() != 0)  {
        std::set<gm_symtab_entry*>& E = arg_fields->get_entries();
        // sort by order
        std::set<gm_symtab_entry*>::iterator I;
        std::map<int, gm_symtab_entry*> M;
        for(I=E.begin(); I!=E.end(); I++) {
            gm_symtab_entry *e = *I;

            int usage = e->find_info_int(GMUSAGE_PROPERTY);
            if (is_node_prop && !e->getType()->is_node_property()) continue;
            if (!is_node_prop && !e->getType()->is_edge_property()) continue;
            if (is_input && !((usage == GMUSAGE_IN) || (usage == GMUSAGE_INOUT))) continue;
            if (!is_input && !((usage == GMUSAGE_OUT) || (usage == GMUSAGE_INOUT))) continue;

            int total_order = e->find_info_int(FE_INFO_ARG_POS_TOTAL);
            assert (M.find(total_order) == M.end());
            M[total_order] = e;
        }

        std::map<int, gm_symtab_entry*>::iterator J; 
        for(J = M.begin(); J != M.end(); J++) {
            gm_symtab_entry *e = J->second;
            L.push_back(e);
        }
    }
}

void gm_gps_opt_analyze_symbol_summary::process(ast_procdef* p) {

    //-----------------------------------------------
    // mark special markers to the property arguments
    //-----------------------------------------------
    std::set<gm_symtab_entry*>::iterator J;
    std::set<gm_symtab_entry*>& args = p->get_symtab_field()->get_entries();
    for (J = args.begin(); J != args.end(); J++) {
        gm_symtab_entry *sym = *J;
        gps_syminfo* syminfo = (gps_syminfo*) sym->find_info(GPS_TAG_BB_USAGE);
        assert(syminfo!=NULL);
        syminfo->set_is_argument(true);
    }

    //-------------------------------------
    // make a flat symbol table
    //-------------------------------------
    gm_gps_beinfo * beinfo = (gm_gps_beinfo *) FE.get_backend_info(p);

    std::set<gm_symtab_entry*>& prop = beinfo->get_node_prop_symbols();
    std::set<gm_symtab_entry*>& e_prop = beinfo->get_edge_prop_symbols();
    gps_flat_symbol_t T(beinfo->get_scalar_symbols(), prop, e_prop);
    p->traverse(&T, false, true);

    //-----------------------------------------------------------
    // Enlist property symbols (todo: opt ordering for cacheline ?)
    //-----------------------------------------------------------
    beinfo->set_total_node_property_size(comp_start_byte(prop));
    beinfo->set_total_edge_property_size(comp_start_byte(e_prop));

    beinfo->compute_max_communication_size();

    //-----------------------------------------------------------
    // Create a symbol table for node/edge input/out generator
    //   create a list in the order of property
    //-----------------------------------------------------------
    create_list_of_props_in_order(true, true,  beinfo->get_node_input_prop_symbols());
    create_list_of_props_in_order(true, false, beinfo->get_node_output_prop_symbols());
    create_list_of_props_in_order(false, true, beinfo->get_edge_input_prop_symbols());
    create_list_of_props_in_order(false, false, beinfo->get_edge_output_prop_symbols());
    //

    //--------------------------------------------------------
    // check if input node parsing parsing is required
    //--------------------------------------------------------
    gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();
    bool need_node_prop_init = false;
    std::set<gm_symtab_entry*>::iterator I;
    for (I = prop.begin(); I != prop.end(); I++) {
        gm_symtab_entry* e = *I;
        if ((e->find_info_int(GMUSAGE_PROPERTY) == GMUSAGE_IN) || (e->find_info_int(GMUSAGE_PROPERTY) == GMUSAGE_INOUT)) {
            /*
             printf("in/inout -> %s :%d\n",
             e->getId()->get_genname(),
             e->find_info_int(GMUSAGE_PROPERTY)
             );
             */
            need_node_prop_init = true;
        }
    }

    ast_procdef* proc = FE.get_current_proc();
    proc->add_info_bool(GPS_FLAG_NODE_VALUE_INIT, need_node_prop_init);

    set_okay(true);
}

