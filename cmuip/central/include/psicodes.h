/*
    ****************************************************************

        Copyright (c) 1992, Carnegie Mellon University

        All Rights Reserved

    Permission  is  hereby  granted   to  use,  copy,  modify,  and
    distribute  this software  provided  that the  above  copyright
    notice appears in  all copies and that  any distribution be for
    noncommercial purposes.

    Carnegie Mellon University disclaims all warranties with regard
    to this software.  In no event shall Carnegie Mellon University
    be liable for  any special, indirect,  or consequential damages
    or any damages whatsoever  resulting from loss of use, data, or
    profits  arising  out of  or in  connection  with  the  use  or
    performance of this software.

    ****************************************************************
*/
#define psi$c_ncb_null   0
#define psi$c_ncb_remdte   1
#define psi$c_ncb_remsubadr   2
#define psi$c_ncb_locdte   3
#define psi$c_ncb_locsubadr   4
#define psi$c_ncb_userdata   5
#define psi$c_ncb_respdata   6
#define psi$c_ncb_objname   7
#define psi$c_ncb_endtoend   8
#define psi$c_ncb_endtoendr   9
#define psi$c_ncb_gateway   10
#define psi$c_ncb_ici   11
#define psi$c_ncb_revchg   12
#define psi$c_ncb_locfacr   13
#define psi$c_ncb_fsel   14
#define psi$c_ncb_fsel_res   15
#define psi$c_ncb_thrucls   16
#define psi$c_ncb_cug   17
#define psi$c_ncb_diagcode   18
#define psi$c_ncb_cause   19
#define psi$c_ncb_reason   20
#define psi$c_ncb_pktsize   21
#define psi$c_ncb_winsize   22
#define psi$c_ncb_locfac   23
#define psi$c_ncb_pvcnam   24



#define psi$c_ncb_network   30
#define psi$c_ncb_userid   31
#define psi$c_ncb_password   32
#define psi$c_ncb_account   33



#define psi$c_ncb_pridst   34
#define psi$c_ncb_prired   35
#define psi$c_ncb_rcv_quota   36



#define psi$c_ncb_net_user_id   37
#define psi$c_ncb_charging_info   38
#define psi$c_ncb_charge_mon   39
#define psi$c_ncb_charge_seg   40
#define psi$c_ncb_charge_time   41
#define psi$c_ncb_rpoa   42
#define psi$c_ncb_addr_mod_rsn   43
#define psi$c_ncb_call_redir_rsn   44
#define psi$c_ncb_call_redir_orig   45
#define psi$c_ncb_transit_delay   46



#define psi$c_ncb_calling_extension   47
#define psi$c_ncb_called_extension   48
#define psi$c_ncb_min_thrucls   49
#define psi$c_ncb_expedite   50



#define psi$c_ncb_cum_trst_dly   51
#define psi$c_ncb_cum_trst_dly_r   52
#define psi$c_ncb_ete_trst_dly   53
#define psi$c_ncb_max_trst_dly   54
#define psi$c_ncb_incoming_dte   55



#define psi$c_ntd_acclvl   1
#define psi$c_ntd_salo   2
#define psi$c_ntd_sahi   3
#define psi$c_ntd_remdte   4
#define psi$c_ntd_usrgrp   5
#define psi$c_ntd_usrdata   6
#define psi$c_ntd_datmsk   7
#define psi$c_ntd_priority   8
#define psi$c_ntd_redrsn   9
#define psi$c_ntd_network   10
#define psi$c_ntd_rcvdte   11
#define psi$c_ntd_called_dte   12
#define psi$c_ntd_incdte   13
#define psi$c_ntd_extmsk   14
#define psi$c_ntd_extval   15



#define psi$c_redrsn_busy   0
#define psi$c_redrsn_out_of_order   1
#define psi$c_redrsn_systematic   2



#define psi$c_l3_local   1
#define psi$c_l3_netwrk   2
#define psi$c_l3_neterr   3
#define psi$c_l3_locerr   4
#define psi$c_l3_lnkrst   5
#define psi$c_l3_lnkdwn   6
#define psi$c_l3_lnkup   7
#define psi$c_l3_lnkrrt   8
#define psi$c_l3_locmgt   9
#define psi$c_l3_calcol   10
#define psi$c_l3_nettim   11
#define psi$c_l3_gatdisc   12
#define psi$c_l3_netdisc   13



#define psi$m_moredata   1
#define psi$m_qualified   2
#define psi$m_sts_pvc   1
#define psi$m_sts_locdtelng   2
#define psi$m_sts_remdtelng   4
#define psi$m_sts_userlng   8
#define psi$m_sts_winbad   16
#define psi$m_sts_pktbad   32
#define psi$m_sts_thrbad   64
#define psi$m_sts_rpoalng   128
#define psi$m_sts_wordbad   256



#define psi$c_err_unknown   0
#define psi$c_err_faclng   1
#define psi$c_err_invitem   2
#define psi$c_err_conflict   3
#define psi$c_err_badparm   4
#define psi$c_err_notrans   5
#define psi$c_err_recurlmt   6
#define psi$c_err_invnum   7
#define psi$c_err_noici   8
#define psi$c_err_manyici   9
#define psi$c_err_notimp   10
#define psi$c_err_nodtes   11
#define psi$c_err_nosuchdte   12
#define psi$c_err_nosuchpvc   13
#define psi$c_err_nosuchnet   14
#define psi$c_err_nolocal   15
#define psi$c_err_nononpag   16
#define psi$c_err_nol3   17
#define psi$c_err_badname   18
#define psi$c_err_l3err   19
#define psi$c_err_pvcalracc   20
#define psi$c_err_bad_pvcname   21
#define psi$c_err_nosuchgroup   22
#define psi$c_err_invexp   23
#define psi$c_err_invtrstdly   24

#define psi$c_err_nolines   11
#define psi$c_err_nosuchline   12



#define psi$c_interrupt   1
#define psi$k_interrupt   1
#define psi$c_intack   2
#define psi$k_intack   2
#define psi$c_reset   3
#define psi$k_reset   3




#define psi$c_restart   7
#define psi$k_restart   7
#define psi$c_x29_set   8
#define psi$k_x29_set   8
#define psi$c_x29_read   9
#define psi$k_x29_read   9
#define psi$c_x29_read_specific   10
#define psi$k_x29_read_specific   10



#define psi$k_x29_temp_nohang   1
#define psi$k_x29_pad_params   2
#define psi$k_x29_int_action   3
#define psi$k_x29_break_action   4
#define psi$k_x29_hold_timer   5
#define psi$k_x29_host_echo_params   6
#define psi$k_x29_local_echo_params   7
#define psi$k_x29_hangup_params   8
#define psi$m_x29_calculated   1
#define psi$m_x29_add_value   2
#define psi$m_x29_user_value   4
#define psi$m_x29_if_changed   8
#define psi$c_x29_param_length   8
#define psi$k_x29_param_length   8
#define psi$s_psi$_x29_param   8
#define psi$s_x29_param_flags   2
#define psi$s_x29_fill_2   12
#define psi$s_x29_fill_1   24
#define psi$m_x29_action_reset   1
#define psi$m_x29_action_purge   2
#define psi$m_x29_action_clear   4
#define psi$c_x29_action_length   20
#define psi$k_x29_action_length   20
#define psi$s_psi$_x29_flags   20
#define psi$s_x29_action_flags   4
#define psi$s_x29_action_string   16
