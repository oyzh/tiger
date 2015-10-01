typedef void * Tr_exp;

struct expty {Tr_exp exp; Ty_ty ty;};

struct expty expTy(Tr_exp e, Ty_ty t) {
	struct expty et;
	et.exp = e;
	et.ty  = t;
	return et;
}
