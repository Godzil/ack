/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*
 * Motorola 68020 keywords
 */

0,	SIZE,		SIZE_B,		".b",
0,	SIZE,		SIZE_W,		".w",
0,	SIZE,		SIZE_L,		".l",

0,	DREG,		00,		"d0",
0,	DREG,		01,		"d1",
0,	DREG,		02,		"d2",
0,	DREG,		03,		"d3",
0,	DREG,		04,		"d4",
0,	DREG,		05,		"d5",
0,	DREG,		06,		"d6",
0,	DREG,		07,		"d7",

0,	AREG,		00,		"a0",
0,	AREG,		01,		"a1",
0,	AREG,		02,		"a2",
0,	AREG,		03,		"a3",
0,	AREG,		04,		"a4",
0,	AREG,		05,		"a5",
0,	AREG,		06,		"a6",
0,	AREG,		07,		"a7",
0,	AREG,		07,		"sp",

0,	PC,		0,		"pc",
0,	PC,		0200,		"zpc",

0,	CREG,		04001,		"vbr",
0,	CREG,		0,		"sfc",
0,	CREG,		00001,		"dfc",
0,	CREG,		00002,		"cacr",
0,	CREG,		04002,		"caar",
0,	CREG,		04003,		"msp",
0,	CREG,		04004,		"isp",

0,	SPEC,		075,		"usp",
0,	SPEC,		076,		"ccr",
0,	SPEC,		077,		"sr",

0,	ABCD,		0140400,	"abcd",
0,	ABCD,		0100400,	"sbcd",

0,	ADDX,		0150400,	"addx",
0,	ADDX,		0110400,	"subx",

0,	ADD,		0153300,	"add",
0,	ADD,		0112700,	"sub",

0,	AND,		0141000,	"and",
0,	AND,		0135000,	"eor",
0,	AND,		0100000,	"or",

0,	BITOP,		0000,		"btst",
0,	BITOP,		0100,		"bchg",
0,	BITOP,		0200,		"bclr",
0,	BITOP,		0300,		"bset",

0,	BITFIELD,	0164300,	"bftst",
0,	BITFIELD,	0165300,	"bfchg",
0,	BITFIELD,	0166300,	"bfclr",
0,	BITFIELD,	0167300,	"bfset",
0,	BF_TO_D,	0164700,	"bfextu",
0,	BF_TO_D,	0164700,	"bfexts",
0,	BF_TO_D,	0164700,	"bfffo",
0,	BFINS,		0167700,	"bfins",

0,	SHIFT,		0160340,	"asr",
0,	SHIFT,		0160740,	"asl",
0,	SHIFT,		0161350,	"lsr",
0,	SHIFT,		0161750,	"lsl",
0,	SHIFT,		0162360,	"roxr",
0,	SHIFT,		0162760,	"roxl",
0,	SHIFT,		0163370,	"ror",
0,	SHIFT,		0163770,	"rol",

0,	SZ_EA,		041000|DTA|ALT,	"clr",
0,	SZ_EA,		042000|DTA|ALT,	"neg",
0,	SZ_EA,		040000|DTA|ALT,	"negx",
0,	SZ_EA,		043000|DTA|ALT,	"not",
0,	SZ_EA,		045000,		"tst",

0,	OP_EA,		044000|DTA|ALT,	"nbcd",
0,	OP_EA,		045300|DTA|ALT,	"tas",
0,	OP_EA,		047200|CTR,	"jsr",
0,	OP_EA,		047300|CTR,	"jmp",
0,	OP_EA,		044100|CTR,	"pea",

0,	OP_EA,		050300,		"st",
0,	OP_EA,		050700,		"sf",
0,	OP_EA,		051300,		"shi",
0,	OP_EA,		051700,		"sls",
0,	OP_EA,		052300,		"scc",
0,	OP_EA,		052700,		"scs",
0,	OP_EA,		053300,		"sne",
0,	OP_EA,		053700,		"seq",
0,	OP_EA,		054300,		"svc",
0,	OP_EA,		054700,		"svs",
0,	OP_EA,		055300,		"spl",
0,	OP_EA,		055700,		"smi",
0,	OP_EA,		056300,		"sge",
0,	OP_EA,		056700,		"slt",
0,	OP_EA,		057300,		"sgt",
0,	OP_EA,		057700,		"sle",

0,	OP_NOOP,	047160,		"reset",
0,	OP_NOOP,	047161,		"nop",
0,	OP_NOOP,	047163,		"rte",
0,	OP_NOOP,	047165,		"rts",
0,	OP_NOOP,	047166,		"trapv",
0,	OP_NOOP,	047167,		"rtr",
0,	OP_NOOP,	045374,		"illegal",

0,	PACK,		0100500,	"pack",
0,	PACK,		0100600,	"unpk",

0,	DIVMUL,		0100,		"divu",
0,	DIVMUL,		0101,		"divs",
0,	DIVMUL,		0000,		"mulu",
0,	DIVMUL,		0001,		"muls",

0,	DIVL,		046100,		"divul",
0,	DIVL,		046101,		"divsl",

0,	BR,		060000,		"bra",
0,	BR,		060400,		"bsr",
0,	BR,		061000,		"bhi",
0,	BR,		061400,		"bls",
0,	BR,		062000,		"bcc",
0,	BR,		062400,		"bcs",
0,	BR,		063000,		"bne",
0,	BR,		063400,		"beq",
0,	BR,		064000,		"bvc",
0,	BR,		064400,		"bvs",
0,	BR,		065000,		"bpl",
0,	BR,		065400,		"bmi",
0,	BR,		066000,		"bge",
0,	BR,		066400,		"blt",
0,	BR,		067000,		"bgt",
0,	BR,		067400,		"ble",

0,	DBR,		050310,		"dbt",
0,	DBR,		050710,		"dbf",
0,	DBR,		050710,		"dbra",
0,	DBR,		051310,		"dbhi",
0,	DBR,		051710,		"dbls",
0,	DBR,		052310,		"dbcc",
0,	DBR,		052710,		"dbcs",
0,	DBR,		053310,		"dbne",
0,	DBR,		053710,		"dbeq",
0,	DBR,		054310,		"dbvc",
0,	DBR,		054710,		"dbvs",
0,	DBR,		055310,		"dbpl",
0,	DBR,		055710,		"dbmi",
0,	DBR,		056310,		"dbge",
0,	DBR,		056710,		"dblt",
0,	DBR,		057310,		"dbgt",
0,	DBR,		057710,		"dble",

0,	OP_EXT,		044000,		"ext",
0,	OP_EXT,		044400,		"extb",

0,	OP_RANGE,	04000,		"chk2",
0,	OP_RANGE,	0,		"cmp2",

0,	TRAPCC,		050370,		"trapt",
0,	TRAPCC,		050770,		"trapf",
0,	TRAPCC,		051370,		"traphi",
0,	TRAPCC,		051770,		"trapls",
0,	TRAPCC,		052370,		"trapcc",
0,	TRAPCC,		052770,		"trapcs",
0,	TRAPCC,		053370,		"trapeq",
0,	TRAPCC,		053770,		"trapvc",
0,	TRAPCC,		054370,		"trapvs",
0,	TRAPCC,		054770,		"trappl",
0,	TRAPCC,		055370,		"trapmi",
0,	TRAPCC,		055770,		"trapge",
0,	TRAPCC,		056370,		"traplt",
0,	TRAPCC,		056770,		"trapgt",
0,	TRAPCC,		057370,		"traple",
0,	TRAPCC,		057770,		"trapne",

0,	CMP,		0,		"cmp",

0,	MOVE,		0,		"move",
0,	MOVESP,		0,		"movep",
0,	MOVEM,		0,		"movem",
0,	MOVESP,		1,		"moves",
0,	MOVEC,		0,		"movec",

0,	SWAP,		0,		"swap",

0,	LINK,		0,		"link",

0,	UNLK,		0,		"unlk",

0,	TRAP,		0,		"trap",

0,	OP_IMM,		047162,		"stop",

0,	CHK,		0,		"chk",

0,	RTM,		0,		"rtm",

0,	EXG,		0,		"exg",

0,	LEA,		0,		"lea",

0,	OP_IMM,		047164,		"rtd",

0,	BKPT,		0,		"bkpt",

0,	CALLM,		0,		"callm",

0,	CAS,		0,		"cas",

0,	CAS2,		0,		"cas2",

0,	CP,		00000,		"c0",
0,	CP,		01000,		"c1",
0,	CP,		02000,		"c2",
0,	CP,		03000,		"c3",
0,	CP,		04000,		"c4",
0,	CP,		05000,		"c5",
0,	CP,		06000,		"c6",
0,	CP,		07000,		"c7",

/* ???? what is this ???? */
0,	CPGEN,		0170000,	".gen",
0,	CPSCC,		0170100,	".s",
0,	CPDBCC,		0170110,	".db",
0,	CPTRAPCC,	0170170,	".trap",
0,	CPBCC,		0170200,	".br", /* 'r' to distinguish from SIZE_B
					        */
0,	CPSAVREST,	0170400,	".save",
0,	CPSAVREST,	0170500,	".restore",
/* ???? end of what is this ???? */

/* M68030 MMU registers */
0,	MREG,		0040000,	"tc",
0,	MREG,		0044000,	"srp",
0,	MREG,		0046000,	"crp",
0,	MREG,		0060000,	"mmusr",
0,	MREG,		0060000,	"psr",
0,	MREG,		0004000,	"tt0",
0,	MREG,		0006000,	"tt1",

/* M68030 MMU instructions */

0,	PFLUSHA,	0022000,	"pflusha",
0,	PFLUSH,		0020000,	"pflush",
0,	PLOAD,		0021000,	"ploadr",
0,	PLOAD,		0020000,	"ploadw",
0,	PTEST,		0101000,	"ptestr",
0,	PTEST,		0100000,	"ptestw",
0,	PMOVE,		0000000,	"pmove",
0,	PMOVE,		0000400,	"pmovefd",

/* floating point coprocessor ... */

0,	FSIZE,		FSIZE_S,	".s",
0,	FSIZE,		FSIZE_X,	".x",
0,	FSIZE,		FSIZE_P,	".p",
0,	FSIZE,		FSIZE_D,	".d",

0,	FPREG,		0,		"fp0",
0,	FPREG,		1,		"fp1",
0,	FPREG,		2,		"fp2",
0,	FPREG,		3,		"fp3",
0,	FPREG,		4,		"fp4",
0,	FPREG,		5,		"fp5",
0,	FPREG,		6,		"fp6",
0,	FPREG,		7,		"fp7",

0,	FPCR,		1,		"fpiar",
0,	FPCR,		2,		"fpsr",
0,	FPCR,		4,		"fpcr",

0,	FMOVE,		0,		"fmove",
0,	FMOVECR,	0,		"fmovecr",
0,	FMOVEM,		0,		"fmovem",

0,	FDYADIC,	042,		"fadd",
0,	FDYADIC,	070,		"fcmp",
0,	FDYADIC,	040,		"fdiv",
0,	FDYADIC,	041,		"fmod",
0,	FDYADIC,	043,		"fmul",
0,	FDYADIC,	045,		"frem",
0,	FDYADIC,	046,		"fscale",
0,	FDYADIC,	044,		"fsgldiv",
0,	FDYADIC,	047,		"fsglmul",
0,	FDYADIC,	050,		"fsub",

0,	FMONADIC,	030,		"fabs",
0,	FMONADIC,	034,		"facos",
0,	FMONADIC,	014,		"fasin",
0,	FMONADIC,	012,		"fatan",
0,	FMONADIC,	015,		"fatanh",
0,	FMONADIC,	035,		"fcos",
0,	FMONADIC,	031,		"fcosh",
0,	FMONADIC,	020,		"fetox",
0,	FMONADIC,	010,		"fetoxm1",
0,	FMONADIC,	036,		"fgetexp",
0,	FMONADIC,	037,		"fgetman",
0,	FMONADIC,	001,		"fint",
0,	FMONADIC,	003,		"fintrz",
0,	FMONADIC,	024,		"flogn",
0,	FMONADIC,	006,		"flognp1",
0,	FMONADIC,	025,		"flog10",
0,	FMONADIC,	026,		"flog2",
0,	FMONADIC,	032,		"fneg",
0,	FMONADIC,	016,		"fsin",
0,	FMONADIC,	002,		"fsinh",
0,	FMONADIC,	004,		"fsqrt",
0,	FMONADIC,	017,		"ftan",
0,	FMONADIC,	011,		"ftanh",
0,	FMONADIC,	022,		"ftentox",
0,	FMONADIC,	021,		"ftwotox",

0,	FSINCOS,	060,		"fsincos",

0,	FBCC,		001,		"fbeq",
0,	FBCC,		016,		"fbne",
0,	FBCC,		022,		"fbgt",
0,	FBCC,		035,		"fbngt",
0,	FBCC,		023,		"fbge",
0,	FBCC,		034,		"fbnge",
0,	FBCC,		024,		"fblt",
0,	FBCC,		033,		"fbnlt",
0,	FBCC,		025,		"fble",
0,	FBCC,		032,		"fbnle",
0,	FBCC,		026,		"fbgl",
0,	FBCC,		031,		"fbngl",
0,	FBCC,		027,		"fbgle",
0,	FBCC,		030,		"fbngle",
0,	FBCC,		002,		"fbogt",
0,	FBCC,		015,		"fbule",
0,	FBCC,		003,		"fboge",
0,	FBCC,		014,		"fbult",
0,	FBCC,		004,		"fbolt",
0,	FBCC,		013,		"fbuge",
0,	FBCC,		005,		"fbole",
0,	FBCC,		012,		"fbugt",
0,	FBCC,		006,		"fbogl",
0,	FBCC,		011,		"fbueq",
0,	FBCC,		007,		"fbor",
0,	FBCC,		010,		"fbun",
0,	FBCC,		000,		"fbf",
0,	FBCC,		017,		"fbt",
0,	FBCC,		020,		"fbsf",
0,	FBCC,		037,		"fbst",
0,	FBCC,		021,		"fbseq",
0,	FBCC,		036,		"fbsne",

0,	FDBCC,		001,		"fdbeq",
0,	FDBCC,		016,		"fdbne",
0,	FDBCC,		022,		"fdbgt",
0,	FDBCC,		035,		"fdbngt",
0,	FDBCC,		023,		"fdbge",
0,	FDBCC,		034,		"fdbnge",
0,	FDBCC,		024,		"fdblt",
0,	FDBCC,		033,		"fdbnlt",
0,	FDBCC,		025,		"fdble",
0,	FDBCC,		032,		"fdbnle",
0,	FDBCC,		026,		"fdbgl",
0,	FDBCC,		031,		"fdbngl",
0,	FDBCC,		027,		"fdbgle",
0,	FDBCC,		030,		"fdbngle",
0,	FDBCC,		002,		"fdbogt",
0,	FDBCC,		015,		"fdbule",
0,	FDBCC,		003,		"fdboge",
0,	FDBCC,		014,		"fdbult",
0,	FDBCC,		004,		"fdbolt",
0,	FDBCC,		013,		"fdbuge",
0,	FDBCC,		005,		"fdbole",
0,	FDBCC,		012,		"fdbugt",
0,	FDBCC,		006,		"fdbogl",
0,	FDBCC,		011,		"fdbueq",
0,	FDBCC,		007,		"fdbor",
0,	FDBCC,		010,		"fdbun",
0,	FDBCC,		000,		"fdbf",
0,	FDBCC,		017,		"fdbt",
0,	FDBCC,		020,		"fdbsf",
0,	FDBCC,		037,		"fdbst",
0,	FDBCC,		021,		"fdbseq",
0,	FDBCC,		036,		"fdbsne",

0,	FNOP,		0,		"fnop",

0,	FSCC,		001,		"fseq",
0,	FSCC,		016,		"fsne",
0,	FSCC,		022,		"fsgt",
0,	FSCC,		035,		"fsngt",
0,	FSCC,		023,		"fsge",
0,	FSCC,		034,		"fsnge",
0,	FSCC,		024,		"fslt",
0,	FSCC,		033,		"fsnlt",
0,	FSCC,		025,		"fsle",
0,	FSCC,		032,		"fsnle",
0,	FSCC,		026,		"fsgl",
0,	FSCC,		031,		"fsngl",
0,	FSCC,		027,		"fsgle",
0,	FSCC,		030,		"fsngle",
0,	FSCC,		002,		"fsogt",
0,	FSCC,		015,		"fsule",
0,	FSCC,		003,		"fsoge",
0,	FSCC,		014,		"fsult",
0,	FSCC,		004,		"fsolt",
0,	FSCC,		013,		"fsuge",
0,	FSCC,		005,		"fsole",
0,	FSCC,		012,		"fsugt",
0,	FSCC,		006,		"fsogl",
0,	FSCC,		011,		"fsueq",
0,	FSCC,		007,		"fsor",
0,	FSCC,		010,		"fsun",
0,	FSCC,		000,		"fsf",
0,	FSCC,		017,		"fst",
0,	FSCC,		020,		"fssf",
0,	FSCC,		037,		"fsst",
0,	FSCC,		021,		"fsseq",
0,	FSCC,		036,		"fssne",

0,	FTST,		0,		"ftst",

0,	FSAVRES,	MEM|ALT|0430,	"fsave",
0,	FSAVRES,	MEM|0540,	"frestore",

0,	FTRAPCC,	001,		"ftrapeq",
0,	FTRAPCC,	016,		"ftrapne",
0,	FTRAPCC,	022,		"ftrapgt",
0,	FTRAPCC,	035,		"ftrapngt",
0,	FTRAPCC,	023,		"ftrapge",
0,	FTRAPCC,	034,		"ftrapnge",
0,	FTRAPCC,	024,		"ftraplt",
0,	FTRAPCC,	033,		"ftrapnlt",
0,	FTRAPCC,	025,		"ftraple",
0,	FTRAPCC,	032,		"ftrapnle",
0,	FTRAPCC,	026,		"ftrapgl",
0,	FTRAPCC,	031,		"ftrapngl",
0,	FTRAPCC,	027,		"ftrapgle",
0,	FTRAPCC,	030,		"ftrapngle",
0,	FTRAPCC,	002,		"ftrapogt",
0,	FTRAPCC,	015,		"ftrapule",
0,	FTRAPCC,	003,		"ftrapoge",
0,	FTRAPCC,	014,		"ftrapult",
0,	FTRAPCC,	004,		"ftrapolt",
0,	FTRAPCC,	013,		"ftrapuge",
0,	FTRAPCC,	005,		"ftrapole",
0,	FTRAPCC,	012,		"ftrapugt",
0,	FTRAPCC,	006,		"ftrapogl",
0,	FTRAPCC,	011,		"ftrapueq",
0,	FTRAPCC,	007,		"ftrapor",
0,	FTRAPCC,	010,		"ftrapun",
0,	FTRAPCC,	000,		"ftrapf",
0,	FTRAPCC,	017,		"ftrapt",
0,	FTRAPCC,	020,		"ftrapsf",
0,	FTRAPCC,	037,		"ftrapst",
0,	FTRAPCC,	021,		"ftrapseq",
0,	FTRAPCC,	036,		"ftrapsne",
