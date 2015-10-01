/**/
#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "types.h"
#include "env.h"

S_table E_base_tenv(void) {
	S_table init_t = S_empty();
	S_enter(init_t, S_Symbol("int"), Ty_Int());
	S_enter(init_t, S_Symbol("string"), Ty_String());
	S_enter(init_t, S_Symbol("double"), Ty_Double());
	return init_t;
}

S_table E_base_venv(void) {
	S_table t = S_empty();
	S_enter(
    t,
    S_Symbol("print"),
    E_FunEntry(Ty_TyList(Ty_String(), NULL), Ty_Void())
  	);
  S_enter(
    t,
    S_Symbol("flush"),
    E_FunEntry(NULL, Ty_Void())
  	);
  S_enter(
    t,
    S_Symbol("getchar"),
    E_FunEntry(NULL, Ty_String())
  	);
  S_enter(
    t,
    S_Symbol("ord"),
    E_FunEntry(Ty_TyList(Ty_String(), NULL), Ty_Int())
  	);
  S_enter(
    t,
    S_Symbol("chr"),
    E_FunEntry(Ty_TyList(Ty_Int(), NULL), Ty_String())
  	);
  S_enter(
    t,
    S_Symbol("size"),
    E_FunEntry(Ty_TyList(Ty_String(), NULL), Ty_Int())
  	);
  S_enter(
    t,
    S_Symbol("substring"),
    E_FunEntry(Ty_TyList(Ty_String(),
                         Ty_TyList(Ty_Int(),
                                   Ty_TyList(Ty_Int(), NULL))),
    Ty_String())
  	);
  S_enter(
    t,
    S_Symbol("concat"),
    E_FunEntry(Ty_TyList(Ty_String(),
                         Ty_TyList(Ty_String(), NULL)),
    Ty_String())
  	);
  S_enter(
    t,
    S_Symbol("not"),
    E_FunEntry(Ty_TyList(Ty_Int(), NULL), Ty_Int())
  	);
  S_enter(
    t,
    S_Symbol("exit"),
    E_FunEntry(Ty_TyList(Ty_Int(), NULL), Ty_Void())
  	);
	return t;
}

E_enventry E_VarEntry(Ty_ty ty) {
	E_enventry final;
	final = checked_malloc(sizeof(*final));
	final->kind = E_varEntry;
	final->u.var.ty = ty;
	return final;
}

E_enventry E_FunEntry(Ty_tyList fms, Ty_ty resl) {
	E_enventry final = checked_malloc(sizeof(*final));
	final->kind = E_funEntry;
	final->u.fun.formals = fms;
	final->u.fun.result  = resl;
	return final;
}
