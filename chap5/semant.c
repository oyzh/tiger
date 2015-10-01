#include <stdio.h>
#include "util.h"
#include "errormsg.h"
#include "symbol.h"
#include "absyn.h"
#include "types.h"
#include "translate.h"
#include "env.h"
#include "semant.h"

static struct expty transVar(S_table venv, S_table tenv, A_var v);
static struct expty transExp(S_table venv, S_table tenv, A_exp e);
static void         transDec(S_table venv, S_table tenv, A_dec d);
static Ty_ty        transTy (              S_table tenv, A_ty  t);
static Ty_tyList    makeFormalTyList(S_table t, A_fieldList p);
static Ty_ty        actual_ty(Ty_ty ty);
static bool args_match(S_table v, S_table t, A_expList el, Ty_tyList fl, A_exp fun); 
static bool ty_match(Ty_ty t1, Ty_ty t2);
static bool efields_match(S_table v, S_table t, Ty_ty ty, A_exp e);
static Ty_fieldList makeFieldTys(S_table t, A_fieldList fs);

void SEM_transProg(A_exp exp){
	struct expty et;

	S_table t = E_base_tenv();
	S_table v = E_base_venv();
	et = transExp(v, t, exp);
	printf("@this expr return: %d\n", et.ty->kind); /* check the return result (use Ty_ty->kind stand) */
}

static struct expty transVar(S_table venv, S_table tenv, A_var v) {
	E_enventry x;
	struct expty et,et2;
	Ty_fieldList fl;

	switch (v->kind) {
	case A_simpleVar:/* var id (a)*/
		x = S_look(venv, v->u.simple);
		if (x && x->kind == E_varEntry) {
			return expTy(NULL, actual_ty(x->u.var.ty));
		} else {
			EM_error(v->pos, "undefined var %s", S_name(v->u.simple));
			return expTy(NULL, Ty_Int());
		}
		break;
	case A_fieldVar:/* var record (a.b)*/
		et = transVar(venv, tenv, v->u.field.var);
		if (et.ty->kind != Ty_record) {
			EM_error(v->pos, "not a record type");
			return expTy(NULL, Ty_Record(NULL));
		} else {
			for (fl = et.ty->u.record; fl; fl = fl->tail) {
				if (fl->head->name == v->u.field.sym) {
					return expTy(NULL, actual_ty(fl->head->ty));
				}
			}
			EM_error(v->pos, "no such field in record %s", S_name(v->u.field.sym));
		}
		return expTy(NULL, Ty_Record(NULL));
		break;
	case A_subscriptVar:/*var array (a[b])*/ 
		et  = transVar(venv, tenv, v->u.subscript.var);
		if (et.ty->kind != Ty_array) {
			EM_error(v->pos, "not a array type");
		} else {
			et2 = transExp(venv, tenv, v->u.subscript.exp);
			if (et2.ty->kind != Ty_int) {
				EM_error(v->pos, "int required");
			} else {
				return expTy(NULL, actual_ty(et.ty->u.array));
			}
		}
		return expTy(NULL, Ty_Array(NULL));
	default:
		assert(0);
	}
}

static struct expty transExp(S_table v, S_table t, A_exp e){
	A_oper oper;
	struct expty left, right, final, final2, final3, final4, final5, lo, hi;
	A_expList list;
	A_decList decs;
	E_enventry callinfo;
	Ty_ty recty, arrayty;

	switch (e->kind) {
	case A_varExp:
		return transVar(v, t, e->u.var);
	case A_nilExp:
		return expTy(NULL, Ty_Nil());
	case A_callExp:
		callinfo = S_look(v, e->u.call.func); /*get params and return from tenv*/
		if (callinfo && callinfo->kind == E_funEntry){
			if (args_match(v, t, e->u.call.args, callinfo->u.fun.formals, e)) {/*check params is matched*/
				if (callinfo->u.fun.result) {
					return expTy(NULL, actual_ty(callinfo->u.fun.result));
				} else {
					return expTy(NULL, Ty_Void());
				}
			} 
		} else {
			EM_error(e->pos, "undefined function %s\n", S_name(e->u.call.func));
		}
		return expTy(NULL, Ty_Void());
	case A_recordExp:
		recty = actual_ty(S_look(t, e->u.record.typ));
	    if (!recty) { /*cant find record-type in table tenv*/ 
			EM_error(e->pos, "undefined type %s (debug recordExp)", S_name(e->u.record.typ)); 
		} else {
			if (recty->kind != Ty_record){
				EM_error(e->pos, "%s is not a record type", S_name(e->u.record.typ));	
				return expTy(NULL, Ty_Record(NULL));
			}
			if (efields_match(v, t, recty, e)) {/*check record field is matched*/
				return expTy(NULL, recty);
			}
		}
		return expTy(NULL, Ty_Record(NULL));
	case A_arrayExp:
		arrayty = actual_ty(S_look(t, e->u.array.typ));
		if (!arrayty) {
			EM_error(e->pos, "undeined array type %s", S_name(e->u.array.typ));
			return expTy(NULL, Ty_Array(NULL));
		}
		if (arrayty->kind != Ty_array) {
			EM_error(e->pos, "%s is not a array type", S_name(e->u.array.typ));
			return expTy(NULL, Ty_Array(NULL));
		}
	    final2 = transExp(v, t, e->u.array.size);
		final3 = transExp(v, t, e->u.array.init);
		if (final2.ty->kind != Ty_int) {
			EM_error(e->pos, "array size should be int %s", S_name(e->u.array.typ));
		} else if (!ty_match(final3.ty, arrayty->u.array)){
			EM_error(e->pos, "unmatched array type in %s", S_name(e->u.array.typ));
		} else {
			return expTy(NULL, arrayty);
		}
		return expTy(NULL, Ty_Array(NULL));
	case A_seqExp:
		list = e->u.seq;
		if (!list) {
			return expTy(NULL, Ty_Void());
		}
		while (list->tail) {
			transExp(v, t, list->head);
			list = list->tail;
		}
		return transExp(v, t, list->head);
	case A_whileExp:
		final = transExp(v, t, e->u.whilee.test);
		if (final.ty->kind != Ty_int) {
			EM_error(e->pos, "int required");
		}
		transExp(v, t, e->u.whilee.body);
		return expTy(NULL, Ty_Void());
	case A_assignExp:
		final4 = transVar(v, t, e->u.assign.var);
		final5 = transExp(v, t, e->u.assign.exp);
		if (!ty_match(final4.ty, final5.ty)) {
			EM_error(e->pos, "unmatched assign exp");
		}
		return expTy(NULL, Ty_Void());
	case A_breakExp:
		return expTy(NULL, Ty_Void());
	case A_forExp:
		{
		struct expty lo = transExp(v, t, e->u.forr.lo);
		struct expty hi = transExp(v, t, e->u.forr.hi);
		struct expty body;

		if (lo.ty != Ty_Int() || hi.ty != Ty_Int()) {
			EM_error(e->pos, "low or high range type is not integer");
		}

		S_beginScope(v);
		transDec(v, t, A_VarDec(e->pos, e->u.forr.var, S_Symbol("int"), e->u.forr.lo));
		body = transExp(v, t, e->u.forr.body);
		S_endScope(v);
		return expTy(NULL, Ty_Void());
		}
	case A_letExp:
		S_beginScope(v);
		S_beginScope(t);
		for (decs = e->u.let.decs; decs; decs = decs->tail) {
			transDec(v, t, decs->head);
		}
		final = transExp(v, t, e->u.let.body);
		S_endScope(v);
		S_endScope(t);
		return final;
	case A_opExp:
		oper = e->u.op.oper;
		left  = transExp(v, t, e->u.op.left); 
		right = transExp(v, t, e->u.op.right);
		if (0 <= oper && oper < 4) {/* check +,-,*,/ */
			if (left.ty->kind != Ty_int && left.ty->kind != Ty_double){
				EM_error(e->u.op.left->pos, "int or double required(op)");	
			}
			if (right.ty->kind != Ty_int && right.ty->kind != Ty_double) {
				EM_error(e->u.op.right->pos, "int or double required(op)");	
			}
			if (left.ty->kind == Ty_int && right.ty->kind == Ty_int && oper != 3) {
				return expTy(NULL, Ty_Int());
			} else {
				/*TODO  divide when return double when return int*/
				/*
				return expTy(NULL, Ty_Int());
				*/
				return expTy(NULL, Ty_Double());
			}
		} else if (3 < oper && oper < 10) {
			if (oper == 4 || oper == 5) {/*check record type can be nil*/
				if (left.ty->kind == Ty_record && right.ty->kind == Ty_nil) {
					return expTy(NULL, Ty_Int());
				}
				if (left.ty->kind == Ty_nil && right.ty->kind == Ty_record) {
					return expTy(NULL, Ty_Int());
				}
			}
			if(left.ty->kind != Ty_int && left.ty->kind != Ty_double && left.ty->kind != Ty_string){
				EM_error(e->u.op.left->pos, "int or double or record-nil required");	
			}
			if (right.ty->kind != Ty_int && right.ty->kind != Ty_double && right.ty->kind !=Ty_string) {
				EM_error(e->u.op.right->pos, "int or double or record-nil required");	
			}
			return expTy(NULL, Ty_Int());
		} else {
			assert(0);	
		}
	case A_ifExp:
		final = transExp(v, t, e->u.iff.test);
		final2 = transExp(v, t, e->u.iff.then);
		if (e->u.iff.elsee) { /*no else-part*/
			final3 = transExp(v, t, e->u.iff.elsee);
			if (final.ty->kind != Ty_int){
				EM_error(e->u.iff.test->pos, "int required");
			} else if(!ty_match(final2.ty, final3.ty)) {
				EM_error(e->pos, "if-else sentence must return same type");
			} else { }
		}
		return expTy(NULL, final2.ty);
	case A_stringExp:
		return expTy(NULL, Ty_String());
	case A_intExp:
		return expTy(NULL, Ty_Int());
	case A_doubleExp:
		return expTy(NULL, Ty_Double());
	default:
		assert(0);
	}
}

static void transDec(S_table v, S_table t, A_dec d) {
	struct expty final;
	A_fundec f;
	Ty_ty resTy, namety, isname;
	Ty_tyList formalTys, s;
	A_fieldList l;
	A_nametyList nl;
	A_fundecList fcl;
	E_enventry fun;
	int iscyl, isset;

	switch (d->kind) {
	case A_varDec:
		final = transExp(v, t, d->u.var.init);
		if (!d->u.var.typ) {/*unpoint type*/
			if (final.ty->kind == Ty_nil || final.ty->kind == Ty_void) {
				/*why keep void type ??*/
				EM_error(d->pos, "init should not be nil without type in %s", S_name(d->u.var.var));
				S_enter(v, d->u.var.var, E_VarEntry(Ty_Int()));
			} else {
				S_enter(v, d->u.var.var, E_VarEntry(final.ty));
			}
		} else {
			resTy = S_look(t, d->u.var.typ);
			if (!resTy) {
				EM_error(d->pos, "undifined type %s", S_name(d->u.var.typ));
			} else {
				if (!ty_match(resTy, final.ty)) {
					EM_error(d->pos, "unmatched type in %s", S_name(d->u.var.typ));
					S_enter(v, d->u.var.var, E_VarEntry(resTy));
				} else {
					S_enter(v, d->u.var.var, E_VarEntry(resTy));
				}
			}
			
		}
		break;
	case A_functionDec:
		for (fcl = d->u.function; fcl; fcl = fcl->tail) {
			if (fcl->head->result) {
				resTy = S_look(t, fcl->head->result);
				if (!resTy) {
					EM_error(fcl->head->pos, "undefined type for return type");
					resTy = Ty_Void();
				} 
			} else {
				resTy = Ty_Void();
			}
			formalTys = makeFormalTyList(t, fcl->head->params);
			S_enter(v, fcl->head->name, E_FunEntry(formalTys, resTy));
		}

		for (fcl = d->u.function; fcl; fcl = fcl->tail) {
			f = fcl->head;
			S_beginScope(v);
			formalTys = makeFormalTyList(t, f->params);
			for (l = f->params, s = formalTys; l && s; l = l->tail, s = s->tail) {
				S_enter(v, l->head->name, E_VarEntry(s->head));
			}
			final = transExp(v, t, f->body);
			fun = S_look(v, f->name);
			if (!ty_match(fun->u.fun.result, final.ty)) {
				EM_error(f->pos, "incorrect return type in function '%s'", S_name(f->name));
			}
			S_endScope(v);
		}
		break;
	case A_typeDec:
		for (nl = d->u.type; nl; nl = nl->tail) {
			S_enter(t, nl->head->name, Ty_Name(nl->head->name,NULL));
		} /* add name to tenv, Ty_ty set NULL*/
		iscyl = TRUE;
		for (nl = d->u.type; nl; nl = nl->tail) {
			resTy = transTy(t, nl->head->ty);
			if (iscyl) {
				if (resTy->kind != Ty_name) {
					iscyl = FALSE;
				}
			}
			if (!nl->tail && resTy->kind != Ty_name && isset) {
				/*line num is some bug*/
				EM_error(d->pos,"warning: actual type should declare brefore nameTy type");
			}
			namety = S_look(t, nl->head->name);
			namety->u.name.ty = resTy;
		}
		if (iscyl) EM_error(d->pos,"illegal type cycle: cycle must contain record, array");
		break;
	default:
		assert(0);
	}
}

static Ty_ty transTy(S_table tb, A_ty ty) {
	Ty_ty final = NULL;
	Ty_fieldList fieldTys;

	switch (ty->kind) {
	case A_nameTy:
		final = S_look(tb, ty->u.name);
		if (!final) {
			EM_error(ty->pos, "undefined type %s", S_name(ty->u.name));
			return Ty_Int();
		}
		return final;
	case A_recordTy:
		fieldTys = makeFieldTys(tb, ty->u.record);
		return Ty_Record(fieldTys);
	case A_arrayTy:
		final = S_look(tb, ty->u.array);
		if (!final) EM_error(ty->pos, "undefined type %s", S_name(ty->u.array));
		return Ty_Array(final);
	default:
		assert(0);
	}
}

static Ty_fieldList makeFieldTys(S_table t, A_fieldList fs) {
	A_fieldList f;
	Ty_fieldList tys = NULL, head;
	Ty_ty ty;
	Ty_field tmp;

	for (f = fs; f; f = f->tail) {
		ty = S_look(t, f->head->typ);
		if (!ty) {
			EM_error(f->head->pos, "undefined type %s", S_name(f->head->typ));
		} else {
		tmp = Ty_Field(f->head->name, ty);
		if (tys) {
			tys->tail = Ty_FieldList(tmp, NULL);
			tys = tys->tail;
		} else {
			tys = Ty_FieldList(tmp, NULL);
			head = tys;
		}

		}
	}
	return head;
}

static Ty_tyList makeFormalTyList(S_table t, A_fieldList fl) {
	Ty_tyList final = NULL, head = final;
	A_fieldList l = fl;
	Ty_ty ty;

	for (; l; l = l->tail) {
		ty = S_look(t, l->head->typ);
		if(!ty) {
			EM_error(l->head->pos, "undefined type %s", S_name(l->head->typ));
			ty = Ty_Int();
		}
		if (!final) {
			final = Ty_TyList(ty, NULL);
			head = final;
		} else {
			final->tail = Ty_TyList(ty, NULL);
			final = final->tail;
		}
	}
	return head;
}

static Ty_ty actual_ty(Ty_ty ty){
	if (!ty) return ty;
	if (ty->kind == Ty_name) actual_ty(ty->u.name.ty);
	else return ty;
}

static bool args_match(S_table v, S_table tt, A_expList ell, Ty_tyList fll, A_exp fun) {
	struct expty t;
	A_expList el = ell;
	Ty_tyList fl = fll;

	while (el && fl) {
		t = transExp(v, tt, el->head);
		if (!ty_match(t.ty, fl->head)){
			EM_error(fun->pos, "unmatched params in function %s\n", S_name(fun->u.call.func));
			return FALSE;
		}
		el = el->tail;
		fl = fl->tail;
	}
	if (el && !fl) {
		EM_error(fun->pos,"too many params in function %s\n", S_name(fun->u.call.func));
		return FALSE;
	} else if (!el && fl) {
		EM_error(fun->pos, "less params in function %s\n", S_name(fun->u.call.func));
		return FALSE;
	} else {
		return TRUE;
	}
}

static bool ty_match(Ty_ty tt, Ty_ty ee) {
	Ty_ty t = actual_ty(tt);
	Ty_ty e = actual_ty(ee);
	int tk = t->kind;
	int ek = e->kind;

	return (((tk == Ty_record || tk == Ty_array) && t == e) ||
			 (tk == Ty_record && ek == Ty_nil) ||
			 (ek == Ty_record && tk == Ty_nil) ||
			 (tk != Ty_record && tk != Ty_array && tk == ek));
}

static bool efields_match(S_table v, S_table t, Ty_ty tyy, A_exp e) {
	struct expty et;
	Ty_fieldList ty = tyy->u.record;
	A_efieldList fl = e->u.record.fields;
	while (ty && fl) {
		et = transExp(v, t, fl->head->exp);
		if (!(ty->head->name == fl->head->name) || !ty_match(ty->head->ty, et.ty)){
			EM_error(e->pos, "unmatched name: type in %s", S_name(e->u.record.typ));
			return FALSE;
		}
		ty = ty->tail;
		fl = fl->tail;
	}
	if (ty && !fl) {
		EM_error(e->pos, "less fields in %s", S_name(e->u.record.typ));
		return FALSE;
	} else if (!ty && fl) {
		EM_error(e->pos, "too many field in %s", S_name(e->u.record.typ));
		return FALSE;
	}
	return TRUE;
}
