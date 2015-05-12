// $Id$
// $Locker$

// Author. Roar Thron�s.

#define HOST_BITS_PER_WIDE_INT 64
#define POINTER_SIZE 32
#define true 1
#define false 0

#include <cli$routines.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int vms_mm = 0;

#include "tree.h"
#include "cld.h"

#include "cli.h"

extern char *strndup(__const char *__string, size_t __n);

int gencode_value_clauses(tree t, struct _cdu * cdu);
int gencode_qual_clauses(tree t, struct _cdu * cdu);
int gencode_para_clauses(tree t, struct _cdu * cdu);
int gencode_keyw_clauses(tree t, struct _cdu * cdu);
int gencode_single_clause(tree t, struct _cdu * cdu);
int gencode_cliflags(tree t, struct _cdu * cdu);
int gencode_type(tree t, struct _cdu * cdu);
int gencode_verb(tree t, struct _cdu * cdu);
genwrite();
int gencode(tree t);

main(int argc, char ** argv)
{
    char * name = 0;
    if (argc == 2)
        name = argv[1];
    extern int yydebug;
    yydebug = 1;
    extern FILE *yyin;
    init_stringpool();
    yyin = fopen(name, "r");
    if (yyin == 0)
    {
        printf("could not fopen %s\n", name);
        return 1;
    }
    yyparse();
    extern tree root_tree;
    gencode(root_tree);
    genwrite();
    return 0;
}

void * get_cli_int(char * c)
{
    return 0;
}

#define CDU_ROOT_SIZE 1000
static struct _cdu parse_cdu_root[CDU_ROOT_SIZE];
int cdu_free = 0;

insert_cdu(int c)
{
#if 0
    c->cdu$l_next=parse_cdu_root;
    parse_cdu_root=c;
#endif
    parse_cdu_root[c].cdu$l_next = parse_cdu_root[0].cdu$l_next;
    parse_cdu_root[0].cdu$l_next = c;
}

#if 0
get_cdu_root()
{
    extern struct _cdu cdu_root[];
    return &cdu_root[0];
}
#endif

alloc_cdu(int t)
{
    int b = cdu_free++;
    if (b == CDU_ROOT_SIZE)
    {
        printf("cdu_root overflow\n");
        exit(0);
    }
    int a = &parse_cdu_root[b];
    memset(a, 0, sizeof(struct _cdu));
    parse_cdu_root[b].cdu$b_type = t;
    return b;
}

alloc_name(char * n)
{
    int name = alloc_cdu(CDU$C_NAME);
    struct _cdu * np = &parse_cdu_root[name];
    memcpy(np->cdu$t_name, n, strlen(n));
    return name;
}

char * module_name = 0;

int gencode(tree t)
{
    while (t)
    {
        switch (TREE_CODE(t) )
        {
        case DEFINE_VERB_STMT:
        {
            int cdu = alloc_cdu(CDU$C_VERB);
            parse_cdu_root[cdu].cdu$l_verb = alloc_name(
                                                 IDENTIFIER_POINTER(TREE_OPERAND(t, 0)) );
            insert_cdu(cdu);
            tree verb = TREE_OPERAND(t, 1);
            gencode_verb(verb, &parse_cdu_root[cdu]);
        }
        break;
        case DEFINE_SYNTAX_STMT:
        {
            int cdu = alloc_cdu(CDU$C_SYNTAX);
            parse_cdu_root[cdu].cdu$l_syntax = alloc_name(
                                                   IDENTIFIER_POINTER(TREE_OPERAND(t, 0)) );
            insert_cdu(cdu);
            tree verb = TREE_OPERAND(t, 1);
            gencode_verb(verb, &parse_cdu_root[cdu]);
        }
        break;
        case DEFINE_TYPE_STMT:
        {
            int cdu = alloc_cdu(CDU$C_TYPE);
            parse_cdu_root[cdu].cdu$l_type = alloc_name(
                                                 IDENTIFIER_POINTER(TREE_OPERAND(t, 0)) );
            insert_cdu(cdu);
            tree type = TREE_OPERAND(t, 1);
            gencode_type(type, &parse_cdu_root[cdu]);
        }
        break;
        case MODULE_STMT:
            module_name = IDENTIFIER_POINTER(TREE_OPERAND(t, 0));
            break;
        default:
        {
        }
        }
        t = TREE_CHAIN(t);
    }
}

int gencode_verb(tree t, struct _cdu * cdu)
{
    while (t)
    {
        switch (TREE_CODE(t) )
        {
        case CLIFLAGS_CLAUSE:
            gencode_cliflags(TREE_OPERAND(t, 0), cdu);
            break;
        case CLIROUTINE_CLAUSE:
            cdu->cdu$l_routine = alloc_name(IDENTIFIER_POINTER(TREE_OPERAND(t, 0)));
            cdu->cdu$l_flags|=CDU$M_INTERNAL;
            break;
        case ROUTINE_CLAUSE:
            cdu->cdu$l_routine = alloc_name(IDENTIFIER_POINTER(TREE_OPERAND(t, 0)));
            break;
        case IMAGE_CLAUSE:
            cdu->cdu$l_image = alloc_name(IDENTIFIER_POINTER(TREE_OPERAND(t, 0)));
            break;
        case QUALIFIER_CLAUSE:
        {
            int q = alloc_cdu(CDU$C_QUALIFIER);
            parse_cdu_root[q].cdu$l_name = alloc_name(IDENTIFIER_POINTER(TREE_OPERAND(t, 0)));
            gencode_qual_clauses(TREE_OPERAND(t, 1),&parse_cdu_root[q]);
            parse_cdu_root[q].cdu$l_next=cdu->cdu$l_qualifiers;
            cdu->cdu$l_qualifiers=q;
        }
        break;
        case PARAMETER_CLAUSE:
        {
            int p = alloc_cdu(CDU$C_PARAMETER);
            parse_cdu_root[p].cdu$l_name=alloc_name(IDENTIFIER_POINTER(TREE_OPERAND(t, 0)));;
            gencode_para_clauses(TREE_OPERAND(t, 1),&parse_cdu_root[p]);
            parse_cdu_root[p].cdu$l_next=cdu->cdu$l_parameters;
            cdu->cdu$l_parameters=p;
        }
        break;
        default:
            break;
        }
        t=TREE_CHAIN(t);
    }
}

int gencode_type(tree t, struct _cdu * cdu)
{
    while (t)
    {
        switch (TREE_CODE(t) )
        {
        case KEYWORD_CLAUSE:
        {
            int k = alloc_cdu(CDU$C_KEYWORD);
            parse_cdu_root[k].cdu$l_name = alloc_name(
                                               IDENTIFIER_POINTER(TREE_OPERAND(t, 0)) );
            gencode_keyw_clauses(TREE_OPERAND(t, 1),&parse_cdu_root[k]);
            parse_cdu_root[k].cdu$l_next = cdu->cdu$l_keywords;
            cdu->cdu$l_keywords = k;
        }
        break;
        default:
            break;
        }
        t = TREE_CHAIN(t);
    }
}

struct flag_table
{
    int code;
    int flag;
};

struct flag_table codeflags[] =
{
    { NODISALLOWS_CLAUSE, CDU$M_NODISALLOWS },
    { NOPARAMETERS_CLAUSE, CDU$M_NOPARAMETERS },
    { NOQUALIFIERS_CLAUSE, CDU$M_NOQUALIFIERS },
    { BATCH_CLAUSE, CDU$M_BATCH },
    { NEGATABLE_CLAUSE, CDU$M_NEGATABLE },
    { NONNEGATABLE_CLAUSE, CDU$M_NONNEGATABLE },
    { DEFAULT_CLAUSE, CDU$M_DEFAULT },
    { ABBREVIATE_CLAUSE, CDU$M_ABBREVIATE },
    { FOREIGN_CLAUSE, CDU$M_FOREIGN },
    { IMMEDIATE_CLAUSE, CDU$M_IMMEDIATE },
    { MCRIGNORE_CLAUSE, CDU$M_MCRIGNORE },
    { MCROPTDELIM_CLAUSE, CDU$M_MCROPTDELIM },
    { MCRPARSE_CLAUSE, CDU$M_MCRPARSE },
    { NOSTATUS_CLAUSE, CDU$M_NOSTATUS },
    { CONCATENATE_CLAUSE, CDU$M_CONCATENATE },
    { NOCONCATENATE_CLAUSE, CDU$M_NOCONCATENATE },
    { IMPCAT_CLAUSE, CDU$M_IMPCAT },
    { LIST_CLAUSE, CDU$M_LIST },
    { REQUIRED_CLAUSE, CDU$M_REQUIRED },
    { 0, 0 },
};

int gencode_set_flag(struct _cdu * cdu, int code)
{
    int i;
    for (i = 0; codeflags[i].code; i++)
        if (codeflags[i].code == code)
        {
            cdu->cdu$l_flags |= codeflags[i].flag;
            return 1;
        }
    return 0;
}

int gencode_para_clauses(tree t, struct _cdu * cdu)
{
    while (t)
    {
        if (gencode_set_flag(cdu, TREE_CODE(t) ))
            goto next;
        switch (TREE_CODE(t) )
        {
        default:
            break;
        }
        gencode_single_clause(t, cdu);
next:
        t = TREE_CHAIN(t);
    }
}

int gencode_qual_clauses(tree t, struct _cdu * cdu)
{
    while (t)
    {
        if (gencode_set_flag(cdu, TREE_CODE(t) ))
            goto next;
        switch (TREE_CODE(t) )
        {
        default:
            break;
        }
        gencode_single_clause(t, cdu);
next:
        t = TREE_CHAIN(t);
    }
}

int gencode_cliflags(tree t, struct _cdu * cdu)
{
    while (t)
    {
        gencode_set_flag(cdu, TREE_CODE(t) );
        t = TREE_CHAIN(t);
    }
}

int gencode_single_clause(tree t, struct _cdu * cdu)
{
    if (gencode_set_flag(cdu, TREE_CODE(t) ))
        return;
    switch (TREE_CODE(t) )
    {
    case CLIFLAGS_CLAUSE:
        gencode_cliflags(TREE_OPERAND(t, 0), cdu);
        break;
    case LABEL_CLAUSE:
        cdu->cdu$l_label=alloc_name(IDENTIFIER_POINTER(TREE_OPERAND(t, 0)));
        cdu->cdu$l_name=cdu->cdu$l_label; // check
        break;
    case SYNTAX_CLAUSE:
        cdu->cdu$l_syntax=alloc_name(IDENTIFIER_POINTER(TREE_OPERAND(t, 0)));
        break;
    case VALUE_CLAUSE:
    {
        int n = alloc_cdu(CDU$C_VALUE);
        cdu->cdu$l_value=n;
        gencode_value_clauses(TREE_OPERAND(t, 0), &parse_cdu_root[n]);
    }
    break;
    default:
        break;
    }
}

int gencode_value_clauses(tree t, struct _cdu * cdu)
{
    while (t)
    {
        if (gencode_set_flag(cdu, TREE_CODE(t) ))
            goto next;
        switch (TREE_CODE(t) )
        {
        case TYPE_CLAUSE:
            cdu->cdu$l_type = alloc_name(
                                  IDENTIFIER_POINTER(TREE_OPERAND(t, 0)) );
        default:
            break;
        }
next:
        t = TREE_CHAIN(t);
    }
}

int gencode_keyw_clauses(tree t, struct _cdu * cdu)
{
    while (t)
    {
        if (gencode_set_flag(cdu, TREE_CODE(t) ))
            goto next;
        switch (TREE_CODE(t) )
        {
        default:
            break;
        }
        gencode_single_clause(t, cdu);
next:
        t = TREE_CHAIN(t);
    }
}

genwrite()
{
    int out;
    if (module_name)
    {
        int len = strlen(module_name);
        char * dup = strndup(module_name, len + 4);
        dup[len] = '.';
        dup[len + 1] = 'c';
        dup[len + 2] = 0;
        char * dupagain = strndup(dup, len + 2);
        out = fopen(dupagain, "w");
    }
    else
        out = fopen("dcltables.c", "w");
    fprintf(out, "#include \"cli.h\"\n\n");
    if (module_name)
        fprintf(out, "struct _cdu %s[] = {\n", module_name);
    else
        fprintf(out, "struct _cdu cdu_root[] = {\n");
    int i = 0;
    struct _cdu * cdu = &parse_cdu_root[0];

    while (i < cdu_free)
    {
        fprintf(out, "// element %x\n", i);
        fprintf(out, "  {\n");

        fprintf(out, "    cdu$l_next: 0x%x,\n", cdu->cdu$l_next);
        fprintf(out, "    cdu$l_cbl: 0x%x,\n", cdu->cdu$l_cbl);
        fprintf(out, "    cdu$w_size: 0x%x,\n", cdu->cdu$w_size);
        fprintf(out, "    cdu$b_type: 0x%x,\n", cdu->cdu$b_type);
        fprintf(out, "    cdu$b_rmod: 0x%x,\n", cdu->cdu$b_rmod);
        fprintf(out, "    cdu$l_parent: 0x%x,\n", cdu->cdu$l_parent);
        fprintf(out, "    cdu$l_child: 0x%x,\n", cdu->cdu$l_child);
        fprintf(out, "    cdu$l_verb: 0x%x,\n", cdu->cdu$l_verb);
        fprintf(out, "    cdu$l_image: 0x%x,\n", cdu->cdu$l_image);
        fprintf(out, "    cdu$l_routine: 0x%x,\n", cdu->cdu$l_routine);
        fprintf(out, "    cdu$l_qualifiers: 0x%x,\n", cdu->cdu$l_qualifiers);
        fprintf(out, "    cdu$l_parameters: 0x%x,\n", cdu->cdu$l_parameters);
        fprintf(out, "    cdu$l_name: 0x%x,\n", cdu->cdu$l_name);
        fprintf(out, "    cdu$l_value: 0x%x,\n", cdu->cdu$l_value);
        fprintf(out, "    cdu$l_flags: 0x%x,\n", cdu->cdu$l_flags);
        fprintf(out, "    cdu$l_label: 0x%x,\n", cdu->cdu$l_label);
        fprintf(out, "    cdu$l_type: 0x%x,\n", cdu->cdu$l_type);
        fprintf(out, "    cdu$l_syntax: 0x%x,\n", cdu->cdu$l_syntax);
        fprintf(out, "    cdu$l_clauses: 0x%x,\n", cdu->cdu$l_clauses);
        fprintf(out, "    cdu$l_keywords: 0x%x,\n", cdu->cdu$l_keywords);
        //    fprintf(out, "    cdu$l_: 0x%x,\n",cdu->cdu$l_);

        fprintf(out, "  },\n");

        i++;
        cdu++;
    }

    fprintf(out, "};\n");

    fclose(out);
}
