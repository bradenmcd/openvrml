typedef union {
	char *string;
	VrmlField *field;
	VrmlNode *node;
        std::vector<VrmlNode *> *nodeList;
} YYSTYPE;
#define	IDENTIFIER	257
#define	DEF	258
#define	USE	259
#define	PROTO	260
#define	EXTERNPROTO	261
#define	TO	262
#define	IS	263
#define	ROUTE	264
#define	SFN_NULL	265
#define	EVENTIN	266
#define	EVENTOUT	267
#define	FIELD	268
#define	EXPOSEDFIELD	269
#define	SF_BOOL	270
#define	SF_COLOR	271
#define	SF_FLOAT	272
#define	SF_INT32	273
#define	SF_ROTATION	274
#define	SF_TIME	275
#define	SF_IMAGE	276
#define	SF_STRING	277
#define	SF_VEC2F	278
#define	SF_VEC3F	279
#define	MF_COLOR	280
#define	MF_FLOAT	281
#define	MF_INT32	282
#define	MF_ROTATION	283
#define	MF_STRING	284
#define	MF_VEC2F	285
#define	MF_VEC3F	286
#define	SF_NODE	287
#define	MF_NODE	288


extern YYSTYPE yylval;
