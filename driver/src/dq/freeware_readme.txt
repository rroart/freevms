DQDRIVER, SOFTWARE, IDE/ATAPI Disk Driver Example for OpenVMS Alpha

    DQDRIVER is an IDE disk driver for OpenVMS Alpha.  The driver was 
    originally written and tested on an add-in IDE controller board as well
    as the on-board IDE controller found on the AlphaStation 400 4/233.  

    The driver has been substantially revised to include ATAPI CD-ROM
    support.  In addition, the driver has been incorporated in OpenVMS
    Alpha and is supported on recent 21264 (EV6) based platforms.

    A recent enhancement to DQDRIVER is the inclusion of the IO$_DIAGNOSE
    interface which can be used for CD-ROM audio, or potentially other
    unique ATAPI devices, such as CD-RW.  This enhancement is expected to
    be included in future OpenVMS Alpha releases and in future update
    patch kit for V7.1-2 and V7.2-1.

    The driver is provided as an example, and as with any hardware based
    tools, you should use at your own risk.  The latest version of the
    source code is provide here to update previous release of the code and
    to provide immediate access to CD-ROM audio support.  The driver can be 
    used in an unsupported fashion to access CD-ROM audio until official
    patch kits or new release are available with the new version of the
    driver. 

    A new version of DECW$CDPLAYER been included in the kit.  This version
    contains enhancements for ATAPI CD-ROM audio support via the new
    IO$_DIAGNOSE interface in DQDRIVER.  The new version of the CD player
    can be used to play CD-ROM audio from either ATAPI or SCSI CD-ROM
    drives.  The new version of DECW$CDPLAYER is expected to be include in
    future versions of the DECwindows kit.


    This package has the following source code:

	DQDRIVER.C		- IDE driver source file.
        DQDRIVER.COM		- Build file for DQDRIVER
        SYS$DQDRIVER.EXE	- Executable for V7.1-2
	ENABLE-IDE.C 		- tool to enable the built in IDE for the
			  	  AlphaStation 400 4/233 and load the IDE 
    				  driver.  DQDRIVER is now autoconfigured on 
    				  supported Alpha systems.
	ENABLE-IDE.COM 		- Build file for ENABLE-IDE.C
	IDE-INFO 		- tool to dump the identity page information 
    				  from an IDE drive.
	IDE-INFO 		- Build file for IDE-INFO.C

	DECW$CDPLAYER.C		- New version of DECwindows CD-ROM audio 
    				  player with support for ATAPI CD-ROMs.
    	DECW$CDPALYER.UIL	- Interface definition file for CD player
	DECW$CDPLAYER.EXE	- Excutable for V7.1-2 
    	DECW$CDPALYER.UIL	- Compiled Interface definition file.  Must be
                                  in same directory as EXE file.

    Also included in this directory is the procedure LOAD_DQ_INTEL_SIO.COM,
    which will connect and load DQDRIVER for operation with the Intel SIO
    peripheral controller found on some members of the Personal Workstation
    -au series Alpha systems.  (DQDRIVER is supported with and autoconfigures
    on and can boot via the Cypress IDE PCI Peripheral Controller found on
    various Personal Workstation -au series systems.  OpenVMS cannot boot
    from IDE devices connected via the Intel SIO PCI IDE adapter.)

    To determine which IDE controller you have, use the SRM console command
    SHOW CONFIGURATION.   If your controller is shown as:

             Bus 00 Slot 07: Cypress PCI Peripheral Controller

    You can directly use DQDRIVER, can boot from supported IDE devices,
    and do not require LOAD_DQ_INTEL_SIO.COM.  If your controller is:

             Bus 00 Slot 07: Intel SIO 82378

    You cannot bootstrap from IDE devices, and DQDRIVER will not configure
    automatically.  If you wish to use DQDRIVER with IDE devices connected
    via the Intel SIO, you will need LOAD_DQ_INTEL_SIO.COM.

