/* $Header$ */
/* EXPRESSION DESCRIPTOR */

/*	What we want to define is the struct expr, but since it contains
	a union of various goodies, we define them first; so be patient.
*/

struct value	{
	struct idf *vl_idf;		/* idf of an external name or 0 */
	arith vl_value;			/* constant, or offset if idf != 0 */
};

struct string	{
	char *sg_value;		/* string of characters repr. the constant */
	label sg_datlab;	/* global data-label			*/
};

struct floating	{
	char *fl_value;		/* pointer to string repr. the fp const. */
	label fl_datlab;	/* global data_label	*/
};

struct oper	{
	struct type *op_type;	/* resulting type of the operation	*/
	struct expr *op_left;
	int op_oper;			/* the symbol of the operator	*/
	struct expr *op_right;
};

/* The following constants indicate the class of the expression: */
#define	Value	0		/* it is a value known at load time */
#define	String	1		/* it is a string constant  */
#define	Float	2		/* it is a floating point constant	*/
#define	Oper	3		/* it is a run-time expression */
#define	Type	4		/* only its type is relevant */

struct expr	{
	struct expr *next;
	char *ex_file;		/* the file it (probably) comes from */
	unsigned int ex_line;	/* the line it (probably) comes from */
	struct type *ex_type;
	char ex_lvalue;
	char ex_flags;
	int ex_class;
	int ex_depth;
	union	{
		struct value ex_value;
		struct string ex_string;
		struct floating ex_float;
		struct oper ex_oper;
	} ex_object;
};

/* some abbreviated selections	*/
#define	VL_VALUE	ex_object.ex_value.vl_value
#define	VL_IDF		ex_object.ex_value.vl_idf
#define	SG_VALUE	ex_object.ex_string.sg_value
#define	SG_DATLAB	ex_object.ex_string.sg_datlab
#define	FL_VALUE	ex_object.ex_float.fl_value
#define	FL_DATLAB	ex_object.ex_float.fl_datlab
#define	OP_TYPE		ex_object.ex_oper.op_type
#define	OP_LEFT		ex_object.ex_oper.op_left
#define	OP_OPER		ex_object.ex_oper.op_oper
#define	OP_RIGHT	ex_object.ex_oper.op_right

#define	EXPRTYPE(e)	((e)->ex_type->tp_fund)

/*	An expression is a `load-time constant' if it is of the form
	<idf> +/- <integral> or <integral>;
	it is a `compile-time constant' if it is an <integral>.
*/
#define	is_ld_cst(e)	((e)->ex_lvalue == 0 && (e)->ex_class == Value)
#define	is_cp_cst(e)	(is_ld_cst(e) && (e)->VL_IDF == 0)

/*	a floating constant expression ?
*/
#define	is_fp_cst(e)	((e)->ex_class == Float)

/*	some bits for the ex_flag field, to keep track of various
	interesting properties of an expression.
*/
#define	EX_SIZEOF	001		/* contains sizeof operator */
#define	EX_CAST		002		/* contains cast */
#define	EX_LOGICAL	004		/* contains logical operator */
#define	EX_COMMA	010		/* contains expression comma */
#define	EX_PARENS	020		/* the top level is parenthesized */

#define	NILEXPR		((struct expr *)0)

extern struct expr *intexpr(), *new_oper();


/* allocation definitions of struct expr */
/* ALLOCDEF "expr" */
extern char *st_alloc();
extern struct expr *h_expr;
#define	new_expr() ((struct expr *) \
		st_alloc((char **)&h_expr, sizeof(struct expr)))
#define	free_expr(p) st_free(p, h_expr, sizeof(struct expr))


#define	ISCOMMA(e) ((e)->ex_class == Oper && (e)->OP_OPER == INITCOMMA)