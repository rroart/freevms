#ifndef ttyvecdef_h
#define ttyvecdef_h

#define CLASS_LENGTH 40
#define CLASSS_CLASS_DEF 40

#define TABLES_LENGTH 100
#define TABLESS_TABLES_DEF 100

#define PORT_LENGTH 76
#define PORTS_PORT_DEF 76

#define ASIAN_LENGTH 96
#define ASIANS_ASIAN_DEF 96

#define FIDRIVER_LENGTH 12
#define FIDRIVERS_FIDRIVER_DEF 12

#define POSIX_TABLES_LENGTH 16
#define POSIX_TABLESS_POSIX_TABLES_DEF 16

struct _tt_class
{
    int (*class_getnxt)(int * chr, int * CC, struct _ucb * u);
    int (*class_putnxt)(int * chr, int * CC, struct _ucb * u);
    int (*class_setup_ucb)(struct _ucb * u);
    int (*class_ds_tran)(void);
    struct _ddt *class_ddt;
    int (*class_readerror)(void);
    int (*class_disconnect)(void);
    int (*class_fork)(void);
    int (*class_powerfail)(struct _ucb * u);
    void *class_tables;
};

struct _tt_tables
{
    unsigned int tables_fill1;
    unsigned int tables_fill2;
    unsigned int tables_fill3;
    unsigned int tables_fill4;
    unsigned int tables_fill5;
    unsigned int tables_fill6;
    unsigned int tables_fill7;
    unsigned int tables_fill8;
    unsigned int tables_fill9;
    unsigned int tables_fill10;
    unsigned int tables_fill11;
    unsigned int tables_fill12;
    unsigned int tables_fill13;
    unsigned int tables_fill14;
    unsigned int tables_fill15;
    unsigned int tables_fill16;
    unsigned int tables_fill17;
    unsigned int tables_init_mid;
    unsigned int tables_fill19;
    unsigned int tables_posix;
    unsigned int tables_asian;
    unsigned int tables_fill22;
    unsigned int tables_fill23;
    unsigned int tables_fill24;
    unsigned int tables_fidriver;
};

struct _tt_port
{
    void (*port_startio)(void);
    int (*port_disconnect)(void);
    int (*port_set_line)(void);
    int (*port_ds_set)(void);
    int (*port_xon)(void);
    int (*port_xoff)(void);
    int (*port_stop)(void);
    int (*port_stop2)(void);
    int (*port_abort)(void);
    int (*port_resume)(void);
    int (*port_set_modem)(void);
    int (*port_glyphload)(void);
    int (*port_maint)(void);
    int (*port_forkret)(void);
    int (*port_fdt)(void);
    void (*port_start_read)(void);
    void (*port_middle_read)(void);
    void (*port_end_read)(void);
    void (*port_cancel)(void);
};

struct _tt_asian
{
    int (*asian_fdtsensem)(void);
    int (*asian_fdtsensec)(void);
    int (*asian_fdt_setm)(void);
    int (*asian_fdt_setc)(void);
    int (*asian_upper)(void);
    int (*asian_jiscon)(void);
    int (*asian_start_read)(void);
    int (*asian_do_setm)(void);
    int (*asian_do_setc)(void);
    int (*asian_begin_echo)(void);
    int (*asian_cursoroverf)(void);
    int (*asian_setup_ucb)(void);
    int (*asian_fontfork)(void);
    int (*asian_cre_control)(void);
    int (*asian_fhpointer)(void);
    int (*asian_delete_asc)(void);
    int (*asian_abort)(void);
    int (*asian_find_bol)(void);
    int (*asian_preload)(void);
    int (*asian_preload_fork)(void);
    int (*asian_del_cache_fork)(void);
    int (*asian_preload_cleanup)(void);
    int (*asian_adjust_cursor)(void);
    int (*asian_clone_ucb)(void);
};

struct _tt_fidriver
{
    int (*fidriver_ttread)(void);
    int (*fidriver_ttcancelio)(void);
    int (*fidriver_ttdisconnect)(void);
};

struct _tt_posix_tables
{
    unsigned int posix_tables_pt_pointer;
    unsigned int posix_tables_pt_putnxt;
    unsigned int posix_tables_pt_getnxt;
    unsigned int posix_tables_pt_writing;
};

#endif

