#ifndef __NET_PKT_CLS_H
#define __NET_PKT_CLS_H

struct rtattr;
struct tcmsg;

/* Basic packet classifier frontend definitions. */

struct tcf_result
{
    unsigned long   class;
    u32     classid;
};

struct tcf_proto
{
    /* Fast access part */
    struct tcf_proto    *next;
    void            *root;
    int         (*classify)(struct sk_buff*, struct tcf_proto*, struct tcf_result *);
    u32         protocol;

    /* All the rest */
    u32         prio;
    u32         classid;
    struct Qdisc        *q;
    void            *data;
    struct tcf_proto_ops    *ops;
};

struct tcf_walker
{
    int stop;
    int skip;
    int count;
    int (*fn)(struct tcf_proto *, unsigned long node, struct tcf_walker *);
};

struct tcf_proto_ops
{
    struct tcf_proto_ops    *next;
    char            kind[IFNAMSIZ];

    int         (*classify)(struct sk_buff*, struct tcf_proto*, struct tcf_result *);
    int         (*init)(struct tcf_proto*);
    void            (*destroy)(struct tcf_proto*);

    unsigned long       (*get)(struct tcf_proto*, u32 handle);
    void            (*put)(struct tcf_proto*, unsigned long);
    int         (*change)(struct tcf_proto*, unsigned long, u32 handle, struct rtattr **, unsigned long *);
    int         (*delete)(struct tcf_proto*, unsigned long);
    void            (*walk)(struct tcf_proto*, struct tcf_walker *arg);

    /* rtnetlink specific */
    int         (*dump)(struct tcf_proto*, unsigned long, struct sk_buff *skb, struct tcmsg*);
};

#endif
