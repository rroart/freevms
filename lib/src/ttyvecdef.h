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
	
struct _tt_class {
  int (*class_getnxt)();
  int (*class_putnxt)();
  int (*class_setup_ucb)();
  int (*class_ds_tran)();
  struct _ddt *class_ddt;
  int (*class_readerror)();
  int (*class_disconnect)();
  int (*class_fork)();
  int (*class_powerfail)();
  void *class_tables;
};

struct _tt_tables {
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

struct _tt_port {
  void (*port_startio)();
  int (*port_disconnect)();
  int (*port_set_line)();
  int (*port_ds_set)();
  int (*port_xon)();
  int (*port_xoff)();
  int (*port_stop)();
  int (*port_stop2)();
  int (*port_abort)();
  int (*port_resume)();
  int (*port_set_modem)();
  int (*port_glyphload)();
  int (*port_maint)();
  int (*port_forkret)();
  struct _fdt *port_fdt;
  unsigned int port_start_read;
  unsigned int port_middle_read;
  unsigned int port_end_read;
  void (*port_cancel)();
};

struct _tt_asian {
  int (*asian_fdtsensem)();
  int (*asian_fdtsensec)();
  int (*asian_fdt_setm)();
  int (*asian_fdt_setc)();
  int (*asian_upper)();
  int (*asian_jiscon)();
  int (*asian_start_read)();
  int (*asian_do_setm)();
  int (*asian_do_setc)();
  int (*asian_begin_echo)();
  int (*asian_cursoroverf)();
  int (*asian_setup_ucb)();
  int (*asian_fontfork)();
  int (*asian_cre_control)();
  int (*asian_fhpointer)();
  int (*asian_delete_asc)();
  int (*asian_abort)();
  int (*asian_find_bol)();
  int (*asian_preload)();
  int (*asian_preload_fork)();
  int (*asian_del_cache_fork)();
  int (*asian_preload_cleanup)();
  int (*asian_adjust_cursor)();
  int (*asian_clone_ucb)();
};

struct _tt_fidriver {
  int (*fidriver_ttread)();
  int (*fidriver_ttcancelio)();
  int (*fidriver_ttdisconnect)();
};

struct _tt_posix_tables {
  unsigned int posix_tables_pt_pointer;
  unsigned int posix_tables_pt_putnxt;
  unsigned int posix_tables_pt_getnxt;
  unsigned int posix_tables_pt_writing;
};
 
#endif
 
