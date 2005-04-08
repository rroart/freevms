!
!	****************************************************************
!
!		Copyright (c) 1992, Carnegie Mellon University
!
!		All Rights Reserved
!
!	Permission  is  hereby  granted   to  use,  copy,  modify,  and
!	distribute  this software  provided  that the  above  copyright
!	notice appears in  all copies and that  any distribution be for
!	noncommercial purposes.
!
!	Carnegie Mellon University disclaims all warranties with regard
!	to this software.  In no event shall Carnegie Mellon University
!	be liable for  any special, indirect,  or consequential damages
!	or any damages whatsoever  resulting from loss of use, data, or
!	profits  arising  out of  or in  connection  with  the  use  or
!	performance of this software.
!
!	****************************************************************
!
!++
! Telnetmsg.Msg		Copyright (c) 1986	Carnegie Mellon University
!
! Description:
!
!	Error messages for the Telnet utility
!	Version V01-000
!
! Facility:	Telnet Utility
!
! Environment:
!
!	VAX/VMS operating system, user mode utility
!
! Author:
!	Dale Moore CMU-CSD May 1983
!--

.TITLE		TELNETMSG Telnet Error and Warning messages
.FACILITY	Telnet,69/PREFIX=Telnet$_

.SEVERITY	FATAL
	GET_INET	<Did not find any Internet device>
	NOT_TERM	<Can't run Telnet on non-terminal device>
	HOST_NOT_FOUND	<Can't find host !AS>			/FAO_COUNT=1
	KEY_SYNTAX	<Bad Key Syntax /!AS=!AS>		/FAO_COUNT=2
	ESC_SYNTAX	<Bad Escape Syntax !AS>			/FAO_COUNT=1
	PORT_SYNTAX	<Bad Port Syntax !AS>			/FAO_COUNT=1
	TCP_RECV	<TCP Receive Error>			/FAO_COUNT=0
	TCP_Send	<TCP Send Error>			/FAO_COUNT=0

.SEVERITY	WARNING
	NO_OPEN		<Can't open connection to !AS>		/FAO_COUNT=1
	PROTOCOL	<Telnet protocol error/confusion>
	WRITEFAILED	<write to terminal failed>

.SEVERITY	ERROR
	No_Switch	<Error reading command line for !AS>	/FAO_COUNT=1
	Control_C	<You hit Control-C>			/FAO_COUNT=0
	Error		<Local processing error>		/FAO_COUNT=0

.SEVERITY	INFO
	Autoflush_ON	<Autoflush is on>			/FAO_COUNT=0
	Autoflush_OFF	<Autoflush is off>			/FAO_COUNT=0
	BINARY_ON	<Binary Mode is on>			/FAO_COUNT=0
	BINARY_OFF	<Binary Mode is off>			/FAO_COUNT=0
	Blocking_ON	<Abort output (Data blocking) is on>	/FAO_COUNT=0
	Blocking_OFF	<Abort output (Data blocking)  is off>	/FAO_COUNT=0
	Connect_ON	<Auto Connect is on>			/FAO_COUNT=0
	Connect_OFF	<Auto Connect is off>			/FAO_COUNT=0
	DEBUG_ON	<Debugging is on>			/FAO_COUNT=0
	DEBUG_OFF	<Debugging is off>			/FAO_COUNT=0
	ECHO_ON		<Local Echoing is on>			/FAO_COUNT=0
	ECHO_OFF	<Local Echoing is off>			/FAO_COUNT=0
	Rem_ECHO_ON	<Remote Echoing requested on>		/FAO_COUNT=0
	Rem_ECHO_OFF	<Remote Echoing requested off>		/FAO_COUNT=0
	Eightbit_ON	<Eightbit is on>			/FAO_COUNT=0
	Eightbit_OFF	<Eightbit is off>			/FAO_COUNT=0
	EOL_TYPE_CRLF	<End of line is CR-LF>			/FAO_COUNT=0
	EOL_TYPE_CRNULL	<End of line is CR-NULL>		/FAO_COUNT=0
	Extended_ON	<Extended command mode is on>		/FAO_COUNT=0
	Extended_OFF	<Extended command mode is off>		/FAO_COUNT=0
	Linemode_NEG	<Line mode is negotiated>		/FAO_COUNT=0
	Linemode_ON	<Line mode is on>			/FAO_COUNT=0
	Linemode_OFF	<Line mode is off>			/FAO_COUNT=0
	LOCALCHARS_ON	<local character recognition is on>	/FAO_COUNT=0
	LOCALCHARS_OFF	<local character recognition is off>	/FAO_COUNT=0
	LOCAL_SYNCH	<Local terminal handles XOFF/XON>	/FAO_COUNT=0
	NO_LOCAL_SYNCH	<Local terminal does not handle XOFF/XON> -
								/FAO_COUNT=0
	LOG_ON		<Logging is on>				/FAO_COUNT=0
	LOG_OFF		<Logging is off>			/FAO_COUNT=0
	Negotiate	<Negotiate options with remote host>	/FAO_COUNT=0
	NO_LOG		<NO Log file open>			/FAO_COUNT=0
	Rem_Synch_ON	<Remote synch requested on>		/FAO_COUNT=0
	Rem_Synch_OFF	<Remote synch requested off>		/FAO_COUNT=0
	SYNCH_ON	<Telnet Synch is on>			/FAO_COUNT=0
	SYNCH_OFF	<Telnet Synch is off>			/FAO_COUNT=0
	Quiet_ON	<Quiet is on>				/FAO_COUNT=0
	Quiet_OFF	<Quiet is off>				/FAO_COUNT=0
	TRY_BINARY_ON	<Attempting to turn binary on>		/FAO_COUNT=0
	TRY_BINARY_OFF	<Attempting to turn binary off>		/FAO_COUNT=0
	NO_BINARY_ON	<Could not turn binary on>		/FAO_COUNT=0
	Urgent_ON	<Urgent is on>				/FAO_COUNT=0
	Urgent_OFF	<Urgent is off>				/FAO_COUNT=0
	X_ON		<Output ON (XON)>			/FAO_Count=0
	X_OFF		<Output OFF (XOFF)>			/FAO_Count=0

.SEVERITY	INFO
	Duplicate_keys	<Duplicate command key definitions exist> -
								/FAO_COUNT=0
	TRYING		<Trying to connect to !AS>		/FAO_COUNT=5
	NOT_ATTACHED	<Failure to attach to !AS>		/FAO_COUNT=1
	Attach_to	<control returned to process [!AS]>	/FAO_COUNT=1
	NOT_CONNECTED	<No remote host.> 			/FAO_COUNT=0
	OPTION_OOR	<Option (!UL) out of range.>		/FAO_COUNT=1
	R_WILL		<!%T Received Will [!AS]>		/FAO_COUNT=2
	R_WONT		<!%T Received Wont [!AS]>		/FAO_COUNT=2
	R_DO		<!%T Received Do [!AS]>			/FAO_COUNT=2
	R_DONT		<!%T Received Dont [!AS]>		/FAO_COUNT=2
	R_TTYLOC	<!%T Received TTYLOC [!OL,!OL]>		/FAO_COUNT=3
	R_TTYTYPE_SEND	<!%T Received Send Terminal Type>	/FAO_COUNT=1
	R_WINDOW_SIZE	<!%T Received Window Size [!UW,!UW]>	/FAO_COUNT=3
	R_TTYSPEED_SEND	<!%T Received Send Terminal Speed>	/FAO_COUNT=1
	R_TOG_FLOW_CNTL_OFF	<!%T Received Flow Cntl OFF>	/FAO_COUNT=1
	R_TOG_FLOW_CNTL_ON	<!%T Received Flow Cntl ON>	/FAO_COUNT=1
	R_LINEMODE_OFF	<!%T Received Linemode OFF>		/FAO_COUNT=1
	R_LINEMODE_ON	<!%T Received Linemode ON>		/FAO_COUNT=1
	S_COMM		<!%T Sent Command !AS>			/FAO_COUNT=2
	S_COMM_UNKNOWN	<!%T Sent Unknown Command [!UB]>	/FAO_COUNT=2
	S_WILL		<!%T Sent Will [!AS]>			/FAO_COUNT=2
	S_WONT		<!%T Sent Wont [!AS]>			/FAO_COUNT=2
	S_DO		<!%T Sent Do [!AS]>			/FAO_COUNT=2
	S_DONT		<!%T Sent Dont [!AS]>			/FAO_COUNT=2
	S_TTYLOC	<!%T Sent TTYLOC [!OL,!OL]>		/FAO_COUNT=3
	S_NOP		<!%T Sent NOP>				/FAO_COUNT=1
	S_TTYTYPE	<!%T Sent TTY Type [!AS]>		/FAO_COUNT=2
	S_LOCATION	<!%T Sent Location !AS>			/FAO_COUNT=2
	S_TTYSPEED	<!%T Sent TTY Speed [!AS]>		/FAO_COUNT=2
	S_WINDOW_SIZE	<!%T Sent Window Size [!UW,!UW]>	/FAO_COUNT=3
	S_LINE_WIDTH	<!%T Sent Line Width [!UW]>		/FAO_COUNT=2
	S_PAGE_SIZE	<!%T Sent PAge Size [!UW]>		/FAO_COUNT=2
	S_USER_ID	<!%T Sent Window Size [!AS]>		/FAO_COUNT=2

.SEVERITY	SUCCESS
	CLOSED		<Connection closed to !AS>		/FAO_COUNT=1
	CONN_CLOSED	<Connection closed to !AS>		/FAO_COUNT=1
	ABORT		<Connection aborted to !AS>		/FAO_COUNT=1
	CONNECTED 	<Connection Opened to !AS>		/FAO_COUNT=1
	OPEN		<Connection Opened to !AS>		/FAO_COUNT=1
	NO_OPTIONS	<Not Sending Options>			/FAO_COUNT=0
	PAUSE		<Pause:Temporarily returning from !AS>	/FAO_COUNT=1
	RETURNING	<Returning to !AS>			/FAO_COUNT=1
	RESUME		<Resume telnet to !AS>			/FAO_COUNT=1
.END

