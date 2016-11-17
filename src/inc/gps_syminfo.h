#ifndef GPS_SYMINFO
#define GPS_SYMINFO

#include "gm_typecheck.h"
#include "gm_traverse.h"

// where the symbol is defined
enum gm_gps_scope_t
{
    GPS_SCOPE_GLOBAL, GPS_SCOPE_OUTER, GPS_SCOPE_INNER
};

// how the symbols is used (LHS/RHS/REDUCE) 
enum gm_gps_symbol_usage_t
{
    GPS_SYM_USED_AS_RHS, GPS_SYM_USED_AS_LHS, GPS_SYM_USED_AS_REDUCE,
};

// where the symbols is used 
enum gm_gps_symbol_usage_location_t
{
    GPS_CONTEXT_MASTER, GPS_CONTEXT_VERTEX, GPS_CONTEXT_RECEIVER
};

// symbol usage info
static const char* GPS_TAG_BB_USAGE = "GPS_TAG_BB";

class gps_syminfo : public ast_extra_info
{
public:
    gps_syminfo(bool sc) :
            _scalar(sc), _is_arg(false), _used_in_multiple_BB(false), last_BB(-1), used_as_rhs(false), used_as_lhs(false), used_as_reduce(false), used_in_vertex(
                    false), used_in_master(false), used_in_receiver(false), reduce_op_type(GMREDUCE_NULL), start_byte(0), scope(GPS_SCOPE_GLOBAL) {
    }
    virtual ~gps_syminfo() {
    }

    void set_is_argument(bool b) {
        _is_arg = b;
    }
    bool is_argument() {
        return _is_arg;
    }
    bool is_scalar() {
        return _scalar;
    }
    void add_usage_in_BB(int bb_no, int usage, int context,  // MASTER, VERTEX, SENDER, RECEIVER
            int reduce_type = GMREDUCE_NULL);

    bool test_usage_in_BB(int bb_no, int usage, int context,  // MASTER, VERTEX, SENDER, RECEIVER
            int reduce_type = GMREDUCE_NULL);

    bool is_used_in_multiple_BB() {
        return _used_in_multiple_BB;
    }

    bool is_used_as_rhs() {
        return used_as_rhs;
    }
    bool is_used_as_lhs() {
        return used_as_lhs;
    }
    bool is_used_as_reduce() {
        return used_as_reduce;
    }

    bool is_used_in_vertex() {
        return used_in_vertex;
    }
    bool is_used_in_master() {
        return used_in_master;
    }
    bool is_used_in_receiver() {
        return used_in_receiver;
    }

    int get_reduce_type() {
        return reduce_op_type;
    }

    // for message/state
    int get_start_byte() {
        return start_byte;
    }
    void set_start_byte(int b) {
        start_byte = b;
    }

    // where the symbol is defined?
    int get_scope() {
        return scope;
    }
    void set_scope(int s) {
        scope = s;
    }

    bool is_scoped_global() {
        return scope == GPS_SCOPE_GLOBAL;
    }
    bool is_scoped_outer() {
        return scope == GPS_SCOPE_OUTER;
    }
    bool is_scoped_inner() {
        return scope == GPS_SCOPE_INNER;
    }

private:
    bool _used_in_multiple_BB;
    int last_BB;
    bool _scalar;
    bool _is_arg;
    std::vector<int> used_BB;
    std::vector<int> used_type;
    std::vector<int> used_context;
    int scope; // GPS_SCOPE_XX

    bool used_as_rhs;
    bool used_as_lhs;
    bool used_as_reduce;

    bool used_in_vertex;
    bool used_in_master;
    //bool used_in_sender;
    bool used_in_receiver;

    int reduce_op_type;
    int start_byte;
};

inline gps_syminfo* gps_get_global_syminfo(ast_id* i) {
    return (gps_syminfo*) i->getSymInfo()->find_info(GPS_TAG_BB_USAGE);
}

class gps_flat_symbol_t : public gm_apply
{
public:
    gps_flat_symbol_t(std::set<gm_symtab_entry*>& s, std::set<gm_symtab_entry*>& p, std::set<gm_symtab_entry*>& e) :
            scalar(s), prop(p), edge_prop(e) {
        set_for_symtab(true);
    }

    virtual bool apply(gm_symtab_entry* sym, int symtab_type) {
        ast_extra_info* info = sym->find_info(GPS_TAG_BB_USAGE);
        if (info == NULL) return true; // no information

        gps_syminfo* syminfo = (gps_syminfo*) info;
        if (syminfo->is_scalar()) {
            // ignore iterator and graph
            if (sym->getType()->is_graph() || sym->getType()->is_node_edge_iterator()) {
                return true;
            }

            if (symtab_type == GM_SYMTAB_ARG) {
                syminfo->set_is_argument(true);
                scalar.insert(sym);
            //} else if (syminfo->is_used_in_multiple_BB() && syminfo->is_scoped_global()) {
            } else if (syminfo->is_scoped_global()) {
                scalar.insert(sym);
            } else {
                // temporary variables. can be ignored
            }
        } else {
            if (sym->getType()->is_node_property()) {
                prop.insert(sym);
            } else if (sym->getType()->is_edge_property()) {
                edge_prop.insert(sym);
            } else {
                printf("sym = %s\n", sym->getId()->get_genname());
                assert(false);
            }

            /*
             if (syminfo->is_argument())
             {
             prop.insert(sym);
             }
             else if (syminfo->is_used_in_multiple_BB()){
             prop.insert(sym);
             } else {
             //assert(false);
             prop.insert(sym);
             }
             */
        }

        return true;
    }
private:
    std::set<gm_symtab_entry*>& scalar;
    std::set<gm_symtab_entry*>& prop;
    std::set<gm_symtab_entry*>& edge_prop;
};

#endif
