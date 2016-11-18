#include "gm_backend_charm.h"
#include "gm_frontend.h"

void gm_charm_gen::begin_class(char *name) {
		char temp[1024];
    Body.push("class ");
    Body.push(name);
    Body.push(": public ");
		sprintf(temp, "CBase_%s", name);
		Body.push(temp);
    Body.push(" {");
    Body.NL();
}

void gm_charm_gen::end_class(char *name) {
	  char temp[1024];
		sprintf(temp, "}; // %s", name);
    Body.push(temp);
    Body.NL();
}

void gm_charm_gen::begin_struct(char *name) {
    Body.push("struct ");
    Body.push(name);
    Body.push(" {");
    Body.NL();
}

void gm_charm_gen::end_struct(char *name) {
    Body.pushln("};");
}


void gm_charm_gen::begin_module(char *name, bool is_mainmodule) {
		if (is_mainmodule) 
			Body_ci.push("mainmodule ");
		else
			Body_ci.push("module ");
    Body_ci.push(name);
    Body_ci.push(" {");
    Body_ci.NL();
}

void gm_charm_gen::end_module(char *name) {
	  char temp[1024];
		sprintf(temp, "}; // %s", name);
    Body_ci.push(temp);
    Body_ci.NL();
}

void gm_charm_gen::begin_chare_array(char *name, int dim ) {
	char temp[1024];
	sprintf(temp, "array [%dD] %s {", dim, name);
	Body_ci.pushln(temp);
}

void gm_charm_gen::end_chare_array(char *name) {
	  char temp[1024];
		sprintf(temp, "}; // %s", name);
    Body_ci.pushln(temp);
}

void gm_charm_gen::begin_chare(char *name, bool is_mainchare) {
		if (is_mainchare) 
			Body_ci.push("mainchare ");
		else
			Body_ci.push("chare ");
    Body_ci.push(name);
    Body_ci.push(" {");
    Body_ci.NL();
}

void gm_charm_gen::end_chare(char *name) {
	  char temp[1024];
		sprintf(temp, "}; // %s", name);
    Body_ci.push(temp);
    Body_ci.NL();
}
