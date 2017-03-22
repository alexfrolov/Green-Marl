#include <stdio.h>
#include "gm_backend_charm.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"
#include "gm_charm_beinfo.h"
#include "gm_backend_gps.h" // FIXME:!


class charm_random_write_rhs_t : public gm_apply
{
public:
    charm_random_write_rhs_t(ast_sentblock* _sb, gm_symtab_entry* _sym, gm_charm_lib* l, gm_code_writer& b) :
            sb(_sb), sym(_sym), lib(l), Body(b) {
        set_for_expr(true);
        U = new gm_gps_comm_unit(GPS_COMM_RANDOM_WRITE, sb, sym);
        INFO = (gm_gps_beinfo *) FE.get_current_backend_info();
    }

    bool apply(ast_expr* e) {
        if (!e->is_id() && !e->is_field()) return true;

        gm_gps_communication_symbol_info* SS;
        gm_symtab_entry* tg;

        if (e->is_id()) {
            tg = e->get_id()->getSymInfo();
        } else {
            tg = e->get_field()->get_second()->getSymInfo();
            if (e->get_field()->get_first()->getSymInfo() == sym) return true;
        }

        SS = INFO->find_communication_symbol_info(*U, tg);
        if (SS == NULL) return true;

				//char mname[1024];
        //const char* mname = lib->get_random_write_message_name(sym);
        const char* mname = "_msg";
        Body.push(mname); // should not delete this.
        Body.push("->");

        Body.push(SS->symbol->getId()->get_genname());
        Body.push(" = ");

        if (e->is_id())
            lib->get_main()->generate_rhs_id(e->get_id());
        else
            lib->generate_vertex_prop_access_rhs(e->get_field()->get_second(), Body);

        Body.pushln(";");
        return true;
    }

private:
    ast_sentblock* sb;
    gm_symtab_entry* sym;
    gm_gps_comm_unit* U;
    gm_gps_beinfo* INFO;
    gm_charm_lib* lib;
    gm_code_writer& Body;

};
const char* gm_charm_lib::get_random_write_message_name(gm_symtab_entry *sym) {
	char temp[1024];
	sprintf(temp, "_msg_%s", sym->getId()->get_genname());
	return gm_strdup(temp); 
}

void gm_charm_lib::generate_message_payload_packing_for_random_write(ast_assign* a, gm_code_writer& Body) {
	char temp [1024];
	Body.pushln("// send random-write message to ... ");

	gm_gps_basic_block *b = (gm_gps_basic_block *)a->get_basic_block();
	char *entry_name = generate_vertex_entry_method_name(b);

	sprintf(temp, "%s_recv_msg *_msg = new %s_recv_msg();", 
			entry_name, entry_name);
	Body.pushln(temp);

	ast_sentblock* sb = (ast_sentblock*) a->find_info_ptr(GPS_FLAG_SENT_BLOCK_FOR_RANDOM_WRITE_ASSIGN);
	assert(sb!=NULL);

	// driver
	gm_symtab_entry* sym = a->get_lhs_field()->get_first()->getSymInfo();
	// traverse rhs and put values in the message
	//printf("sb:%p, sym:%p\n", sb, sym);
	charm_random_write_rhs_t T(sb, sym, this, Body);
	a->get_rhs()->traverse_post(&T);

	if (sb->has_info_set(GPS_FLAG_RANDOM_WRITE_SYMBOLS_FOR_SB)) {
		std::set<void*> S = sb->get_info_set(GPS_FLAG_RANDOM_WRITE_SYMBOLS_FOR_SB);
		std::set<void*>::iterator I;
		assert(S.size()==1);
		for (I = S.begin(); I != S.end(); I++) {
			gm_symtab_entry* sym = (gm_symtab_entry*) *I;
			//get_lib()->generate_message_create_for_random_write(sb, sym, Body);
			sprintf(temp, "thisProxy[%s].%s_recv(_msg);", sym->getId()->get_genname(), entry_name);
			Body.push(temp);
		}
		Body.NL();
	}

	//assert(false);
	delete [] entry_name;
}
