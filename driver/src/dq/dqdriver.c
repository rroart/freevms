#pragma module DQDRIVER "X-26"

#pragma message disable ignorecallval	/* Billions and billions of these... */

#ifdef DEBUG
#define TRACING 4096*4			/* Size of buffer or undefine this if none */
//#define TRACE_DATA_TOO		/* If you'd like all the data register read/writes logged */
//#define TRACE_PER_DRIVE		/* Per-drive trace buffers */
#define TRACE_COMMON			/* One global trace buffer for all four drives */
#endif

#ifdef DEBUG
#define BREAKPOINTS			/* Include or exclude BREAK breakpoints */
#endif

#ifdef DEBUG
#define EXTRA_STATS			/* Include or exclude extra stats in RDSTATS */
#endif

/*************************************************************************
 *                                                                       *
 * Copyright 1994-2000 Compaq Computer Corporation                       *
 *                                                                       *
 * COMPAQ Registered in U.S. Patent and Trademark Office.                *
 *                                                                       *
 * Confidential computer software. Valid license from Compaq or          *
 * authorized sublicensor required for possession, use or copying.       *
 * Consistent with FAR 12.211 and 12.212, Commercial Computer Software,  *
 * Computer Software Documentation, and Technical Data for Commercial    *
 * Items are licensed to the U.S. Government under vendor's standard     *
 * commercial license.                                                   *
 *                                                                       *
 * Compaq shall not be liable for technical or editorial errors or       *
 * omissions contained herein. The information in this document is       *
 * subject to change without notice.                                     *
 *                                                                       *
 *************************************************************************/

/************************************************************************/
/*									*/
/* Facility:								*/
/*      IDE Disk Driver							*/
/*									*/
/* Abstract:								*/
/*      This driver controls a standard IDE/ATA/EIDE r/w disk		*/
/*        or an ATAPI CD-/DVD-ROM drive.				*/
/*									*/
/* Author:								*/
/*      Benjamin J. Thomas III / May 1994				*/
/*									*/
/* Original dedication from Ben Thomas:					*/
/*									*/
/* My brother-in-law and nephew were killed in a small plane crash just	*/
/* off Nantucket island, on June 6, 1994, shortly after I started	*/
/* writing this driver.  This effort is dedicated to the memory of	*/
/* of Reginald Marden and Christopher Marden.  They will be missed.	*/
/*									*/
/*									*/
/* Revision History:							*/
/*									*/
/*	X-26	SGS0185 Steve Skonetski			14-Feb-2001	*/
/*		Really big drives cause problems with current geometry  */
/*		calcuation. Cause divide-by-zero crashes when truncated */
/*		cylinder count is zero. Redo algorithm to use the full  */
/*              range of cyl/trk/sec that a ATA can handle.             */
/*		PTR 70-3-3939.						*/
/*                                                                      */
/*	X-25	SGS0181		Steve Skonetski		26-Aug-2000     */
/*		Privateer Pass 2 has new Acer chip.  DMA no longer works*/
/*		and system has to be reset to recover from it.  Fix is  */
/*              to:                                                     */
/*		- select drive/head before writing DMA info             */
/*              - write DMA descriptor as a longword                    */
/*              - reset disk if ATAPI_PACKET_COMMAND WFIKPCH timeout    */
/*									*/
/*   	X-24	            	Sue Lewis          	10-Aug-2000     */
/*		In the previous check-in, I neglected to initialize     */
/*		a variable, which leaves cypress-based machines in 	*/
/*		deep trouble.						*/
/*   	X-23	            	Sue Lewis          	11-Jul-2000     */
/*		Propagate from Pele:				        */
/*		Re-do determination of CSR and DMA addresses.  		*/
/*		This allows multiple Acer IDE adapters per system, and  */
/*		removes the restriction that every primary must have    */
/*		a secondary. 						*/
/*									*/
/*      X-22    JMB267          James M. Blue           10-Jul-2000     */
/*              Add modifications to make the driver QIOServer          */
/*              capable.  This includes adding entry point to DDT,      */
/*              a flag to DPT, a flag in DEVCHAR, masks to the FDT,     */
/*              initialization code to the unit init fork routine,      */
/*              and a helper routine.                                   */
/*									*/
/*   	X-21	PAJ1129  	Paul A. Jacobi	   	27-Mar-2000     */
/*		Add support for CD-ROM audio via the IO$_DIAGNOSE       */
/*		function.  Add DPT$M_SVP flags as required for          */
/*              ioc_std$movtouser()/ioc_std$movfromuser().              */
/*              Reset module IDENT to match VDE.                        */
/*                                                                      */
/*      X-24    Atlant G. Schmidt                       28-DEC-1999	*/
/*									*/
/*              - Modify the ATAPI "Sony bypass", similar to the	*/
/*                removal of the bypass from the ATA/IDE code in X-14.	*/
/*                On our fastest processors, this bypass seems to 	*/
/*                hang the system as the driver tries to read the data	*/
/*                with PIO while the drive tries to DMA the data.	*/
/*                This change may be problematic for Sony drives	*/
/*                but they aren't supported by the hardware group	*/
/*                anyway and I haven't seen any Sony drives since	*/
/*                the very earliest one. They are believed to have	*/
/*                not been working correctly with this driver for a	*/
/*                while now, anyway.					*/
/*									*/
/*									*/
/*      X-23    Atlant G. Schmidt                       27-SEP-1999	*/
/*									*/
/*              - What a mess! While we believe that it *IS* possible	*/
/*                to make the Cypress and the Clipper co-exist (by	*/
/*                using buffers that are aligned to a 64KB boundary	*/
/*                in PCI space, thereby allowing a single PRDT entry	*/
/*                to cover the entire buffer, thereby allowing the	*/
/*                Cypress chip to work), we're concerned that there	*/
/*                may be other Cypress weirdnesses, especially given	*/
/*                the problems shown with certain models of disk	*/
/*                drives on the Eiger platform.				*/
/*									*/
/*                Therefore, we're finally following Fred K's lead	*/
/*                and de-committing from any DMA support on Cypress.	*/
/*                For performance reasons, this also means we're	*/
/*                de-committing from hard-disk support on Cypress.	*/
/*									*/
/*                This change is easily made -- if we see that the	*/
/*                controller is a Cypress, we'll clear the "Controller	*/
/*                is DMA-capable" bit in the UCB.			*/
/*									*/
/*                Having done this, this version re-instates the X-19	*/
/*                ("Clipper KZPAC in Hose 0") fix.			*/
/*									*/
/*									*/
/*									*/
/*      X-22    Atlant G. Schmidt                       01-SEP-1999	*/
/*									*/
/*              - The X-21 correction to the X-19 change was not	*/
/*                completely effective. While it did cure the		*/
/*                problem with the PRDT pointer, the Cypress chip	*/
/*                seemed to be suffering from several other problems	*/
/*                as well. X-22 backs out both the X-19 and X-21	*/
/*                changes, temporarily re-instating the Clipper		*/
/*                "KZPAC in Hose 0" bug but leaving in force		*/
/*                the X-20 build fix.					*/
/*									*/
/*									*/
/*      X-21   (THIS CHANGE HAS BEEN REMOVED!)				*/
/*									*/
/*              Atlant G. Schmidt                       27-AUG-1999	*/
/*									*/
/*              - Corrects a problem introduced in X-19 whereby		*/
/*                the Cypress chip doesn't reset its DMA pointer	*/
/*                if the transfer is an exact multiple of 8KB.		*/
/*									*/
/*                This is believed to be related to the fact that	*/
/*                X-19 made the PRDT windows each 8KB in length.	*/
/*									*/
/*                The PRDT is accessed by a 32-bit pointer value.	*/
/*                At the start of a block of DMA transfers, this	*/
/*                pointer is broadside loaded by DQDRIVER writing	*/
/*                the four bytes of the register, one byte at a time.	*/
/*                The low 16 bits of this register then increment	*/
/*                (by four) from time-to-time as PRD table entries	*/
/*                are consumed. I'm guessing that, immediately after	*/
/*                the counter has been incremented, a logic bug makes	*/
/*                the broadside-load fail. This leaves the PRD pointer	*/
/*                pointing to wrong entry in the PRD table and makes	*/
/*                our DMA target adresses that are "farther along"	*/
/*                in the buffer than its beginning. (Reading the	*/
/*                PRD pointer doesn't reveal the problem -- perhaps	*/
/*                there's an internal copy. Obvious hacks such as	*/
/*                reading back the bytes or writing the bytes twice	*/
/*                writing them in a different order had no effect	*/
/*                on the problem.)					*/
/*									*/
/*                The correction employed, at least for the moment,	*/
/*                is to avoid, on Cypress, transfers that are an	*/
/*                exact multiple of 8KB in length. These transfers	*/
/*                are instead fragmented into two transfers: one	*/
/*                of n-1 blocks and one of 1 block. This doubtless	*/
/*                hurts some because paging I/O tends to be multiples	*/
/*                of 8KB, but the subsequent second transfer is		*/
/*                almost certainly satisfied out of the disk drive's	*/
/*                cache so it shouldn't hurt as much as it might.	*/
/*									*/
/*									*/
/*      X-20    Atlant G. Schmidt                       24-JUN-1999	*/
/*									*/
/*              - Corrects a problem building in the V71R stream	*/
/*                (where the EV6'ish symbol IOC$K_BYTE isn't defined	*/
/*                and that I/O subfunction doesn't yet exist). We	*/
/*                now do the byte-laning ourselves (as in the EV4	*/
/*                through EV56 worlds). We also define the		*/
/*                IOC$K_BYTE_LANED symbol ourselves if it isn't		*/
/*                already defined (which it isn't in V71R).		*/
/*									*/
/*									*/
/*      X-19    Atlant G. Schmidt                       07-JUN-1999	*/
/*									*/
/*              - Corrects the problem whereby the PRDT didn't		*/
/*                handle the situation when the transfer buffer		*/
/*                (xfer_buffer) spanned more than one 64KB region	*/
/*                of PCI-bus address space. This typically occurred	*/
/*                when a Clipper had a KZPAC in Hose 0; the KZPAC	*/
/*                could allocate some map registers ahead of us,	*/
/*                forcing our map registers out of natural alignment.	*/
/*									*/
/*									*/
/*      X-18    Atlant G. Schmidt                       29-APR-1999	*/
/*									*/
/*              - Allow the SFF-8038 DMA registers to be located	*/
/*                anywhere within a 32MB I/O space. (For some		*/
/*                reason, when there's no video card in some machines,	*/
/*                the Console assigns these registers at *VERY* high	*/
/*                addresses.)						*/
/*              - Cleans up a few warnings from DECC /WARN=ENAB=ALL.	*/
/*              - Removes any reading of the register at offset		*/
/*                0x3F7/0x377 -- this register really belongs to	*/
/*                the Floppy Disk controller. Just for convenience,	*/
/*                we don't remove the CRAM or the UCB CRAM vector	*/
/*                entries so that all that stuff doesn't shift around	*/
/*                yet again.						*/
/*              - Tracing changes:					*/
/*                  o When compiled with NEVER defined, traces the	*/
/*                    initial state of all IDE registers whenever	*/
/*                    each drive does a PACKACK (or half-packack).	*/
/*                  o Traces the call to ioc$kp_reqchan because		*/
/*                    this is often the point where the trace changes	*/
/*                    from one drive to another.			*/
/*									*/
/*									*/
/*      X-17    Atlant G. Schmidt                       18-MAR-1999	*/
/*									*/
/*              This is the version released into V7.1-2R for		*/
/*                the Clipper/Brick hardware kit.			*/
/*									*/
/*              - Ignores the REL bit in the ATAPI Interrupt Reason	*/
/*                register. For some reason, when we operate multiple	*/
/*                drive simultaneously, we're seeing that bit set	*/
/*                even though we don't set OVL in the ATAPI Features	*/
/*                register.						*/
/*              - Fixes a problem whereby ATA drives operating in	*/
/*                PIO mode could miss errors.				*/
/*              - Fixes a problem introduced sometime around the	*/
/*                addition of DMA whereby ATAPI devices that used	*/
/*                2Kbyte sectors transfer only 1/4 as much data per	*/
/*                transfer as they should (owing to a 2048- versus	*/
/*                512-byte miscalculation.				*/
/*              - Includes a work-around to a UCB corruption problem	*/
/*                discovered by Dave Carlson during Brick testing.	*/
/*                This exhibited itself as VBNMAPFAIL bugchecks when	*/
/*                running QVET while booted from an IDE system disk.	*/
/*                The problem may or may not be within DQDRIVER but	*/
/*                the workaround is effective.				*/
/*              - Allows configuration of all odd-lettered units	*/
/*                (DQAn:, DQCn:, DQEn:, etc.) as using the Primary	*/
/*                IDE bus while all even-lettered units (DQBn:,		*/
/*                DQDn:, DQFn:, etc.) use the Secondary IDE bus.	*/
/*                This change may someday help resolve the problem	*/
/*                of redundant DQan: names in a SCSI cluster (because	*/
/*                the second system could configure DQCn: and DQDn:,	*/
/*                the thrid system DQEn: and DQFn:, and so forth.)	*/
/*              - Changes the tracing to optionally allow a single	*/
/*                trace log shared among the four units.		*/
/*              - Changes the tracing to allow STARTIO to capture	*/
/*                the IRP address, the passed-in LBA, and the passed-   */
/*                in bytecount info and also all four words of the	*/
/*                IOSB eventually returned.				*/
/*									*/
/*									*/
/*      X-16    Atlant G. Schmidt                       15-MAR-1999	*/
/*									*/
/*              - Adds "volatile" to the counter in the Brick delay	*/
/*                kludge so that the kludge will survive the		*/
/*                compiler's optimization.				*/
/*              - Updates the register cheat sheet, corrects some	*/
/*                typos regarding the DMA registers, and re-orders	*/
/*                REGDUMP's saving of the DMA registers.		*/
/*									*/
/*									*/
/*      X-15    Atlant G. Schmidt                       04-FEB-1999	*/
/*									*/
/*              - Introduces formal support for IDE (ATA) hard drives.	*/
/*                While this support has been latent in the driver	*/
/*                all along (and was the only point of the driver in	*/
/*                X-1 and X-2), only ATAPI drives were officially	*/
/*                supported since X-3. This restriction is now lifted.	*/
/*              - Adds half-DMA. This version will DMA into our		*/
/*                transfer buffer with us still using the CPU to	*/
/*                move data between the user and our transfer buffer.	*/
/*              - Unfortunately, this means we've now got some		*/
/*                code that's controller-chip-dependent. It may		*/
/*                also mean that we can only execute on PCIbus-based	*/
/*                controllers; although I've tried to allow continued	*/
/*                ISA bus operation, I have no ISA bus test platform	*/
/*                into which to stick my DTC2280 interface card.	*/
/*              - Removes a limitation with IDE (ATA) hard drives	*/
/*                larger than ~8.4GB. From Day 1, we were depending	*/
/*                on calculating IDE (ATA) hard drive capacity from	*/
/*                the Cylinder/Head/Sector (C/H/S) information		*/
/*                returned by the drive. Unfortunately, this tops	*/
/*                out at 8.455GB. Now, we look at the total LBN		*/
/*                information returned by the drive and use that	*/
/*                if it's non-zero and the C/H/S info seems to suggest	*/
/*                0x3FFF cylinders, 16 heads, and 63 sectors. (Unix	*/
/*                experienced problems with an old drive that reported	*/
/*                the total LBN information, but with the words		*/
/*                swapped! Hopefully, our check will avoid that		*/
/*                problem.)						*/
/*              - Consolidates all the very-similar WFIKPCH paragraphs	*/
/*                into a single routine and which now allows all of	*/
/*                the callers to handle unsolicited interrupts.		*/
/*              - Corrects the declaration of ucb$xxx_iohandle so that	*/
/*                its storage doesn't overlap ucb$l_unsolicited_int.	*/
/*                This bug, while latent in previous versions, had	*/
/*                no effects before the centralized interrupt-handler	*/
/*                was added in this baselevel.				*/
/*              - Hacks around a problem found very late in testing	*/
/*                whereby ATAPI drives that are sharing a bus with	*/
/*                a DQ-controlled system disk don't seem to have the	*/
/*                expected ATAPI signature in them. (Probably someone	*/
/*                before us has munched the signature already. Pre-)	*/
/*                viously, this problem ws mostly-masked by the fact	*/
/*                that such a drive was never auto-configured, but	*/
/*                Fred K's new SYS$ICBM.EXE will reveal this problem.)	*/
/*                We probably ought to re-init the drive, but that's	*/
/*                too risky a change to make just before submitting	*/
/*                the driver so I'm going to just bypass the ready-	*/
/*                test in this one instance. (The drives are known to	*/
/*                operate correctly henceforth.)			*/
/*              - Always builds the same UCB whether TRACING or not,	*/
/*                but marks the UCB distinctively if we're not tracing.	*/
/*              - The formerly-disjoint TRACING and INIBRK schemes	*/
/*                were rationalized and merged into one unified scheme.	*/
/*              - Introduces one more supportable hard drive (from	*/
/*                Fujitsu, a vendor we haven't seeen before) and one	*/
/*                more supportable CD-ROM drive (from Hitachi,		*/
/*                another vendor we haven't seen before), and one	*/
/*                more supportable DVD-ROM drive.			*/
/*									*/
/*									*/
/*      X-14    Atlant G. Schmidt                       14-JAN-1999	*/
/*									*/
/*              - Fixes a bug introduced by the X-13 change whereby	*/
/*                certain drives (such as the most-recent WDC Caviar	*/
/*                drives) are slow to de-assert DRQ when you've fed	*/
/*                them a sector of write data. They keep DRQ asserted	*/
/*                for a microsecond or two and we were mis-interpreting	*/
/*                that as the signal to bypass the wait-for-interrupt.	*/
/*                This was causing all sorts of trouble in the drive.	*/
/*                By comparison, the Quantum drives seem to immediately	*/
/*                remove DRQ and didn't provoke this problem.		*/
/*              - Kludges a problem whereby the Brick processor was	*/
/*                apparently able to over-run the DATA register upon	*/
/*                writing to the drive. Rather than just losing data,	*/
/*                this seemed to provoke some metastable behavior in	*/
/*                the ALT_STS register of both the Quantum and WDC	*/
/*                drives, causing them to occasionally show 0xFF as	*/
/*                their status. The kludge fix adds a small delay	*/
/*                after each write in MOVE_SEC_TO_DRIVE.		*/
/*              - Introduces several more supportable hard drives.	*/
/*              - Introduces support for the DS-10 platform.		*/
/*									*/
/*									*/
/*      X-13    Atlant G. Schmidt                       05-JAN-1999	*/
/*									*/
/*              This is the version released into V7.1-2R for		*/
/*                Blizzard Update 2.					*/
/*									*/
/*              - Fixes an apparent Day-1 bug whereby a fast IDE/ATA	*/
/*                drive can beat the loop around to the WFIKPCH,	*/
/*                generating an apparently unsolicited interrupt	*/
/*                and a subsequent WFIKPCH timeout. This condition	*/
/*                is analagous to the situation already handled by	*/
/*                the ATAPI loop. This problem was detected using	*/
/*                the latest WDC 36400 6.4GB IDE drive.			*/
/*									*/
/*									*/
/*      X-12    Atlant G. Schmidt                       23-DEC-1998	*/
/*              - Forks before calling EXE$KP_RESTART. This fix		*/
/*                was developed by Stephen Shirron to correct a		*/
/*                Day-1 problem which manifested itself as Clipper	*/
/*                multiprocessors hanging during installation		*/
/*                from the V7.1-2 CD-ROM.				*/
/*              - Adds init-time code to correctly identify the drive	*/
/*                type, removing the confusing "Generic SCSI disk"	*/
/*                appelation. This is done by using the logic that	*/
/*                used to pass the IO$_PACKACK for system disks to	*/
/*                now pass IO$_SENSECHAR for non-system disks.		*/
/*                STARTIO now dispatches this function to the half-	*/
/*                packack logic that's been latent in the driver	*/
/*                for several versions now. Also, the devtype string	*/
/*                is pre-loaded to "Generic IDE/ATAPI disk" prior to	*/
/*                doing any sizing.					*/
/*              - Upon failing to get basic information from a unit	*/
/*                during PACKACK or SENSECHAR, we now set its devtype	*/
/*                string to "Nonexistent IDE/ATAPI disk".		*/
/*              - Corrected an apparent typo in the ATA reset logic	*/
/*                which looks like it would have prevented recovering	*/
/*                from an attempt to reset a drive.			*/
/*              - The words of the ATAPI packet are now written with	*/
/*                a separate outw_t routine that always traces even	*/
/*                if TRACE_DATA_TOO isn't defined.			*/
/*              - We now trace the storing of the sense_key, asc, and	*/
/*                ascq. This makes it easier to read traces where	*/
/*                TRACE_DATA_TOO is turned off but the drive has	*/
/*                passed us a sense code or three.			*/
/*              - The old ucb$l_read_cmd and ucb$l_write_cmd fields,	*/
/*                which were mostly unused, are now entirely unused.	*/
/*                We're going to do DMA commands in a different way	*/
/*                than Ben had apparently originally envisioned.	*/
/*									*/
/*									*/
/*      X-11    Atlant G. Schmidt                       04-DEC-1998	*/
/*              - Re-ordered a few routines in the source so related	*/
/*                routines are closer together.				*/
/*              - Splits the read/write_atapi_segment routines into	*/
/*                separate routines for 512-byte-sector devices and	*/
/*                2Kbyte-sector devices. This allows easier support	*/
/*                of multi-block ATAPI reads.				*/
/*              - Re-introduces ATA transfers larger than 1 512-byte	*/
/*                block (backs out the X-2 change). This required	*/
/*                correcting a day-1 bug with regard to taking the	*/
/*                device lock for each sector transferred and also	*/
/*                correcting an X-10-introduced bug whereby our		*/
/*                transfer buffer offset was no longer getting		*/
/*                updated after each sector transferred. This bug	*/
/*                was introduced when I took out the level of		*/
/*                indirection in the params to move_sec_to/from_drive.  */
/*                This change appears to *DOUBLE* the PIO transfer	*/
/*                rate of ATA disk drives.				*/
/*              - Introduces ATAPI transfers larger than 1 512-byte	*/
/*                block for 512-byte sector devices (such as the Zip).	*/
/*                This change appears to improve the transfer rate of	*/
/*                the Zip drive (which is PIO only, so this is		*/
/*                important!) by about 15%.				*/
/*              - Introduces ATAPI transfers larger than 1 512-byte	*/
/*                block for 2Kbyte sector devices (such as CD-ROMs).	*/
/*                This change appears to ??? the PIO transfer rate	*/
/*                of a typical CD-ROM.					*/
/*              - wait_ready no longer depends on wait_busy for the	*/
/*                usual case where the drive is actually ready.		*/
/*              - I now set a ucb$l_media_id of DQ|IDE50 so that	*/
/*                MSCP will be willingto serve my disks across a	*/
/*                cluster.						*/
/*              - I now set the DEV$M_NLT bit in DEVCHAR2 so that	*/
/*                $INITIALIZE and ANALYZE/DISK won't look for		*/
/*                bad block info in the last track. This should		*/
/*                fix my own PTR 75-3-2500.				*/
/*              - Introduces some of the underpinnings to support	*/
/*                DMA. At least for the moment, the register mapping	*/
/*                window is increased to 64Kbytes to allow access	*/
/*                to the DMA registers.					*/
/*									*/
/*									*/
/*      X-10    Atlant G. Schmidt                       04-DEC-1998	*/
/*									*/
/*              This is the version released into V7.1-2R for		*/
/*                Blizzard Update 1.					*/
/*									*/
/*              - Adds Iomega Zip support				*/
/*              - As part of adding Zip support, adds an		*/
/*                write_atapi_segment routine to match up with		*/
/*                the existing write_ata_segment routine.		*/
/*              - As part of adding Zip support, moved the		*/
/*                interrupt-acknowledging RD_STS (almost) exclusively	*/
/*                to the ISR. It probably should have been this way	*/
/*                since Day 1.						*/
/*              - Corrects a customer-reported bug whereby ATAPI discs	*/
/*                that had a maximum LBN of 0x...1FF were sized 0x100	*/
/*                2k blocks (1024 512-byte blocks) too small.		*/
/*              - Corrects a possible buglette whereby an ATAPI drive	*/
/*                could have told us to transfer an invalid amount of	*/
/*                data and possibly cause us to over-run our allocated	*/
/*                buffer. The lower-level routines probably would have	*/
/*                prevented damage, but it's nicer to catch it early	*/
/*                and flag the error explicitly.			*/
/*              - Adds a full tracing facility that can be condition-	*/
/*                ally compiled into the driver.			*/
/*              - Takes out a level of indirection in passing the	*/
/*                buffer_adx parameter to both move_sec_to/from_drive.	*/
/*              - Splits the DEBUG feature into its four separate	*/
/*                components for easier selection.			*/
/*              - Standardizes a lot of comments and automatic		*/
/*                variable names among similar routines.		*/
/*									*/
/*									*/
/*      X-9     Atlant G. Schmidt                       18-NOV-1998	*/
/*									*/
/*              This is the version released into V7.1-2 and V7.2.	*/
/*									*/
/*              - Fix a Day-1 bug whereby the memory that we allocate	*/
/*                to build our system disk PACKACK IRP wasn't being	*/
/*                cleared (initialized). Sometimes, this led to the	*/
/*                IRP$V_FAST_FINISH bit (and others?) being		*/
/*                inadvertently set. This led to IOC_STD$REQCOM not	*/
/*                cleaning up after us and thereby leaving our		*/
/*                UCB$V_BSY bit set. This led to subsequent I/Os	*/
/*                assuming we were already busy and thereby not		*/
/*                calling our STARTIO entry point. This (finally!)	*/
/*                led to the system hanging trying to load SYSINIT.EXE.	*/
/*                Now we memset the memory to zeroes.			*/
/*									*/
/*									*/
/*      X-8     Atlant G. Schmidt                       13-OCT-1998	*/
/*              - Fixed-up the build instructions.			*/
/*              - Adds symbols for many items, removing "magic		*/
/*                numbers"						*/
/*              - Corrects a bug whereby, during packack, a second	*/
/*                command could be issued to a drive while the first	*/
/*                command was still executing. This caused many		*/
/*                very odd flaky effects including most of the		*/
/*                faiures originally blamed on the TEAC drive.		*/
/*                This fix also fixes an interrupt-timeout problem	*/
/*                the Panasonic SR-8583 DVD-ROM drive and some		*/
/*                intermittent errors with all Toshiba drives.		*/
/*              - Adds support for the Compaq/Panasonic SR8583		*/
/*                DVD-ROM.						*/
/*              - Substantially modifies and extends my INIBRK		*/
/*                debugging scheme.					*/
/*									*/
/*									*/
/*      X-7     Atlant G. Schmidt                       08-OCT-1998	*/
/*              - Changes the device timeout to 15 seconds		*/
/*                to accommodate the Toshiba XM-6302B CD-ROM drive.	*/
/*									*/
/*									*/
/*      X-6     Atlant G. Schmidt                       11-JUN-1998	*/
/*              - Corrects the problem Goldrush was having		*/
/*                autoconfiguring the DQDRIVER. The "hack" that we	*/
/*                grew from the original Ben Thomas code proved too	*/
/*                clever for its long-term good. Now the code only	*/
/*                accepts autoconfiguration or a manual, explicit	*/
/*                ISA bus I/O address.					*/
/*									*/
/*									*/
/*      X-5     Atlant G. Schmidt                       01-JUN-1998	*/
/*              - Correct two compile-time diagnostics newly issued	*/
/*                by DECC V5.7.						*/
/*									*/
/*									*/
/*      X-4     Atlant G. Schmidt                       27-MAY-1998	*/
/*              - Supply "F11" as the default ACP prefix at DDB$L_ACPD.	*/
/*                This wasn't necessary for Files-11 ODS2 support,	*/
/*                but *IS* necessary for ISO-9660 (F11CACP) and		*/
/*                High Sierra (F11DACP) support.			*/
/*									*/
/*									*/
/*      X-3     Atlant G. Schmidt                       30-SEP-1997	*/
/*									*/
/*              This is the version released as the V7.1-1H2 SHIP kit.	*/
/*									*/
/*              - Add ATAPI (IDE CD-ROM) support			*/
/*									*/
/*									*/
/*      X-2     CMF378          C M Fariz               19-Feb-1996	*/
/*              Update the driver with the necessary changes for 64bits.*/
/*									*/
/*              TEMPORARILY change the driver so that it writes only 1	*/
/*              sector at a time.  This is to work-around a problem	*/
/*              discovered in 3PB testing				*/
/*									*/
/*									*/
/*      X-1     Benjamin J. Thomas III                  May, 1994	*/
/*              Initial version.					*/
/*									*/
/*									*/
/************************************************************************/



/* Miscellaneous tidbits (from Ben, 'way back when)
 *
 *   o Always check the BUSY bit.  If set, none of the other bits
 *     can be fully trusted.  If clear, it's ok to believe the other
 *     bits and to issue commands.
 *
 *   o Make sure the correct drive is selected before any action.
 *     This means checking the BUSY bit for clear, then selecting
 *     the desired driver, then checking BUSY again for clear for
 *     that drive.  Note that VMS allows a channel concept, and this
 *     driver obtains ownership of the channel.  Therefore, we don't
 *     have to be quite so paranoid about ownership changes.  Get
 *     the channel, then get ownership...release the channel when
 *     all done.
 *
 *   o It's mandatory to keep the sector count to 256 or fewer
 *     sectors because that's the biggest value you can express
 *     in the byte-size SSEC_CNT register. And even then, do you
 *     trust all drives to interpret 0x00 as 256?  [AGS].
 *
 *   o It's a good idea to keep sector count under 128.  This is
 *     just a caution against drives that may not handle the number
 *     as unsigned. [Well, it's probably a doubly-good idea because
 *     the ATAPI byte-count is restricted to 65,536 (or 65,535 if
 *     you're a Zip drive) and 128*512=65,536! -- AGS]
 *
 *   o Read ALT_STS for the status bits unless the direct intent is
 *     also to clear the interrupt.
 *
 *   o Read/Write multiple commands don't seem to work very well
 *     all the time.  Based on advice, those commands seem aimed at
 *     treating the disk as if it had a different sector size.  This
 *     driver uses the simple read/write commands.  This means an
 *     interrupt per sector, which can't be avoided.  A DMA version
 *     could possibly help this (someday).  For now, read/writes
 *     will be done with the simple commands, and not exceeding 127
 *     blocks at a time (see previous bullet).
 *
 *
 * Basic ATA (IDE) transfer algorithm
 *
 *   1) Use ALT_STATUS to check for BUSY = 0
 *                      Error -> RESET, then retry once
 *   2) Select the proper drive
 *   3) Check ALT_STATUS again, for BUSY=0, DRDY = 1
 *                      Error -> RESET, retry from step 1
 *   4) Write the CSRs
 *   5) Write the command
 *   6) Wait for interrupt
 *                      Timeout -> RESET, retry from step 3
 *   7) Read STATUS (which clears interrupt)
 *   8) Transfer data is DRQ=1, BUSY=0
 *                      Error ->  DRQ=0 -> goto step 9 (ERR should =1)
 *                      BUSY=1-> RESET, retry from step 3
 *   9) Check that STATUS (saved from step 7) has ERR=0
 *                      Error -> handle error
 *  10) If not done, goto step 6 (multisector transfers)
 *      If done, goto step 3 (next command/transfer)
 *
 * A write would modify as:
 *
 *  5a) Check ALT_STATUS for BUSY=0, DRQ=1
 *  5b) Send all of the data
 *   8) Remove step 8
 *                       
 * Powerup algorithm
 *
 *   1) Poll ALT_STATUS for up to 40 seconds for BUSY=0, DRDY=1
 *                      Error -> Fatal
 *   2) Select drive 0
 *   3) Read ALT_STATUS for BUSY=0, DRDY=1
 *                      Error -> Fatal
 *   4) Drive 0 exists
 *   5) Select drive 1
 *   6) Read ALT_STATUS for BUSY=0, DRDY=1
 *                      Error->BUSY=1 fatal master/slave incompatability
 *                      DRDY=0  no drive 1
 *   7) drive 1 exists
 *
 *
 */



/* Basic Build instructions:
 * =========================
 *
 * $ COMPILE_IT:
 * $  CC /STANDARD=RELAXED_ANSI89 -
 *      /INSTRUCTION=NOFLOATING_POINT -
 *      /EXTERN=STRICT -
 *      /DEFINE=(BASEALIGN_SUPPORT)                           ! Not debugging
 *      /DEBUG /NOOPTIMIZE /DEFINE=(DEBUG,BASEALIGN_SUPPORT)  ! Debugging
 *      'CC_OPT' -
 *      /LIS='LISFILE' -
 *      /MACHINE_CODE -
 *      /OBJ='OBJFILE' -
 *      'SRCFILE'+SYS$LIBRARY:SYS$LIB_C.TLB /LIBRARY
 * $
 * $ LINK_IT:
 * $  GOSUB WRITE_OPTFILE
 * $  LINK /ALPHA -
 *         /USERLIB=PROC -
 *         /NATIVE_ONLY -
 *         /BPAGE=14 -
 *         /SECTION -
 *         /REPLACE -
 *         /NODEMAND_ZERO -
 *         /NOTRACEBACK -
 *         /SYSEXE -
 *         /NOSYSSHR -
 *         /SHARE='EXEFILE' -              ! Driver image
 *         /DSF='DSFFILE' -                ! Debug symbol file
 *         /SYMBOL='STBFILE' -             ! Symbol table
 *         /MAP='MAPFILE' /FULL /CROSS -   ! Map listing
 *         'OPTFILE' /OPTIONS              ! Linker options file
 * $
 * $ FINI:
 * $  EXIT 01
 * $
 * $
 * $ WRITE_OPTFILE:
 * $  OPEN/WRITE OPTIONS 'OPTFILE'
 * $  WRITE OPTIONS "SYMBOL_TABLE=GLOBALS"
 * $  WRITE OPTIONS ""
 * $  WRITE OPTIONS "CLUSTER=VMSDRIVER,,, -    ! Cluster_name, base_address, pfc,"
 * $  WRITE OPTIONS "      ", OBJFILE, ", -    ! Now filenames..."
 * $  WRITE OPTIONS "!     USER$DISK1:[]SYSLOA                         /INCLUDE=(KPRINTF)        /LIB, -"
 * $  WRITE OPTIONS "      SYS$LIBRARY:VMS$VOLATILE_PRIVATE_INTERFACES /INCLUDE=(BUGCHECK_CODES) /LIB, -"
 * $  WRITE OPTIONS "      SYS$LIBRARY:STARLET                         /INCLUDE=(SYS$DRIVER_INIT,SYS$DOINIT)"
 * $  WRITE OPTIONS ""
 * $  WRITE OPTIONS "COLLECT=NONPAGED_EXECUTE_PSECTS   /ATTRIBUTES=RESIDENT, -"
 * $  WRITE OPTIONS "    $CODE$"
 * $  WRITE OPTIONS ""
 * $  WRITE OPTIONS "COLLECT=NONPAGED_READWRITE_PSECTS /ATTRIBUTES=RESIDENT, -"
 * $  WRITE OPTIONS "    $PLIT$, -"
 * $  WRITE OPTIONS "    $INITIAL$, -"
 * $  WRITE OPTIONS "    $GLOBAL$, -"
 * $  WRITE OPTIONS "    $OWN$, -"
 * $  WRITE OPTIONS "    $$$105_PROLOGUE, -"
 * $  WRITE OPTIONS "    $$$110_DATA, -"
 * $  WRITE OPTIONS "    $$$115_LINKAGE, -"
 * $  WRITE OPTIONS "    $BSS$, -"
 * $  WRITE OPTIONS "    $DATA$, -"
 * $  WRITE OPTIONS "    $LINK$, -"
 * $  WRITE OPTIONS "    $LITERAL$, -"
 * $  WRITE OPTIONS "    $READONLY$"
 * $  WRITE OPTIONS ""
 * $  WRITE OPTIONS "COLLECT=INITIALIZATION_PSECTS     /ATTRIBUTES=INITIALIZATION_CODE, -"
 * $  WRITE OPTIONS "    EXEC$INIT_000, -"
 * $  WRITE OPTIONS "    EXEC$INIT_001, -"
 * $  WRITE OPTIONS "    EXEC$INIT_002, -"
 * $  WRITE OPTIONS "    EXEC$INIT_CODE, -"
 * $  WRITE OPTIONS "    EXEC$INIT_LINKAGE, -"
 * $  WRITE OPTIONS "    EXEC$INIT_SSTBL_000, -"
 * $  WRITE OPTIONS "    EXEC$INIT_SSTBL_001, -"
 * $  WRITE OPTIONS "    EXEC$INIT_SSTBL_002"
 * $  WRITE OPTIONS ""
 * $  WRITE OPTIONS "PSECT_ATTR=$LINK$,WRT"
 * $  WRITE OPTIONS "PSECT_ATTR=$INITIAL$,WRT"
 * $  WRITE OPTIONS "PSECT_ATTR=$LITERAL$,NOPIC,NOSHR,WRT"
 * $  WRITE OPTIONS "PSECT_ATTR=$READONLY$,NOPIC,NOSHR,WRT"
 * $  WRITE OPTIONS "PSECT_ATTR=$$$105_PROLOGUE,NOPIC"
 * $  WRITE OPTIONS "PSECT_ATTR=$$$110_DATA,NOPIC"
 * $  WRITE OPTIONS "PSECT_ATTR=$$$115_LINKAGE,WRT"
 * $  WRITE OPTIONS "PSECT_ATTR=EXEC$INIT_CODE,NOSHR"
 * $  WRITE OPTIONS ""
 * $  WRITE OPTIONS ""
 * $  CLOSE OPTIONS
 * $  RETURN
 *
 */



/* Usage Instructions:
 * ===================
 *
 * This driver was originally written as an IDE/ATA-only driver
 * and was tested with a $19 ISA IDE controller plugged into an
 * ISA bus. Since that time, it was extensively modified to also
 * operate ATAPI drives and to use the built-in IDE controllers
 * in several low-end systems. In either environment, someone
 * (VMS or the user) needs to make sure that everything is set up.
 *
 * For the system supported directly by VMS, this is easy. We include
 * an appropriate paragraph in [SYSEXE]SYS$CONFIG.DAT. This will
 * cause recognition of the PCI-to-ISA bridge chip and the driver
 * will be automatically loaded and units configured.
 *
 * For other systems where we don't explicitly support the driver
 * (such as the Digital AlphaStation 400 4/233), you must run the
 * ENABLE-IDE progam (from the Freeware CD) to enable the IDE port.
 * This port will use IRQ 14 and is on the ISA bus.  You may need
 * to run the console's ISACFG utility to reserve IRQ 14 for the
 * device.  For a separate ISA board, there is no need to do any
 * special hardware setup.
 *
 *
 * A sample ISACFG line, might look like:
 *
 * >>>isacfg -mk -slot 3 -dev 0 -iobase 1f0 -irq0 14 -etyp 1 -enadev 1 -handle IDE
 *
 *
 * To load the driver, use the following command:
 *
 *   $ MCR SYSMAN
 *     SYSMAN> IO CONNECT DQA0: -
 *                 /DRIVER=SYS$DQDRIVER -
 *                 /CSR=%X1F0 -
 *                 /ADAPTER=x -
 *                 /VECTOR=y -
 *                 /NODE=n
 *
 * This command assumes (or requires):
 *
 *   CSR is %X1F0, this is the standard address for a primary IDE
 *
 *   VECTOR is usually 56 (IRQ=14, 14*4=56)
 *
 *   ADAPTER is the TR Number of the bus adapter that hosts
 *     the controller, usually found using $ MCR SYSMAN IO SHOW BUS
 *     or SDA> CLUE CONFIGURATION.
 *
 *   NODE is the slot number into which the board was plugged.
 *
 *
 */

        

/* Supported and/or tested devices
 *
 * So far, this driver is known to have been recently tested
 * with the following devices:
 *
 * Magnetic disk drives:
 *
 *   Conner CP-3024          (Ancient 20MB drive)
 *   Conner CP-2084          (Old 85MB 2-1/2" drive, aka RED11-E)
 *   Fujitsu MPD3108AT       (Very recent 10GB drive)
 *   Quantum ProDrive LPS52A (Ancient 52MB drive)
 *   Quantum LP240A          (Old 245MB drive, aka RE24L-E, like ProDrive LPS240)
 *   Quantum Fireball 1080A  (Recent 1.090 GB drive)
 *   Quantum Fireball SE8.4A (Very Recent 8.455GB drive)
 *   Quantum Fireball SE6.4A (Very Recent 6.4GB drive)
 *   Quantum Fireball SE4.3A (Very Recent 4.3GB drive)
 *   WDC Caviar 2850         (Recent 833MB drive)
 *   WDC Caviar 31200        (Recent 1.xGB drive) (1)
 *   WDC Caviar 21600        (Recent 1.6GB drive)
 *   WDC Caviar 24300        (Recent 4.3GB drive)
 *   WDC Caviar 33200        (Recent 3.2GB drive)
 *   WDC Caviar 36400        (Very Recent 6.4GB drive) (2)
 *
 *
 * CD-ROM drives:
 *
 *   Compaq CR-588     (32x CD-ROM OEM'ed from Panasonic/Matsushita)
 *   Compaq CRD-8322B  (32x CD-ROM OEM'ed from Lucky/Goldstar)
 *   Hitachi CDR-8435  (32x CD-ROM) (3,4)
 *   Sony CDU711       (32x CD-ROM)
 *   TEAC CD-532E      (32x CD-ROM) (3,5)
 *   Toshiba XM-5702B  (12x CD-ROM)
 *   Toshiba XM-6102B  (12x-to-24x CD-ROM)
 *   Toshiba XM-6202B  (32x CD-ROM)
 *   Toshiba XM-6302B  (32x CD-ROM) (6)
 *   Toshiba XM-6402B  (32x CD-ROM rebranded as a "Compaq XM-6402B")
 *
 *
 * DVD-ROM drives:
 *
 *   Compaq SR-8583    (as a ?? CD-ROM drive, OEM'ed from Panasonic/Matsushita)
 *   Toshiba SD-M1102  (as a 24x CD-ROM drive) (7)
 *   Toshiba SD-M1212  (as a ?? CD-ROM drive)
 *
 *
 * ATAPI Read/Write drives:
 *
 *   Iomega ZIP 100    100 MB diskette drive (8)
 *
 *
 * Notes:
 *
 *   (1) This WDC Caviar drive eventually experienced data corruption
 *       on writes. The drive also failed PC-based diagnostics so I
 *       assume a genuine hardware failure occurred; the drive was
 *       returned to the vendor for exchange and a different, more-
 *       recent model was returned.
 *
 *   (2) This WDC Caviar drive was observed to be slow to de-assert
 *       DRQ after you've fed it the 0x200 bytes of write data.
 *       Because of this, you can't use DRQ .AND. NOT_BUSY to
 *       bypass the WFIKPTCH the way we do for CD-ROM drives.
 *
 *   (3) The Hitachi and TEAC CD-ROM drives contain a green activity LED
 *       rather than the more-usual orange LED.
 *
 *   (4) The Hitachi CD-ROM drive doesn't contain any front-panel audio
 *       output (earphone) connector or volume control.
 *
 *   (5) Due to electrical considerations on the IDE bus regarding
 *       pull-up and pull-down resistors, the TEAC CD532-E CD-ROM
 *       drive causes problems in multi-drive configurations. The
 *       drive is believed to be unsupported by hardware engineering.
 *
 *   (6) One sample (out of three) of this Toshiba XM-6302B CD-ROM
 *       drive was getting random positioning errors and was returned
 *       to Storage Engineering (along with the failing CD-ROM disc);
 *       this sample of this drive is no longer available for testing.
 *
 *   (7) My one sample of this Toshiba SD-M1102 DVD-ROM drive has
 *       since suffered a hardware failure and is no longer available
 *       for testing.
 *
 *   (8) The version of the Zip drive that was tested succesfully was
 *       labled as Iomega "P/N 270928-003" (beige bezel) or the apparently
 *       electrically-similar "P/N 270928-703" (with a white bezel). These
 *       were "Oprah 2" mechanisms and contained V12.A firmware.
 *
 *       Another version labled as Digital "P/N PCXRJ-AG, Rev 001" was
 *       also tested but experienced *VERY OCCASIONAL* timeout errors
 *       that were recovered on retries within the driver. This drive,
 *       on the other hand, appears about one third faster than the
 *       Iomega-labled drive when running the "ALIGN" buffer-alignment
 *       stress test. This was an "Oprah" drive and contained V23.D firmware.
 *
 *      "Bought-off-the-street" generic Iomega Zip drives were not tested
 *       but we believe they track the revs of the Iomega-branded part.
 *
 */



/* Caveats:
 *
 * This driver was written from the X3T9.2 specs, which may or may not
 * accurately reflect working drives. The driver has been tested on
 * a few different IDE drives, but the following are known items to
 * watch out for:
 *
 *   o The driver ran into a lot of weird problems supporting
 *     DMA on the Cypress chips (documented in the revision
 *     history). For this reason, DMA is only supported on
 *     the Acer chips.
 *
 *   o Driver has been used only with 16 bit data interfaces
 *
 * I'm sure that there are others - good luck.
 *
 *
 * One obvious place for improvement is the copying of data to and from
 * the transfer buffer.  It would be better to move the data directly
 * to/from the user buffer, but there were some reasons that I didn't.
 * First, it's just plain a pain to get it right.  You have to account
 * for the alignment issues;  this is no good if you are constantly
 * taking alignment faults or if you are constantly loading/merging
 * data.  Secondly, you need to always empty/fill the disk's buffer
 * and that means handling non-sector-sized counts.  Overall, not too
 * hard, but a pain. Finally, the SFF-8038i spec simply isn't capable
 * of handling buffers that aren't at least word-aligned, so we'd often
 * end up copying data around anyway.
 *
 *
 */



/* Registers - Here's my register cheat sheet.
 *
 * +-----+-----+-----+-----++-----+-----+-----+-----+
 * |  7  |  6  |  5  |  4  ||  3  |  2  |  1  |  0  |   Pri ISA Adx   SYMBOL
 * +-----+-----+-----+-----++-----+-----+-----+-----+   [CRAM idx]    Name
 *
 * +-----+-----+-----+-DSC/++-----+-----+-----+-----+   [00]    Alternate Status
 * | BSY | DRDY| DWF | SVC || DRQ |CORR | IDX | ERR |   3F6 R   REG_ALT_STS
 * |-----+-----+-----+-----++-----+-----+-----+-----|
 * |  x  |   x |  x  |  x  ||  1  | SRST| nIEN|  0  |   3F6 W   REG_DEV_CTL
 * +-----+-----+-----+-----++-----+-----+-----+-----+   [01]    Device control
 *
 *
 * +-----+-----+-----+-----++-----+-----+-----+-----+
 * | HiZ | nWTG| nHS3| nHS2|| nHS1| nHS0| nDS1| nDS0|   3F7 R   REG_DRV_ADDR
 * +-----+-----+-----+-----++-----+-----+-----+-----+   [02/03] Drive address
 *                                                      (Belongs to FDC!!!)
 *
 * +-----+-----+-----+-----++-----+-----+-----+-----+
 * |                                                |   1F0 RW  REG_DATA
 * +-----+-----+-----+-----++-----+-----+-----+-----+   [04/05] Data (16 bits)
 *
 *
 * +-----+-----+-----+-----++-----+-----+-----+-----+   [06]    Error
 * | BBK | UNC | MC  | IDNF|| MCR |ABRT |TK0NF|AMNF |   1F1 R   REG_ERROR
 * +-----+-----+-----+-----++-----+-----+-----+-----+
 * |                                                |   1F1 W   REG_FEATURES
 * +-----+-----+-----+-----++-----+-----+-----+-----+   [07]    Features
 *
 *
 * +-----+-----+-----+-----++-----+-----+-----+-----+
 * |                                                |   1F2 RW  REG_SEC_CNT
 * +-----+-----+-----+-----++-----+-----+-----+-----+   [08/09] Sector count
 * |                              | RLS | I/O | CoD |           Reason
 * +-----+-----+-----+-----++-----+-----+-----+-----+
 *
 * +-----+-----+-----+-----++-----+-----+-----+-----+           (REG_LBA_0)
 * |                                                |   1F3 RW  REG_SECTOR
 * +-----+-----+-----+-----++-----+-----+-----+-----+   [0A/0B] Sector number
 *                                                              (or LBA0-7)
 *
 * +-----+-----+-----+-----++-----+-----+-----+-----+           (REG_LBA_8)
 * |                                                |   1F4 RW  REG_CYL_LO
 * +-----+-----+-----+-----++-----+-----+-----+-----+   [0C/0D] Cylinder low
 *                                                              (or LBA8-15)
 *
 * +-----+-----+-----+-----++-----+-----+-----+-----+           (REG_LBA_16)
 * |                                                |   1F5 RW  REG_CYL_HI
 * +-----+-----+-----+-----++-----+-----+-----+-----+   [0E/0F] Cylinder high
 *                                                              (or LBA16-23)
 *
 * +-----+-LBA-+-----+-----++-----+-----+-----+-----+           (REG_LBA_24)
 * |  1  | MODE|  1  | DRV || HS3 | HS2 | HS1 | HS0 |   1F6 RW  REG_DRV_HD
 * +-----+-----+-----+-----++-----+-----+-----+-----+   [10/11] Drive/head
 *                                                              (or LBA24-27)
 *
 * +-----+-----+-----+-DSC/++-----+-----+-----+-----+   [12]    Status
 * | BSY | DRDY| DWF | SVC || DRQ |CORR | IDX | ERR |   1F7 R   REG_STS
 * +-----+-----+-----+-----++-----+-----+-----+-----+
 * |                                                |   1F7 W   REG_CMD
 * +-----+-----+-----+-----++-----+-----+-----+-----+   [13]    Command
 *
 */



/* DMA Registers
 *
 * +-----+-----+-----+-----++-----+-----+-----+-----+
 * |  7  |  6  |  5  |  4  ||  3  |  2  |  1  |  0  |   Pri ISA Adx   SYMBOL
 * +-----+-----+-----+-----++-----+-----+-----+-----+   [CRAM idx]    Name
 *
 * +-----+-----+-----+-----++-----+-----+-----+-----+
 * |XXXXX|XXXXX|XXXXX|XXXXX|| R/W |XXXXX|XXXXX| ACT |   nnn0 R/W  DMA_CMD
 * +-----+-----+-----+-----++-----+-----+-----+-----+   [14/15]   DMA Cmd Register
 *                                                              
 * +-----+-----+-----+-----++-----+-----+-----+-----+
 * |  ?  |  ?  |  ?  |  ?  ||  ?  |  ?  |  ?  |  ?  |   nnn1 R/W  DMA_DS1
 * +-----+-----+-----+-----++-----+-----+-----+-----+   [16/17]   Device-specific Register #1
 *
 * +-----+-----+-----+-----++-----+-----+-----+-----+
 * |Smplx| DRV1| DRV0|XXXXX||XXXXX| Int | Err | BSY |   nnn2 R/W  DMA_STS
 * +-Only+-DMA-+-DMA-+-----++-----+-----+-----+-----+   [18/19]   DMA Status Register
 *
 * +-----+-----+-----+-----++-----+-----+-----+-----+
 * |  ?  |  ?  |  ?  |  ?  ||  ?  |  ?  |  ?  |  ?  |   nnn3 R/W  DMA_DS2
 * +-----+-----+-----+-----++-----+-----+-----+-----+   [1A/1B]   Device-specific Register #2
 *
 * +-----+-----+-----+-----++-----+-----+-----+-----+             (LSB)
 * |                                    |XXXXX|XXXXX|   nnn4 R/W  DMA_AD0
 * +-----+-----+-----+-----++-----+-----+-----+-----+   [1C/1D]   PRDT Address Byte 0
 *
 * +-----+-----+-----+-----++-----+-----+-----+-----+
 * |                                                |   nnn5 R/W  DMA_AD1
 * +-----+-----+-----+-----++-----+-----+-----+-----+   [1E/1F]   PRDT Address Byte 1
 *
 * +-----+-----+-----+-----++-----+-----+-----+-----+
 * |                                                |   nnn6 R/W  DMA_AD2
 * +-----+-----+-----+-----++-----+-----+-----+-----+   [20/21]   PRDT Address Byte 2
 *
 * +-----+-----+-----+-----++-----+-----+-----+-----+             (MSB)
 * |                                                |   nnn7 R/W  DMA_AD3
 * +-----+-----+-----+-----++-----+-----+-----+-----+   [22/23]   PRDT Address Byte 3
 *
 *
 *
 *   +----------------------+--------------------+-+
 *   |          Base Address  [31:01]            |0|
 *   +---+------------------+--------------------+-+    A PRDT entry
 *   |EOT| ---------------- | Byte Count [15:01] |0|
 *   +---+------------------+--------------------+-+
 *
 *
 */



/* IDE/ATA Basics
 *
 * IDE ("Integrated Device Electronics" or "Integrated Drive Electronics",
 * depending on whom you ask) is a very inexpensive way to connect disk
 * drives to computers. The drive bus is, essentially, an extension of
 * the ISA bus with more-limited functionality. This interface first
 * made its appearance on the IBM PC-AT computer, hence its other
 * name: ATA  -- AT Attachment.
 *
 * Most modern IDE controller chips actually emit two separate
 * IDE busses known as the Primary and Secondary busses. On each
 * bus, you can connect up to two drives which are knwon for
 * historical reasons as the "Master" and "Slave" drives although
 * all modern drives are completely autonomous. This leads to the
 * following basic block diagram:
 *
 *                                        +-------+      +-------+
 *                                        |Master |      | Slave |
 *                                        | Drive |      | Drive |
 *                                        | DQA0: |      | DQA1: |
 *       +------------+                   +-------+      +-------+
 *       |            |______________________| |____________| |
 *  -----|    Dual    |___Primary IDE Bus_____________________|
 *   PCI |    IDE     |
 *   bus | Controller |_______________________________________
 *  -----|            |___Secondary IDE Bus__   ____________  |
 *       |            |                      | |            | |
 *       +------------+                   +-------+      +-------+
 *                                        |Master |      | Slave |
 *                                        | Drive |      | Drive |
 *                                        | DQB0: |      | DQB1: |
 *                                        +-------+      +-------+
 *
 * From an "ISA" perspective, the two halves of the IDE controllers
 * usually are completely autonomous and use separate sets of "legacy"
 * addresses and IRQs:
 *
 *                       |      Primary      |    Secondary
 *  ---------------------+-------------------+-------------------
 *    IDE Address        |  0x3F6 and 0x1F0  |  0x376 and 0x170
 *    DMA Reg Addresses  |      0x90A0       |      0x90A8
 *    IRQ                |        14.        |        15.
 *
 *
 * NOTE:
 *
 *   Not all implementations "pin-out" the Secondary IDE bus.
 *
 */



/* IDE/ATA Basics (Cont'd)
 *
 *  From a PCI perspective, it's a little more confusing.
 *  There are two basic approaches:
 *
 *  Shared PCI Config Space registers
 *  ---------------------------------
 *
 *  Here, the two IDE ports share one set of PCI config space registers.
 *  The Intel SIO and Acer Labs chips exemplify this school of thought.
 *
 *                                            +-------+      +-------+
 *           +------------+                   | Drive |      | Drive |
 *      -----|   ISA      |                   |Master |      | Slave |
 *     |   --|  Bridge    |                   | DQA0: |      | DQA1: |
 *     |  |  +------------+                   +-------+      +-------+
 *     |  |  |            |______________________| |____________| |
 *   --    --|    Dual    |___Primary IDE Bus_____________________|
 *      PCI  |    IDE     |
 *      bus  | Controller |_______________________________________
 *   --    --|            |___Secondary IDE Bus__   ____________  |
 *     |  |  |            |                      | |            | |
 *     |  |  +------------+                   +-------+      +-------+
 *     |   --|    USB     |                   |Master |      | Slave |
 *      -----|   Bridge   |                   | Drive |      | Drive |
 *           +------------+                   | DQB0: |      | DQB1: |
 *                                            +-------+      +-------+
 *
 *
 *  Separate PCI Config Space registers
 *  -----------------------------------
 *
 *  Here, the each IDE port has its own set of PCI config space registers.
 *  The Cypress chip exemplifies this school of thought.
 *
 *                                            +-------+      +-------+
 *           +------------+                   | Drive |      | Drive |
 *      -----|   ISA      |                   |Master |      | Slave |
 *     |   --|  Bridge    |                   | DQA0: |      | DQA1: |
 *     |  |  +------------+                   +-------+      +-------+
 *     |   --|  Primary   |______________________| |____________| |
 *     |   --|    IDE     |___Primary IDE Bus_____________________|
 *   --   |  | Controller |
 *   PCI  |  +------------+
 *   bus  |  | Secondary  |_______________________________________
 *   --    --|    IDE     |___Secondary IDE Bus__   ____________  |
 *     |   --| Controller |                      | |            | |
 *     |  |  +------------+                   +-------+      +-------+
 *     |   --|    USB     |                   |Master |      | Slave |
 *      -----|   Bridge   |                   | Drive |      | Drive |
 *           +------------+                   | DQB0: |      | DQB1: |
 *
 */




/* ATAPI Basics
 *
 * ATAPI (ATA Packet Interface) eveolved when it became apparent
 * that the limited functionality of an ATA interface would not
 * be sufficient to handle devices that were more-complicated
 * than ordinary fixed-media magnetic disk drives.
 *
 * Essentially, ATAPI allows SCSI commands to be passed over
 * the ATA bus and SCSI status to be returned over the ATA bus.
 * It's vastly simplified SCSI, though, with no concept of
 * disconnect/reconnect, much-simplified phases, and so on.
 *
 */



/* DISK ADDRESSING
 *
 * Disk addressing in the ATA world is, umm, "less than straight-forward".
 *
 * Firstly, there are two major modes of addressing:
 *
 *  o CHS -- "Cylinder, Head, and Sector" mode
 *  o LBA -- "Logical Block Addressing" mode
 *  o MSF -- "Minutes:Seconds:Frames" mode
 *
 *
 * CHS mode is the legacy mode (from whence came the famous 528MB
 * IDE disk-size limit). In this mode, you select the disk address
 * by loading values into three registers:
 *
 *   o The 16-bit Cylinder Register
 *   o The 8-bit Sector Register
 *   o The 4-bit Head (Track) Register
 *
 * This sounds easy enough, but just to make your life more interesting,
 * the numbering scheme is a bit screwy:
 *
 *   o The Cylinders are numbered from 0 to x.
 *     The identify_device command returns the number of cylinders (so x+1).
 *
 *   o The Sectors are numbered from 1 to y.
 *     The identify_device command returns the number of sectors (so y).
 *
 *   o The Heads are numbered from 0 to z.
 *     The identify_device command returns the number of heads (so z+1).
 *
 *
 * And just FYI, early BIOS's were limited to 1024 cylinders [0000:03FF],
 * 63 sectors [00:3F], and 16 heads [00:0F]. This produces 1032192 total
 * blocks or 528,482,304 total bytes.
 *
 *
 * LBA mode is newer and simple. The 28-bit Logical Block Number is
 * simply parsed up into an 8-bit low portion that is then stuffed
 * into the Sector Register, a 16-bit middle portion that is then
 * stuffed into the Cylinder Register, and a 4-bit high portion
 * that is stuffed into the Head Register. And all three parts
 * are zero-based.
 *
 *
 * As more FYI, many recent PC BIOS are limited to a 24-bit disk address
 * consisting of 14 bits of cylinder, 4 bits of head, and six bits of
 * sector. This limits disks on these BIOS's to either
 *
 *   16383 (cyls) * 16 (heads) * 63 (sectors) * 512 = 8,455 million bytes
 *
 *     - or -
 *
 *   16383-1024 (cyls) * 16 (heads) * 63 (sectors) * 512 = 7,927 million bytes
 *
 * This driver suffers from no such limit.
 *
 *
 * Fiinally, all CDs (SCSI or ATA) can also be addressed in an audio-
 * relevant mode called "Minutes:Seconds:Frame" (MSF) mode. The
 * resulting LBN is simply:
 *
 *     LBN = ( ( ( Min * 60 ) + Seconds ) * 75 ) + Frames
 *
 */



/* CD-DA BASICS
 *
 * CD's all use the same basic physical structure, a 2352-byte
 * block ("big frame") of data. On the original CD-DA (Compact Disk
 * Digital Audio) disks, this corresponds to 1/75 of a second of music
 * sampled at 44,100 Hz rate, 2 channels, 16-bits/sample.
 *
 * So:  44,100 (samples/s) * 2 (chans/sample) * 2 (bytes/chan) =
 *                                           176,400 bytes/sec
 * And: 2352 (bytes/frame) * 75 (frames/s) = 176,400 bytes/second
 *
 * This is the famous 1X CD data transfer rate.
 *
 *
 * Within this 2352-byte frame, there's actually a finer-grained structure.
 * The big frame actually consists of 98 "small frames" of 24 data bytes
 * each. These small frames actually contain:
 *
 *   +------+----------+-----------+----------+-----------+----------+
 *   | Sync | Sub-chnl |   data    |   CRC    |   data    |   CRC    |
 *   | 24+3 |   14+3   | 12*(14+3) | 4*(14+3) | 12*(14+3) | 4*(14+3) |
 *   +------+----------+-----------+----------+-----------+----------+
 *
 * For a total of 588 bits per small frame.
 *
 * The sets of "14" bits come about through EFM -- Eight-to-Fourteen
 * Modulation. To mimimize dc and high frequency content and to control
 * the maximum and minimum pit lengths (the dual of the frequency-domain
 * stuff), each byte is converted by a look-up table in the drive from/
 * to a unique run of 14 bits. The sets of "3" bits then come into play
 * in so-called "merging bits" that further minimize dc content and
 * control minimum and maximum pit length between bytes.
 *
 * The CRC bits aren't exactly a CRC but I'll describe them more
 * fully in the next release.
 *
 * So 98 small frames contain 98 * (12*2) = 2352 data bytes.
 *
 * The sub-channel byte is used (bitwise) to provide eight sub-
 * channels of data called P, Q, R, S, T, U, V, and W:
 *
 *   o The "P" Channel provides "Pause" information, signalling
 *     the time interval between tracks or times when the audio
 *     should be muted (such as computer data tracks).
 *
 *   o The "Q" Channel provides most of the interesting information
 *     such as the lead-in and lead-out indications, track number,
 *     minutes:seconds:frames absolute and relative time, the
 *     Universal Product Code (UPC), and International Standard
 *     Record Code (ISRC).
 *
 */



/* CD-ROM STANDARDS
 *
 * The great thing about standards is that there are so many of them!
 *
 * Built atop the basic CD-DA standard are a variety of CD data storage
 * standards. Basically, they all devolve down to storing data in five
 * types of records, all superimposed within the basic 2352-byte CD-DA
 * frame:
 *
 *    CD-DA           +-----------------------------------------------------+
 *   "Red Book"       |     2352 data bytes, no extra error-correction      |
 *    Audio           +-----------------------------------------------------+
 *
 *                    +------+--------+---------------+-----+-------+-------+
 *    CD-ROM          | sync | header |   user data   | EDC | blank |  ECC  |
 *   "Yellow Book"    | 12 b |  4 b   |  2048 bytes   | 4 b |  8 b  | 276 b |
 *    Mode-1 Data     +------+--------+---------------+-----+-------+-------+
 *
 *                    +------+--------+-------------------------------------+
 *    CD-ROM          | sync | header |    user data, no extra error-corr   |
 *   "Yellow Book"    | 12 b |  4 b   |             2336 bytes              |
 *    Mode-2 Data     +------+--------+-------------------------------------+
 *
 *                    +------+--------+--------+---------------+-----+-------+
 *    CDROM-XA        | sync | header | subhdr |   user data   | EDC |  ECC  |
 *   "Green Book"     | 12 b |  4 b   |  8 b   |   2048 bytes  | 4 b | 276 b |
 *    Mode-2, Form 1  +------+--------+--------+---------------+-----+-------+
 *
 *                    +------+--------+--------+-----------------------+-----+
 *    CDROM-XA        | sync | header | subhdr |       user data       | EDC |
 *   "Green Book"     | 12 b |  4 b   |  8 b   |       2324 bytes      | 4 b |
 *    Mode-2, Form 2  +------+--------+--------+-----------------------+-----+
 *
 *
 *
 * The "header" field is the LBN, used for position-checking just as in
 * magnetic disk drive headers.
 *
 * Green Book disks allow intermixing audio, video, and data sectors
 * within a track. The sub-header describes which type of data is stored
 * in each sector.      
 *
 * The EDC is the extended Error Detection Code.
 *
 * The ECC is the Error Correction Code.
 *
 *
 * *NOTE*
 *
 *   *THIS DRIVER ONLY SUPPORTS YELLOW BOOK, MODE 1 (2048-byte) SECTORS!*
 *
 */



/* CD TRACKS, TOCs, AND SESSIONS
 *
 * CDs are divided into "tracks, which are simply collections of
 * contiguous blocks. A Table-of-Contents describes the tracks
 * within a disk (or session of a multi-session disk).
 *
 * Within a track, the blocks must all be of the same "color".
 * That is, they must be entirely:
 *
 *    o Red Book CD-DA, or
 *    o Yellow-Book CD-ROM, or
 *    o Green Book (which allows mixing Audio, Video, and Data)
 *
 *
 * So-called "Mixed Mode" disks may store one or more tracks of CD-ROM
 * and one or more tracks of audio data. The most common way to do this
 * is to record the CD-ROM data on Track 1 and the Audio data on all
 * subsequent tracks.
 *
 *
 * The Table-of-Contents (TOC) is stored as Q-subcode data within the
 * Lead-in track of the CD.
 *
 *
 * Conceptually, an entire CD is recorded in one "session", from
 * lead-in (and TOC) through the data tracks, through to the lead-out
 * track. At that point, the session is "closed" and no further data
 * can be recorded on the CD-ROM (because it won't be described in
 * the already-recorded TOC).
 *
 * More-modern CD drives can read additional complete "sessions"
 * (complete with their own TOCs) beyond the first session. PhotoCDs
 * are probably the most common example of such a multi-session
 * CD(-ROM). I have never tested this driver with a multi-session
 * CD and have no idea if it works. It's quite unlikely: since each
 * session is, conceptually, a complete CD, we would probably need
 * to materialize additional devices to the VMS file system so that
 * each session could be separately mounted.
 *
 */



/* Mapping 512-byte VMS blocks to 2K-byte sectors
 *
 * Because CD-ROM drives read 2Kbyte sectors, it is necessary to
 * map VMS's 512-byte blocks onto this structure. The mapping is
 * straightforward, mapping 4 VMS LBNs into each CD-ROM LBN. The
 * only interesting part is determining how many CD-ROM sectors
 * to read. The equation used is:
 *
 *    # of CD_ROM sectors = ( Int( VMS_blocks + VMS_LBN%4 - 1 ) / 4 ) + 1
 *
 * The diagrams below show all the interesting cases of differing
 * VMS read lengths and VMS 512-byte block alignment within the 2Kbyte
 * CD-ROM sectors.
 *
 *
 *                                                                                   __
 *  Length: 0                                                                          |
 *                                                                                     |
 *  +|--------+---------+---------+---------+  Length =  0                             |
 *  ||0 1 2 3 | 4 5 6 7 | 8 9 A B | C D E F |  Offset = _0_                            |
 *  +|--------+---------+---------+---------+            0 --> Would read 0 2K blocks  |
 *                                                                                     |
 *  +--|------+---------+---------+---------+  Length =  0                             |
 *  | 0|1 2 3 | 4 5 6 7 | 8 9 A B | C D E F |  Offset = _1_                            |
 *  +--|------+---------+---------+---------+            1 --> Would read 1 2K block   |-  Special case all these
 *                                                                                     |
 *  +----|----+---------+---------+---------+  Length =  0                             |
 *  | 0 1|2 3 | 4 5 6 7 | 8 9 A B | C D E F |  Offset = _2_                            |
 *  +----|----+---------+---------+---------+            2 --> Would read 1 2K block   |
 *                                                                                     |
 *  +------|--+---------+---------+---------+  Length =  0                             |
 *  | 0 1 2|3 | 4 5 6 7 | 8 9 A B | C D E F |  Offset = _3_                            |
 *  +------|--+---------+---------+---------+            3 --> Would read 1 2K block   |
 *                                                                                   __|
 *
 *  Length: 1
 *    _
 *  +|-|------+---------+---------+---------+  Length =  1
 *  ||0|1 2 3 | 4 5 6 7 | 8 9 A B | C D E F |  Offset = _0_
 *  +|_|------+---------+---------+---------+            1 --> Read 1 2K block
 *      _
 *  +--|-|----+---------+---------+---------+  Length =  1
 *  | 0|1|2 3 | 4 5 6 7 | 8 9 A B | C D E F |  Offset = _1_
 *  +--|_|----+---------+---------+---------+            2 --> Read 1 2K block
 *        _
 *  +----|-|--+---------+---------+---------+  Length =  1
 *  | 0 1|2|3 | 4 5 6 7 | 8 9 A B | C D E F |  Offset = _2_
 *  +----|_|--+---------+---------+---------+            3 --> Read 1 2K block
 *          _
 *  +------|-|+---------+---------+---------+  Length =  1
 *  | 0 1 2|3|| 4 5 6 7 | 8 9 A B | C D E F |  Offset = _3_
 *  +------|_|+---------+---------+---------+            4 --> Read 1 2K block
 *
 *
 *  Length: 2
 *    ___
 *  +|---|----+---------+---------+---------+  Length =  2
 *  ||0 1|2 3 | 4 5 6 7 | 8 9 A B | C D E F |  Offset = _0_
 *  +|___|----+---------+---------+---------+            2 --> Read 1 2K block
 *      ___
 *  +--|---|--+---------+---------+---------+  Length =  2
 *  | 0|1 2|3 | 4 5 6 7 | 8 9 A B | C D E F |  Offset = _1_
 *  +--|___|--+---------+---------+---------+            3 --> Read 1 2K block
 *        ___
 *  +----|---|+---------+---------+---------+  Length =  2
 *  | 0 1|2 3|| 4 5 6 7 | 8 9 A B | C D E F |  Offset = _2_
 *  +----|___|+---------+---------+---------+            4 --> Read 1 2K block
 *          _____
 *  +------|--+--|------+---------+---------+  Length =  2
 *  | 0 1 2|3 | 4|5 6 7 | 8 9 A B | C D E F |  Offset = _3_
 *  +------|_____|------+---------+---------+            5 --> Read 2 2K blocks
 *
 *
 *  Length: 3
 *    _____
 *  +|-----|--+---------+---------+---------+  Length =  3
 *  ||0 1 2|3 | 4 5 6 7 | 8 9 A B | C D E F |  Offset = _0_
 *  +|_____|--+---------+---------+---------+            3 --> Read 1 2K block
 *      _____
 *  +--|-----|+---------+---------+---------+  Length =  3
 *  | 0|1 2 3|| 4 5 6 7 | 8 9 A B | C D E F |  Offset = _1_
 *  +--|_____|+---------+---------+---------+            4 --> Read 1 2K block
 *        _______
 *  +----|----+--|------+---------+---------+  Length =  3
 *  | 0 1|2 3 | 4|5 6 7 | 8 9 A B | C D E F |  Offset = _2_
 *  +----|_______|------+---------+---------+            5 --> Read 2 2K blocks
 *          _______
 *  +------|--+----|----+---------+---------+  Length =  3
 *  | 0 1 2|3 | 4 5|6 7 | 8 9 A B | C D E F |  Offset = _3_
 *  +------|_______|----+---------+---------+            6 --> Read 2 2K blocks
 *
 *
 *  Length: 4
 *    _______
 *  +|-------|+---------+---------+---------+  Length =  4
 *  ||0 1 2 3|| 4 5 6 7 | 8 9 A B | C D E F |  Offset = _0_
 *  +|_______|+---------+---------+---------+            4 --> Read 1 2K block
 *      _________
 *  +--|------+--|------+---------+---------+  Length =  4
 *  | 0|1 2 3 | 4|5 6 7 | 8 9 A B | C D E F |  Offset = _1_
 *  +--|_________|------+---------+---------+            5 --> Read 2 2K blocks
 *        _________
 *  +----|----+----|----+---------+---------+  Length =  4
 *  | 0 1|2 3 | 4 5|6 7 | 8 9 A B | C D E F |  Offset = _2_
 *  +----|_________|----+---------+---------+            6 --> Read 2 2K blocks
 *          _________
 *  +------|--+------|--+---------+---------+  Length =  4
 *  | 0 1 2|3 | 4 5 6|7 | 8 9 A B | C D E F |  Offset = _3_
 *  +------|_________|--+---------+---------+            7 --> Read 2 2K blocks
 *
 *
 *  Length: 5
 *    ___________
 *  +|--------+--|------+---------+---------+  Length =  5
 *  ||0 1 2 3 | 4|5 6 7 | 8 9 A B | C D E F |  Offset = _0_
 *  +|___________|------+---------+---------+            5 --> Read 2 2K blocks
 *      ___________
 *  +--|------+----|----+---------+---------+  Length =  5
 *  | 0|1 2 3 | 4 5|6 7 | 8 9 A B | C D E F |  Offset = _1_
 *  +--|___________|----+---------+---------+            6 --> Read 2 2K blocks
 *        ___________
 *  +----|----+------|--+---------+---------+  Length =  5
 *  | 0 1|2 3 | 4 5 6|7 | 8 9 A B | C D E F |  Offset = _2_
 *  +----|___________|--+---------+---------+            7 --> Read 2 2K blocks
 *          ___________
 *  +------|--+--------|+---------+---------+  Length =  5
 *  | 0 1 2|3 | 4 5 6 7|| 8 9 A B | C D E F |  Offset = _3_
 *  +------|___________|+---------+---------+            8 --> Read 2 2K blocks
 *
 *
 *  Length: 6
 *    _____________
 *  +|--------+----|----+---------+---------+  Length =  6
 *  ||0 1 2 3 | 4 5|6 7 | 8 9 A B | C D E F |  Offset = _0_
 *  +|_____________|----+---------+---------+            6 --> Read 2 2K blocks
 *      _____________
 *  +--|------+------|--+---------+---------+  Length =  6
 *  | 0|1 2 3 | 4 5 6|7 | 8 9 A B | C D E F |  Offset = _1_
 *  +--|_____________|--+---------+---------+            7 --> Read 2 2K blocks
 *        _____________
 *  +----|----+--------|+---------+---------+  Length =  6
 *  | 0 1|2 3 | 4 5 6 7|| 8 9 A B | C D E F |  Offset = _2_
 *  +----|_____________|+---------+---------+            8 --> Read 2 2K blocks
 *          _______________
 *  +------|--+---------+--|------+---------+  Length =  6
 *  | 0 1 2|3 | 4 5 6 7 | 8|9 A B | C D E F |  Offset = _3_
 *  +------|_______________|------+---------+            9 --> Read 3 2K blocks
 *
 *
 *  Length: 7
 *    _______________
 *  +|--------+------|--+---------+---------+  Length =  7
 *  ||0 1 2 3 | 4 5 6|7 | 8 9 A B | C D E F |  Offset = _0_
 *  +|_______________|--+---------+---------+            7 --> Read 2 2K blocks
 *      _______________
 *  +--|------+--------|+---------+---------+  Length =  7
 *  | 0|1 2 3 | 4 5 6 7|| 8 9 A B | C D E F |  Offset = _1_
 *  +--|_______________|+---------+---------+            8 --> Read 2 2K blocks
 *        _________________
 *  +----|----+---------+--|------+---------+  Length =  7
 *  | 0 1|2 3 | 4 5 6 7 | 8|9 A B | C D E F |  Offset = _2_
 *  +----|_________________|------+---------+            9 --> Read 3 2K blocks
 *          _________________
 *  +------|--+---------+----|----+---------+  Length =  7
 *  | 0 1 2|3 | 4 5 6 7 | 8 9|A B | C D E F |  Offset = _3_
 *  +------|_________________|----+---------+           10 --> Read 3 2K blocks
 *
 *
 *  Length: 8
 *    _________________
 *  +|--------+--------|+---------+---------+  Length =  8
 *  ||0 1 2 3 | 4 5 6 7|| 8 9 A B | C D E F |  Offset = _0_
 *  +|_________________|+---------+---------+            8 --> Read 2 2K blocks
 *      ___________________
 *  +--|------+---------+--|------+---------+  Length =  8
 *  | 0|1 2 3 | 4 5 6 7 | 8|9 A B | C D E F |  Offset = _1_
 *  +--|___________________|------+---------+            9 --> Read 3 2K blocks
 *        ___________________
 *  +----|----+---------+----|----+---------+  Length =  8
 *  | 0 1|2 3 | 4 5 6 7 | 8 9|A B | C D E F |  Offset = _2_
 *  +----|___________________|----+---------+           10 --> Read 3 2K blocks
 *          ___________________
 *  +------|--+---------+------|--+---------+  Length =  8
 *  | 0 1 2|3 | 4 5 6 7 | 8 9 A|B | C D E F |  Offset = _3_
 *  +------|___________________|--+---------+           11 --> Read 3 2K blocks
 *
 *
 */



/* BLOCKS and SECTORS
 *
 * Most ATA devices employ 512-byte sectors the same as any traditional
 * VMS or SCSI disk. Most magnetic read-write ATAPI devices (such as the
 * Iomega Zip drive) also employ 512-byte sectors.
 *
 * Devices based on CD technology, however, employ CD-sized sectors
 * (which contain at least 2352 raw bytes of data but, after subtracting
 * extra error-handling codes, result in a net data transfer of 2048 bytes).
 *
 * Generally speaking, within the driver, I've tried to refer to the
 * on-disk, variable-sized things as "sectors" and to only use the term
 * "blocks" to refer to the standard VMS-sized 512-byte data blocks.
 *
 */


                           
/* REQUEST SEGMENTATION
 *
 * A single large I/O request may be segmented (fragmented) into several
 * smaller I/O requests at least two different points in the life of the
 * I/O request. In particular:
 *
 *   - [SYS]SYSFASTIO  or [SYS]SYSACPFDT will segment I/Os into transfers
 *     of no more than UCB$L_MAXBCNT (or 127 blocks/65024 bytes if that
 *     field is zero). I'm not sure why this is done, but it's handy for us. ...
 *
 *   - Our READ/WRITE/DATACHECK routines will also segment request
 *     into transfers of varying sizes (depending on exactly which
 *     segment-handler will then process the I/O onwards to which
 *     target device). This is required because the ATA, ATAPI, and
 *     SFF-8038i (DMA) specs impose limits on the size of transfers
 *     or DMA buffers.
 *
 *     In particular:
 *
 *       o a single ATA PIO command can't transfer more than one
 *         sector (typically, 512 bytes) bytes per interrupt (although
 *         it can certainly transfer more in a single command that
 *         spans several interrupts).
 *
 *       o a single ATAPI PIO command can't transfer more than 65,535
 *         bytes per interrupt (although it can probably transfer
 *         more in a single command that spans several interrupts).
 *
 *       o a single ATA or ATAPI DMA command can't transfer more than
 *         65,536 bytes using a single DMA window (although the PRDT
 *         mapping table can specify more than one DMA window.)
 *
 *
 * Devices that employ 2KB sectors make this all a bit more interesting.
 * Although VMS's upper-level segmentation shields ATA and 512-byte ATAPI
 * devices from a lot of these concerns, 2KByte ATAPI devices could
 * have to transfer 33 sectors (33*2048=67584 bytes) to fulfill any
 * arbitrarily-aligned group of 127 512-byte blocks. VMS's upper-level
 * segmentation won't help here -- the driver must correctly handle
 * this case or set UCB$L_MAXBCNT to, say 124*512 so no transfer can
 * span more than 65536 bytes worth of on-disk data.
 *
 * In principal, we could probably set UCB$L_MAXBCNT and rely on VMS
 * to do all the segmentation, but:
 *
 *   - Belt-and-suspenders (VMS doing it *AND* us doing it) is probably
 *     safer, and
 *
 *   - Because our segmentation need pass through fewer levels, we can
 *     do it faster. We may well someday want to set a large UCB$L_MAXBCNT
 *     to disable some or all of VMS's segmentation.
 *
 */



/* IDE/ATAPI Bus Mastering DMA
 *
 * This is a term used in the PC industry to describe the concept
 * that you or I would simply think of as "DMA". This describes
 * the capability of a controller to actually become the bus master
 * (Be still my heart!) and thereby affect its own DMA transfers.
 * This is also described as "First Party DMA", as compared to
 * "Third Party DMA" where a dedicated DMA controller on the bus does
 * PIO to the device and then transfers the data into main memory.
 *
 * For ATA and ATAPI devices, this capability is defined not by
 * the ATAPI spec but rather by a working group of the Small Factor
 * Task force. The ruling spec is known as SFF-8038i.
 *                            
 * Essentially, the spec defines two sets of registers (one each
 * for the Primary and Secondary IDE controllers) that each point
 * to tables (Called the PRDT or Physical Region Descriptor Table)
 * that then allow scatter-gather DMA through the PCI bus memory
 * window(s). In each table, the PRDT entries are stored sequentially
 * in the table and the data transfer proceeds until all regions
 * described by the table have been transfered or the drive
 * terminates the transfer.
 *
 * Each PRDT entry has the following format:
 *
 *    +----------------------+--------------------+-+
 *    |          Base Address  [31:01]            |0|
 *    +---+------------------+--------------------+-+
 *    |EDT| ---------------- | Byte Count [15:01] |0|
 *    +---+------------------+--------------------+-+
 *
 *
 * NOTE WELL:
 *
 *   - Regions *CAN NOT* exceed 64K bytes. Transfers (or at least
 *     mapping windows) must be segmented to be smaller than that.
 *     A byte count of 0x0000 implies 64Kbytes.
 *
 *   - Regions *CAN NOT SPAN* a 64K boundary in the PCI address space!
 *
 *   - Regions *MUST BE* word-aligned! We need to manually shuffle
 *     bytes to the user buffer if they aren't.
 *
 *   - The Descriptor Table itself *CAN NOT SPAN* a 64K boundary in
 *     the PCI address space!
 *
 *   - The Descriptor Table itself *MUST BE* longword aligned!
 *
 *   - The EDT (End-Descriptor-Table) flag is set in the last PRDT entry.
 *
 *   - The table must describe at least enough space to contain
 *     the entire transfer conducted by the drive. If the table
 *     describes more bytes than will be transfered by the drive,
 *     the driver must clear the "ACTIVE" bit (in DMA_CMD) after
 *     the transfer is complete!
 *
 */



/* IDE/ATAPI Bus Mastering DMA (Continued)
 *
 * We open two mapping windows per unit: One for the PRDT (controller-
 * chip-based scatter/gather map table) and one for the actual data
 * window.
 *
 * Right now, our data window is pointed at our own buffer. In the
 * near future, it should be pointed to directly the user buffer
 * any time the DMA is "simple". Simple DMAs are those where:
 *
 *   o The transfer length is less than or equal to 64KB.
 *     This is a "gimme" given that VMS defaults to fragmenting
 *     our requests to 127 or fewer blocks (63.5K) and we force
 *     smaller fragmentation for 2K-byte sector devices.
 *
 *   o The transfer length is an integral number of blocks.
 *     For non-integral write transfers, we probably need to
 *     pad the last block. (Although the DVDRIVER has discovered
 *     that this is automatic for floppy controllers, I won't yet
 *     take that on faith for all possible IDE controllers.)
 *     For non-integral read transfers, we need to do further
 *     investigation at a future date.
 *
 *   o The transfer is word-aligned. This is required because
 *     there's no hardware to do the shuffle.
 *
 *
 * Notes:
 *
 *   Rather than open one window per unit, we could probably open
 *   only one window per controller, but I'm not going to get into
 *   that now.
 *
 *   For systems with less than 1GB of memory, we could use the
 *   direct-mapping window rather than the scatter/gather window.
 *
 */



/* More PRDT Fun
 *
 * Some day when we start doing direct I/O, the PRDT could, in
 * principle, be used to automate some of work that needs to be
 * done. For example:
 *
 *   o We currently face the problem that for CD/DVD-ROM read
 *     transfers, we must transfer 2Kbyte sectors and then (often)
 *     extract only a portion of our buffer to transfer onwards
 *     to the user. We could easily use the PRDT to cause the
 *     transfer of the leading/trailing unwanted VMS blocks
 *     into a "junk" buffer while the desired blocks are
 *     transfered into the actual user buffer.
 *
 *     Similarly, for writes to 2KByte-sector devices, we could
 *     easily use the PRDT to do the "merge" between the old
 *     data blocks (previously read into our buffer) and the
 *     new data blocks (transferred from the user's buffer).
 *     This would make the required RMW operation much simpler.
 *
 *
 * We could get even fancier still:
 *
 *   o Currently, we must do data extraction for all devices (not
 *     just 2Kbyte-sector devices) when the user asks for the
 *     reading of less than a full sector. We could use the
 *     PRDT to accomplish this for any transfer with an even
 *     byte-count, transferring the desired bytes to the user's
 *     buffer while transferring the undesired bytes to a junk
 *     buffer. (We're still stuck doing it by hand for at least
 *     the last sector/block of transfers with odd byte counts.)
 *
 *   o Currently, we must do buffer-filling (with zeroes)
 *     extraction for all devices (not just 2Kbyte-sector
 *     devices) when the user asks for the writing of less
 *     than a full sector. We could use the PRDT to accomplish
 *     this for any even byte-count, transferring the desired
 *     bytes from the user's buffer and transferring the rest
 *     of the bytes from a pre-zeroed buffer. (We're still
 *     stuck doing it by hand for the last sector/block of
 *     transfers with odd byte counts.)
 *
 *  o We could also use the PRDT to facilitate transfers that
 *    are less-than page-aligned (all the way down to transfers
 *    that are just word-aligned).  (We're still stuck doing
 *    it by entirely by hand for transfers to odd addresses,
 *    that is, transfers that are not even word-aligned.)
 *
 */



/* Define system data structure types and constants */

#include   "../../../freevms/lib/src/bufiodef.h"				/* BUFfered I/O header                      */
#include   "../../../freevms/lib/src/ccbdef.h"				/* Channel Control Block                    */
#include   "../../../freevms/lib/src/crbdef.h"				/* Controller Request Block                 */
#include   "../../../freevms/lib/src/crabdef.h"				/* Counted Resource Allocation Block        */
#include   "../../../freevms/lib/src/cramdef.h"				/* Controller Register Access Method        */
#include   "../../../freevms/lib/src/crctxdef.h"				/* Counted Resource Context Block           */
#include   "../../../freevms/starlet/src/dcdef.h"				/* Device Codes                             */
#include   "../../../freevms/lib/src/ddbdef.h"				/* Device Data Block                        */
#include   "../../../freevms/lib/src/ddtdef.h"				/* Driver dispatch table                    */
#include   "../../../freevms/starlet/src/devdef.h"				/* Device characteristics                   */
#include   "../../../freevms/lib/src/dptdef.h"				/* Driver Prologue Table                    */
#include   "../../../freevms/lib/src/dtndef.h"				/* Define the DTNs                          */
#include   "../../../freevms/lib/src/dyndef.h"				/* Dynamic type definitions                 */
#include   "../../../freevms/lib/src/embdvdef.h"				/* Error log entry for devices              */
#include   "../../../freevms/lib/src/fdtdef.h"				/* Function Decision Table                  */
#include   "../../../freevms/lib/src/fkbdef.h"				/* ForK Block                               */
#include   "../../../freevms/lib/src/idbdef.h"				/* Interrupt Data Block                     */
#include   "../../../freevms/lib/src/iocdef.h"				/* IOC constants                            */
#include   "../../../freevms/starlet/src/iodef.h"				/* I/O function codes                       */
#include   "../../../freevms/lib/src/irpdef.h"				/* I/O Request Packet                       */
#include   "../../../freevms/lib/src/orbdef.h"				/* Object Rights Block                      */
#include   "../../../freevms/starlet/src/pagedef.h"				/* Get page definitions and disk block size */
#include   "../../../freevms/lib/src/pcbdef.h"				/* Process Control Block                    */
#include   "../../../freevms/lib/src/pcidef.h"				/* PCIbus definitions                       */
#include   "../../../freevms/starlet/src/prvdef.h"				/* Privilege bit definitions                */
#include   "../../../freevms/lib/src/qssysdef.h"				/* QIOServer PACKACK UCB update buffer      */
#include   "../../../freevms/starlet/src/ssdef.h"				/* System service status codes              */
#include   <stddef.h>				/* Common definitions                       */
#include   "../../../freevms/starlet/src/stsdef.h"				/* STatuS value fields                      */
#include   "../../../freevms/lib/src/ucbdef.h"				/* Unit Control Block                       */
#include   "../../../freevms/starlet/src/vadef.h"		 		/* Virtual Address definitions              */

#ifndef IOC$K_BYTE_LANED			/* Defined in V7-1.2 onwards... */
#define IOC$K_BYTE_LANED 1			/* Define this for V71R as well */
#endif

/* Define function prototypes for system routines */

#include   "../../../freevms/lib/src/acp_routines.h"				/* ACP$ and ACP_STD$ routines */
#include   "../../../freevms/lib/src/erl_routines.h"				/* erl$ and erl_std$ routines */
#include   "../../../freevms/lib/src/exe_routines.h"				/* exe$ and exe_std$ routines */
#include   "../../../freevms/lib/src/ioc_routines.h"				/* ioc$ and ioc_std$ routines */
#include   "../../../freevms/lib/src/ldr_routines.h"				/* ldr$ and ldr_std$ routines */
#include   "../../../freevms/starlet/src/starlet.h"				/* Define SYS$SETPRV prototypes */ 

/* Define various device driver macros */

#include   "../../../freevms/lib/src/vms_drivers.h"				/* Device driver support macros */
#include   "../../../freevms/lib/src/vms_macros.h"				/* Define bug_check and such */

/* Define the DEC C functions used by this driver */

//#include   "../../../freevms/lib/src/builtins.h"				/* C builtin functions -- unused, but... */
//#include   "../../../freevms/lib/src/string.h"				/* String rtns from "kernel CRTL" */

/* Define some useful types */

typedef unsigned short int WORD;		/* Define a WORD (16 bits) */
typedef unsigned char      BYTE;		/* Define a BYTE (8 bits) */
typedef unsigned int       UINT;		/* Usigned int (32 bits) */



/* Define constants specific to this driver */

						/* Miscellaneous controller-related constants: */

#define NUMBER_OF_NON_DMA_CRAMS     10*2	/* Number of CRAMs needed to map the non-DMA CSRs*/
#define NUMBER_OF_DMA_CRAMS          9*2	/* Number of CRAMs needed to map the DMA CSRs */
#define NUMBER_OF_CRAMS      NUMBER_OF_NON_DMA_CRAMS+NUMBER_OF_DMA_CRAMS
						/* Total number of CRAMs needed */

#define MODEL_LENGTH        40			/* Model string length */
#define ERR_BYTES       (EMB$C_DV_LENGTH+12+5+8)/* Size of error log buffer (in bytes) */
#define RDSTATS_LEN     ((TIMEOUT_TIME + 9) * 4)/* Size of RDSTATS_LEN buffer */
#define DEVICE_IPL          21			/* IPL of device */


						/* Timeout times */
#define DRQ_TIME     (1000 * 1000)		/* DRQ wait time (i.e, 1 millisecond) */
#define RESET_TIME           4			/* Reset time (seconds) (Ensure two passes) */
#define READY_TIME    (100 * 1000)		/* Ready time (i.e., 100 microseconds) */

#define TIMEOUT_TIME        15			/* I/O Timeout time (seconds) */


						/* Geometry and transfer constants                                */
#define MAX_UCB_CYL       4095			/* UCB max values based on max ata lba of 0x0fffffff              */
#define MAX_UCB_TRK        255
#define MAX_UCB_SEC        255
#define MAX_RETRY            8			/* Maximum number of retries                                      */
#define MAX_SECTOR          63			/* Max sector allowed [1:n]                                       */
#define MAX_HEAD            15			/* Max head allowed [0:n]                                         */
#define MAX_CYLINDER     16383			/* Max cylinder allowed [0:n]                                     */
						/* (That could be as big as 65535                                 */
						/*   but real drives seem to top out at 14 bits = 0x3FFF)         */
#define MAX_BLOCKS_PER_CYLINDER 1008		/* 63 sectors per track * 16 heads                                */

#define MAX_ATA_XFER       127			/* Max ATA/IDE transfer size (blocks)                             */
						/*                                                                */
#define MAX_ATAPI_512_XFER 127			/* Max ATAPI transfer size (blocks) on a 512-byte-sector device   */
						/*                                                                */
						/*   Both of these are limited to 127 blocks (63.5K) just in      */
						/*   case there are drives out there that have trouble with       */
						/*   a 64K transfer. The Zip *IS* an example of such a drive --   */
						/*   It reports a SCSI parity Error is you command it to do       */
						/*   a read with the byte count register = 0x0000.                */
						/*                                                                */
						/*   127 is also a good value because it matches the size         */
						/*   to which VMS segments transfers by default if ucb$l_maxbcnt  */
						/*   is left set to 0.                                            */
						/*                                                                */
						/*                                                                */
#define MAX_ATAPI_2K_XFER  120			/* Max ATAPI transfer size (blocks) on a 2Kbyte-sector device     */
						/*                                                                */
						/*   In much the same fashion as we limit ATA and ATAPI_512 reads */
						/*   to 63.5K, we've also limited ATAPI_2K reads to 62K (31 2K    */
						/*   blocks) to avoid any problems with drives that might have    */
						/*   trouble with 64K transfers.                                  */
						/*                                                                */
						/*   We then further limit this to 60K so that we can fulfill,    */
						/*   from within this 62K buffer, any arbitrary set of 512-byte   */
						/*   blocks. And keeping this "by 4" will keep an aligned         */
						/*   transfer aligned through all segments.                       */


#define BLK_SIZE_CAPACITY    8			/* Size of returned capacity data block in bytes           */
#define BLK_SIZE_SENSE      18			/* Size of returned sense data block in bytes              */
#define BLK_SIZE_512       512			/* Size of a VMS (and typical IDE/ATA) disk block in bytes */
#define BLK_SIZE_2048     2048			/* Size of a typical CD-ROM data block in bytes            */
#define BLK_SIZE_2352     2352			/* Size of a maximum CD-ROM data block in bytes            */
#define BLK_SIZE_63_5K   (127*512)		/* Size of a 127 block buffer in bytes                     */
						/*   (maximum practical ATA/ATAPI transfer)                */
#define BLK_SIZE_64K     (128*512)		/* Size of a 128 block buffer in bytes                     */
						/*   (maximum ATA/ATAPI transfer)                          */

#define BLK_MASK     IOC$M_BLOCK_BYTEMASK	/* "Byte within block" mask */

#define BLK_SHIFT            9			/* Shift factor for blocks to bytes */

#define XFER_BUFFER_SIZE  BLK_SIZE_63_5K	/* Size of each transfer buffer                   */
						/*                                                */
						/* When the page size calculation is applied,     */
						/*   this will round up to a 64K buffer. But      */
						/*   we want the smaller size in the constant so  */
						/*   we don't take 0x00010000 (64K) and end up    */
						/*   passing 0x0000 to the ATA and ATAPI drives.  */
						/*   At least some of them (like the Zip) take    */
						/*   that as 0, not 64K.                          */


//#saythis "XFER_BUFFER_MAP_PAGES ought to be calculated, not forced!"
#define XFER_BUFFER_MAP_PAGES  8		/* Number of map pages to cover the xfer buffer   */

#define SENSE_BUFFER_SIZE ( (BLK_SIZE_CAPACITY>BLK_SIZE_SENSE) ? BLK_SIZE_CAPACITY : BLK_SIZE_SENSE )
						/* Size of each sense buffer (Max of those two)   */
						/*                                                */
						/* When the page size calculation is applied,     */
						/*   this will round up to at least an 8K buffer. */
						/*                                                */
						/* Later, rather than waste the space, maybe      */
						/*    re-use the end of this buffer for the       */
						/*    for the DMA PRDT table.                     */


#define PRDT_ENTRIES 8				/* Number of PRDT (Scatter/Gather Table) entries  */
#define PRDT_TABLE_SIZE (PRDT_ENTRIES*sizeof(PRDT))
						/* Total size of the PRDT table                   */
#define PRDT_ADX_MASK ~(PRDT_TABLE_SIZE-1)	/* Mask to force natural alignment of the PRDT    */



/* External references */

#include "mytypedefs.h"
extern unsigned long mmg$gl_bwp_mask;
static DDT   _ddt;			/* Prototype DDT */
static DPT   _dpt;			/* Prototype DPT */
static FDT   _fdt;			/* Prototype FDT */


extern int   MMG$GL_PAGE_SIZE;			/* Page size in bytes */
extern int   MMG$GL_VPN_TO_VA;			/* Page to byte shift count */

extern PTE   * const mmg$gl_sptbase;		/* Base of system page table */

extern DDT   driver$ddt;			/* Prototype DDT */
extern DPT   driver$dpt;			/* Prototype DPT */
extern FDT   driver$fdt;			/* Prototype FDT */
extern UCB   *sys$ar_bootucb;			/* Boot UCB */

/* Shortcuts for some of the external references */

//#define _ddt  driver$ddt			/* Abbreviation for DDT */
//#define _dpt  driver$dpt			/* Abbreviation for DPT */
//#define _fdt  driver$fdt			/* Abbreviation for FDT */



/* OWN values used for debugging only */

#ifdef TRACE_COMMON

int  trc_dummy;					/* An ASCII tag to help you find this               */
int *trc_buf;					/* Pointer to the base of the common tracing buffer */
int  trc_index;					/* Current index into the common tracing buffer     */
int  trc_buf_alloc=0;				/* Have we already allocated this?                  */

//#saythis "Remove these fixup counters after the root cause of the UCB corruption problem is found */
int  fixup_dummy;				/* An ASCII tag to help you find this           */
int  fixup_bcnt;				/* The number of times we've fixed UCB$L_BCNT   */
int  fixup_boff;				/* The number of times we've fixed UCB$L_BOFF   */
int  fixup_svapte;				/* The number of times we've fixed UCB$L_SVAPTE */

#endif



/* Define the IDE disk controller CSRs.
 *
 * Here are the customary values for PC AT compatible machines.
 * Note that the CSRs may be anywhere, and are defined here as
 * offsets from a base.  That base is the address of the DATA
 * register.
 *                              Primary         Secondary
 * Data/Control Ports           1F0-1F7h        170-177h
 * Control/Status Ports         3F7-3F6h        377-376h
 * DMA Registers               90A0-90A7h      90A8-90AFh
 *
 */


/* Offsets for control block registers */

					/*                            Actual Legacy Address  */
					/*                             Primary    Secondary  */
					/*                           ----------------------- */
#define REG_ALT_STS     0x206		/* READ: Alternate status       0x3F6       0x376    */
#define REG_DEV_CTL     0x206		/* WRITE:Device control         0x3F6       0x376    */
#define REG_DRV_ADDR    0x207		/* READ: FDC Drive address      0x3F7       0x377    */

/* Offsets for command block registers */

#define REG_DATA        0		/* R/W:  Data                   0x1F0       0x170    */
#define REG_ERROR       1		/* READ: Error                  0x1F1       0x171    */
#define REG_FEATURES    1		/* WRITE:Features               0x1F1       0x171    */
#define REG_SEC_CNT     2		/* R/W:  Sector count           0x1F2       0x172    */
#define REG_SECTOR      3		/* R/W:  Sector number          0x1F3       0x173    */
#define REG_CYL_LO      4		/* R/W:  Cylinder (low)         0x1F4       0x174    */
#define REG_CYL_HI      5		/* R/W:  Cylinder (high)        0x1F5       0x175    */
#define REG_DRV_HD      6		/* R/W:  Drive / Head           0x1F6       0x176    */
#define REG_STATUS      7		/* READ: Status                 0x1F7       0x177    */
#define REG_CMD         7		/* WRITE:Command                0x1F7       0x178    */

/* LBA fields (read and write) */

#define REG_LBA_0       3		/* LBA bits 0-7                 0x1F3       0x173    */
#define REG_LBA_8       4		/* LBA bits 8-15                0x1F4       0x174    */
#define REG_LBA_16      5		/* LBA bits 16-23               0x1F5       0x175    */
#define REG_LBA_24      6		/* LBA bits 24-27               0x1F6       0x176    */


/* Device Control Register */

#define CTL_M_nIEN     0x01		/* Interrupt enable bit for the host */
#define CTL_M_SRST     0x02		/* Host software reset bit */


/* Drive/Head Register */

#define DRVHD_M_BASE  0xA0		/* Bits 7 and 5 must be 1 */
#define DRVHD_M_LBA   0x40		/* LBA addressing bit */


/* ATAPI Features Register */

#define FEAT_M_DMA   0x01		/* Use DMA */
#define FEAT_M_OVL   0x02		/* Overlap operations */


/* Status (and Alternate Status) Register */

#define STS_M_ERR   0x01		/* Error */
#define STS_M_IDX   0x02		/* Index mark */
#define STS_M_CORR  0x04		/* Corrected data */
#define STS_M_DRQ   0x08		/* Data request */
#define STS_M_DSC   0x10		/* Drive seek complete (in old spec)*/
#define STS_M_SVC   0x10		/* Service request (in new spec)*/
#define STS_M_DWF   0x20		/* Drive write fault */
#define STS_M_DRDY  0x40		/* Drive ready */
#define STS_M_BSY   0x80		/* Busy */

#define STS_V_ERR   0			/* Error */
#define STS_V_IDX   1			/* Index mark */
#define STS_V_CORR  2			/* Corrected data */
#define STS_V_DRQ   3			/* Data request */
#define STS_V_DSC   4			/* Drive seek complete (in old spec)*/
#define STS_V_SVC   5			/* Service request (in new spec)*/
#define STS_V_DWF   6			/* Drive write fault */
#define STS_V_DRDY  7			/* Drive ready */
#define STS_V_BSY   8			/* Busy */



/* Offsets for DMA block (SFF-8038i) registers */

					/*                            Actual Legacy Address  */
					/*                             Primary    Secondary  */
					/*                           ----------------------- */
#define DMA_CMD        0		/* R/W: Command                0xnnnnn0    0xnnnnn8  */
#define DMA_DS1        1		/* R/W: Device-Specific 1      0xnnnnn1    0xnnnnn9  */
#define DMA_STS        2		/* R/W: Status                 0xnnnnn2    0xnnnnnA  */
#define DMA_DS2        3		/* R/W: Device-Specific 2      0xnnnnn3    0xnnnnnB  */
#define DMA_AD0        4		/* R/W: PRD Table Address LSB  0xnnnnn4    0xnnnnnC  */
#define DMA_AD1        5		/* R/W:  :                     0xnnnnn5    0xnnnnnD  */
#define DMA_AD2        6		/* R/W:  :                     0xnnnnn6    0xnnnnnE  */
#define DMA_AD3        7		/* R/W: PRD Table Address MSB  0xnnnnn7    0xnnnnnF  */


/* DMA (SFF-8038i) Command Register */

#define DMA_CMD_M_INBOUND  0x08		/* The DMA direction will be "INBOUND"    */
					/*   (That is, a disk READ/memory WRITE!) */
#define DMA_CMD_M_OUTBOUND 0x00		/* The DMA direction will be "OUTBOUND"   */
					/*   (That is, a disk WRITE/memory READ!) */
#define DMA_CMD_M_ACTIVE   0x01		/* Make the DMA controller active         */
#define DMA_CMD_M_INACTIVE 0x00		/* Make the DMA controller inactive       */


/* DMA (SFF-8038i) Status Register */

#define DMA_STS_M_SIMPLEX 0x80		/* The DMA controller can only operate    */
					/*   one disk at a time (Master or Slave) */
#define DMA_STS_M_DRV1    0x40		/* Drive 1 is DMA-capable                 */
#define DMA_STS_M_DRV0    0x20		/* Drive 0 is DMA-capable                 */
#define DMA_STS_M_RSV4    0x10		/*   <Reserved>                           */
#define DMA_STS_M_RSV3    0x08		/*   <Reserved>                           */
#define DMA_STS_M_INT     0x04		/* Interrupt                              */
#define DMA_STS_M_ERR     0x02		/* Error                                  */
#define DMA_STS_M_ACTIVE  0x01		/* Active                                 */


/* DMA (SFF-8038i) PRDT */

typedef struct
  {					/* The PRDT structure */
    UINT phys_adx;			/* Physical address */
    WORD count;				/* Byte count for this region */
    WORD flags;				/* Flags: Only high bit is meaningful as EDT (end) flag */
      } PRDT;

#define DMA_PRDT_M_EDT  0x8000		/* The EDT (End-Descriptor-Table) flag */



/* ATAPI magic "Signature" values */

#define ATAPI_SIG_STS    0x00		/* In the Status/Alternate Status Register */
#define ATAPI_SIG_STSE   0x01		/* In the Status/Alternate Status Register (Error bit set) */

#define ATAPI_SIG_CYL_HI 0xEB		/* In the Cylinder "Hi" Register */

#define ATAPI_SIG_CYL_LO 0x14		/* In the Cylinder "Lo" Register */



/* ATA Commands
 *
 * Notes:
 *
 *  o All of these commands are issued to the command register
 *    in the IDE/ATA "Task file" (the controller's register file).
 *
 *  o CMD_ATA_ATAPI_SOFT_RESET belongs here because that command
 *    is issued to the task file also, and not sent as a command
 *    within an ATAPI packet.
 *
 *  o This list is complete as of ATA-3.
 *
 */

#define CMD_ATA_NOP                      0x00	/* NOP                                                */
#define CMD_ATA_ATAPI_SOFT_RESET         0x08	/* Reset an ATAPI drive                               */

#define CMD_ATA_RECALIBRATE              0x10	/* Recalibrate                                        */

#define CMD_ATA_READ_SECS                0x20	/* Read Sector(s) w/ retries                          */
#define CMD_ATA_READ_SECS_WO_RET         0x21	/* Read Sector(s) w/o retries                         */
#define CMD_ATA_READ_LONG                0x22	/* Read Long (i.e., including ECC bytes) w/ retries   */
#define CMD_ATA_READ_LONG_WO_RET         0x23	/* Read Long (i.e., including ECC bytes) w/o retries  */

#define CMD_ATA_WRITE_SECS               0x30	/* Write Sector(s) w/ retries                         */
#define CMD_ATA_WRITE_SECS_WO_RET        0x31	/* Write Sector(s) w/o retries                        */
#define CMD_ATA_WRITE_LONG               0x32	/* Write Long (i.e., including ECC bytes) w/ retries  */
#define CMD_ATA_WRITE_LONG_WO_RET        0x33	/* Write Long (i.e., including ECC bytes) w/o retries */
#define CMD_ATA_WRITE_VFY                0x3C	/* Write Verify                                       */

#define CMD_ATA_READ_VFY_SECS            0x40	/* Read Verify Sector(s) w/ retries                   */
#define CMD_ATA_READ_VFY_SECS_WO_RET     0x41	/* Read Verify Sector(s) w/o retries                  */

#define CMD_ATA_FORMAT_TRACK             0x50	/* Format Track                                       */

#define CMD_ATA_SEEK                     0x70	/* Seek                                               */

#define CMD_ATA_80                       0x80	/* Unused group 0x8n                                  */

#define CMD_ATA_EXEC_DEV_DIAGS           0x90	/* Execute Device Diagnostic                          */
#define CMD_ATA_INIT_DEV_PARAMS          0x91	/* Initialize Device Parameters                       */
#define CMD_ATA_DOWNLOAD_UCODE           0x92	/* Download Microcode                                 */
#define CMD_ATA_STANDBY_IMMED_94         0x94	/* Standby Immediate 94                               */
#define CMD_ATA_IDLE_IMMED_95            0x95	/* Idle Immediate 95                                  */
#define CMD_ATA_STANDBY_96               0x96	/* Standby 96                                         */
#define CMD_ATA_IDLE_97                  0x97	/* Idle 97                                            */
#define CMD_ATA_CHK_PWR_MODE_98          0x98	/* Check Power Mode 98                                */
#define CMD_ATA_SLEEP_99                 0x99	/* Sleep 99                                           */

#define CMD_ATA_PACKET_CMD               0xA0	/* Send a command packet to ATAPI drive               */
#define CMD_ATA_PACKET_IDENTIFY          0xA1	/* Get an ATAPI drive to identify itself              */

#define CMD_ATA_SMART_DSBL_OPS           0xB0	/* SMART Disable Operations                           */
#define CMD_ATA_SMART_ENBLDSBL_ATTR_AUTO 0xB0	/* SMART Enable/Disable Attribute Autosave            */
#define CMD_ATA_SMART_ENBL_OPERATIONS    0xB0	/* SMART Enable Operations                            */
#define CMD_ATA_SMART_READ_ATTR_THRESH   0xB0	/* SMART Read Attribute Thresholds                    */
#define CMD_ATA_SMART_RETURN_STATUS      0xB0	/* SMART Return Status                                */
#define CMD_ATA_SECUR_SET_PSWD_OBS       0xBA	/* Security Set Password (Obsolete Version)           */
#define CMD_ATA_SECUR_UNLOCK_OBS         0xBB	/* Security Unlock (Obsolete Version)                 */
#define CMD_ATA_SECUR_ERASE_PREPARE_OBS  0xBC	/* Security Erase Prepare (Obsolete Version)          */
#define CMD_ATA_SECUR_ERASE_UNIT_OBS     0xBD	/* Security Erase Unit (Obsolete Version)             */
#define CMD_ATA_SECUR_FREEZE_LOCK_OBS    0xBE	/* Security Freeze Lock (Obsolete Version)            */
#define CMD_ATA_SECUR_DSBL_PSWD_OBS      0xBF	/* Security Disable Password (Obsolete Version)       */

#define CMD_ATA_READ_MULTIPLE            0xC4	/* Read Multiple                                      */
#define CMD_ATA_WRITE_MULTI              0xC5	/* Write Multiple                                     */
#define CMD_ATA_SET_MULTI_MODE           0xC6	/* Set Multiple Mode                                  */
#define CMD_ATA_READ_DMA                 0xC8	/* Read DMA w/ retries                                */
#define CMD_ATA_READ_DMA_WO_RET          0xC9	/* Read DMA w/o retries                               */
#define CMD_ATA_WRITE_DMA                0xCA	/* Write DMA w/ retries                               */
#define CMD_ATA_WRITE_DMA_WO_RET         0xCB	/* Write DMA w/o retries                              */

#define CMD_ATA_DOOR_LOCK                0xDE	/* Door Lock                                          */
#define CMD_ATA_DOOR_UNLOCK              0xDF	/* Door Unlock                                        */

#define CMD_ATA_STANDBY_IMMED_E0         0xE0	/* Standby Immediate E0                               */
#define CMD_ATA_IDLE_IMMED_E1            0xE1	/* Idle Immediate E1                                  */
#define CMD_ATA_STANDBY_E2               0xE2	/* Standby E2                                         */
#define CMD_ATA_IDLE_E3                  0xE3	/* Idle E3                                            */
#define CMD_ATA_READ_BUFFER              0xE4	/* Read Buffer                                        */
#define CMD_ATA_CHK_PWR_MODE_E5          0xE5	/* Check Power Mode E5                                */
#define CMD_ATA_SLEEP_E6                 0xE6	/* Sleep E6                                           */
#define CMD_ATA_WRITE_BUFFER             0xE8	/* Write Buffer                                       */
#define CMD_ATA_IDENTIFY_DEV             0xEC	/* Identify Device                                    */
#define CMD_ATA_MEDIA_EJECT              0xED	/* Media Eject                                        */
#define CMD_ATA_IDENTIFY_DEV_DMA         0xEE	/* Identify Device DMA                                */
#define CMD_ATA_SET_FEATURES             0xEF	/* Set Features                                       */

#define CMD_ATA_SECUR_SET_PSWD           0xF1	/* Security Set Password                              */
#define CMD_ATA_SECUR_UNLOCK             0xF2	/* Security Unlock                                    */
#define CMD_ATA_SECUR_ERASE_PREPARE      0xF3	/* Security Erase Prepare                             */
#define CMD_ATA_SECUR_ERASE_UNIT         0xF4	/* Security Erase Unit                                */
#define CMD_ATA_SECUR_FREEZE_LOCK        0xF5	/* Security Freeze Lock                               */
#define CMD_ATA_SECUR_DSBL_PSWD          0xF6	/* Security Disable Password                          */



/* ATAPI Commands
 *
 * Notes:
 *
 *  o These commands are issued in ATAPI packets.
 *
 *  o These commands are now defined by Annex B of the "SCSI-3
 *     Multi-Media Commands". Details of the commands can be
 *     found in several different SCSI specs:
 *
 *        - SCSI-3 Primary Commands    (SPC)
 *        - SCSI-3 Block Commands      (SBC)
 *        - SCSI-3 Multimedia Commands (MMC)
 *
 *
 *  o This list is complete as of MMC-3.
 *
 */

#define CMD_ATAPI_TEST_UNIT_READY        0x00	/* Test Unit Ready                              (SPC) */
#define CMD_ATAPI_REQUEST_SENSE          0x03	/* Request Sense                                (SPC) */
#define CMD_ATAPI_FORMAT_UNIT            0x04	/* Format Unit                                        */

#define CMD_ATAPI_INQUIRY                0x12	/* Inquiry                                      (SPC) */
#define CMD_ATAPI_START_STOP_UNIT        0x1B	/* Start/Stop Unit                              (SBC) */
#define CMD_ATAPI_PREVENT_ALLOW          0x1E	/* Prevent/Allow Medium Removal                 (SPC) */

#define CMD_ATAPI_READ_CAPACITY          0x25	/* Read Recorded Capacity                             */
#define CMD_ATAPI_READ_10                0x28	/* Read (10)                                    (SBC) */
#define CMD_ATAPI_WRITE_10               0x2A	/* Write (10)                                   (SBC) */
#define CMD_ATAPI_SEEK                   0x2B	/* Seek                                         (SBC) */

#define CMD_ATAPI_SYNCHRONIZE_CACHE      0x35	/* Synchronize Cache                                  */

#define CMD_ATAPI_READ_SUBCHANNEL        0x42	/* Read Sub-channel                                   */
#define CMD_ATAPI_READ_TOC_PMA_ATIP      0x43	/* Read TOC/PMA/ATIP                                  */
#define CMD_ATAPI_READ_HEADER            0x44	/* Read Header                                        */
#define CMD_ATAPI_PLAY_AUDIO_10          0x45	/* Play Audio (10)                                    */
#define CMD_ATAPI_PLAY_AUDIO_MSF         0x47	/* Play Audio MSF                                     */
#define CMD_ATAPI_PAUSE_RESUME           0x4B	/* Pause/Resume                                       */
#define CMD_ATAPI_STOP_PLAY_SCAN         0x4E	/* Stop Play/Scan                                     */

#define CMD_ATAPI_READ_DISK_INFORMATION  0x51	/* Read Disk Information                              */
#define CMD_ATAPI_READ_TRACK_INFORMATION 0x52	/* Read Track Information                             */
#define CMD_ATAPI_RESERVE_TRACK          0x53	/* Reserve Track                                      */
#define CMD_ATAPI_SEND_OPC_INFORMATION   0x54	/* Send OPC Information                               */
#define CMD_ATAPI_MODE_SELECT_10         0x55	/* Mode Select (10)                             (SPC) */
#define CMD_ATAPI_REPAIR_TRACK           0x58	/* Repair Track                                       */
#define CMD_ATAPI_READ_MASTER_CUE        0x59	/* Read Master Cue                                    */
#define CMD_ATAPI_MODE_SENSE_10          0x5A	/* Mode Sense (10)                              (SPC) */
#define CMD_ATAPI_CLOSE_TRACK_SESSION    0x5B	/* Close Track/Session                                */
#define CMD_ATAPI_READ_BUFFER_CAPACITY   0x5C	/* Read Buffer Capacity                               */
#define CMD_ATAPI_SEND_CUE_SHEET         0x5D	/* Send Cue Sheet                                     */

#define CMD_ATAPI_60                     0x60	/* Unused group 0x6n                                  */

#define CMD_ATAPI_70                     0x70	/* Unused group 0x7n                                  */

#define CMD_ATAPI_80                     0x80	/* Unused group 0x8n                                  */

#define CMD_ATAPI_90                     0x90	/* Unused group 0x9n                                  */

#define CMD_ATAPI_BLANK                  0xA1	/* Blank                                              */
#define CMD_ATAPI_PLAY_AUDIO_12          0xA5	/* Play Audio (12)                                    */
#define CMD_ATAPI_LOAD_UNLOAD_CD         0xA6	/* Load/Unload CD                                     */
#define CMD_ATAPI_READ_12                0xA8	/* Read (12)                                    (SBC) */
#define CMD_ATAPI_WRITE_12               0xAA	/* Write (12)                                   (SBC) */

#define CMD_ATAPI_READ_CD_MSF            0xB9	/* Read CD MSF                                        */
#define CMD_ATAPI_SCAN                   0xBA	/* Scan                                               */
#define CMD_ATAPI_SET_CD_SPEED           0xBB	/* Set CD Speed                                       */
#define CMD_ATAPI_PLAY_CD                0xBC	/* Play CD                                            */
#define CMD_ATAPI_MECHANISM_STATUS       0xBD	/* Mechanism Status                                   */
#define CMD_ATAPI_READ_CD                0xBE	/* Read CD                                            */

#define CMD_ATAPI_C0                     0xC0	/* Unused group 0xCn                                  */

#define CMD_ATAPI_D0                     0xD0	/* Unused group 0xDn                                  */

#define CMD_ATAPI_E0                     0xE0	/* Unused group 0xEn                                  */

#define CMD_ATAPI_F0                     0xF0	/* Unused group 0xFn                                  */



/* Set up the table for CRAM initialization.  This table contains the
 * CSR offset, the command used in this CRAM and the byte lane shift
 * value.  The byte lane shift value is computed at run time.
 *
 */

typedef struct
  {					/* The CRAM initialization structure */
    int cmd;				/* Command index */
    int offset;				/* Register offset */
    int shift;				/* Byte lane shift count */
      } cram_item;


/* Define the indices in this (and the UCB) table */

#define RD_ALT_STS      0
#define WT_DEV_CTL      1

#define RD_DRV_ADDR     2		/* Belongs to the FDC -- Don't read!  */
#define WT_DRV_ADDR     3		/* Read-only register -- Don't write! */

#define RD_DATA         4
#define WT_DATA         5

#define RD_ERROR        6
#define WT_FEATURES     7

#define RD_SEC_CNT      8
#define WT_SEC_CNT      9

#define RD_SECTOR      10
#define WT_SECTOR      11

#define RD_CYL_LO      12
#define WT_CYL_LO      13

#define RD_CYL_HI      14
#define WT_CYL_HI      15

#define RD_DRV_HD      16
#define WT_DRV_HD      17

#define RD_STS         18
#define WT_CMD         19


/* DMA (SFF-8038i) CRAMs */

#define RD_DMA_CMD     20
#define WT_DMA_CMD     21

#define RD_DMA_DS1     22
#define WT_DMA_DS1     23

#define RD_DMA_STS     24
#define WT_DMA_STS     25		/* Probably Read-ONLY! */

#define RD_DMA_DS2     26
#define WT_DMA_DS2     27

#define RD_DMA_AD0     28
#define WT_DMA_AD0     29

#define RDL_DMA_AD0    30
#define WTL_DMA_AD0    31

#define RD_DMA_AD1     32
#define WT_DMA_AD1     33

#define RD_DMA_AD2     34
#define WT_DMA_AD2     35

#define RD_DMA_AD3     36
#define WT_DMA_AD3     37


#define cram_def(cmd,csr) CRAMCMD$K_##cmd##32, csr, ((csr & 3) <<3)

cram_item cram_init[NUMBER_OF_CRAMS] =
  {

    cram_def(RDBYTE,REG_ALT_STS), 
    cram_def(WTBYTE,REG_DEV_CTL),

    cram_def(RDBYTE,REG_DRV_ADDR),		/* Belongs to FDC ------ Don't read!  */
    cram_def(WTBYTE,REG_DRV_ADDR),		/* Read-only register -- Don't write! */

    cram_def(RDWORD,REG_DATA),
    cram_def(WTWORD,REG_DATA),

    cram_def(RDBYTE,REG_ERROR),
    cram_def(WTBYTE,REG_FEATURES),

    cram_def(RDBYTE,REG_SEC_CNT),
    cram_def(WTBYTE,REG_SEC_CNT),

    cram_def(RDBYTE,REG_SECTOR),
    cram_def(WTBYTE,REG_SECTOR),

    cram_def(RDBYTE,REG_CYL_LO),
    cram_def(WTBYTE,REG_CYL_LO),

    cram_def(RDBYTE,REG_CYL_HI),
    cram_def(WTBYTE,REG_CYL_HI),

    cram_def(RDBYTE,REG_DRV_HD),
    cram_def(WTBYTE,REG_DRV_HD),

    cram_def(RDBYTE,REG_STATUS),
    cram_def(WTBYTE,REG_CMD),


/* DMA (SFF-8038i) CRAMs */

    cram_def(RDBYTE,DMA_CMD),
    cram_def(WTBYTE,DMA_CMD),

    cram_def(RDBYTE,DMA_DS1),
    cram_def(WTBYTE,DMA_DS1),

    cram_def(RDBYTE,DMA_STS),
    cram_def(WTBYTE,DMA_STS),		/* Probably read-only register --  Don't write! */

    cram_def(RDBYTE,DMA_DS2),
    cram_def(WTBYTE,DMA_DS2),

    cram_def(RDBYTE,DMA_AD0),
    cram_def(WTBYTE,DMA_AD0),

    cram_def(RDLONG,DMA_AD0),
    cram_def(WTLONG,DMA_AD0),

    cram_def(RDBYTE,DMA_AD1),
    cram_def(WTBYTE,DMA_AD1),

    cram_def(RDBYTE,DMA_AD2),
    cram_def(WTBYTE,DMA_AD2),

    cram_def(RDBYTE,DMA_AD3),
    cram_def(WTBYTE,DMA_AD3)

      };



/* Define Device-Dependent Unit Control Block with extensions for DQ device
 *
 */

#define MAX_DIAGNOSE_COMMAND_LENGTH 12
#define MAX_DIAGNOSE_DATA_SIZE BLK_SIZE_64K
#define MIN(x,y) (x<y?x:y)

typedef struct _diagnose_param {
    UINT opcode;
    UINT flags;
    unsigned char *command;
    UINT command_length;
    unsigned char *data;
    UINT data_length;
    UINT pad_length;
    UINT phase_timeout;
    UINT disconnect_timeout;
} DIAGNOSE_PARAM;
           

typedef struct
  {
    DT_UCB ucb$r_dtucb;			/* Generic UCB                                    */
    union				/* LBN as a longword (LBN) or CHS                 */
      {
        UINT lbn;			/* Block number                                   */
        struct
          {
            BYTE sec;			/* Sector number                                  */
            BYTE trk;			/* Track number                                   */
            WORD cyl;			/* Cylinder number                                */
              } pa;
          } ucb$l_media;
    int    ucb$l_bcr;			/* Byte count remaining                           */
    UINT   ucb$l_org_media;		/* Original LBN                                   */
    void   *ucb$l_org_svapte;		/* Original SVAPTE address                        */
    UINT   ucb$l_org_bcnt;		/* Original byte count                            */
    UINT   ucb$l_org_boff;		/* Original byte offset                           */
    UINT   ucb$l_drv_head;		/* Drive/head info                                */
    KPB    *ucb$ps_kpb;			/* KPB pointer                                    */
    CRAM   *ucb$ps_crams[NUMBER_OF_CRAMS];/* Table of CRAMs                               */
    UINT   *ucb$ps_xfer_buffer;		/* Transfer buffer pointer                        */
    UINT   *ucb$ps_sense_buffer;	/* Sense buffer pointer                           */
    int    ucb$l_dummy_flgs;		/* ASCII-tag the beginning of flag words          */
    UINT   ucb$l_drive_lba_capable;	/* 0=CSH, 1=Drive is capable of LBA addressing    */
    UINT   ucb$l_drive_dma_capable;	/* 0=PIO, 1=Drive is capable of DMA               */
    int    ucb$l_ctrl_id;		/* PCIbus ID of the controller                    */
    UINT   ucb$l_ctrl_dma_capable;	/* 0=PIO, 1=Controller is capable of DMA          */
    UINT   ucb$l_atapi_flag;		/* 0=ATA, 1=ATAPI                                 */
    UINT   ucb$l_2K_flag;		/* 0=512 byte blocks, 1=CD-ROM-style 2KB blocks   */
    int    ucb$l_dummy_sens;		/* ASCII-tag the beginning of saved sense keys    */
    UINT   ucb$l_sense_key;		/* Latest sense key                               */
    UINT   ucb$l_asc;			/* Latest additional sense code                   */
    UINT   ucb$l_ascq;			/* Latest additional sense code qualifier         */
    PTE    *ucb$ps_s0_svapte;		/* Pointer to base SPTE                           */
    BYTE   *ucb$ps_s0_va;		/* Pointer to user buffer                         */
    uint64 ucb$q_iohandle_1;		/* Handle for main I/O regs -- kept for debugging */
    uint64 ucb$q_iohandle_2;		/* Handle for DMA I/O regs --- kept for debugging */
    int    ucb$l_unsolicited_int;	/* An unsolicited interrupt is pending            */
    int    ucb$l_dummy_pakt;		/* ASCII-tag the beginning of saved packet        */
    BYTE   ucb$b_packet[12];		/* The most-recent ATAPI packet                   */
    int    ucb$l_dummy_ints;		/* ASCII-tag the total interrupts                 */
    int    ucb$l_total_ints;		/* Total interrupts count                         */
    int    ucb$l_dummy_unso;		/* ASCII-tag the unsolicited interrupts           */
    int    ucb$l_unsol_ints;		/* Count of unsolicited interrupts                */
    int    ucb$l_dummy_hist;		/* ASCII-tag the beginning of interrupt histogram */
    int    ucb$l_int_hist[TIMEOUT_TIME+1];/* Timeout histogram vector, allowing an entry for [0] as well */
    int    ucb$l_dummy_tmo;		/* ASCII-tag the count of interrupt timeouts      */
    int    ucb$l_int_tmo;		/* Count interrupt timeouts                       */
    int    ucb$l_dummy_resets;		/* ASCII-tag the count of drive resets            */
    int    ucb$l_resets;		/* Count of drive resets issued by us             */
    int    ucb$l_dummy_trace;		/* ASCII-tag the trace-buffer pointer and index   */
    int    *ucb$l_trc_buf;		/* Pointer to the tracing buffer                  */
    int    ucb$l_trc_index;		/* Current index within the tracing buffer        */
    int    ucb$l_trc_unit;		/* DQA0:=1, DQA1:=2, DQB0:=3, DQB1:=4             */
    int    *ucb$l_prdt;			/* Pointer to the PRDT                            */
    CRCTX  *ucb$ps_prdt_crctx;		/* Pointer to the PRDT map-register CRCTX         */
    void   *ucb$l_prdt_phy;		/* Pointer to the PRDT PCIbus (DMA) address       */
    CRCTX  *ucb$ps_xfer_crctx;		/* Pointer to the xfer buffer map-register CRCTX  */
    void   *ucb$l_xfer_phy;		/* Pointer to the xfer buffer PCIbus (DMA) adx    */

					/* Diagnose command information */
    UINT diagnose_opcode; 
    UINT diagnose_flags;
    unsigned char diagnose_command[MAX_DIAGNOSE_COMMAND_LENGTH];
    UINT diagnose_command_length;
    UINT diagnose_data_length;
    UINT diagnose_pad_length;
    UINT diagnose_phase_timeout;
    UINT diagnose_disconnect_timeout;

    int    ucb$l_dummy_end;		/* ASCII-tag the end of the UCB                   */
      } DQ_UCB;
                                           

#define ucb$r_dq_ucb ucb$r_dtucb.ucb$r_dpucb.ucb$r_erlucb.ucb$r_ucb
#define ucb$r_dq_erl ucb$r_dtucb.ucb$r_dpucb.ucb$r_erlucb
#define ucb$r_dq_dp  ucb$r_dtucb.ucb$r_dpucb
#define ucb$r_dq_dt  ucb$r_dtucb

#define baseucb ucb->ucb$r_dq_ucb
                                       


/* Define the Identify Drive information buffer
 *   Use the nomember_alignment to make sure that this structure
 *   matches what the drive uses
 *
 */

#pragma member_alignment save
#pragma nomember_alignment                 

typedef struct
  {					/* Word(s):  ATA-5 description                            */
					/*--------------------------------------------------------*/
    WORD  config;			/*  0:       Configuration information                    */
    WORD  cyls;				/*  1:       Number of cylinders                          */
    WORD  rsvd2;			/*  2:       Reserved word                                */
    WORD  heads;			/*  3:       Number of heads                              */
    WORD  ubytes_track;			/*  4:       Unformatted bytes/track           (retired)  */
    WORD  ubytes_sector;		/*  5:       Unformatted bytes/sector          (retired)  */
    WORD  sectors;			/*  6:       Number of sectors                            */
    WORD  unique7[3];			/*  7-9:     Vendor unique                     (retired)  */
    char  serial_number[20];		/*  10-19:   ASCII serial number                          */
    WORD  buffer_type;			/*  20:      Buffer type (retired)                        */
    WORD  buffer_size_blocks;		/*  21:      Buffer size (in blocks)           (retired)  */
    WORD  ecc_bytes;			/*  22:      Number of ECC bytes/sector        (obsolete) */
    char  firmware_revision[8];		/*  23-26:   ASCII firmware revision                      */
    char  model_number[MODEL_LENGTH];	/*  27-46:   ASCII drive model                            */
    BYTE  rw_multiple;			/*  47:      Max number of sectors/interrupt              */
    BYTE  unique47;			/*  47.5:    0x80                                         */
    WORD  rsvd48;			/*  48:      Reserved                                     */
    WORD  capabilities_49;		/*  49:      Capabilities                                 */
    WORD  capabilities_50;		/*  50:      More Capabilities                            */
    WORD  pio_cycle;			/*  51:      PIO data transfer mode                       */
    WORD  dma_cycle;			/*  52:      DMA I/O cycle times               (retired)  */
    WORD  valid_bits;			/*  53:      Valid bits for several fields                */
    WORD  curr_cyls;			/*  54:      Current logical cylinder count               */
    WORD  curr_heads;			/*  55:      Current logical head count                   */
    WORD  curr_sectors;			/*  56:      Current logical sector count                 */
    int   curr_capacity;		/*  57-58:   Current capacity in sectors                  */
    WORD  multiple_sectors;		/*  59:      Current sectors/interrupt setting            */
    UINT  lba_total_blocks;		/*  60-61:   Total number of user-adx'ible sectors        */
    WORD  single_word_dma;		/*  62:      Single word DMA info              (retired)  */
    WORD  multi_word_dma;		/*  63:      Multi word DMA info                          */
    WORD  pio_modes_supported;		/*  64:      Advanced PIO modes supported                 */
    WORD  min_dma_cycle_time;		/*  65:      Min multiword DMA transfer cycle time        */
    WORD  rec_dma_cycle_time;		/*  66:      Rec multiword DMA transfer cycle time        */
    WORD  min_pio_cycle_time;		/*  67:      Min non-IORDY PIO transfer cycle time        */
    WORD  min_iordy_pio_cycle_time;	/*  68:      Min IORDY PIO transfer cycle time            */
    WORD  rsvd69;			/*  69:      Reserved (for command queuing)               */
    WORD  rsvd70;			/*  70:      Reserved (for command queuing)               */
    WORD  atapi_pkt_time;		/*  71:      ns from PACKET cmd to bus release (ATAPI)    */
    WORD  atapi_svc_time;		/*  72:      ns from SERVICE to clearing BSY   (ATAPI)    */
    WORD  rsvd73;			/*  73:      Reserved                          (ATAPI)    */
    WORD  rsvd74;			/*  74:      Reserved                          (ATAPI)    */
    WORD  queue_depth;			/*  75:      Maximum queue depth                          */
    WORD  rsvd76;			/*  76:      Reserved                                     */
    WORD  rsvd77;			/*  77:      Reserved                                     */
    WORD  rsvd78;			/*  78:      Reserved                                     */
    WORD  rsvd79;			/*  79:      Reserved                                     */
    WORD  major_version_number;		/*  80:      Major version number (e.g, 4=ATA/ATAPI-4)    */
    WORD  minor_version_number;		/*  81:      Minor version number                         */
    WORD  cmd_set_supported_1;		/*  82:      Command set supported, word 1                */
    WORD  cmd_set_supported_2;		/*  83:      Command set supported, word 2                */
    WORD  cmd_set_supported_x;		/*  84:      Command set supported, extension             */
    WORD  cmd_set_enabled_1;		/*  85:      Command set enabled, word 1                  */
    WORD  cmd_set_enabled_2;		/*  86:      Command set enabled, word 2                  */
    WORD  cmd_set_default;		/*  87:      Command set default                          */
    WORD  ultra_dma;			/*  88:      Ultra DMA control                            */
    WORD  dse_time;			/*  89:      Data Security Erase time (~secs/2)           */
    WORD  enhanced_dse_time;		/*  90:      Enhanced Data Security Erase time (~secs/2)  */
    WORD  cur_apm_value;		/*  91:      Current Advanced Power Management value      */
    WORD  master_passwd_rev;		/*  92:      Master Password Revision Code                */
    WORD  rsvd94[33];			/*  94-126:  Reserved                                     */
    WORD  media_status_notification;	/*  127:     Removable Media Status Notification          */
    WORD  security_status;		/*  128:     Security Status                              */
    WORD  vendor_specific[31];		/*  129-159: Vendor specific                              */
    WORD  rsvd160[96];			/*  160-255: Reserved                                     */
      } ID_PAGE;

#pragma member_alignment restore

/* Capabilities bits */
#define  CAP_M_LBA  0x200			/* Handles LBA mode */
#define  CAP_M_DMA  0x100			/* Handles DMA */



#define IS_SET(   reg, bits ) ( (reg & bits) == bits )
#define IS_CLEAR( reg, bits ) ( (reg & bits) == 0    )

#define $SUCCESS( code )  ( (code & STS$M_SUCCESS) == 1)
#define $FAIL( code )     ( (code & STS$M_SUCCESS) == 0)

#define TRUE    1
#define FALSE   0



/* Prototypes for driver routines defined in this module */
        
int     atapi_packet_command(     DQ_UCB *ucb, BYTE *buffer, int xfer_req, int *xfer_cnt, int dma_flag );
								/* xfer_req is implicit in the command packet */
int     atapi_process_size(       DQ_UCB *ucb );
int     atapi_read_capacity(      DQ_UCB *ucb, BYTE *buffer );
int     atapi_request_sense(      DQ_UCB *ucb, BYTE *buffer );
int     atapi_xlate_error_to_vms( DQ_UCB *ucb );
#ifdef BREAKPOINTS
void    call_ini$brk( int code, int p1, int p2, int p3 );
#endif
void    compute_address(          DQ_UCB *ucb, int *sec, int *head, int *cyl );
int     ctrl_init(       IDB *idb, DDB *ddb, CRB *crb );
int     datacheck(                DQ_UCB *ucb );
int     diagnose(                 DQ_UCB *ucb);
int     diagnose_fdt(             IRP *irp, PCB *pcb, DQ_UCB *ucb, CCB *ccb );
int     dq_wfikpch(      KPB *kpb, int orig_ipl, int erl_param );
int     dq_qsrv_helper(  int action, IRP *irp, uint32 *dqqh_iost1, uint32 *dqqh_iost2 );
void    dq_qsrv_unit_init_comp(   IRP *irp );
void    dq_qsrv_unit_init_strt(   UCB *ucb );
int     driver$init_tables();
int     drvclr(                   DQ_UCB *ucb );
int     fetch_drive_info(         DQ_UCB *ucb, int atapi_flag, int init_time_flag );
int     fill_packet_w_adx(        DQ_UCB *ucb );
BYTE    inp(                      DQ_UCB *ucb, int reg );
WORD    inpw(                     DQ_UCB *ucb, int reg );
void    isr(             IDB *idb);
void    load_prdt(                DQ_UCB *ucb );
int     locate_dma_regs(          DQ_UCB *ucb, int *csr_base );
BYTE   *map_user_buffer(          DQ_UCB *ucb, int offset, int length );
void    move_sec_from_drive(      DQ_UCB *ucb, BYTE *buffer, int bytecount );
void    move_sec_to_drive(        DQ_UCB *ucb, BYTE *buffer, int bytecount );
void    out(                      DQ_UCB *ucb, int reg, BYTE data );
void    outw(                     DQ_UCB *ucb, int reg, WORD data );
void    outw_t(                   DQ_UCB *ucb, int reg, WORD data );
void	outl(			  DQ_UCB *ucb, int reg, int data );
int     packack(                  DQ_UCB *ucb, int init_time_flag );
int     process_drive_info(       DQ_UCB *ucb );
int     rct_fdt(         IRP *irp, PCB *pcb, DQ_UCB *ucb, CCB *ccb );
int     rdstats_fdt(     IRP *irp, PCB *pcb, DQ_UCB *ucb, CCB *ccb );
int     read(                     DQ_UCB *ucb );
int     read_ata_seg_pio(         DQ_UCB *ucb, int xfer_req, int *xfer_cnt );	/* Buffer adx comes from UCB */
int     read_ata_seg_dma(         DQ_UCB *ucb, int xfer_req, int *xfer_cnt );	/* Buffer adx comes from UCB */
int     read_atapi_512_seg(       DQ_UCB *ucb, int xfer_req, int *xfer_cnt, int dma_flag );
										/* Buffer adx comes from UCB */
int     read_atapi_2K_seg     (   DQ_UCB *ucb, int xfer_req, int *xfer_cnt, int dma_flag );
										/* Buffer adx comes from UCB */
int     read_dispatcher(          DQ_UCB *ucb, int xfer_req, int *xfer_cnt );
int     readrct(                  DQ_UCB *ucb );
void    regdump(         BYTE *buffer, int arg_2, DQ_UCB *ucb );
int     reset_ctrl(               DQ_UCB *ucb );
int     seek(                     DQ_UCB *ucb );
int     set_features(             DQ_UCB *ucb, int feature, int value );
int     set_geom(                 DQ_UCB *ucb );
int     sleep(                    DQ_UCB *ucb, int seconds );
void    startio(         KPB *kpb );
void    struc_init(      CRB *crb, DDB *ddb, IDB *idb, ORB *orb, DQ_UCB *ucb );
void    struc_reinit(    CRB *crb, DDB *ddb, IDB *idb, ORB *orb, DQ_UCB *ucb );
#ifdef TRACING
void    trace(                    DQ_UCB *ucb, int code, int bpt );
#endif
int     unit_init(       IDB *idb, DQ_UCB *ucb );
void    unit_init_fork(  void *fr3, IDB *idb, DQ_UCB *ucb );
int     unload(                   DQ_UCB *ucb );
int     wait_busy(                DQ_UCB *ucb );
int     wait_drq(                 DQ_UCB *ucb );
int     wait_ready(               DQ_UCB *ucb );
int     write(                    DQ_UCB *ucb );
int     write_ata_seg_pio(        DQ_UCB *ucb, int xfer_req, int *xfer_cnt );	/* Buffer adx comes from UCB */
int     write_ata_seg_dma(        DQ_UCB *ucb, int xfer_req, int *xfer_cnt );	/* Buffer adx comes from UCB */
int     write_atapi_512_seg(      DQ_UCB *ucb, int xfer_req, int *xfer_cnt, int dma_flag );
										/* Buffer adx comes from UCB */
int     write_atapi_2K_seg(       DQ_UCB *ucb, int xfer_req, int *xfer_cnt, int dma_flag );
										/* Buffer adx comes from UCB */
int     write_dispatcher(         DQ_UCB *ucb, int xfer_req, int *xfer_cnt );

/* following is a modified prototype for exe_std$alononpaged in [lib_h]exe_routines.h   */
int   exe$alononpaged_aln (int reqsize, int align, void **pool_p, int32 *alosize_p);



#ifdef TRACE_PER_DRIVE

/* TRACE - This routine is used to write a debugging entry in our tracing tumble-table
 *
 * Input:
 *      ucb     pointer to the UCB
 *      data    A longword to be written to the table
 *      bpt     A flag as to whether or not to do a breakpoint trap after tracing
 *
 * Output:
 *      none
 *
 * Side effect(s):
 *   The tracing buffer and its index value are updated.
 *   ini$brk may be invoked.
 *
 */

void trace( DQ_UCB *ucb, int code, int bpt )

  {

    ADP *adp;					/* Address of ADP */
    int *ptr;					/* Bind onto the ucb's pointer value */
    int save_ipl;				/* Place to save the old IPL */

    adp = baseucb.ucb$ps_adp;			/* Get ADP address */
    device_lock( adp->adp$ps_spinlock, RAISE_IPL, &save_ipl );
						/* Ensure exclusive access at IPL 31 */

    ptr = &ucb->ucb$l_trc_index;		/* Fill our local pointer */
    code = code | (ucb->ucb$l_trc_unit<<28);	/* Shift the unit into the high nibble, .OR. into the code */
    ucb->ucb$l_trc_buf[*ptr] = code;		/* Save the new trace code */
    (*ptr)++;					/* Bump the pointer */
    if (*ptr >= TRACING)			/* Beyond the end? */
      *ptr = 0;					/* If so, back to the beginning */
    ucb->ucb$l_trc_buf[*ptr] = 0x0FEEEEEE;	/* Mark the current end point */

    device_unlock( adp->adp$ps_spinlock, save_ipl, SMP_RESTORE );

#ifdef BREAKPOINTS
						/* Release exclusive access, back to old IPL */
    if (bpt)					/* Does this caller want a breakpoint? */
        call_ini$brk( code, (int) ucb, (int) ucb->ucb$l_trc_buf, ucb->ucb$l_trc_index );
						/* If so, make it so */
#endif

      }


#define TRACE( data )    trace( ucb, data, FALSE )
#define BPTRACE( data )  trace( ucb, data, TRUE  )

#endif



#ifdef TRACE_COMMON

/* TRACE - This routine is used to write a debugging entry in our tracing tumble-table
 *
 * Input:
 *      ucb     pointer to the UCB
 *      data    A longword to be written to the table
 *      bpt     A flag as to whether or not to do a breakpoint trap after tracing
 *
 * Output:
 *      none
 *
 * Side effect(s):
 *   The tracing buffer and its index value are updated.
 *   ini$brk may be invoked.
 *
 */

void trace( DQ_UCB *ucb, int code, int bpt )

  {

    ADP *adp;					/* Address of ADP */
    int save_ipl;				/* Place to save the old IPL */

    adp = baseucb.ucb$ps_adp;			/* Get ADP address */
    device_lock( adp->adp$ps_spinlock, RAISE_IPL, &save_ipl );
						/* Ensure exclusive access at IPL 31 */

    code = code | (ucb->ucb$l_trc_unit<<28);	/* Shift the unit into the high nibble, .OR. into the code */
    trc_buf[trc_index] = code;			/* Save the new trace code */
    trc_index++;				/* Bump the pointer */
    if (trc_index >= TRACING)			/* Beyond the end? */
      trc_index = 0;				/* If so, back to the beginning */
    trc_buf[trc_index] = 0x0FEEEEEE;		/* Mark the current end point */

    device_unlock( adp->adp$ps_spinlock, save_ipl, SMP_RESTORE );

#ifdef BREAKPOINTS
						/* Release exclusive access, back to old IPL */
    if (bpt)					/* Does this caller want a breakpoint? */
        call_ini$brk( code, (int) ucb, (int) trc_buf, trc_index );
						/* If so, make it so */
#endif

      }


#define TRACE( data )    trace( ucb, data, FALSE )
#define BPTRACE( data )  trace( ucb, data, TRUE  )

#endif


/* One way or another, make sure we have TRACE and BPTRACE macros defined
 *
 * If neither real tracing routine defined them,
 * then define them as nothing.
 *
 */

#ifndef TRACE
#define TRACE( data )
#define BPTRACE( data )
#endif



/* Define or null-out the debugging breakpoints */

#ifdef BREAKPOINTS

/* CALL_INI$BRK - This routine is used to help debug the driver
 *
 * Input:
 *      code    A code to clue me in as to who called us
 *      ucb     The affected unit's ucb, also as a clue
 *
 * Output:
 *      The side-effect of a breakpoint trap if XDELTA is installed.
 *      Look in R16 to see the code.
 *      Typically, look in R17 to see the ucb address.
 *      Typically, look in R18 to see the trace buffer base.
 *      Typically, look in R19 to see the trace buffer index.
 *
 */

void call_ini$brk( int code, int p1, int p2, int p3 )
  {
    ini$brk( );					/* And then break */
      }


#define BREAK( code, p1, p2, p3 )  call_ini$brk( code, p1, p2, p3 )

#else

#define BREAK( code, p1, p2, p3 )

#endif

//#define insque(x,y) (__PAL_INSQUEL_D((void **)(x),(void *)(y)))
//#define remque(x,y) (__PAL_REMQUEL_D((void **)(x),(void **)(y))>=0)


/* DRIVER$INIT_TABLES - Initialize Driver Tables
 *
 * This routine is used to initialize the driver tables.  The DPT, DDT
 * and FDT structures are set up.
 *
 * Usage:
 *      status = driver$init_tables();
 *
 * Input:
 *      none
 *
 * Output:
 *      none
 *
 * Return value:
 *      SS$_NORMAL  -- tables successfully set up
 *
 */

int driver$init_tables( void )

  {

/*  BREAK( 0x00010000, 0, 0, 0 );				/@ BREAK: driver$init_tables called -- Can't TRACE yet */

/* Finish initialization of the Driver Prologue Table (DPT) */

    ini_dpt_name(         &_dpt, "DQDRIVER" );		/* Driver name */
    ini_dpt_adapt(        &_dpt, AT$_ISA );		/* ISA bus device */
    ini_dpt_flags(        &_dpt, DPT$M_SMPMOD|DPT$M_SVP|DPT$M_QSVR);
							/* Set flags */
    ini_dpt_maxunits(     &_dpt, 4 );			/* 4 units max */
    ini_dpt_ucbsize(      &_dpt, sizeof(DQ_UCB) );	/* UCB size */
    ini_dpt_struc_init(   &_dpt, struc_init );		/* Structure init rtn */
    ini_dpt_struc_reinit( &_dpt, struc_reinit );	/* Structure reinit rtn */
    ini_dpt_ucb_crams(    &_dpt, NUMBER_OF_CRAMS );	/* Allocate some CRAMs */
    ini_dpt_end(          &_dpt );      

/* Finish initialization of the Driver Dispatch Table (DDT) */

    ini_ddt_ctrlinit(      &_ddt, ctrl_init );		/* Controller init rtn */
    ini_ddt_unitinit(      &_ddt, unit_init );		/* Unit init rtn */
    ini_ddt_start(         &_ddt, exe_std$kp_startio );	/* Exec's Start I/O rtn */
    ini_ddt_kp_startio(    &_ddt, startio );		/* KP's Start I/O rtn */
    ini_ddt_kp_stack_size( &_ddt, KPB$K_MIN_IO_STACK );	/* KP stack size */
    ini_ddt_kp_reg_mask(   &_ddt, KPREG$K_HLL_REG_MASK );/* KP register mask */
    ini_ddt_cancel(        &_ddt, ioc_std$cancelio );	/* Cancel rtn */
    ini_ddt_regdmp(        &_ddt, regdump );		/* Register dump routine */
    ini_ddt_erlgbf(        &_ddt, ERR_BYTES );		/* Set error log size */
    ini_ddt_qsrv_helper(   &_ddt, dq_qsrv_helper );	/* Specify QIOServer helper routine */
    ini_ddt_end(           &_ddt);

/* Finish initialization of the Function Decision Table (FDT) */

    ini_fdt_act( &_fdt, IO$_READLBLK,   acp_std$readblk,     DIRECT_64   );
    ini_fdt_act( &_fdt, IO$_READPBLK,   acp_std$readblk,     DIRECT_64   );
    ini_fdt_act( &_fdt, IO$_READVBLK,   acp_std$readblk,     DIRECT_64   );
    ini_fdt_act( &_fdt, IO$_WRITECHECK, acp_std$readblk,     DIRECT_64   );

    ini_fdt_act( &_fdt, IO$_WRITELBLK,  acp_std$writeblk,    DIRECT_64   );
    ini_fdt_act( &_fdt, IO$_WRITEPBLK,  acp_std$writeblk,    DIRECT_64   );
    ini_fdt_act( &_fdt, IO$_WRITEVBLK,  acp_std$writeblk,    DIRECT_64   );

    ini_fdt_act( &_fdt, IO$_ACCESS,     acp_std$access,      BUFFERED    );
    ini_fdt_act( &_fdt, IO$_CREATE,     acp_std$access,      BUFFERED    );

    ini_fdt_act( &_fdt, IO$_DEACCESS,   acp_std$deaccess,    BUFFERED    );

    ini_fdt_act( &_fdt, IO$_ACPCONTROL, acp_std$modify,      BUFFERED    );
    ini_fdt_act( &_fdt, IO$_DELETE,     acp_std$modify,      BUFFERED    );
    ini_fdt_act( &_fdt, IO$_MODIFY,     acp_std$modify,      BUFFERED    );
                                                      
    ini_fdt_act( &_fdt, IO$_MOUNT,      acp_std$mount,       BUFFERED    );

    ini_fdt_act( &_fdt, IO$_READRCT,    rct_fdt,             DIRECT      );
    ini_fdt_act( &_fdt, IO$_RDSTATS,    rdstats_fdt,         DIRECT      );

    ini_fdt_act( &_fdt, IO$_UNLOAD,     exe_std$lcldskvalid, BUFFERED_64 );
    ini_fdt_act( &_fdt, IO$_AVAILABLE,  exe_std$lcldskvalid, BUFFERED_64 );
    ini_fdt_act( &_fdt, IO$_PACKACK,    exe_std$lcldskvalid, BUFFERED_64 );

    ini_fdt_act( &_fdt, IO$_NOP,        exe_std$zeroparm,    BUFFERED_64 );
    ini_fdt_act( &_fdt, IO$_DRVCLR,     exe_std$zeroparm,    BUFFERED_64 );
    ini_fdt_act( &_fdt, IO$_RELEASE,    exe_std$zeroparm,    BUFFERED_64 );

    ini_fdt_act( &_fdt, IO$_SEEK,       exe_std$oneparm,     BUFFERED    );
    ini_fdt_act( &_fdt, IO$_FORMAT,     exe_std$oneparm,     BUFFERED    );

    ini_fdt_act( &_fdt, IO$_SETMODE,    exe_std$setchar,     BUFFERED_64 );
    ini_fdt_act( &_fdt, IO$_SETCHAR,    exe_std$setchar,     BUFFERED_64 );

    ini_fdt_act( &_fdt, IO$_SENSEMODE,  exe_std$sensemode,   BUFFERED_64 );
    ini_fdt_act( &_fdt, IO$_SENSECHAR,  exe_std$sensemode,   BUFFERED_64 );

    ini_fdt_act( &_fdt, IO$_DIAGNOSE,   diagnose_fdt,        DIRECT );

/* Set QIOServer control flags */

    ini_fdt_qsrv( &_fdt, IO$_PACKACK,   IRP$M_QSVD|IRP$M_QBARRIER|IRP$M_QCNTRL|IRP$M_QRQT_SRVR_HLPR );
    ini_fdt_qsrv( &_fdt, IO$_SENSECHAR, IRP$M_QSVD|IRP$M_QBARRIER|IRP$M_QCNTRL|IRP$M_QRQT_SRVR_HLPR );

    ini_fdt_qsrv( &_fdt, IO$_AVAILABLE, IRP$M_QBARRIER|IRP$M_QCNTRL );
    ini_fdt_qsrv( &_fdt, IO$_NOP,       IRP$M_QBARRIER|IRP$M_QCNTRL );
    ini_fdt_qsrv( &_fdt, IO$_UNLOAD,    IRP$M_QBARRIER|IRP$M_QCNTRL );

    ini_fdt_qsrv( &_fdt, IO$_ACCESS,    IRP$M_QCNTRL );
    ini_fdt_qsrv( &_fdt, IO$_ACPCONTROL,IRP$M_QCNTRL );
    ini_fdt_qsrv( &_fdt, IO$_CREATE,    IRP$M_QCNTRL );
    ini_fdt_qsrv( &_fdt, IO$_DEACCESS,  IRP$M_QCNTRL );
    ini_fdt_qsrv( &_fdt, IO$_DELETE,    IRP$M_QCNTRL );
    ini_fdt_qsrv( &_fdt, IO$_DRVCLR,    IRP$M_QCNTRL );
    ini_fdt_qsrv( &_fdt, IO$_FORMAT,    IRP$M_QCNTRL );
    ini_fdt_qsrv( &_fdt, IO$_MODIFY,    IRP$M_QCNTRL );
    ini_fdt_qsrv( &_fdt, IO$_MOUNT,     IRP$M_QCNTRL );
    ini_fdt_qsrv( &_fdt, IO$_SEEK,      IRP$M_QCNTRL );
    ini_fdt_qsrv( &_fdt, IO$_SENSEMODE, IRP$M_QCNTRL );

    ini_fdt_end( &_fdt );

/* If we got this far then everything worked, so return success. */

    return( SS$_NORMAL );			/* Return with success status */

      }                 
                  


/* STRUC_INIT - Device Data Structure Initialization Routine 
 *
 * This routine is used to initialize the data structures at driver
 * loading time.
 *
 * Usage:
 *      struc_init( crb, ddb, idb, orb, ucb )
 *
 * Input:
 *      crb     pointer to CRB
 *      ddb     pointer to DDB
 *      idb     pointer to IDB
 *      orb     pointer to ORB
 *      ucb     pointer to UCB
 *
 * Output:
 *      none
 *
 * Return value:
 *      none
 *
 */

void struc_init( CRB *crb, DDB *ddb, IDB *idb, ORB *orb, DQ_UCB *ucb )

  {

/*  BREAK( 0x00020000, (int) ucb, 0, 0 );	/@ BREAK: struc_init called -- Can't TRACE yet */

/* Initialize the fork lock and device IPL fields */

    baseucb.ucb$b_flck = SPL$C_IOLOCK8;		/* set up fork lock index */
    baseucb.ucb$b_dipl = DEVICE_IPL;		/*  and device IPL */

/* Initialize some UCB fields */

    baseucb.ucb$l_devchar = ( DEV$M_DIR		/* Device is directory-structured */
                            + DEV$M_FOD		/* File-oriented device */
                            + DEV$M_AVL		/* Device is available for use */
                            + DEV$M_ELG		/* Device has error-Logging enabled */
                            + DEV$M_IDV		/* Device is capable of providing input */
                            + DEV$M_ODV		/* Device is capable of providing output */
                            + DEV$M_SHR		/* Device is shareable */
                            + DEV$M_RND );	/* Device allows random-access */
    baseucb.ucb$l_devchar2 =
                            ( DEV$M_CLU		/* The device is cluster accessible */
                            + DEV$M_NNM		/* Use "node$" device names */
                            + DEV$M_NLT );	/* "No Last Track" bad block info on these devices */
    baseucb.ucb$b_devclass  = DC$_DISK;		/* Device class is a disk */
    baseucb.ucb$b_devtype   = DT$_GENERIC_DK;	/* Device type for DDR */
    baseucb.ucb$l_devsts    = UCB$M_NOCNVRT;	/* Do NOT convert LBNs */
    baseucb.ucb$w_devbufsiz = BLK_SIZE_512;	/* Default to ATA-sized blocks */
    baseucb.ucb$l_media_id  = 0x245242B2;	/* Media ID of DQ|IDE50 in magic encoding */

    return;

      }



/* STRUC_REINIT - Device Data Structure Re-Initialization Routine
 *
 * This routine is used to reinitialize the data structures at driver
 * reloading time.
 *
 * Usage:
 *      struc_init( crb, ddb, idb, orb, ucb )
 *
 * Input:
 *      crb     pointer to CRB
 *      ddb     pointer to DDB
 *      idb     pointer to IDB
 *      orb     pointer to ORB
 *      ucb     pointer to UCB
 *
 * Output:
 *      none
 *
 * Return value:
 *      none
 *
 */

void struc_reinit ( CRB *crb, DDB *ddb, IDB *idb, ORB *orb, DQ_UCB *ucb )

  {

/*  BREAK( 0x00030000, (int) ucb, 0, 0 );	/@ BREAK: struc_reinit called -- Can't TRACE yet */

    ddb->ddb$ps_ddt = &_ddt;			/* Point ddb to the ddt */
    ddb->ddb$l_acpd = 'F11';			/* Fill-in the default ACP name */
    dpt_store_isr( crb, isr );			/* Set up ISR address */

    return;					/* Return to caller */

      }



/* RCT_FDT - IO$_READRCT FDT Processing
 *
 * This routine is the FDT processing routine for the RCT function
 * code.  The LBN and size are checked and, if ok, the buffer is locked
 * down and the I/O handed off to be processed.
 *
 * Input:
 *      irp     pointer to IRP
 *      pcb     pointer to PCB
 *      ucb     pointer to UCB
 *      ccb     pointer to CCB
 *
 * Output:
 *
 * Return value:
 *      SS$_FDT_COMPL -- shows that the routine completed correctly
 *
 */

int rct_fdt( IRP *irp, PCB *pcb, DQ_UCB *ucb, CCB *ccb )

  {

    int status;					/* Returned routine status */

    irp->irp$l_bcnt = irp->irp$l_qio_p2;	/* Copy the byte count */
    irp->irp$l_media= irp->irp$l_qio_p3;	/* and the LBN */

    if (    (irp->irp$l_bcnt <= BLK_SIZE_512)	/* Byte count is less than or equal 512? */
         && (irp->irp$l_media == 0) )		/* LBN = 0? */
      {						/* Met the tests */
        status = exe_std$readlock( irp,		/* Then do it! */
                                   pcb,
                                   (UCB *) ucb,
                                   ccb,
                                   (void *) irp->irp$l_qio_p1,
                                   irp->irp$l_bcnt,
                                   0 );
        exe_std$qiodrvpkt( irp, (UCB *) ucb );	/* Queue the packet */
        return( SS$_FDT_COMPL );		/*  and exit */
          }
    else
      {						/* Failed the tests */
        irp->irp$l_iost1 = SS$_BADPARAM;	/* Load error code */
        irp->irp$l_iost2 = 0;			/* Clear high IOSB */
        exe_std$finishio( irp, (UCB *) ucb );	/* Finish with error */
          }

    return( SS$_FDT_COMPL );			/* exit */
      
      }



/* RDSTATS_FDT - IO$_RDSTATS FDT Processing
 * 
 * This routine is the FDT processing routine for the RDSTATS
 * function code.  If the EXTRA_STATS conditional is on, several
 * statistics are returned to the caller.  Otherwise, the SS$_NODATA
 * error is returned.
 *
 * Input:
 *      irp     pointer to IRP
 *      pcb     pointer to PCB
 *      ucb     pointer to UCB
 *      ccb     pointer to CCB
 *
 * Output:
 *
 * Return value:
 *      SS$_FDT_COMPL -- shows that the routine completed correctly
 *
 */

int rdstats_fdt( IRP *irp, PCB *pcb, DQ_UCB *ucb, CCB *ccb )

  {

    int  *bp;					/* Longword buffer pointer */
    int  i;					/* Loop counter */

/* Check that LBN = 0 and byte count is large enough  */

#ifdef EXTRA_STATS
    irp->irp$l_iost1 = SS$_BADPARAM;		/* Assume an error - Load error code */
           
    if ( !( baseucb.ucb$l_devchar & DEV$M_QSVD ) &&
         (irp->irp$l_qio_p2 >= RDSTATS_LEN) && (irp->irp$l_qio_p3 == 0) )
      {
        bp = (void *) irp->irp$l_qio_p1;	/* Point to the buffer */
        *bp = ucb->ucb$l_total_ints;		/* Get count of all interrupts */
        bp++;					/* Move to next longword */
        *bp = ucb->ucb$l_unsol_ints;		/* Get count of unsolicited interrupts */
        bp++;					/* Move to next longword */
        *bp = NUMBER_OF_CRAMS;			/* Copy over the number of CRAMS */
        bp++;
        *bp = (int) ucb->ucb$ps_xfer_buffer;	/* Transfer buffer address */
        bp++;
        *bp = (int) ucb->ucb$ps_s0_svapte;	/* Base SPTE address */
        bp++;
        *bp = (int) ucb->ucb$ps_s0_va;		/* S0 VA (user buffer) */
        bp++;
        *bp = TIMEOUT_TIME+2;			/* Save size of TIMEOUT vector */
        bp++;					/* Move to next location */

/* Copy over the timeout histogram vector */

        for (i=0; i<=(TIMEOUT_TIME+1); i++)
          {
            *bp = ucb->ucb$l_int_hist[i];	/* Copy over the interrupt time histogram */
            bp++;				/* Advance pointer */
              }

        *bp = ucb->ucb$l_int_tmo;		/* Copy over the timeout count */
        bp++;					/* Advance pointer */

        irp->irp$l_iost1 = (RDSTATS_LEN << 16) + SS$_NORMAL;
          }
#else
    irp->irp$l_iost1 = SS$_NODATA;		/* Load error code */
#endif

    irp->irp$l_iost2 = 0;			/* Clear high IOSB */
    exe_std$finishio( irp, (UCB *) ucb );	/* Finish the I/O */
    return( SS$_FDT_COMPL );			/*  and exit */

      }



/* DIAGNOSE_FDT - IO$_DIAGNOSE FDT Processing
 *
 * This routine is the FDT processing routine for the DIAGNOSE
 * function code.
 *
 * Input:
 *      irp     pointer to IRP
 *      pcb     pointer to PCB
 *      ucb     pointer to UCB
 *      ccb     pointer to CCB           
 *
 * Output:
 *
 * Return value:
 *      SS$_FDT_COMPL -- shows that the routine completed correctly
 *
 */

int diagnose_fdt( IRP *irp, PCB *pcb, DQ_UCB *ucb, CCB *ccb )
{
    DIAGNOSE_PARAM *diagnose_param;
    int status;
    int64   prvprv;

    /* For now disable remote DIAGNOSTICS */

    if ( ( ( ( UCB * )( ucb ) )->ucb$l_devchar & DEV$M_QSVD ) != 0 )
	return(exe_std$abortio( irp, pcb, (UCB *) ucb, SS$_UNSUPPORTED));

    /* Check if process has the required privilege */

    sys$setprv(0, 0, 0, &prvprv);
            
    if ((prvprv & PRV$M_DIAGNOSE) == 0)
	return(exe_std$abortio( irp, pcb, (UCB *) ucb, SS$_NOPRIV));

    diagnose_param = (DIAGNOSE_PARAM *) irp->irp$l_qio_p1;

    ucb->diagnose_opcode = diagnose_param->opcode;
    ucb->diagnose_flags = diagnose_param->flags;

    ucb->diagnose_command_length = MIN(diagnose_param->command_length, 
                                       MAX_DIAGNOSE_COMMAND_LENGTH);

    if (ucb->diagnose_command_length > 0) 
        memcpy(ucb->diagnose_command, diagnose_param->command,
               ucb->diagnose_command_length);
                               
    ucb->diagnose_data_length = MIN(diagnose_param->data_length, 
                                    MAX_DIAGNOSE_DATA_SIZE);
               
    if (ucb->diagnose_data_length > 0)
        memcpy(ucb->ucb$ps_xfer_buffer, diagnose_param->data, 
               ucb->diagnose_data_length);
                                    
    ucb->diagnose_pad_length = diagnose_param->pad_length;
    ucb->diagnose_phase_timeout = diagnose_param->phase_timeout;
    ucb->diagnose_disconnect_timeout = diagnose_param->disconnect_timeout;

    irp->irp$l_bcnt = ucb->diagnose_data_length;

    if (irp->irp$l_bcnt > 0) {
        status = exe_std$readlock( irp,		/* Then do it! */
                                   pcb,
                                   (UCB *) ucb,
                                   ccb,
                                   (void *) diagnose_param->data,
                                   irp->irp$l_bcnt,
                                   0 );
    }

    exe_std$qiodrvpkt( irp, (UCB *) ucb );	/* Queue the packet */
    return( SS$_FDT_COMPL );			/*  and exit */

}
           


/* CTRL_INIT - Controller Initialization Routine
 * 
 * This routine is used to perform controller specific initialization
 * and is called by 1) system startup, 2) during driver loading and
 * 3) during power failure recovery.
 *
 * Usage:
 *
 *      status = ctrl_init ( idb, ddb, crb )
 *
 * Input:
 *      idb     pointer to the idb
 *      ddb     pointer to the ddb
 *      crb     Pointer to the crb
 *
 * Output:
 *      None.
 *
 * Return value:
 *      SS$_NORMAL -- unit was initialized successfully.
 *
 */

int ctrl_init ( IDB *idb, DDB *ddb, CRB *crb )

  {

/*  BREAK( 0x00040000, 0, 0, 0 );		/@ BREAK: ctrl_init called -- Can't TRACE yet */

    return( SS$_NORMAL );			/* Return SUCCESS */

      }



/* UNIT_INIT - Unit Initialization Routine
 *
 * This routine is used to perform unit specific initialization
 * and is called by 1) system startup, 2) during driver loading and
 * 3) during power failure recovery.
 *
 * This routine does very little work.  Its primary job is to start up
 * the fork process that will do the bulk of the unit initialization.
 *
 * Usage:
 *
 *      status = unit_init ( idb, ucb )
 *
 * Input:
 *      idb     pointer to the IDB
 *      ucb     pointer to the UCB
 *
 * Output:
 *      None.
 *
 * Return value:
 *      SS$_NORMAL -- unit was initialized successfully.
 *
 */

int unit_init ( IDB *idb, DQ_UCB *ucb )

  {

/*  BREAK( 0x00050000, (int) ucb, (int) idb, 0 );/@ BREAK: unit_init called -- Can't TRACE yet */

    if (baseucb.ucb$v_power)			/* Is this power recovery ? */
        return( SS$_NORMAL );			/* Power recovery - just exit */

/* Set up and queue fork process to complete the unit initialization */

    baseucb.ucb$l_fpc = &unit_init_fork;	/* Point to fork routine address */
    exe_std$primitive_fork( 0, (int64) idb, (FKB *) ucb );
						/* Start fork process */
    return( SS$_NORMAL );			/* Return with success */

      }



/* UNIT_INIT_FORK - Unit Initialization Fork Routine
 *
 * This is the fork routine that does the bulk of the
 * unit initialization work.
 *
 * Usage:
 *
 *      unit_init_fork ( fr3, idb, ucb )
 *
 * Input:
 *      fr3     Fork routine parameter (unused)
 *      idb     pointer to the IDB
 *      ucb     pointer to the UCB
 *
 * Output:
 *      None.
 *
 * Return value:
 *      none
 *
 * Note:
 *
 *   The default device name of "Generic IDE/ATAPI disk" should not be
 *   seen in normal operation. This will normally either be superceded
 *   by either a real device name (read from the device) or the
 *   "Nonexistent IDE/ATAPI disk" fake device name stored by the
 *   PACKACK/SENSECHAR when we fail to read a real name from a
 *   non-existent device.
 *
 */

void unit_init_fork( void *fr3, IDB *idb, DQ_UCB *ucb )

  {

    char    model[DTN$K_NAMELEN_MAX+1] = "Generic IDE/ATAPI disk";
						/* Default model name */
    int     mod_len = 22;			/* Length of model string (*WITHOUT* trailing <null>!) */
    DTN     *dtn;				/* Dummy DTN pointer */
    CRAM    *cram_ptr;				/* Pointer to a CRAM */
    CRCTX   *ctx;				/* Pointer to a CRCTX */
    int     index;				/* Index for walking the CRAM list */
    ADP     *adp;				/* Address of ADP */
    CRB     *crb;				/* Address of CRB */
    DDB     *ddb;				/* Address of DDB */
    int     status;				/* Routine status values */
    int     page_cnt;				/* Number of pages to allocate */
    int     offset;				/* PTE offset in page table */
    int     csr_base=0;				/* Base CSR address */
    IDB     *idb_ptr;				/* CRAM IDB pointer value to use */
    uint64  q_nul = 0;				/* A quadword of zeroes */
    uint64  q_dma_csr_base;			/* A quadword of dma register address */
    IRP     *irp;				/* Pointer to the IRP we'll build */
    int32   size;				/* The size of several structures we'll allocate */
    PTE     *svapte;				/* Pointer to PTE that maps our VA */


    BREAK( 0x00060000, (int) ucb, (int) idb, 0 );/* BREAK: unit_init_fork called --- Can't TRACE yet */

    /*
     * If this is a QIOServer client unit, then allocate an IRP to request a
     * UCB update.  Otherwise, fall through and actually initialize the unit.
     */

    if ( baseucb.ucb$l_devchar & DEV$M_QSVD )
        {
	dq_qsrv_unit_init_strt( ( UCB * ) ucb );/* Allocate and initiate an IO$_PACKACK IRP */
	return;					/* and return */
        }

    adp = baseucb.ucb$ps_adp;			/* Get ADP address */
    crb = baseucb.ucb$l_crb;			/* Get CRB address */
    ddb = baseucb.ucb$l_ddb;			/* Get DDB address */

    ucb->ucb$l_dummy_flgs   = 'Flgs';		/* Put markers in the UCB */
    ucb->ucb$l_dummy_sens   = 'Sens';		/*   :                    */
    ucb->ucb$l_dummy_pakt   = 'Pakt';		/*   :                    */
    ucb->ucb$l_dummy_ints   = 'Ints';		/*   :                    */
    ucb->ucb$l_dummy_unso   = 'Unso';		/*   :                    */
    ucb->ucb$l_dummy_hist   = 'Hist';		/*   :                    */
    ucb->ucb$l_dummy_tmo    = 'Tmo!';		/*   :                    */
    ucb->ucb$l_dummy_resets = 'Rst!';		/*   :                    */
    ucb->ucb$l_dummy_trace  = 'Trac';		/*   :                    */
    ucb->ucb$l_dummy_end    = 'End!';		/*   :                    */

    ucb->ucb$l_unsolicited_int   = 0;		/* Forget any pending unsolicited interrupts */
    ucb->ucb$l_drive_lba_capable = 0;		/* Clear all the flags */
    ucb->ucb$l_drive_dma_capable = 0;		/*   :   */
    ucb->ucb$l_ctrl_id           = 0;		/*   :   */
    ucb->ucb$l_ctrl_dma_capable  = 0;		/*   :   */
    ucb->ucb$l_atapi_flag        = 0;		/*   :   */
    ucb->ucb$l_2K_flag           = 0;		/*   :   */

    ucb->ucb$l_drv_head = DRVHD_M_BASE + (baseucb.ucb$w_unit << 4);
						/* Set up drive/head unit bit for later use in commands */

    ucb->ucb$l_trc_buf      = (void *) 0xDEADDEAD;
						/* Indicate no tracing (yet) */
    ucb->ucb$l_trc_index    = 0x0000DEAD;	/*   :   */

    ucb->ucb$l_trc_unit     = baseucb.ucb$w_unit + 1;
						/* Set up part of our canonical unit number    */
    if (     ( (ddb->ddb$t_name_str[2] & 0x01 ) ==0 )
						/* Check controller letter:                    */
						/* Secondary controller (DQB, DQD, DQF, etc.)? */
          || (baseucb.ucb$w_unit>=2) )		/* DQA2:, DQA3:, DQC2:, DQC3, etc. ?           */
        ucb->ucb$l_trc_unit += 2;		/* If either, bump canonical unit by 2         */
						/* Now, 1->DQA0:, 2->DQA1:, 3->DQB0:, 4->DQB1: */
						/* allocate enough memory for two device id pages */
    status = exe$alononpaged_aln( 1024, 9, (void **)&crb->crb$l_auxstruc, &size);
    if ( $FAIL( status ) )			/* Check the return status */
        return;					/* Return if error */

#ifdef TRACE_PER_DRIVE

    status = exe_std$alononpaged( TRACING*4, &size, (void **) &ucb->ucb$l_trc_buf );
						/* Allocate pool for our tracing buffer */
    if ( $FAIL( status ) )			/* Check the return status */
        return;					/* Return if error */
    ucb->ucb$l_trc_index = 0;			/* Point the index to the beginning of the buffer */
    TRACE( 0x0FFFFFFF );			/* Record a distinctive starting pattern */
    TRACE( 0x0FF0F0F0 );			/*   :   */
    TRACE( 0x0F0F0F0F );			/*   :   */
    TRACE( 0x0FF0FFFF );			/*   :   */

#endif


#ifdef TRACE_COMMON

    ucb->ucb$l_trc_buf = &trc_dummy;		/* Provide a pointer in the UCB to the common trace buffer */

    if (trc_buf_alloc == 0)			/* Only allocate the buffer once */
      {
        trc_buf_alloc++;			/* Remember we've allocated this */

        trc_dummy    = 'Trac';			/* Set the ASCII tags */
        fixup_dummy  = 'FxUp';			/*   :                */
        fixup_bcnt   = 0;			/* Zero some counters */
        fixup_boff   = 0;			/*   :                */
        fixup_svapte = 0;			/*   :                */

        status = exe_std$alononpaged( TRACING*4, &size, (void **) &trc_buf );
						/* Allocate pool for our tracing buffer */
        if ( $FAIL( status ) )			/* Check the return status */
            return;					/* Return if error */
        trc_index = 0;				/* Point the index to the beginning of the buffer */
        TRACE( 0x0FFFFFFF );			/* Record a distinctive starting pattern */
        TRACE( 0x0FF0F0F0 );			/*   :   */
        TRACE( 0x0F0F0F0F );			/*   :   */
        TRACE( 0x0FF0FFFF );			/*   :   */
          }

#endif


/* Clear the histogram buffer counts.  Clear each entry from 0 to */
/* TIMEOUT_TIME and the overflow count at the end of the vector. */

    for (index = 0 ; index < TIMEOUT_TIME+1; index++)
      {
        ucb->ucb$l_int_hist[index] = 0;		/* Clear the interrupt histogram counters */
          }


    status = ioc$add_device_type( model, mod_len, (UCB *) ucb, &dtn );
						/* Set up a default model name of "Generic IDE/ATAPI disk" */



/* Decide which PCI controller chip, if any, we're using.
 * Then, by reading the BASE_ADDRESS_V register in the controller chip,
 * figure out where the Console has "located" the DMA registers today.
 *
 * Note:
 *
 *   These registers didn't exist back in ISA days, so there
 *   doesn't seem to be any "legacy" address like there is
 *   for the main blocks of registers.
 *
 */

        q_dma_csr_base = locate_dma_regs( ucb, &csr_base );/* Locate the DMA registers, if any, for this chip */
						/* This also sets the node_id and ctrl_dma_enable */
						/*   fields in the ucb. */



/* Ok, here's a hack.  We're going to pick up the IDB$Q_CSR value.
 * If it's <= 0x80000000, then it's treated as an offset from the
 * the base of ISA space.  For example, 0x1F0.  If not, we'll
 * assume it's the VA of the base of ISA space (as might be
 * passed by Autoconfig). And in some cases, the routine called 
 * above, locate_dma_regs, has more or better knowledge about
 * what the value should be, so allow it to set the base if
 * it can. If it has, it will be non-zero now. 
 *
 * Putting this another way, we are passed either:
 *
 *      1. An ISA offset.  Clear the CRAM IDB pointer so that
 *         only the ADP$Q_CSR field is used.  Use the IDB CSR
 *         value as the offset to the register (csr_base).
 *
 *      2. The VA of base of ISA space. We could use IOC$NODE_DATA
 *         to ask the PCI config space registers for the actual
 *         ISA addresses (as shown below), but, in fact, we'll
 *         take the less machine-dependent legacy easy way out
 *         and just always configure:
 *
 *           o DQA0 and DQA1 at 0x1F0
 *           o DQB0 and DQB1 (or DQA2: and DQA3:) at 0x170
 *
 *         Then, proceed as in 1.
 *
 *
 *            int va[8];			/@ Storage for the array returned by ioc$node_data @/
 *
 *            idb_ptr= NULL;			/@ Use pointer to IDB in CRAM @/
 *            ioc$node_data( crb,		/@ Get ISA address of one of the two IDE ports @/
 *                           IOC$K_EISA_IO_PORT,/@ "EISA"? Oh well, go with the flow @/
 *                           &va[0] );
 *
 *            if (this_is_a_Cypress_CY82C693)	/@ Cypress chip? We need this non-existent test!    @/
 *            va[0] = va[0] & 0xFFFFFFF8;	/@ If so, mask off the Cypress's "block size" bits  @/
 *						/@                                                  @/
 *						/@ Note: Bits <31:16> in that register are only R/W @/
 *						/@       if bit <4> in PCI Config Space Register 4D @/
 *						/@       ("Stand-Alone Control) is set to 1. This   @/
 *						/@       Resgister and bit may only exist in the    @/
 *						/@       CY82C693U (USB version), not the vanilla   @/
 *						/@       version.                                   @/
 *
 *            csr_base = va[0];			/@ Use that returned CSR as the base @/
 *
 */

    idb_ptr= NULL;				/* Use no IDB pointer in CRAM */
    
    if ( csr_base == 0) {
        if (idb->idb$q_csr < 0x80000000)		/* Check if it's in ISA space */
          {
            csr_base = idb->idb$q_csr;		/* Apparently, so use the passed CSR as the base */
	    q_dma_csr_base = 0;			/* What to do about the DMA registers in this case??? */
            ucb->ucb$l_ctrl_dma_capable = 0;	/* For the moment, disable DMA for this controller */
            }
        else					/* No, big VA so Autoconfig passing base adx of ISA bus */
            if (ucb->ucb$l_trc_unit <= 2)		/* DQA0: (=1) or DQA1: (=2) ?                           */
              {					/* If either, then...                                   */
                csr_base     =  0x1F0;		/* Use legacy primary addresses                    */
              }
            else					/* Else DQB0:/DQA2: (=3) or DQB1:/DQA3: (=4)            */
              {
                csr_base     =  0x170;		/* Use legacy secondary addresses                   */
                q_dma_csr_base += 0x8;		/* Use secondary group of DMA CSRs                  */
                  }
    }



/* !!!???
 *
 * Note:
 *
 * In the paragraph below, several of the values are hard-wired.
 * Realistically, they should vary depending on the type of bus
 * that we're mapping. This is just to get me off the ground.
 *
 */


//#saythis "Consider creating just one pair of mappings and sharing them among our four units"

    status = ioc$map_io( adp,			/* Map the main CSRs into our space */
                         crb->crb$l_node,	/* Node number of the bus to map */
                         &q_nul,		/* physical_offset */
                         0x1000,		/* Bytes to map including *ALL OF*: */
						/*   - Primary   ISA main regs at 0x01F0 and 0x3F6 */
						/*   - Secondary ISA main regs at 0x0170 and 0x376 */
                         IOC$K_BUS_IO_BYTE_GRAN,/* attributes */
                         &ucb->ucb$q_iohandle_1 );
    if ( $FAIL( status ) )			/* Check the return status */
        return;					/* Return if error */


    status = ioc$map_io( adp,			/* Map the DMA CSRs into our space */
                         crb->crb$l_node,	/* Node number of the bus to map */
                         &q_dma_csr_base,	/* physical_offset */
                         0x8,			/* Bytes to map including *EITHER*: */
						/*   - Primary   ISA DMA regs at 0xnnnnn0 */
						/*   - Secondary ISA DMA regs at 0xnnnnn8 */
                         IOC$K_BUS_IO_BYTE_GRAN,/* attributes */
                         &ucb->ucb$q_iohandle_2 );
    if ( $FAIL( status ) )			/* Check the return status */
        return;					/* Return if error */


/*
 * Now, load the CRAMs that we'll use for register accesses
 *
 */

    cram_ptr = baseucb.ucb$ps_cram;		/* Point to the first CRAM in our chain */

    for ( index=0; index<NUMBER_OF_NON_DMA_CRAMS; index++ )
      {						/* For each non-DMA CSR... */
        cram_ptr->cram$l_idb = idb_ptr;		/* Set IDB pointer in the CRAM */
        ucb->ucb$ps_crams[index] = cram_ptr;	/* Set up UCB table */
        status = ioc$cram_cmd( cram_init[index].cmd,
                               csr_base+cram_init[index].offset,
                               adp,
                               cram_ptr,
                               (uint64*) &ucb->ucb$q_iohandle_1 );
        cram_ptr->cram$l_idb = idb;		/* Set the IDB pointer correctly */
        if ( $FAIL( status ) )			/* Check the return status */
            return;				/* Return if error */
        cram_ptr->cram$v_der = 1;		/* Disable error reporting */
        cram_ptr = cram_ptr->cram$l_flink;	/* On to next CRAM pointer, preparing for a possible next pass */
      }

    for ( ; index<NUMBER_OF_CRAMS; index++ )	/* (Continuing our use of the already-initialized index...) */
      {						/* For each DMA CSR... */
        cram_ptr->cram$l_idb = idb_ptr;		/* Set IDB pointer in the CRAM */
        ucb->ucb$ps_crams[index] = cram_ptr;	/* Set up UCB table */
        status = ioc$cram_cmd( cram_init[index].cmd,
                               cram_init[index].offset,
                               adp,
                               cram_ptr,
                               (uint64*) &ucb->ucb$q_iohandle_2 );
        cram_ptr->cram$l_idb = idb;		/* Set the IDB pointer correctly */
        if ( $FAIL( status ) )			/* Check the return status */
            return;				/* Return if error */
        cram_ptr->cram$v_der = 1;		/* Disable error reporting */
        cram_ptr = cram_ptr->cram$l_flink;	/* On to next CRAM pointer, preparing for a possible next pass */
      }

/*
 * Touch some device registers, just to prove we can do it
 * (In other words, if we can't, crash here-and-now, not later)
 *
 * Note:
 *
 *   Even if this fails, we may not actually crash because
 *   ISA-space registers just return 0xFF for non-existent
 *   registers.
 *
 */

    inp( ucb, RD_ALT_STS );			/* Get alternate status register */
    inp( ucb, RD_DMA_STS );			/* Get a DMA register */

/*
 * Allocate transfer buffer
 *
 */

    page_cnt = ( XFER_BUFFER_SIZE + MMG$GL_PAGE_SIZE - 1 ) >> MMG$GL_VPN_TO_VA;
						/* Compute the size of the buffer in pages */
    status = exe_std$alophycntg( page_cnt, (void *) &ucb->ucb$ps_xfer_buffer );


/*
 * Allocate a buffer to hold last ATAPI request-sense
 *
 */

    status = exe_std$alophycntg( (SENSE_BUFFER_SIZE + MMG$GL_PAGE_SIZE - 1) >> MMG$GL_VPN_TO_VA,
                                (void *) &ucb->ucb$ps_sense_buffer );
    if ( $FAIL( status) )			/* Allocate the sense buffer (usually, just one page -- plenty) */
        return;					/* Just exit on failure */


/* Allocate SPTEs for double mapping the user buffer (plus guard + spillage) */

    status = ldr_std$alloc_pt( page_cnt+3, (void *) &ucb->ucb$ps_s0_svapte );
    if ( $FAIL( status ) )
        return;					/* Just exit on failure */


/* Compute S0 address of the double map buffer.  Note that "offset" will */
/* be the number of PTEs, not the offset from SPTBASE.  So, the shift is */
/* page number to VA, not PTE offset to VA.  A small factor of PTE size. */

    offset = ucb->ucb$ps_s0_svapte - mmg$gl_sptbase;
    ucb->ucb$ps_s0_va = (BYTE *) ( (offset << MMG$GL_VPN_TO_VA) | VA$M_SYSTEM );



/*
 * Allocate and initialize the data buffer CRCTX structure.
 * Then load the map registers that cover our data buffer
 *
 */

    status = ioc$alloc_crctx( adp->adp$l_crab,		/* CRAB address */
                              &ucb->ucb$ps_xfer_crctx,	/* Address to save the CRCTX address */
                              SPL$C_IOLOCK8);		/* Lock information */
    if ( $FAIL( status ) )				/* Did that go okay? */
        return;						/* Just return on failure */
    ctx = ucb->ucb$ps_xfer_crctx;			/* Point to the context we just created */
    ctx->crctx$l_item_cnt = XFER_BUFFER_MAP_PAGES + 2;	/* Including 2 for guard pages */
    status = ioc$alloc_cnt_res( adp->adp$l_crab,	/* CRAB address */
                                ucb->ucb$ps_xfer_crctx,	/* xfer buffer CRCTX address */
                                0,			/* Unused */
                                0,			/*    :   */
                                0 );			/*    :   */
    if ( $FAIL( status ) )				/* Did that go okay? */
        return;						/* Just return on failure */

    mmg_std$svaptechk( ucb->ucb$ps_xfer_buffer, 0, 0, &svapte );
							/* Get SVAPTE for the data buffer's VA */

    status = ioc$load_map( adp,				/* ADP address */
                           ucb->ucb$ps_xfer_crctx,	/* xfer buffer CRCTX address */
                           svapte,			/* SVAPTE */
                           (int) ucb->ucb$ps_xfer_buffer & mmg$gl_bwp_mask,
							/* Byte offset into the page */
                           &(ucb->ucb$l_xfer_phy) );
							/* Address to save the resulting DMA address */
    if ( $FAIL( status ) )				/* Did that go okay? */
        return;						/* Just return on failure */


           
/*
 * Allocate and align a small space to hold our PRDT table
 * Allocate and initialize the PRDT CRCTX structure.
 * Then load the map registers that cover the PRDT
 *
 */


    status = exe_std$alononpaged( PRDT_TABLE_SIZE*2, &size, (void **) &ucb->ucb$l_prdt );
    if ( $FAIL( status) )				/* Allocate the PRDT table space */
        return;						/* Just exit on failure */
    ucb->ucb$l_prdt = (int *)  ( (  ( (int) ucb->ucb$l_prdt ) + PRDT_TABLE_SIZE - 1 ) & PRDT_ADX_MASK );
							/* Now, force the pointer into alignment. */
							/* This also ensures that it doesn't      */
							/*   cross any page boundaries            */

    status = ioc$alloc_crctx( adp->adp$l_crab,		/* CRAB address */
                              &ucb->ucb$ps_prdt_crctx,	/* Address to save the CRCTX address */
                              SPL$C_IOLOCK8);		/* Lock information */
    if ( $FAIL( status ) )				/* Did that go okay? */
        return;						/* Just return on failure */

    ctx = ucb->ucb$ps_prdt_crctx;			/* Point to the context we just created */
    ctx->crctx$l_item_cnt = 3;				/* Including 1 page for spillover and 1 page for guard */
    status = ioc$alloc_cnt_res( adp->adp$l_crab,	/* CRAB address */
                                ucb->ucb$ps_prdt_crctx,	/* PRDT CRCTX address */
                                0,			/* Unused */
                                0,			/*    :   */
                                0 );			/*    :   */
    if ( $FAIL( status ) )				/* Did that go okay? */
        return;						/* Just return on failure */

    mmg_std$svaptechk( ucb->ucb$l_prdt, 0, 0, &svapte );/* Get SVAPTE for the PRDT's VA */

    status = ioc$load_map( adp,				/* ADP address */
                           ucb->ucb$ps_prdt_crctx,	/* PRDT CRCTX address */
                           svapte,			/* SVAPTE */
                           (int) ucb->ucb$l_prdt & mmg$gl_bwp_mask,
							/* Byte offset into the page */
                           &(ucb->ucb$l_prdt_phy) );
							/* Address to save the resulting DMA address */
    if ( $FAIL( status ) )				/* Did that go okay? */
        return;						/* Just return on failure */



/* Do any controller-specific initialization
 *
 */


    switch (ucb->ucb$l_ctrl_id)
      {

        case 0x522910B9:			/* The Acer chip */
          {
            status = ioc$write_pci_config( adp,		/* Write the CDRC -- CD-ROM (ATAPI?) Control Register  */
                                          crb->crb$l_node,
                                          0x53,		/* Register at offset 0x53 in config space             */
                                          IOC$K_BYTE_LANED,
						  	/* For V71R's benefit, avoid EV6 IOC$K_BYTE feature    */
                                          0x01<<24 );	/* Enabling CD-ROM DMA, shifted into the MS byte lane  */
            if ( $FAIL( status ) )			/* Check the return status                             */
                return;					/* Return if error                                     */
            break;					/* Done with Acer-specific stuff                       */
              }


        case 0xC6931080:			/* The Cypress chip */
          {					/*   :              */
            break;				/* (Nothing to do)  */
              }					/*   :              */


        default:				/* Anything else (hopefully ISA comes here too!) */
          {					/*   :              */
            break;				/* (Nothing to do)  */
              }					/*   :              */


          }



/*
 * Enable interrupts
 *
 */

    status = ioc$node_function( baseucb.ucb$l_crb, IOC$K_ENABLE_INTR );
    if ( $FAIL( status ) )			/* Check status and */
        return;					/*  simply exit if error */


/*
 * Size the disk (for non-system disks)
 * or size and pack-ack the disk (for system disks)
 *
 */

    status = exe_std$alononpaged( sizeof(IRP), &size, (void **) &irp );
    if ( $FAIL( status ) )			/* Check status */
        return;					/* If it failed, return */

    memset( irp, 0x0, size );			/* Clear all the memory we just allocated */
    irp->irp$w_size   = size;			/* And make it all into an IO$_PACKACK or IO$_SENSECHAR IRP */
    irp->irp$b_type   = DYN$C_IRP;		/*   :   */
    irp->irp$l_ucb    = &baseucb;		/*   :   */
    if (&baseucb == sys$ar_bootucb)		/*   :   */
        irp->irp$l_func   = IO$_PACKACK;	/*   :   */
    else					/*   :   */
        irp->irp$l_func   = IO$_SENSECHAR;	/*   :   */
    irp->irp$v_physio = 1;			/*   :   */
    irp->irp$l_pid    = (unsigned int) exe_std$deanonpaged;
    baseucb.ucb$l_qlen++;			/* Bump up our IO queue length */


//#saythis "Don't forget to knock it offline upon a possible failure later!"
    baseucb.ucb$v_online = 1;			/* Mark the purported disk as on-line */
    ucb->ucb$r_dq_dt.ucb$l_maxblock = 0x7FFFFFFF;
						/* Give it a temporary (but valid) capacity */
    baseucb.ucb$v_bsy = 1;			/* Mark the unit as busy */

    ioc_std$initiate( irp, &baseucb );		/* Initiate processing of that IRP */

    return;					/* And return */

      }

/* DQ_QSRV_UNIT_INIT_STRT - Allocate and initialize a PACKACK IRP
 *
 * This routine causes the unit to be added to the QIOServer Client's
 * list of devices.  It then allocates an IRP to be sent to the server
 * to acquire general device data to update the UCB.  Since not all
 * data in the UCB is used on the client side, only selected portions
 * are updated.
 *
 * A completion routine will de-allocate the IRP and set it online.
 *
 *
 * Input:
 *	ucb	pointer to the UCB
 *
 * Output:
 *	none
 *
 * Return Value:
 *	none
 *
 */

void dq_qsrv_unit_init_strt( UCB *ucb )
    {
    IRP			*irp;
    IRP			*scratch_irp;
    UCB			*scratch_ucb;
    int32		size;
    unsigned int	status;

    status = exe_std$qioserver_new_unit( ucb );
    if ( $SUCCESS( status ) )
        {
	status = exe_std$alononpaged( sizeof( IRP ), &size, (void **) &irp );
							/* Allocate an IRP */
        if ( $SUCCESS( status ) )
            {
            memset( irp, 0x0, size );			/* Clear all the memory we just allocated */
	    irp->irp$w_size   = size;			/* Initialize the IRP */
	    irp->irp$b_type   = DYN$C_IRP;
            irp->irp$l_ucb    = ucb;			/* And make it into an IO$_PACKACK IRP */
            irp->irp$l_func   = IO$M_QSRV_CLIENT_UPDATE|IO$_PACKACK;
            irp->irp$l_sts    = IRP$M_PHYSIO|IRP$M_FAST_FINISH|IRP$M_FINIPL8;
            irp->irp$l_sts2   = IRP$M_NORETRY|IRP$M_PID_S0_MV;
            irp->irp$l_pid    = ( unsigned int ) &dq_qsrv_unit_init_comp;
            ucb->ucb$l_qlen++;				/* Bump up our IO queue length */
            ioc_std$initiate( irp, ucb );		/* Initiate the request */
            return;
            }
        }
    ucb->ucb$l_sts &= ~( UCB$M_ONLINE|UCB$M_BSY|UCB$M_VALID );
    while ( remque( &ucb->ucb$l_ioqfl, &irp ) )
        ioc_std$altreqcom( SS$_MEDOFL, 0, ( CDRP * ) &irp->irp$l_fqfl, &scratch_irp, &scratch_ucb );
    }

/* DQ_QSRV_UNIT_INIT_COMP - De-allocate the IRP and place the unit online
 *
 * This routine finishes up the QIOServer unit intialization process.  It
 * deallocates the IRP, drops the queue length back, and places the unit
 * into the online state if appropriate.
 *
 *
 * Inputs:
 *	irp	pointer to the IRP
 *
 * Outputs:
 *	none
 *
 * Return Value:
 *	none
 */

void dq_qsrv_unit_init_comp( IRP *irp )
    {
    unsigned int	iost1;
    IRP			*scratch_irp;
    UCB			*scratch_ucb;
    UCB			*ucb;

    irp->irp$l_ucb->ucb$l_qlen -= 1;
    exe_std$deanonpaged( irp );
    }

/* LOCATE_DMA_REGS -- Locate the base address for the DMA register block
 *
 * Decide which PCI controller chip, if any, we're using.
 * Then, by reading the BASE_ADDRESS_V register in the controller chip,
 * figure out where the Console has "located" the DMA registers today.
 * Additionally, if possible, determine the base address of the CSRs.
 * (currently only done for Acer bridges). 
 *
 *
 * Input:
 *      ucb     pointer to the UCB
 *
 * Output:
 *      csr_base : If it can be determined, return the csr_base.
 *
 * Return value:
 *      ISA address of DMA registers, if found, else 0
 *
 * Side effects:
 *      ucb$l_ctrl_id          is updated
 *      ucb$l_ctrl_dma_capable is updated
 *
 *
 * Notes:
 *
 *   o These registers didn't exist back in ISA days, so there
 *     doesn't seem to be any "legacy" address like there is
 *     for the main blocks of registers.
 *
 *   o The Acer I/O chip has one set of PCI Config Space registers
 *     so just one BA_V register for both the Primary and Secondary
 *     IDE controllers. This makes it easy to find its DMA registers.
 *
 *   o The Cypress has two blocks of PCI Config Space registers, but
 *     only the set for the Primary IDE controller has a BA_V register.
 *     This makes it tough for the us to find the DMA registers for
 *     the Secondary IDE controller, so we wander around in PCI Config
 *     Space looking for the answer.
 *
 */

int locate_dma_regs( DQ_UCB *ucb, int 	*csr_base )  

  {

    ADP     *adp;				/* Address of ADP */
    CRB     *crb;				/* Address of CRB */
    int     dma_csr_base;			/* Base DMA CSR address */
    int     node;				/* PCI node number of this device */
    int     status;				/* Routine status values */


    adp = baseucb.ucb$ps_adp;			/* Get ADP address */
    crb = baseucb.ucb$l_crb;			/* Get CRB address */

    ucb->ucb$l_ctrl_dma_capable = 0;		/* For the moment, disable DMA for this controller */

    status = ioc$read_pci_config( adp,		/* Read the vendor ID and device ID fields of the IDE controller */
                                  crb->crb$l_node,
                                  PCI$K_VENDOR_ID,	/* *AND* DEVICE_ID! */
                                  4,
                                  &(ucb->ucb$l_ctrl_id) );
    if ( $FAIL( status ) )			/* Failure? */
        return( 0x0 );				/* If so, then indicate no DMA registers */


    if (ucb->ucb$l_ctrl_id == 0x522910B9)	/* Acer Aladdin chip? */
      {						/* If so, then... */
        status = ioc$read_pci_config( adp,	/* Read the BA_V register */
                                      crb->crb$l_node,
                                      0x20,
                                      4,
                                      &dma_csr_base );
        if ( $FAIL( status ) )			/* Check the return status */
            return( 0x0 );			/* Return if error */
        ucb->ucb$l_ctrl_dma_capable = 1;	/* Else, enable DMA for this controller */
        dma_csr_base = dma_csr_base & 0xFFFFFFF0;
						/* Mask off the low four bits */

        /* Turns out the code in [sysloa]acer_support assigns a
         * PCI function number of 0 for the primary channel, and 1 for the
         * secondary channel. Trade on that knowledge here and assign the address
         * based on that rather than device name. This allows platforms to pin
         * out only one channel, and to have multiple adapters in their system.
         */
	node = crb->crb$l_node;
	if (( node & 1) == 0)
            *csr_base     =  0x1F0;		/* Use legacy primary addresses    */
	else {
            *csr_base     =  0x170;		/* Use legacy secondary addresses  */
            dma_csr_base += 0x8;		/* Use secondary group of DMA CSRs */
            }



        return( dma_csr_base );			/* Return the found value */
          }


#ifdef CYPRESS_DMA

    if ( ucb->ucb$l_ctrl_id == 0xC6931080 )	/* Cypress chip? */
      {

        int     base_node;			/* Base node to look for the Cypress registers */
        int     i;				/* A counter to look through the Cypress registers */
        int     temp;				/* Temporary storage for a returned value */


        base_node = crb->crb$l_node & 0xFFF0;	/* Find the base node for this bus */

        for (i=0;i<16;i++)			/* For each node on this bus... */

          {
            status = ioc$read_pci_config( adp,	/* Read the BA_V register */
                                          base_node+i,
                                          PCI$K_VENDOR_ID,	/* *AND* DEVICE_ID! */
                                          4,
                                          &temp );
            if ( $FAIL( status ) )		/* Check the return status */
                continue;			/* If error, next i */
            if (temp != 0xC6931080)		/* Is this a Cypress register block? */
               continue;			/* If not, next i */

            status = ioc$read_pci_config( adp,	/* Read the BA_V register */
                                          base_node+i,
                                          PCI$K_REVISION_ID,	/* *AND* PROGRAMMING_IF! */
                                          4,
                                          &temp );
            if ( $FAIL( status ) )		/* Check the return status */
                return( 0x0 );			/* Return if error */
            temp = temp & 0xFFFFFF00;		/* Mask off the revision byte */
            if (temp != 0x01018000)		/* Is this a Cypress primary IDE register block? */
               continue;			/* If not, next i */


            status = ioc$read_pci_config( adp,	/* Read the BA_V register */
                                          base_node+i,
                                          0x20,
                                          4,
                                          &dma_csr_base );
            if ( $FAIL( status ) )		/* Check the return status */
                return( 0x0 );			/* Return if error */
            ucb->ucb$l_ctrl_dma_capable = 1;	/* Else enable DMA for this controller */
            dma_csr_base = dma_csr_base & 0xFFFFFFF0;
						/* Mask off the low four bits */
            return( dma_csr_base );		/* Return the found value */

              }					/* Next i */

            return( 0x0 );			/* Can't find the regs; indicate no DMA registers */
						/* And leave ucb$l_ctrl_dma_capable cleared */
						/* (We probably should bugcheck here */

          }


#endif


    return( 0x0 );				/* Nothing we recognize; indicate no DMA registers */
						/* And leave ucb$l_ctrl_dma_capable cleared */

  }



/* REGDUMP - Register Dump Routine
 *
 * This is the register dump routine.  It is used to dump the registers
 * at the time of an error.  It is called at device IPL.
 *
 * Input:
 *      buffer  address of buffer to store registers
 *      arg_2   additional argument passed by caller
 *      ucb     pointer to UCB
 *
 * Output:
 *      none
 *
 *
 * Note:
 *
 *  For some reason, the error packet isn't displaying well.
 *  So, hack to. Fudge the pointer based on empirical results
 *  and add "ssss" and "eeee" to bracket the packet.
 *
 */

void regdump( BYTE *buffer, int arg_2, DQ_UCB *ucb )

  {

    TRACE( 0x03500000 + arg_2 );		/* REGDUMP beginning */

    buffer += 5;				/* Advance pointer */

    *buffer++ = 's';				/* Bracket the buffer */
    *buffer++ = 's';				/*   :   */
    *buffer++ = 's';				/*   :   */
    *buffer++ = 's';				/*   :   */

						/* Put all of the registers into the buffer. */
						/* Pad to an even longword                   */
    *buffer++ = arg_2;				/* Copy over the marker                      */
    *buffer++ = inp( ucb, RD_DMA_CMD  );	/* Get the DMA command register              */
    *buffer++ = inp( ucb, RD_DMA_DS1  );	/* Get the DMA device-specific register 1    */
    *buffer++ = inp( ucb, RD_DMA_STS  );	/* Get the DMA status register               */
    *buffer++ = inp( ucb, RD_DMA_DS2  );	/* Get the DMA device-specific register 2    */
    *buffer++ = inp( ucb, RD_DMA_AD0  );	/* Get the DMA PRDT Address Register 0       */
    *buffer++ = inp( ucb, RD_DMA_AD1  );	/* Get the DMA PRDT Address Register 1       */
    *buffer++ = inp( ucb, RD_DMA_AD2  );	/* Get the DMA PRDT Address Register 2       */
    *buffer++ = inp( ucb, RD_DMA_AD3  );	/* Get the DMA PRDT Address Register 3       */
    *buffer++ = inp( ucb, RD_ERROR    );	/* Get error                                 */
    *buffer++ = inp( ucb, RD_SEC_CNT  );	/* Get sector count                          */
    *buffer++ = inp( ucb, RD_SECTOR   );	/* Get sector number                         */
    *buffer++ = inp( ucb, RD_CYL_LO   );	/* Get cylinder number (low)                 */
    *buffer++ = inp( ucb, RD_CYL_HI   );	/* Get cylinder number (high)                */
    *buffer++ = inp( ucb, RD_DRV_HD   );	/* Get drive/head information                */
    *buffer++ = inp( ucb, RD_STS      );	/* Get status, quashing any pending interrupts as well */
    *buffer++ = 0;				/* Round up to an even                       */
    *buffer++ = 0;				/*  number of longwords                      */

    *buffer++ = 'e';				/* Add tail of buffer bracket */
    *buffer++ = 'e';				/*   :   */
    *buffer++ = 'e';				/*   :   */
    *buffer++ = 'e';				/*   :   */

    TRACE( 0x03510000 + arg_2 );		/* REGDUMP ending */

      }



/* STARTIO - Start I/O Routine
 *
 * This is the driver start I/O routine.  This routine processes each
 * of the I/O requests.
 *
 * Input:
 *      irp     Pointer to I/O request packet
 *      ucb     Pointer to unit control block
 *
 * Output:
 *      none
 *
 *
 * Note:
 *
 *   IO$_SENSECHAR is never queued to us by VMS. Instead, we
 *   queued this IO function code to ourselves as part of our
 *   startup; we do this to size non-system disks. (System
 *   disks get IO$_PACKACK.)
 *           
 */

void startio( KPB *kpb )

  {

    int     iost1, iost2;			/* IOSB fields */
    int     temp;				/* Temporary value */
    DQ_UCB  *ucb;				/* Pointer to UCB */
    IRP     *irp;				/* Pointer to IRP */
    int     status;

/* Set up necessary pointers */


    ucb = (DQ_UCB *) kpb->kpb$ps_ucb;		/* Get UCB pointer */
                     
//#saythis "Temporary test for the V_BSY bit.."
    if (baseucb.ucb$v_bsy == 0)			/* Is this an expected interrupt?            */
        BPTRACE( 0x010E0000 );			/* STARTIO starting *WITHOUT* V_BSY!          */

    irp = kpb->kpb$ps_irp;			/* Get IRP pointer */
    ucb->ucb$ps_kpb = kpb;			/* Save the KPB address */
    ucb->ucb$l_media.lbn = irp->irp$l_media;	/* Copy the disk address */


//#saythis "Temporary new copies to hack around VBNMAPFAIL crashes..."

    if (baseucb.ucb$l_bcnt != irp->irp$l_bcnt)	/* Is bcnt correct?                            */
      {						/* If not, then...                             */
#ifdef TRACE_COMMON				/*                                             */
        fixup_bcnt++;				/* Bump the event counter                      */
#endif						/*                                             */
        TRACE(   0x01200000 );			/* UCB$L_BCNT corruption (by IRP over-copy!)   */
//      BPTRACE( 0x01200000 );			/* Blammo!                                     */
        baseucb.ucb$l_bcnt   = irp->irp$l_bcnt;	/* Copy the bcnt from the IRP                  */
          }

    if (baseucb.ucb$l_boff != irp->irp$l_boff)	/* Is boff correct?                            */
      {						/* If not, then...                             */
#ifdef TRACE_COMMON				/*                                             */
        fixup_boff++;				/* Bump the event counter                      */
#endif						/*                                             */
        TRACE(   0x01210000 );			/* UCB$L_BOFF corruption (by IRP over-copy!)   */
//      BPTRACE( 0x01210000 );			/* Blammo!                                     */
        baseucb.ucb$l_boff   = irp->irp$l_boff;	/* Copy the boff from the IRP                  */
          }

    if (baseucb.ucb$l_svapte != irp->irp$l_svapte)	/* Is bcnt correct?                    */
      {							/* If not, then...                     */
#ifdef TRACE_COMMON				/*                                             */
        fixup_svapte++;				/* Bump the event counter                      */
#endif						/*                                             */
        TRACE(   0x01220000 );			/* UCB$L_SVAPTE corruption (by IRP over-copy!) */
//      BPTRACE( 0x01220000 );			/* Blammo!                                     */
        baseucb.ucb$l_svapte  = irp->irp$l_svapte;	/* Copy the bcnt from the IRP          */
          }

//#saythis "...End of Temporary new copies to hack around VBNMAPFAIL crashes"


    ucb->ucb$l_bcr = baseucb.ucb$l_bcnt;	/* Copy remaining byte count */

    TRACE( 0x01000000 +   (irp->irp$v_fcode    )           );	/* STARTIO starting...         */
    TRACE( 0x01010000 + ( ( (int) irp>>16      ) & 0xFFFF) );	/*   :  Log starting IRP_hi    */
    TRACE( 0x01020000 + ( ( (int) irp          ) & 0xFFFF) );	/*   :  Log starting IRP_lo    */
    TRACE( 0x01030000 + ( (irp->irp$l_media>>16) & 0xFFFF) );	/*   :  Log starting LBA_hi    */
    TRACE( 0x01040000 + ( (irp->irp$l_media    ) & 0xFFFF) );	/*   :  Log starting LBA_lo    */
    TRACE( 0x01050000 + ( (baseucb.ucb$l_bcnt  ) & 0xFFFF) );	/*   :  Log starting bytecount */


/* Check that either volume is valid or this is a physical I/O */

    if ( !irp->irp$v_physio && !baseucb.ucb$v_valid)
      {
        ioc_std$reqcom( SS$_VOLINV, 0, (UCB *) ucb );
						/* Finish I/O */
        BPTRACE( 0x01100000 );			/* BREAK: STARTIO punting on volume not valid... */
        return;					/* And return */
          }

/* Interpret the LBN according to PHYSIO bit */

    if (irp->irp$v_physio)			/* Convert from physical format? */
      {
        switch (irp->irp$v_fcode)		/* Does this command use an address? */
          {

            case IO$_READLBLK:			/* These shouldn't occur with v_phys set, right? */
            case IO$_WRITELBLK:			/*    :   */
              BPTRACE( 0x01110000 );		/* BREAK: IO$_READLBLK or IO$_WRITELBLOCK with V_PHYS set */
						/* Fall through anyway... */
            case IO$_SEEK:			/* These can be physical and use an address */
            case IO$_WRITECHECK:		/*    :   */
            case IO$_READPBLK:			/*    :   */
            case IO$_WRITEPBLK:			/*    :   */
              {					/* So range-check the address */
                if (    (ucb->ucb$l_media.pa.sec == 0)				/* [1:n]   */
                     || (ucb->ucb$l_media.pa.sec >  baseucb.ucb$b_sectors )	/*   :     */
                     || (ucb->ucb$l_media.pa.trk >= baseucb.ucb$b_tracks  )	/* [0:n-1] */
                     || (ucb->ucb$l_media.pa.cyl >= baseucb.ucb$w_cylinders ) )	/* [0:n-1] */
                  {
                    BPTRACE( 0x0112000 );	/* BREAK: CHS address out of range */
                    ioc_std$reqcom( SS$_BADPARAM, 0, (UCB *) ucb );
						/* Complete the I/O failing */
                    return;			/* And return */
                    break;
                      }
                  }

            default:				/* No address used -- no need to range-check */
              break;

              }

        ucb->ucb$l_media.lbn = (   (   (   (ucb->ucb$l_media.pa.cyl * baseucb.ucb$b_tracks )
                                     + ucb->ucb$l_media.pa.trk) * baseucb.ucb$b_sectors )
                                 + ucb->ucb$l_media.pa.sec - 1 );
						/* Convert the physical address to an LBN */
          }


/* Remember the transfer parameters */

    ucb->ucb$l_org_media = ucb->ucb$l_media.lbn;/* LBN */
    ucb->ucb$l_org_svapte= baseucb.ucb$l_svapte;/* Page table address */
    ucb->ucb$l_org_bcnt  = baseucb.ucb$l_bcnt;	/* Byte count */
    ucb->ucb$l_org_boff  = baseucb.ucb$l_boff;	/* Byte offset */

/* Handle based on function code */

    TRACE( 0x01060000 );			/* Log our calling reqchan... */
    iost1 = ioc$kp_reqchan( kpb, KPB$K_LOW );	/* Get the data channel */
    if ( $FAIL( iost1 ) )			/* Check for failure to get channel */
      {
        ioc_std$reqcom( iost1, 0, (UCB *) ucb );/* Finish I/O */
        BPTRACE( 0x01130000 + (iost1 &0xFFFF) );/* BREAK: STARTIO punting on failure to get channel... */
        return;					/* And exit */
          }

    iost1 = SS$_ILLIOFUNC;			/* Assume illegal I/O function */
    iost2 = 0;					/* Assume no data transferred */

    switch (irp->irp$v_fcode)
      {

        case IO$_NOP:
            BPTRACE( 0x01070000 );		/* BREAK: IO$_NOP */
            iost1 = SS$_NORMAL;			/* Status is "normal" */
            break;				/*  and complete the I/O */

        case IO$_UNLOAD:
            BPTRACE( 0x01070001 );		/* BREAK: IO$_UNLOAD */
            iost1 = unload( ucb );		/* Call the unload function */
            break;				/*  and complete the I/O */

        case IO$_SEEK:
            BPTRACE( 0x01070002 );		/* BREAK: IO$_SEEK */
            iost1 = seek( ucb );		/* Call the SEEK function */
            break;				/*  and complete the I/O */

        case IO$_DRVCLR:
            BPTRACE( 0x01070004 );		/* BREAK: IO$_DRVCLR */
            iost1 = drvclr( ucb );		/* Call the DRIVE CLEAR function */
            break;				/*  and complete the I/O */

        case IO$_PACKACK:
            iost1 = packack( ucb, 0 );		/* Call PACKACK w/o asserting the init_time_flag */
            break;				/*  and complete the I/O */

        case IO$_READRCT:
            BPTRACE( 0x01070009 );		/* BREAK: IO$_READRCT */
            iost1 = readrct( ucb );		/* Get back the drive data */
            iost1 = (iost1 & 0xFFFF) + (baseucb.ucb$l_bcnt << 16);
            break;				/*  and complete the I/O */

        case IO$_AVAILABLE:
            BPTRACE( 0x01070011 );		/* BREAK: IO$_AVAILABLE */
            iost1 = unload( ucb );		/* Call the unload function */
            break;				/*  and complete the I/O */

        case IO$_DIAGNOSE:
            ucb->ucb$l_sense_key = 0;
            status = diagnose( ucb );	        /* Call the audio function */
            temp  = baseucb.ucb$l_bcnt - ucb->ucb$l_bcr;
            iost1 = SS$_NORMAL | ((temp & 0xFFFF) << 16);
            iost2 = ((ucb->ucb$l_sense_key & 0xFF) << 24);                    
            break;				/*  and complete the I/O */

        case IO$_FORMAT:
            BPTRACE( 0x0107001E );		/* BREAK: IO$_FORMAT */
            iost1 = SS$_UNSUPPORTED;		/* Return UNSUPPORTED error for now */
            break;				/*  and complete the I/O */

        case IO$_SENSECHAR:
            iost1 = packack( ucb, 1 );		/* Call PACKACK asserting the init_time_flag */
            break;				/*  and complete the I/O */

        case IO$_WRITECHECK:                                  
            BPTRACE( 0x0107000A );		/* BREAK: IO$_WRITECHECK */
        case IO$_READLBLK:
        case IO$_READPBLK:
            iost1 = read( ucb );		/* Read the required blocks */
            if ( $FAIL( iost1 ) )		/* Did the read go akay? */
              break;				/* If not, bug out now */
            if ( IS_SET( irp->irp$l_func, IO$M_DATACHECK ) )
						/* Datacheck requested? */
                iost1 = datacheck( ucb );	/*  Yes, do the datacheck */
            temp  = baseucb.ucb$l_bcnt - ucb->ucb$l_bcr;
            iost1 = (iost1 & 0xFFFF) + (temp << 16);
            break;				/*  and complete the I/O */

        case IO$_WRITELBLK:
        case IO$_WRITEPBLK:
            iost1 = write( ucb );		/* Write the required blocks */
            if ( $FAIL( iost1 ) )		/* Did the read go akay? */
              break;				/* If not, bug out now */
            if ( IS_SET( irp->irp$l_func, IO$M_DATACHECK ) )
						/* Datacheck requested? */
                iost1 = datacheck( ucb );	/*  Yes, do the datacheck */
            temp  = baseucb.ucb$l_bcnt - ucb->ucb$l_bcr;
            iost1 = (iost1 & 0xFFFF) + (temp << 16);
            break;				/*  and complete the I/O */

        case IO$_AUDIO:
            BPTRACE( 0x01070037 );		/* BREAK: IO$_AUDIO (IO$_READPROMPT) */
            iost1 = SS$_UNSUPPORTED;		/* Return UNSUPPORTED error for now */
/*          iost1 = audio_audio( ucb );		/@ Call the audio function */
            break;				/*  and complete the I/O */

        default:				/* Better not happen! */
            BPTRACE( 0x01071FFF );		/* BREAK: Default case taken at IO$_function dispatch */
            break;				/* But if it does, ILLIOFUNC gets returned */

          }


    TRACE( 0x01FC0000 + ( iost1      & 0xFFFF));/* STARTIO finishing... (IOSB_1 low word)  */
    TRACE( 0x01FD0000 + ((iost1>>16) & 0xFFFF));/* STARTIO finishing... (IOSB_1 high word) */
    TRACE( 0x01FE0000 + ( iost2      & 0xFFFF));/* STARTIO finishing... (IOSB_2 low word)  */
    TRACE( 0x01FF0000 + ((iost2>>16) & 0xFFFF));/* STARTIO finishing... (IOSB_2 high word) */
    ioc_std$relchan( (UCB *) ucb );		/* Release the data channel */

//#saythis "Temporary test for the V_BSY bit..."
    if (baseucb.ucb$v_bsy == 0)			/* Is this an expected interrupt?            */
        BPTRACE( 0x010F0000 );			/* STARTIO ending *WITHOUT* V_BSY!           */


//#saythis "Temporary test for UCB corruption..."

    if (baseucb.ucb$l_bcnt != irp->irp$l_bcnt)	/* Is bcnt correct?                            */
      {						/* If not, then...                             */
//      TRACE(   0x01300000 );			/* UCB$L_BCNT corruption (by IRP over-copy!)   */
        BPTRACE( 0x01300000 );			/* Blammo!                                     */
          }

    if (baseucb.ucb$l_boff != irp->irp$l_boff)	/* Is boff correct?                            */
      {						/* If not, then...                             */
//      TRACE(   0x01310000 );			/* UCB$L_BOFF corruption (by IRP over-copy!)   */
        BPTRACE( 0x01310000 );			/* Blammo!                                     */
          }

    if (baseucb.ucb$l_svapte != irp->irp$l_svapte)	/* Is bcnt correct?                    */
      {							/* If not, then...                     */
//      TRACE(   0x01320000 );			/* UCB$L_SVAPTE corruption (by IRP over-copy!) */
        BPTRACE( 0x01320000 );			/* Blammo!                                     */
          }

//#saythis "...End of temporary new copies to hack around VBNMAPFAIL crashes"


    ioc_std$reqcom( iost1, iost2, (UCB *) ucb );/* Finish I/O, providing status in IOSB */
    return;					/* And return */

      }



/* PACKACK - Perform PACKACK operation
 *
 * This routine is used to determine information about the drive so
 * that is can be mounted and put to use.
 *
 * Input:
 *      ucb             pointer to UCB
 *      init_time_flag  Set to indicate we're being called during
 *                        initialization time: modifies processing.
 *
 * Output:
 *      none
 *
 * Return value:
 *      status value:
 *         SS$_NORMAL - success
 *         SS$_NODATA - failed to get drive info
 *         SS$_other -- specific error from a lower level
 *
 * Notes:
 *
 *   This is also the function called to process the IO$_SENSECHAR
 *   that we queue to ourselves during initialization of non-system
 *   disks. Processing is essentially the same, but we stop short
 *   of getting the capacity (for ATAPI drives) and setting the
 *   ucb$v_valid volume valid bit.
 *
 *   If the ATA fetch_drive_info() fails, we could, conceptually,
 *   look for the magic ATAPI signature in the STS, CYL_LO, and
 *   CYL_HI registers (STS=0x00: neither 'Ready' nor 'Busy',
 *   CYL_HI=0xEB, CYL_LO=0x14). But instead, we just barge ahead
 *   and do an ATAPI PACKET_IDENTIFY command and see if we get a
 *   response.
 *
 *   Reportedly, certain TEAC CD-ROM drives refuse to speak to
 *   us until we read the "signature". (This signature is presented
 *   by ATAPI devices after a reset or refused command such as
 *   ATA_GET_INFO.) We read the signature, just in case.
 *
 *   If an ATAPI drive has just powered up or the medium has just
 *   been changed, there are certain special conditions we want
 *   to handle. If we were a more-sophisticated driver, we'd
 *   probably have a state machine handle all this but for now,
 *   we'll just do some empirically-determined retry code. These
 *   errors can also "stack up": If the drive has just powered-up
 *   and you have just inserted media into it, you can get SENSE=6,
 *   ASC=0x29 ("Power on, reset, or bus reset occurred") on one
 *   retry and immediately get SENSE=6, ASC=0x28 ("``Not ready''
 *   to ``ready'' change , medium may have changed") on the
 *   next retry.
 *
 */
                        

int packack( DQ_UCB *ucb, int init_time_flag )

  {

    char  model[DTN$K_NAMELEN_MAX+1] = "Nonexistent IDE/ATAPI disk";
						/* Model name upon failure */
    int   mod_len = 26;				/* Length of model string (*WITHOUT* trailing <null>!) */
    DTN   *dtn;					/* Dummy DTN pointer */
    int   status;				/* Return status from various routines */
    int   retry;				/* Retry counter */
    int   cyl_lo;				/* Drive CYL_LO register */
    int   cyl_hi;				/* Drive CYL_HI register */
    int   drvsts;				/* Drive status register */


    ucb->ucb$l_sense_key = 0xDEADDEAD;		/* Forget any remembered sense key */
    ucb->ucb$l_asc       = 0xDEADDEAD;		/* Forget any remembered additional sense code */
    ucb->ucb$l_ascq      = 0xDEADDEAD;		/* Forget any remembered additional sense code qualifier */

    status = fetch_drive_info( ucb, 0, init_time_flag );
						/* Try an ATA get_info */

    if ( $FAIL( status ) )			/* Did it fail? */
      {						/* If so, then read "signature", just in case */
        drvsts = inp( ucb, RD_STS );		/* Read device status, quashing any pending interupts as well */
        cyl_hi = inp( ucb, RD_CYL_HI );		/* Read high order cylinder bits */
        cyl_lo = inp( ucb, RD_CYL_LO );		/* Read low order cylinder bits */
        status = fetch_drive_info( ucb, 1, init_time_flag );
						/* And try an ATAPI get_info instead */
          }

    if ( $FAIL( status ) )			/* Is status still failing? */
      {						/* If so, then... */
        status = ioc$add_device_type( model, mod_len, (UCB *) ucb, &dtn );
						/* Change the device name to "Nonexistent IDE/ATAPI disk" */
        return( SS$_NOSUCHDEV );		/* And exit with appropriate status */
          }
						/* Either ATA or ATAPI get_info worked */
    status = process_drive_info( ucb );		/* Collect the returned drive info */

    if ( $FAIL( status ) )			/* Success? */
        return( status );			/* If not, then exit with error */

    if (init_time_flag)				/* Doing this during init_time? */
        {
						/* If not QIOServer serveable yet */
        if ( ( ( ( UCB * )( ucb ) )->ucb$l_devchar & DEV$M_QSVBL ) == 0 )
            {
            ( ( UCB * )( ucb ) )->ucb$l_devchar |= DEV$M_QSVBL;	/* Mark the unit as QIOServeable and add the unit */
            exe_std$qioserver_new_unit( ( UCB * ) ucb );	/* to the QIOServer data base */
            }
        return( SS$_NORMAL );			/* If so, all done -- go no further */
        }

    if (ucb->ucb$l_atapi_flag != 0)		/* ATAPI ? */
      {

        for (retry=0;retry<8;retry++)		/* Try this eight times... */
          {					/* Drives take ~10 seconds to become ready */

            status = atapi_read_capacity( ucb, (BYTE *) ucb->ucb$ps_sense_buffer );
						/* Read the drive capacity */

            if ( $SUCCESS( status ) )		/* Success? */
                break;				/* If so, then break out of retry loop */

            BPTRACE( 0x04200000 );		/* BREAK: Error during packack */

            status = atapi_request_sense( ucb, (BYTE *) ucb->ucb$ps_sense_buffer );
						/* Read the sense data to see what went wrong */
    
            BPTRACE( 0x04210000 );		/* BREAK: After request_sense during packack */

            if ( $FAIL( status ) )		/* Success? */
                return( status );		/* If not, then exit with error */

            if (    (ucb->ucb$l_asc==0x04)	/* "Logical unit is in process of becoming ready" */
                 && (ucb->ucb$l_ascq==0x01) )	/*   :   */
              {					/* If so, then... */
                sleep( ucb, 2 );		/* Hang out for 2 seconds */
                continue;			/* And commence the next retry */
                  }

            if (ucb->ucb$l_asc==0x28)		/* "Medium may have changed" */
               continue;			/* If so, commence the next retry */

            if (ucb->ucb$l_asc==0x29)		/* Various "Reset occurred" errors */
               continue;			/* If so, commence the next retry */

            if (ucb->ucb$l_asc==0x30)		/* Incompatible medium in drive */
                return( SS$_MEDOFL );		/* Not much point in re-trying */

            if (ucb->ucb$l_asc==0x3A)		/* No medium in drive */
                return( SS$_MEDOFL );		/* Not much point in re-trying */

            if (    (ucb->ucb$l_asc==0x00)	/* The drive doesn't think an error occurred */
                 && (ucb->ucb$l_ascq==0x00) )	/*   :   */
              {
                BPTRACE( 0x04220000 );		/* BREAK: Drive denies any error occurred during packack */
                continue;			/* Commence the next retry anyway */
                  }

						/* Any other sense keys... */
            BPTRACE( 0x04230000 );		/* BREAK: Unhandled sense key during packack */
            return( SS$_DRVERR );		/* And default to a nice, safe disaster */
						/*   "SYSTEM-W-DRVERR, fatal driver error" */

              }					/* Next retry */

        status = atapi_process_size( ucb );	/* Collect the returned drive info */
        if ( $FAIL( status ) )			/* Success? */
            return( status );			/* If not, then exit with error */

          }

    baseucb.ucb$v_valid = 1;			/* Set the Volume VALID bit */

    return( SS$_NORMAL );			/* Return to caller with success */

      }



/* FETCH_DRIVE_INFO - This routine is used to read a drive's
 *                    drive information page
 *
 * Input:
 *      ucb             pointer to the UCB
 *      atapi_flag      0 -- to do an ATA drive_info
 *                      1 -- to do an ATAPI drive_info
 *      init_time_flag  Set to indicate we're being called during
 *                        initialization time: modifies processing.
 *
 * Output:
 *      none
 *
 * Status:
 *      SS$_NORMAL --- success
 *      SS$_NODATA --- error reading the page
 *      SS$_TIMEOUT -- device timeout
 *
 */

int fetch_drive_info( DQ_UCB *ucb, int atapi_flag, int init_time_flag )

  {

    int  status;				/* Routine return status */
    int  orig_ipl;				/* Original IPL */
    int  drverr;				/* Drive error register */
    int  drvsts;				/* Drive status register */


//#saythis "Hacking around the funny buddy-init problem here"
						/* (This test is bypassed for ATAPI devices during init time!) */
    if (    (init_time_flag == 0)		/* After init time? */
         || (atapi_flag     == 0) )		/* Or trying an ATA (IDE) device? */
      {						/* If either, then... */
        status = wait_ready( ucb );		/* Wait for drive to be ready */
        if ( $FAIL( status ) )			/* Check the status for failure */
            return( status );			/* Return with error */
          }

/*
 * Take out the device lock and raise IPL
 * Write the registers
 * Then issue the appropriate command
 *
 */

    device_lock( baseucb.ucb$l_dlck, RAISE_IPL, &orig_ipl );
    out( ucb, WT_DRV_HD, ucb->ucb$l_drv_head );	/* Select drive and head 0 */

#ifdef NEVER
    inp( ucb, RD_ALT_STS );			/* Read all the registers except STS */
    inp( ucb, RD_DATA );			/*   :                               */
    inp( ucb, RD_ERROR );			/*   :                               */
    inp( ucb, RD_SEC_CNT );			/*   :                               */
    inp( ucb, RD_SECTOR );			/*   :                               */
    inp( ucb, RD_CYL_LO );			/*   :                               */
    inp( ucb, RD_CYL_HI );			/*   :                               */
    inp( ucb, RD_DRV_HD );			/*   :                               */
#endif

    if (atapi_flag)				/* Expecting ATA or ATAPI drive? */
        out( ucb, WT_CMD, CMD_ATA_PACKET_IDENTIFY);
						/* Expecting ATAPI drive */
    else
        out( ucb, WT_CMD, CMD_ATA_IDENTIFY_DEV);/* Expecting ATA drive */

    status = dq_wfikpch( ucb->ucb$ps_kpb, orig_ipl, 1 );
						/* Wait for the interrupt */
    if ( $FAIL( status ) )			/* Any error (timeout, etc.) ? */
        return( status );			/* If so, return with status */

    drvsts = inp( ucb, RD_ALT_STS );		/* Get the status byte */
    if ( IS_SET( drvsts, STS_M_ERR ) )		/* Any errors?  */
      {						/* If so, then... */
        drverr = inp( ucb, RD_ERROR );		/* Get the error byte */
        return( SS$_NODATA );			/* Return error */
          }
						/* Else success, so... */
    ucb->ucb$l_atapi_flag = atapi_flag;		/* Remember ATA or ATAPI */
    return( SS$_NORMAL );			/* Return success */

      }



/* PROCESS_DRIVE_INFO - This routine is used to read and process the
 *                      information returned by the drive in the ID page.
 *
 * Input:
 *      ucb     pointer to the UCB
 *
 * Output:
 *      none
 *
 * Status:
 *      SS$_NORMAL -- success
 *
 *
 * Note:
 *
 *   ATAPI CD-ROM drives don't return any information in the
 *   sectors, heads, and cylinders field of the block returned
 *   returned by identify_drive. So for now, we dummy this up.
 *   atapi_process_size() will fill in the actual values later.
 *
 */

int process_drive_info( DQ_UCB *ucb )

  {

    char    model[DTN$K_NAMELEN_MAX+1];		/* ASCIZ model name */
    int     mod_len;				/* Length of model string */
    ID_PAGE *id_ptr;				/* Pointer to the ID page */
    DTN     *dtn;				/* Dummy DTN pointer */
    int     status;				/* Returned routine status */
    int     i;					/* String index */
    CRB	    *crb;

/* Read the data from the sector buffer into the right place in the id page */
    
    crb=baseucb.ucb$l_crb;
    id_ptr = (ID_PAGE *)((crb->crb$l_auxstruc) + (baseucb.ucb$w_unit * 512));
    move_sec_from_drive( ucb, (BYTE *) id_ptr, BLK_SIZE_512 );

/*
 * Do some sanity checks for magnetic drives
 *
 * Else force some data for ATAPI drives
 *
 */
    if (ucb->ucb$l_atapi_flag == 0)		/* ATA or ATAPI ? */
        {					/* ATA */
        if (    (id_ptr->cyls > MAX_CYLINDER)	/* Check for too many cylinders */
             || (id_ptr->heads > MAX_HEAD+1)	/*   or too many heads          */
             || (id_ptr->sectors > MAX_SECTOR)	/*   or too many sectors        */
             || (id_ptr->cyls == 0)		/*   or too few cylinders       */
             || (id_ptr->heads == 0)		/*   or too few heads           */
             || (id_ptr->sectors == 0) )	/*   or too few sectors         */
          {					/* Any of those are bad, so...  */
            BPTRACE( 0x04300000 );		/* BREAK: Sanity checks failed during PROCESS_DRIVE_INFO */
            return( SS$_IVADDR );		/* Sanity failed - exit */
              }
						/* Copy over the geometry information... */
        baseucb.ucb$w_cylinders= id_ptr->cyls;	/* Set the cylinders */
        baseucb.ucb$b_tracks   = id_ptr->heads;	/* and tracks */
        baseucb.ucb$b_sectors  = id_ptr->sectors;/* and sectors */
        ucb->ucb$r_dq_dt.ucb$l_maxblock =	/* Now set maxblock based on those */
                       baseucb.ucb$b_sectors	/*   :   */
                     * baseucb.ucb$b_tracks	/*   :   */
                     * baseucb.ucb$w_cylinders;	/*   :   */
        set_geom( ucb );			/* Set the geometry in the drive */

	/* If drive will is capable of logical block addressing then use    */
	/* max lba as maxblock and fake  ucb geometry.  An exact geometry   */
	/* doesn't seem to be needed, but cyl*trk*sec does have to be >=    */
	/* maxblock or the Volume Control Block blockfactor will be 0 and   */
	/* cause divide by 0 crashes in mount and/or f11xqp.                */

	ucb->ucb$l_drive_lba_capable = 0;		/* Assume no LBA capability */
	if ( IS_SET( id_ptr->capabilities_49, CAP_M_LBA ) )
	    {					/* If LBA capable */
	    ucb->ucb$l_drive_lba_capable = 1;	/* Set the LBA flag */
	    if ( (UINT)(ucb->ucb$r_dq_dt.ucb$l_maxblock) < id_ptr->lba_total_blocks)
		{
		unsigned int cyl,trk,sec,lba;
		ucb->ucb$r_dq_dt.ucb$l_maxblock = id_ptr->lba_total_blocks;
		lba=id_ptr->lba_total_blocks;
		cyl=sec=trk=0;
		while ( (lba > (cyl*trk*sec)) && (cyl < MAX_UCB_CYL) )
		    {
		    trk=0;
		    while ( (lba > (cyl*trk*sec)) && (trk < MAX_UCB_TRK) )
			{
			sec=0;
			while ( (lba > (cyl*trk*sec)) && (sec < MAX_UCB_SEC) )
			    {
			    sec++;
			    }
			trk++;
			}
		    cyl++;
		    }
		baseucb.ucb$b_sectors=sec;
		baseucb.ucb$b_tracks=trk;
		baseucb.ucb$w_cylinders=cyl;
		}
	    }
        }
    else
	{					/* ATAPI */
        baseucb.ucb$w_cylinders= MAX_CYLINDER+1;/* Set the cylinders */
        baseucb.ucb$b_tracks   = MAX_HEAD+1;	/* and tracks */
        baseucb.ucb$b_sectors  = MAX_SECTOR;	/* and sectors */
        ucb->ucb$r_dq_dt.ucb$l_maxblock =	/* and now maxblock, based on those */
                       baseucb.ucb$w_cylinders	/*   :   */
                     * baseucb.ucb$b_tracks	/*   :   */
                     * baseucb.ucb$b_sectors;	/*   :   */
        set_geom( ucb );			/* Set the geometry in the drive */
	}
/*
 * Set flags based on capabilities flag
 *
 */

    ucb->ucb$l_drive_dma_capable = 0;		/* Assume no DMA capability */
    if ( IS_SET( id_ptr->capabilities_49, CAP_M_DMA ) )
						/* If the drive is DMA capable, then... */
        ucb->ucb$l_drive_dma_capable = ucb->ucb$l_ctrl_dma_capable;
						/* Set the drive's DMA flag based on the controller's DMA flag */

//    if (ucb->ucb$l_drive_dma_capable)		/* Is the drive DMA capable? */
//     set_features( ucb, 0x03, 0x20 );		/* If so, then set multiword-DMA mode 0 */
////       set_features( ucb, 0x03, 0x21 );	/* If so, then set multiword-DMA mode 1 */
//    else					/* Else if not, then... */
//       set_features( ucb, 0x03, 0x08 );	/* Set PIO mode 0 with flow-control */


/*
 * Add the device type name
 *
 * Ok, this is brain damaged, but we have to do it.
 * Each of the bytes in the ASCII string is byte swapped.
 * So, swap them back
 *
 */
    mod_len =(MODEL_LENGTH>DTN$K_NAMELEN_MAX) ?
              DTN$K_NAMELEN_MAX : MODEL_LENGTH;	/* Set the length of string */

    for (i=0; i < (mod_len>>1)<<1; i += 2)	/* For each word... */
      {
        model[i]   = id_ptr->model_number[i+1];	/* Copy the swapped bytes */
        model[i+1] = id_ptr->model_number[i];	/*   :   */
          }

    if ( (mod_len & 1) == 1)			/* Get the odd last byte if needed */
        model[mod_len-1] = id_ptr->model_number[mod_len];

    model[mod_len] = '\0';			/* Make the string ASCIZ so strlen can size it */

/*
 * Now, working backwards along the string, remove trailing spaces
 *
 */
    for (i=1; i < mod_len; i++)
      {
        if (model[mod_len - i] != ' ')		/* Is this a space ? */
            break;				/* Non-space - leave loop */
        model[mod_len - i] = '\0';		/* Terminate string at space */
          }

    mod_len = strlen( model );			/* Get the new length (as ASCII, not ASCIZ) */


/*
 * Now, add the device type and name for Dynamic Device Recognition
 *
 */
    status = ioc$add_device_type( model, mod_len, (UCB *) ucb, &dtn );
    return( SS$_NORMAL );			/* Return success to caller */

      }



/* ATAPI_PROCESS_SIZE - This routine is used to process the READ_CAPACITY
 *                      information returned by the ATAPI drive.
 *
 * Input:
 *      ucb     pointer to the UCB
 *
 * Output:
 *      none
 *
 * Status:
 *      SS$_NORMAL ---- Success
 *      SS$_IVBUFLEN -- Not a 512, 2048, or 2352 byte blocksize
 *
 *                                    
 * Notes:
 *
 *   1. Both values in the buffer are in big-endian format
 *   2. The value in the buffer is the *MAXIMUM* LBN, i.e., blocks-1
 *   3. The value in the buffer may be expressed in 2KB (not 512B) blocks
 *
 *
 */

int atapi_process_size( DQ_UCB *ucb )

  {

    BYTE   *sense_ptr;				/* Pointer to the ID page */
    int    blocks;				/* Number of blocks on the volume*/
    int    blocksize;				/* Bytes per block for the volume */


    sense_ptr = (BYTE *) ucb->ucb$ps_sense_buffer;
						/* Bind onto returned data as a byte array */

    blocks =   (   sense_ptr[0]<<24		/* Re-order the maxblock value */
                 | sense_ptr[1]<<16		/*   :   */
                 | sense_ptr[2]<<8		/*   :   */
                 | sense_ptr[3] )		/*   :   */
              + 1;				/* Account for blocks vs. max LBN */
                               
    blocksize = sense_ptr[4]<<24		/* And the blocksize value */
              | sense_ptr[5]<<16		/*   :   */
              | sense_ptr[6]<<8			/*   :   */
              | sense_ptr[7];			/*   :   */

    if (    (blocksize!=BLK_SIZE_512)		/* Do we recognize this blocksize? */
         && (blocksize!=BLK_SIZE_2048)		/* SCSI-3 spec'd CD-ROM? blocksize*/
         && (blocksize!=BLK_SIZE_2352) )	/* ATAPI tested CD-ROM blocksize? */
        return( SS$_IVBUFLEN );			/* If not, "Invalid buffer length" */

    ucb->ucb$l_2K_flag = 0;			/* Clear the 2K block flag */
    if (blocksize>=BLK_SIZE_2048)		/* CD-ROM-sized blocks? */
      {						/* If so, then... */
        ucb->ucb$l_2K_flag = 1;			/* Set the 2K block flag */
        ucb->ucb$r_dq_dt.ucb$l_maxbcnt = (MAX_ATAPI_2K_XFER * BLK_SIZE_512);
						/* And set the appropriate maximum transfer size */
        blocks = (blocks<<2);			/* And account for 4-to-1 packing */
          }
						/* Copy over the geometry information */
    ucb->ucb$r_dq_dt.ucb$l_maxblock = blocks;	/* Set maxblock */
    baseucb.ucb$b_sectors   = 8;		/* Dummy-up the sectors */
    baseucb.ucb$b_tracks    = 4;		/* Dummy-up the tracks */
    baseucb.ucb$w_cylinders = (blocks>>5)	/* Compute the cylinders (/32) */
                      + ( (blocks&0x1F) != 0 );	/* Did our division have a remainder? */
						/* If so, add another cylinder */
						/* This will get us to 1.05 GBs, then ucb$w_cylinders overflows */

    return( SS$_NORMAL );			/* Return succeeding */

      }



/* SET_GEOM - this routine is used to set the current geometry in the drive.
 *
 * Input:
 *      ucb     pointer to the UCB
 *
 * Output:
 *      none
 *
 * Status:
 *      SS$_NORMAL ---- success, geometry is ok
 *      SS$_BADPARAM -- geometry is incorrect
 *
 */

int set_geom( DQ_UCB *ucb )

  {

    int   sector;					/* Sector number */
    int   drv_head;					/* Drive drive/head register */
    int   cyl;						/* Cylinder number */
    int   status;					/* Status returned from routines */
    int   orig_ipl;					/* Original IPL */
    int   drvsts;					/* Drive status register */
    int   drverr;					/* Drive error register */

/* Attempt to read the maximum block */

    sector   = baseucb.ucb$b_sectors;			/* Use highest sector number */
    drv_head = ucb->ucb$l_drv_head+(baseucb.ucb$b_tracks-1);
							/* Use highest head number */
    cyl      = baseucb.ucb$w_cylinders - 1;		/* Use highest cylinder number */

/*
 * Take out the device lock and raise IPL
 * Write the registers
 * Then issue the command
 *
 */

    device_lock( baseucb.ucb$l_dlck, RAISE_IPL, &orig_ipl );
    out( ucb, WT_SEC_CNT, sector);			/* Set sectors/track */
    out( ucb, WT_DRV_HD, drv_head);			/* Set heads/cylinder */
    out( ucb, WT_CMD, CMD_ATA_INIT_DEV_PARAMS);		/* Issue the command */

    status = dq_wfikpch( ucb->ucb$ps_kpb, orig_ipl, 2 );/* Wait for the interrupt */
    if ( $FAIL( status ) )				/* Any error (timeout, etc.) ? */
        return( status );				/* If so, return with status */

    drvsts = inp( ucb, RD_ALT_STS );			/* Get the status byte */
    if ( IS_SET( drvsts, STS_M_ERR ) )			/* Any errors? */
      {							/* If so, then... */
        drverr = inp( ucb, RD_ERROR );			/* Get the error byte */
        BPTRACE( 0x04400000 );				/* BREAK: Drive error during set_geometry */
        return( SS$_IVADDR );				/*  and return an error */
          }

    if ( IS_CLEAR( drvsts, STS_M_DRDY ) )		/* If not READY */
      {
        BPTRACE( 0x04410000 );				/* BREAK: Drive not ready during set_geometry */
        return( SS$_DRVERR );				/*  return with DRIVE ERROR */
          }

    return( SS$_NORMAL );				/* Return with success */

      }



/* SET_FEATURES - This routine is used to set the current features in the drive.
 *
 * Input:
 *      ucb     pointer to the UCB
 *
 * Output:
 *      none
 *
 * Status:
 *      SS$_NORMAL ---- success, geometry is ok
 *      SS$_BADPARAM -- geometry is incorrect
 *
 */

int set_features( DQ_UCB *ucb, int feature, int value )

  {

    int   drvsts;					/* Drive status register */
    int   drverr;					/* Drive error register */
    int   orig_ipl;					/* Original IPL */
    int   status;					/* Status returned from routines */


    device_lock( baseucb.ucb$l_dlck, RAISE_IPL, &orig_ipl );
    wait_ready( ucb );					/* Make sure unit is selected and ready */
    out( ucb, WT_FEATURES, feature );			/* Select the specific feature to be set */
    out( ucb, WT_SEC_CNT, value );			/* Set specific value */
							/*   (A value is only meaningful for feature 0x03) */
    out( ucb, WT_CMD, CMD_ATA_SET_FEATURES );		/* Issue the command */

    status = dq_wfikpch( ucb->ucb$ps_kpb, orig_ipl, 3 );/* Wait for the interrupt */
    if ( $FAIL( status ) )				/* Any error (timeout, etc.) ? */
        return( status );				/* If so, return with status */

    drvsts = inp( ucb, RD_ALT_STS );			/* Get the status byte */
    if ( IS_SET( drvsts, STS_M_ERR ) )			/* Any errors? */
      {							/* If so, then... */
        drverr = inp( ucb, RD_ERROR );			/* Get the error byte */
        BPTRACE( 0x04080300 );				/* BREAK: Drive error during set_features */
        return( SS$_IVADDR );				/*  and return an error */
          }

    if ( IS_CLEAR( drvsts, STS_M_DRDY ) )		/* If not READY */
      {
        BPTRACE( 0x04080301 );				/* BREAK: Drive not ready during set_features */
        return( SS$_DRVERR );				/*  return with DRIVE ERROR */
          }

    return( SS$_NORMAL );				/* Return with success */

      }



/* SEEK - Perform Seek operation
 *
 * IDE drives will return immediately upon issuing the first seek
 * command.  Subsequent commands will actually wait until the seek
 * is completed.  This routine issues only one seek command and then
 * completes the I/O.  Any subsequent command will be stalled until
 * the seek is completed.
 *
 * Input:
 *      ucb     pointer to UCB
 *
 * Output:
 *      status value
 *              SS$_NORMAL -- seek complete
 *
 */

int seek( DQ_UCB *ucb )

  {

    int   status;				/* Status of calls */
    int   orig_ipl;				/* Original IPL */
    int   cyl;					/* Cylinder number */
    int   drvsts;				/* Drive status register */
    int   drverr;				/* Drive error register */

    status = wait_ready( ucb );			/* Wait for drive to be ready */
    if ( $FAIL( status ) )			/* Check the status for failure */
        return( status );			/* Return with error */

/* Set up seek parameters */

    cyl = ucb->ucb$l_media.lbn;			/* Get the cylinder number */

/*
 * Take out the device lock and raise IPL
 * Write the registers
 * Then issue the command
 *
 */

    device_lock( baseucb.ucb$l_dlck, RAISE_IPL, &orig_ipl );
    out( ucb, WT_DRV_HD, ucb->ucb$l_drv_head);	/* Select drive and head */
    out( ucb, WT_SECTOR, 1);			/* Put in the sector number */
    out( ucb, WT_CYL_LO, cyl);			/* Low order cylinder bits */
    out( ucb, WT_CYL_HI, cyl>>8);		/* High order cylinder bits */
    out( ucb, WT_CMD,    CMD_ATA_SEEK);		/* Attempt to seek to the sector */

    status = dq_wfikpch( ucb->ucb$ps_kpb, orig_ipl, 4 );
						/* Wait for the interrupt */
    if ( $FAIL( status ) )			/* Any error (timeout, etc.) ? */
        return( status );			/* If so, return with status */

    drvsts = inp( ucb, RD_ALT_STS );		/* Get the status byte */
    if ( IS_SET( drvsts, STS_M_ERR ) )		/* Any errors? */
      {						/* If so, then... */
        drverr = inp( ucb, RD_ERROR );		/* Get the error byte */
        BPTRACE( 0x04100000 );			/* BREAK: Drive error during seek */
        return( SS$_DRVERR );			/* Return with DRIVE ERROR status */
          }

    return( SS$_NORMAL );			/* Return with success */

      }



/* DRVCLR - Perform Drive Clear operation
 *
 * Input:
 *      ucb     pointer to UCB
 *
 * Output:
 *      status value
 *
 */

int drvclr( DQ_UCB *ucb )

  {

    return( SS$_NORMAL );

      }



/* READRCT - Perform READRCT operation
 *
 * This routine returns the drive information page.
 *
 * Input:
 *      ucb     pointer to UCB
 *
 * Output:
 *      none
 *
 * Return value:
 *      status value
 *              SS$_NORMAL -- success
 *              SS$_NODATA -- failed to get drive information
 *
 */

int readrct( DQ_UCB *ucb )

  {

    int   status;				/* Routine return status */
    int   orig_ipl;				/* Original IPL */
    int   drvsts;				/* Drive status register */
    int   drverr;				/* Drive error register */
    void  *temp;				/* Dummy for IOC$MOVTOUSER call */


    status = wait_ready( ucb );			/* Wait for drive ready */
    if ( $FAIL( status ) )			/* Check for error */
        return( status );			/*   and return if there is one */

    device_lock( baseucb.ucb$l_dlck, RAISE_IPL, &orig_ipl );
    out( ucb, WT_DRV_HD, ucb->ucb$l_drv_head );	/* Select drive and head 0 */
    out( ucb, WT_CMD, CMD_ATA_IDENTIFY_DEV );	/* Ask for drive info */

    status = dq_wfikpch( ucb->ucb$ps_kpb, orig_ipl, 5 );
						/* Wait for the interrupt */
    if ( $FAIL( status ) )			/* Any error (timeout, etc.) ? */
        return( status );			/* If so, return with status */

    drvsts = inp( ucb, RD_ALT_STS );		/* Get the status byte */
    if ( IS_SET( drvsts, STS_M_ERR) )		/* Any errors? */
      {						/* If so, then.. */
        drverr = inp( ucb, RD_ERROR );		/* Get the error byte */
        return( SS$_NODATA );			/* Exit with error */
          }

    move_sec_from_drive( ucb, (BYTE *) ucb->ucb$ps_xfer_buffer, BLK_SIZE_512 );
						/* Get the returned data from our drive */

    ioc_std$movtouser( ucb->ucb$ps_xfer_buffer, baseucb.ucb$l_bcnt, (UCB *) ucb, &temp );
						/* Move the data to the user */

    return( SS$_NORMAL );			/* Return with success */

      }

   


/* DIAGNOSE - Perform DIAGNOSE operation
 *
 * This routine implements pass-through of user formatted ATAPI commands
 * directly to the device, usually for audio function of CD-ROM players.
 *
 * Input:
 *      ucb     pointer to UCB
 *
 * Output:
 *      none
 *
 * Return value:
 *      status value
 *              SS$_NORMAL -- success
 *              SS$_NODATA -- failed to get drive information
 *
 */

int diagnose(DQ_UCB *ucb )
{
    BYTE  *packet;				/* The packet bytes within the UCB */
    int   *packetl;			       	/* The packet (as longwords) within the UCB */
    int    status;				/* Routine return status */
    int    xfer_cnt;				/* Count of blocks actually transferred (dummy here) */
    BYTE  *buffer;				/* Pointer to our transfer buffer */
    BYTE  *user_va;				/* Returned user buffer address */
    void *svaptr;

    packetl = (int *) ucb->ucb$b_packet;	/* Bind onto packet in the UCB as a longword array */
    packetl[0] = 0;				/* Clear the packet */
    packetl[1] = 0;				/*   :   */
    packetl[2] = 0;				/*   :   */

    packet = (BYTE *) ucb->ucb$b_packet;	/* Bind onto packet in the UCB as a byte array */
    memcpy(packet, ucb->diagnose_command, ucb->diagnose_command_length);
    buffer = (BYTE *) ucb->ucb$ps_xfer_buffer;	/* Initialize our buffer pointer */
    status = atapi_packet_command(ucb, buffer, baseucb.ucb$l_bcnt, &xfer_cnt, FALSE );

    if (status == SS$_NORMAL) {
        if (baseucb.ucb$l_bcnt > 0) {
            ioc_std$movtouser(buffer, xfer_cnt, (UCB *) ucb, &svaptr);
        }
    }

    ucb->ucb$l_bcr = baseucb.ucb$l_bcnt - xfer_cnt;

    return(status);
}
 


/* READ - Performs IO$_READxBLK driver function
 *
 * This routine issues READ commands to the drive.  This routine will
 * break up the request into segments of not more than 127 sectors per
 * command.      
 * 
 * Input:              
 *      ucb     pointer to UCB
 *
 * Output:
 *      status value
 *
 */

int read( DQ_UCB *ucb )

  {

    int   offset;				/* Offset within a possible 2K block */
    int   xfer_size;				/* Size (in sectors) */
    int   xfer_cnt;				/* Count of 512-byte blocks read in the latest segment*/
    int   blks_xfrd;				/* For-loop index */
    int   byte_cnt;				/* Number of bytes read */
    BYTE  *buffer;				/* Pointer to our transfer buffer */
    int   xfer_req;				/* Number of sectors requested */
    int   status;				/* Routine return status */
    int   retry_cnt;				/* Error retry count */
    int   buf_ofs;				/* Offset into user buffer */
    BYTE  *user_va;				/* Returned user buffer address */

    TRACE( 0x07000000 );			/* READ starting */

    status = wait_ready( ucb );			/* Wait for drive to be ready for a command */
    if ( $FAIL( status ) )			/* Check status for error */
        return( status );			/* Return with error */

						/* Compute number of blocks and set up */
    xfer_size = (baseucb.ucb$l_bcnt + BLK_SIZE_512 - 1) >> BLK_SHIFT;
    if (xfer_size == 0)				/* Was there any work to do ? */
        return( SS$_NORMAL );			/* Exit with success if not */
             
    buffer = (BYTE *) ucb->ucb$ps_xfer_buffer;	/* Initialize our buffer pointer */
    retry_cnt = 0;				/* Initialize the retry counter */

    for (blks_xfrd = 0; blks_xfrd < xfer_size;)	/* For each segment... */
      {

        xfer_req = xfer_size - blks_xfrd;	/* Compute 512-byte blocks left to be read */

/* Later, for unbuffered DMA, set up the map registers here */

        if (ucb->ucb$l_2K_flag)			/* A 2KB sector device? */
             offset = ucb->ucb$l_media.lbn & 0x03;/* Calculate offset within our transfer buffer */
           else					/* Else if no... */
             offset = 0;			/* No offset required */

        status = read_dispatcher( ucb, xfer_req, &xfer_cnt );
						/* Read this segment */
        if ( $FAIL( status ) )			/* How did that segment go? */
          {

            if (    (ucb->ucb$l_asc==0x04)	/* "Logical unit is in process of becoming ready" */
                 && (ucb->ucb$l_ascq==0x01) )	/*   :   */
                sleep( ucb, 10 );		/* Hang out for 10 seconds */

            if (ucb->ucb$l_asc==0x30)		/* Incompatible medium in drive */
                return( status );		/* Not much point in re-trying */

            if (ucb->ucb$l_asc==0x3A)		/* No medium in drive */
                return( status );		/* Not much point in re-trying */

            if (status==SS$_BADPARAM)		/* Bad parameter (e.g., LBN out of range)? */
                return( status );		/* We won't retry that either */

            if (status==SS$_VOLINV)		/* Did the volume go invalid? */
                return( status );		/* We won't retry that either */
						/* (A retry might erroneously succeed!) */

            if (xfer_cnt == 0)
              {
                retry_cnt++;			/* Update retry count */
                if (retry_cnt == MAX_RETRY/2)	/* Halfway through the retries? */
                  {
                    BPTRACE( 0x07010000 );	/* BREAK: read wants to do a reset */
                    reset_ctrl(ucb);		/* If so, reset things */
                      }
                if (retry_cnt > MAX_RETRY)	/* Were there too many retries yet? */
                    return( status );		/* Yes, exit with error */
                  }
              }      
        else
          {
            if (xfer_cnt > 0)			/* Was any data transferred ? */
                retry_cnt = 0;			/* Clear retry count on each success */
              }

        if (xfer_cnt == 0)			/* Check that we got something */
            continue;				/* Next retry */

/* Later, for unbuffered DMA, skip this block */

						/* This segment is now in the transfer buffer. */
						/* Move the data segment to the user */
        byte_cnt = xfer_cnt << BLK_SHIFT;	/* Calculate the byte count for this segment */
        if (byte_cnt > ucb->ucb$l_bcr)		/* Check if the transfer exceeded the user's desired bytecount */
            byte_cnt = ucb->ucb$l_bcr;		/* If so, minimize it to the user's actual request */
        buf_ofs = (ucb->ucb$l_media.lbn - ucb->ucb$l_org_media) * BLK_SIZE_512;
						/* Calculate the offset (so far) into the user's buffer */
        user_va = map_user_buffer( ucb, buf_ofs, byte_cnt );
						/* Map that part of the user buffer */
        TRACE( 0x07020000 + byte_cnt );		/* READ moving bytes to the user */
        memcpy( user_va, &buffer[offset*BLK_SIZE_512], byte_cnt );
						/* And copy our data to the user buffer */

        ucb->ucb$l_bcr -= byte_cnt;		/* Update the byte count remaining */
        ucb->ucb$l_media.lbn += xfer_cnt;	/* Update the LBN */
        blks_xfrd += xfer_cnt;			/* Update the for-loop index */

          }					/* Next segment */

    return( SS$_NORMAL );			/* Return with success */

      }



/* READ_DISPATCHER - Figure out which routine to use for this segment
 *
 * We have (up to) six different handlers for doing the actual reads.
 * Based on three flags, dispatch to one of them.
 *
 * Input:
 *      ucb           pointer to UCB
 *      xfer_req      number of blocks remaining to transfer
 *
 * Output:
 *      xfer_cnt      count of blocks actually transferred
 *      status value
 *
 * Note:
 *
 *   You can modify this routine to select, on a case-by-case basis,
 *   which transfers will be done using DMA.
 *
 */

int read_dispatcher( DQ_UCB *ucb, int xfer_req, int *xfer_cnt )

  {

    int dispatch;


    dispatch = (ucb->ucb$l_atapi_flag << 2)		/* Decide which routine to use */
             + (ucb->ucb$l_2K_flag    << 1)		/*   :   */
             + (ucb->ucb$l_drive_dma_capable    );	/*   :   */

    switch (dispatch)					/* Switch to the appropriate handler */
      {

        case (0x0):
          return read_ata_seg_pio(   ucb, xfer_req, xfer_cnt );		/* ATA, 512-byte sectors, via PIO */

        case (0x1):
          return read_ata_seg_dma(   ucb, xfer_req, xfer_cnt );		/* ATA, 512-byte sectors, via DMA */

        case (0x4):
          return read_atapi_512_seg( ucb, xfer_req, xfer_cnt, FALSE );	/* ATAPI, 512-byte sectors, via PIO */

        case (0x5):
          return read_atapi_512_seg( ucb, xfer_req, xfer_cnt, TRUE );	/* ATAPI, 512-byte sectors, via DMA */

        case (0x6):
          return read_atapi_2K_seg(  ucb, xfer_req, xfer_cnt, FALSE );	/* ATAPI, 2KB sectors, via PIO */

        case (0x7):
          return read_atapi_2K_seg(  ucb, xfer_req, xfer_cnt, TRUE );	/* ATAPI, 2KB sectors, via DMA */

        default:							/* Unexpected case */
          break;							/* Fall into the bugcheck... */

          }

    //    bug_check( INCONSTATE, FATAL, COLD );				/* So be it */
    panic("INCONSTATE, FATAL, COLD\n");
    return( SS$_ABORT );						/* (You should live so long as to get here) */

      }



/* READ_ATA_SEG_PIO - Read one segment from an ATA drive using PIO
 *
 * This routine performs the read of a single I/O segment.
 * Each segment is a single read command of not more the MAX_ATA_XFER
 * sectors. The overall read I/O routine calls this routine for each
 * of the segments until the entire read is completed.
 *
 * Input:
 *      ucb           pointer to UCB
 *      xfer_req      number of blocks remaining to transfer
 *
 * Output:
 *      xfer_cnt      count of blocks actually transferred
 *      status value
 *
 * Note:
 *                      
 *   o Some drives sometimes give the interrupt *VERY* quickly,
 *     before I can get back to the WFIKPCH. (This probably occurs
 *     when cached data is available in the drive.) I handle this
 *     by caching the fact that an as-yet-unsolicited interrupt
 *     occurred.
 *
 */

int read_ata_seg_pio( DQ_UCB *ucb, int xfer_req, int *xfer_cnt )

  {

    int   sec;					/* Disk location (sector) */
    int   head;					/* Disk location (head) */
    BYTE *buffer;				/* Pointer to our transfer buffer */
    int   cyl;					/* Disk location (cyl) */
    int   drv_head;				/* Drive drive/head register */
    int   status;				/* Returned status from routine */
    int   orig_ipl;				/* Saved IPL */
    int   drvsts;				/* Drive status register */
    int   drverr;				/* Drive error register */

    TRACE( 0x07100000 + xfer_req );		/* READ_ATA_SEG_PIO starting */
    ucb->ucb$l_unsolicited_int = 0;		/* Forget any pending unsolicited interrupts */

    if (xfer_req > MAX_ATA_XFER)		/* Check for too large a request*/
        xfer_req = MAX_ATA_XFER;		/*  and minimize if too big */

    buffer = (BYTE *) ucb->ucb$ps_xfer_buffer;	/* Initialize our buffer pointer */
    *xfer_cnt = 0;				/* Clear count of blocks read */
    drv_head= ucb->ucb$l_drv_head;		/* Get drive info */
    if (ucb->ucb$l_drive_lba_capable)		/* If LBA mode, ... */
        drv_head |= DRVHD_M_LBA;		/* Set the LBA bit */
    compute_address( ucb, &sec, &head, &cyl );	/* Compute physical address */

/*
 * Take out the device lock and raise IPL
 * Write the registers
 * Then issue the command
 *
 */

    device_lock( baseucb.ucb$l_dlck, RAISE_IPL, &orig_ipl );
						/* Take out the device lock for the first sector */
    out( ucb, WT_DRV_HD, drv_head|head );	/* Select drive and head */
    out( ucb, WT_SEC_CNT, xfer_req );		/* Ask for "n" sectors */
    out( ucb, WT_SECTOR, sec );			/* Put in the sector number */
    out( ucb, WT_CYL_LO, cyl );			/* Low order cylinder bits */
    out( ucb, WT_CYL_HI, cyl>>8 );		/* High order cylinder bits */
    out( ucb, WT_CMD, CMD_ATA_READ_SECS );	/* Attempt to read the sector */

    for (;;)					/* Do forever (for each sector in the transfer request)... */

      {

        status = dq_wfikpch( ucb->ucb$ps_kpb, orig_ipl, 6 );
						/* Wait for the interrupt */
        if ( $FAIL( status ) )			/* Any error (timeout, etc.) ? */
            return( status );			/* If so, return with status */

        drvsts = inp( ucb, RD_ALT_STS );	/* Get the status byte */
        if ( IS_SET( drvsts, STS_M_ERR ) )	/* Check the status */
          {					/* If any errors, then... */
            drverr = inp( ucb, RD_ERROR );	/* Get the error byte */
            BPTRACE( 0x07110000 );		/* BREAK: Drive error during READ_ATA_SEG_PIO */
            return( SS$_DRVERR );		/* Return with error status */
              }

        move_sec_from_drive( ucb, &buffer[(*xfer_cnt)*BLK_SIZE_512], BLK_SIZE_512 );
						/* Move the sector from the drive to our transfer buffer */
        *xfer_cnt += 1;				/* Count a block read */

        if (*xfer_cnt >= xfer_req )		/* Finished? */
            break;				/* If so, break out of the do-forever loop */

        device_lock( baseucb.ucb$l_dlck, RAISE_IPL, &orig_ipl );
						/* Else take out the deviced lock and go 'round again */

          }					/* Next sector in the do-forever loop */

    return( SS$_NORMAL );			/* Return to caller succeeding */

      }



/* READ_ATA_SEG_DMA - Read one segment from an ATA drive using DMA
 *
 * This routine performs the read of a single I/O segment.
 * Each segment is a single read command of not more the MAX_ATA_XFER
 * sectors. The overall read I/O routine calls this routine for each
 * of the segments until the entire read is completed.
 *
 * Input:
 *      ucb           pointer to UCB
 *      xfer_req      number of blocks remaining to transfer
 *
 * Output:
 *      xfer_cnt      count of blocks actually transferred
 *      status value
 *
 * Note:
 *
 *   o Some drives sometimes give the interrupt *VERY* quickly,
 *     before I can get back to the WFIKPCH. (This probably occurs
 *     when cached data is available in the drive.) I handle this
 *     by caching the fact that an as-yet-unsolicited interrupt
 *     occurred.
 *
 */

int read_ata_seg_dma( DQ_UCB *ucb, int xfer_req, int *xfer_cnt )

  {

    int   sec;					/* Disk location (sector) */
    int   head;					/* Disk location (head) */
    BYTE *buffer;				/* Pointer to our transfer buffer */
    int   cyl;					/* Disk location (cyl) */
    int   drv_head;				/* Drive drive/head register */
    int   status;				/* Returned status from routine */
    int   orig_ipl;				/* Saved IPL */
    int   drvsts;				/* Drive status register */
    int   drverr;				/* Drive error register */


    TRACE( 0x07800000 + xfer_req );		/* READ_ATA_SEG_DMA starting */
    ucb->ucb$l_unsolicited_int = 0;		/* Forget any pending unsolicited interrupts */
    *xfer_cnt = 0;				/* Consider none of the blocks read */

    if (xfer_req > MAX_ATA_XFER)		/* Check for too large a request*/
        xfer_req = MAX_ATA_XFER;		/*  and minimize if too big */

    buffer = (BYTE *) ucb->ucb$ps_xfer_buffer;	/* Initialize our buffer pointer */
    drv_head= ucb->ucb$l_drv_head;		/* Get drive info */
    if (ucb->ucb$l_drive_lba_capable)		/* If LBA mode, ... */
        drv_head |= DRVHD_M_LBA;		/* Set the LBA bit */
    compute_address( ucb, &sec, &head, &cyl );	/* Compute physical address */

    load_prdt( ucb );				/* Load the PRDT */

    device_lock( baseucb.ucb$l_dlck, RAISE_IPL, &orig_ipl );
						/* Take out the device lock so we can write the registers */

    out( ucb, WT_DMA_CMD, DMA_CMD_M_INBOUND | DMA_CMD_M_INACTIVE );
						/* Make sure the DMA controller is inbound     */
						/*   (that is, reading disk -> writing memory) */
						/*   but not active yet                        */
    out( ucb, WT_DMA_AD0, ( ( (UINT) ucb->ucb$l_prdt_phy )      ) & 0xFF );
    out( ucb, WT_DMA_AD1, ( ( (UINT) ucb->ucb$l_prdt_phy ) >> 8 ) & 0xFF );
    out( ucb, WT_DMA_AD2, ( ( (UINT) ucb->ucb$l_prdt_phy ) >>16 ) & 0xFF );
    out( ucb, WT_DMA_AD3, ( ( (UINT) ucb->ucb$l_prdt_phy ) >>24 ) & 0xFF );
						/* Point the controller to the PCI address of our PRDT table */
    out( ucb, WT_DMA_STS, DMA_STS_M_DRV1 | DMA_STS_M_DRV0 | DMA_STS_M_INT | DMA_STS_M_ERR );
						/* For now, set both drives as DMA-capable */
						/* Write "1"s to INT and ERR to clear them in case they're set */
    out( ucb, WT_DRV_HD, drv_head|head );	/* Select drive and head */
    out( ucb, WT_SEC_CNT, xfer_req );		/* Ask for "n" sectors */
    out( ucb, WT_SECTOR, sec );			/* Put in the sector number */
    out( ucb, WT_CYL_LO, cyl );			/* Low order cylinder bits */
    out( ucb, WT_CYL_HI, cyl>>8 );		/* High order cylinder bits */
    out( ucb, WT_CMD, CMD_ATA_READ_DMA );	/* Attempt to read the sector(s) */
    out( ucb, WT_DMA_CMD, DMA_CMD_M_INBOUND | DMA_CMD_M_ACTIVE );
						/* Set the DMA controller inbound              */
						/*   (that is, reading disk -> writing memory) */
						/*   and active                                */

    status = dq_wfikpch( ucb->ucb$ps_kpb, orig_ipl, 7 );
						/* Wait for the interrupt */
    if ( $FAIL( status ) )			/* Any error (timeout, etc.) ? */
        return( status );			/* If so, return with status */

    drvsts = inp( ucb, RD_ALT_STS );		/* Get the status byte */

    if ( IS_SET( drvsts, STS_M_ERR ) )		/* Check the status (saved from above) */
      {						/* If any errors, then... */
        drverr = inp( ucb, RD_ERROR );		/* Get the error byte */
        BPTRACE( 0x07810000 );			/* BREAK: Drive error during READ_ATA_SEG_DMA */
        return( SS$_DRVERR );			/* Return with error status */
          }

    device_lock( baseucb.ucb$l_dlck, RAISE_IPL, &orig_ipl );
						/* Take out the deviced lock */
    out( ucb, WT_DMA_CMD, DMA_CMD_M_INACTIVE );	/* Set the DMA controller inactive */
    device_unlock( baseucb.ucb$l_dlck, orig_ipl, SMP_RESTORE );
						/* Return the deviced lock */

    *xfer_cnt = xfer_req;			/* Consider all of the blocks read */

    return( SS$_NORMAL );			/* Return to caller succeeding */

      }



/* READ_ATAPI_512_SEG_PIO - Read one segment from a 512-byte-sector ATAPI drive using PIO
 *
 * This routine performs the read of a single I/O segment.
 * Each segment is a single read command of not more the MAX_ATAPI_512_XFER
 * sectors. The overall read I/O routine calls this routine for each
 * of the segments until the entire read is completed.
 *
 * Input:
 *      ucb           pointer to UCB
 *      xfer_req      number of blocks remaining to transfer
 *      dma_flag      whether or not to use DMA
 *           
 *
 * Output:
 *      xfer_cnt      count of blocks actually transferred
 *      status value
 *
 */

int read_atapi_512_seg( DQ_UCB *ucb, int xfer_req, int *xfer_cnt, int dma_flag )

  {

    BYTE *buffer;				/* Pointer to our transfer buffer */
    int   offset;				/* Offset within 2K block */
    BYTE  *packet;				/* The packet bytes within the UCB */
    int   *packetl;				/* The packet (as longwords) within the UCB */
    int   status;				/* Routine status value */
    int   orig_ipl;				/* Originial IPL */
             

    TRACE( 0x07200000 + xfer_req );		/* READ_ATAPI_512_SEG_PIO starting */

    if (xfer_req > MAX_ATAPI_512_XFER)		/* Check for too large a transfer */
        xfer_req = MAX_ATAPI_512_XFER;		/*  and limit it if so */

    buffer = (BYTE *) ucb->ucb$ps_xfer_buffer;	/* Initialize our buffer pointer */

    packetl = (int *) ucb->ucb$b_packet;	/* Bind onto packet in the UCB as a longword array */
    packetl[0] = 0;				/* Clear the packet */
    packetl[1] = 0;				/*   :   */
    packetl[2] = 0;				/*   :   */

    packet  = (BYTE *) ucb->ucb$b_packet;	/* Bind onto packet in the UCB as a byte array */
    packet[0]  = CMD_ATAPI_READ_12;		/* Read(12) command */
/*  packet[1]  = 0x00;				/@ (Reserved) */
/*  packet[2]  = 0x00;				/@ Address MSB (filled in momentarily) */
/*  packet[3]  = 0x00;				/@   :      :   */
/*  packet[4]  = 0x00;				/@   :      :   */
/*  packet[5]  = 0x00;				/@   :     LSB  */
    packet[6]  = xfer_req>>24;			/* Transfer length MSB in blocks */
    packet[7]  = xfer_req>>16;			/*   :              :  */
    packet[8]  = xfer_req>>8;			/*   :              :  */
    packet[9]  = xfer_req;			/*   :             LSB */
/*  packet[10] = 0x00;				/@ (Reserved) */
/*  packet[11] = 0x00;				/@ (Reserved) */
    offset     = fill_packet_w_adx( ucb );	/* Fill the packet address cells*/

    status = atapi_packet_command( ucb, buffer, xfer_req, xfer_cnt, dma_flag );
						/* Do the common packet-command processing using appropriate mode */

    if (dma_flag)				/* Using DMA? */
      {						/* If so, then... */
        device_lock( baseucb.ucb$l_dlck, RAISE_IPL, &orig_ipl );
						/* Take out the deviced lock */
        out( ucb, WT_DMA_CMD, DMA_CMD_M_INACTIVE ); /* Set the DMA controller inactive */
        device_unlock( baseucb.ucb$l_dlck, orig_ipl, SMP_RESTORE );
          }					/* Return the deviced lock */

    if (status == SS$_DRVERR)			/* Did it result in a drive error? */
      {						/* If so, then... */
        atapi_request_sense( ucb, (BYTE *) ucb->ucb$ps_sense_buffer );
						/* Get error info from drive */
        status = atapi_xlate_error_to_vms( ucb );
						/* Turn it into a VMS error code */
        return( status );			/* and exit */
          }

    if ( $FAIL( status ) )			/* Check for any other error */
        return( status );			/*  and exit if so */

						/* All looks okay */
    return( SS$_NORMAL );			/* Return to caller */

      }



/* READ_ATAPI_2K_SEG - Read one segment from a 2Kbyte-sector ATAPI drive
 *
 * This routine performs the read of a single I/O segment.
 * Each segment is a single read command of not more the MAX_ATAPI_2K_XFER
 * sectors. The overall read I/O routine calls this routine for each
 * of the segments until the entire read is completed.
 *
 * Input:
 *      ucb           pointer to UCB
 *      xfer_req      number of blocks remaining to transfer
 *      dma_flag      whether or not to use DMA
 *
 * Output:
 *      xfer_cnt      count of blocks actually transferred
 *      status value
 *
 */

int read_atapi_2K_seg( DQ_UCB *ucb, int xfer_req, int *xfer_cnt, int dma_flag )

  {

    BYTE *buffer;				/* Pointer to our transfer buffer */
    int   offset;				/* Offset within 2K block */
    BYTE  *packet;				/* The packet bytes within the UCB */
    int   *packetl;				/* The packet (as longwords) within the UCB */
    int   status;				/* Routine status value */
    int   orig_ipl;				/* Original IPL */
    int   xfer_req_2K;				/* Number of 2Kbyte sectors to transfer */


    TRACE( 0x07300000 + xfer_req );		/* READ_ATAPI_2K_SEG starting */

    if (xfer_req > MAX_ATAPI_2K_XFER)		/* Check for too large a transfer */
        xfer_req = MAX_ATAPI_2K_XFER;		/*  and limit it if so */

    buffer = (BYTE *) ucb->ucb$ps_xfer_buffer;	/* Initialize our buffer pointer */

    packetl = (int *) ucb->ucb$b_packet;	/* Bind onto packet in the UCB as a longword array */
    packetl[0] = 0;				/* Clear the packet */
    packetl[1] = 0;				/*   :   */
    packetl[2] = 0;				/*   :   */

    packet  = (BYTE *) ucb->ucb$b_packet;	/* Bind onto packet in the UCB as a byte array */

    offset     = fill_packet_w_adx( ucb );	/* Fill the packet address cells and calculate the offset */

    xfer_req    = xfer_req + offset - 1;	/* Maximize the transfer so it spans any 2K-byte sectors needed */
    xfer_req_2K = (xfer_req >> 2) + 1;		/* Divide the transfer request by 4 to account for 2K blocks */
						/*   and add 1 */
    xfer_req    = xfer_req_2K<<2;		/* Now expand the 512-byte-block-oriented xfer_req to encompass */
						/*   all of the 2K-byte block(s) to be transferred              */

    packet[0]  = CMD_ATAPI_READ_12;		/* Read(12) command */
/*  packet[1]  = 0x00;				/@ (Reserved) */
/*  packet[2]  = 0x00;				/@ Address MSB (already filled-in) */
/*  packet[3]  = 0x00;				/@   :      :   */
/*  packet[4]  = 0x00;				/@   :      :   */
/*  packet[5]  = 0x00;				/@   :     LSB  */
    packet[6]  = xfer_req_2K>>24;		/* Transfer length MSB in 2Kbyte sectors */
    packet[7]  = xfer_req_2K>>16;		/*   :              :  */
    packet[8]  = xfer_req_2K>>8;		/*   :              :  */
    packet[9]  = xfer_req_2K;			/*   :             LSB */
/*  packet[10] = 0x00;				/@ (Reserved) */
/*  packet[11] = 0x00;				/@ (Reserved) */


    status = atapi_packet_command( ucb, buffer, xfer_req, xfer_cnt, dma_flag );
						/* Do the common packet-command processing using desired mode */

    if (dma_flag)				/* Using DMA? */
      {						/* If so, then... */
        device_lock( baseucb.ucb$l_dlck, RAISE_IPL, &orig_ipl );
	     					/* Take out the deviced lock */
        out( ucb, WT_DMA_CMD, DMA_CMD_M_INACTIVE ); /* Set the DMA controller inactive */
        device_unlock( baseucb.ucb$l_dlck, orig_ipl, SMP_RESTORE );
          }					/* Return the deviced lock */

    if (status == SS$_DRVERR)			/* Did it result in a drive error? */
      {						/* If so, then... */
        atapi_request_sense( ucb, (BYTE *) ucb->ucb$ps_sense_buffer );
						/* Get error info from drive */
        status = atapi_xlate_error_to_vms( ucb );
						/* Turn it into a VMS error code */
        return( status );			/* and exit */
          }

    if ( $FAIL( status ) )			/* Check for any other error */
        return( status );			/*  and exit if so */


    return( SS$_NORMAL );			/* Return to caller */

      }



/* DATACHECK - Performs data check function for read and write
 * requests.
 *
 * Input:
 *      ucb     pointer to UCB
 *
 * Output:
 *      status value
 *              SS$_NORMAL ----- success
 *              SS$_DATACHECK -- data failed to compare
 *
 */

int datacheck( DQ_UCB *ucb )

  {

    int   offset;				/* Offset within a possible 2K block */
    int   xfer_size;				/* Size (in sectors) */
    int   xfer_cnt;				/* Count of blocks read (to be compared) in latest segment */
    int   blks_xfrd;				/* For-loop index */
    BYTE  *buffer;				/* Pointer to our transfer buffer */
    int   byte_cnt;				/* Number of bytes compared */
    int   xfer_req;				/* Number of sectors requested */
    int   status;				/* Routine return status */
    int   retry_cnt;				/* Error retry count */
    int   buf_ofs;				/* Offset into user buffer */
    BYTE  *user_va;				/* Returned user buffer address */


    TRACE( 0x08000000 );			/* DATACHECK starting */

    ucb->ucb$l_bcr = ucb->ucb$l_org_bcnt;	/* Get byte count */
    ucb->ucb$l_media.lbn = ucb->ucb$l_org_media;/* Get first block number */
    baseucb.ucb$l_bcnt = ucb->ucb$l_org_bcnt;	/* Get byte count */
    baseucb.ucb$l_svapte = ucb->ucb$l_org_svapte;/* Get SVAPTE */
    baseucb.ucb$l_boff = ucb->ucb$l_org_boff;	/* Get BOFF */

    status = wait_ready( ucb );			/* Wait for drive to be ready for a command */
    if ( $FAIL( status ) )			/* Check return status */
        return( status );			/*  and exit on error */

						/* Compute number of blocks and set up */
    xfer_size = (baseucb.ucb$l_bcnt + BLK_SIZE_512 - 1) >> BLK_SHIFT;
    if (xfer_size == 0)				/* Was there any work to do ? */
        return( SS$_NORMAL );			/* Exit with success if not */

    buffer = (BYTE *) ucb->ucb$ps_xfer_buffer;	/* Initialize our buffer pointer */
    retry_cnt = 0;				/* Initialize the retry counter */

    for (blks_xfrd = 0; blks_xfrd < xfer_size;)	/* For each segment... */
      {
        xfer_req = xfer_size - blks_xfrd;	/* Compute 512-byte blocks left to be read */

        if (ucb->ucb$l_2K_flag)			/* A 2KB sector device? */
             offset = ucb->ucb$l_media.lbn & 0x03;/* Calculate offset within our transfer buffer */
           else					/* Else if no... */
             offset = 0;			/* No offset required */

        status = read_dispatcher( ucb, xfer_req, &xfer_cnt );
						/* Read this segment */

        if ( $FAIL( status ) )			/* How did that segment go? */
          {
            if (xfer_cnt == 0)			/* If no data was transferred... */
              {
                retry_cnt++;			/* Update retry count */
                if (retry_cnt == MAX_RETRY/2)	/* Halfway through the retries? */
                  {
                    BPTRACE( 0x08010000 );	/* BREAK: datacheck wants to do reset */
                    reset_ctrl( ucb );		/* If so, reset things */
                      }
                if (retry_cnt > MAX_RETRY)	/* Were there too many retries yet? */
                    return( status );		/* Yes, exit with error */
                  }
              }
        else
          {
            if (xfer_cnt > 0)			/* Was any data transferred ? */
                retry_cnt = 0;			/* Clear retry count on each success */
              }
						/* This segment is now in our transfer buffer */
						/* Do the data comparison */

        byte_cnt = xfer_cnt << BLK_SHIFT;	/* Compute byte count */
        if (byte_cnt > ucb->ucb$l_bcr)		/* Check if the transfer exceeded the user's desired bytecount */
            byte_cnt = ucb->ucb$l_bcr;		/* If so, minimize it to the user's actual request */

        buf_ofs = (ucb->ucb$l_media.lbn - ucb->ucb$l_org_media) * BLK_SIZE_512;
						/* Calculate the offset (so far) into the user's buffer */
        user_va = map_user_buffer( ucb, buf_ofs, byte_cnt );
						/* Map that part of the user buffer */
        TRACE( 0x08020000 + byte_cnt );		/* DATACHECK comparing bytes with the user */
        status = memcmp( &buffer[offset*BLK_SIZE_512], user_va, byte_cnt );
						/* And compare user buffer and our transfer buffer */
        if (status != 0)			/* Check comparison results */
            return( SS$_DATACHECK );		/* Failed - return DATACHECK error */

        ucb->ucb$l_bcr -= byte_cnt;		/* Update the byte count remaining */
        ucb->ucb$l_media.lbn += xfer_cnt;	/* Bump the LBN */
        blks_xfrd += xfer_cnt;			/* Update the for-loop index */

          }					/* Next segment */

    return( SS$_NORMAL );			/* Return with success */

      }



/* WRITE - Performs IO$_WRITExBLK driver functions
 *
 * Input:
 *      ucb     pointer to UCB
 *
 * Output:
 *      status value
 *
 */

int write( DQ_UCB *ucb )

  {

    int   byte_cnt;				/* Number of bytes written */
    int   xfer_size;				/* Size (in sectors) */
    int   xfer_cnt;				/* Count of blocks actually written in latest segment */
    int   blks_xfrd;				/* For-loop index */
    int   xfer_req;				/* Number of sectors requested */
    int   status;				/* Routine status */
    int   retry_cnt;				/* Retry counter */

    TRACE( 0x09000000 );			/* WRITE starting */

    status = wait_ready( ucb );			/* Wait for drive to be ready for a command */
    if ( $FAIL( status ) )			/* Check the error */
        return( status );			/*  if an error, then return */


						/* Compute number of blocks and set up */
    xfer_size = (baseucb.ucb$l_bcnt + BLK_SIZE_512 - 1) >> BLK_SHIFT;
    if (xfer_size == 0)				/* Was there any work */
        return( SS$_NORMAL );			/* Exit with success if not */

    retry_cnt = 0;				/* Initialize the retry counter */

    for (blks_xfrd = 0; blks_xfrd < xfer_size;)	/* For each segment... */
      {
        xfer_req = xfer_size - blks_xfrd;	/* Compute 512-byte blocks left to be written */

        status = write_dispatcher( ucb, xfer_req, &xfer_cnt );

        if ( $FAIL( status ) )			/* How did that segment go? */
          {

            if (ucb->ucb$l_asc==0x30)		/* Incompatible medium in drive */
                return( status );		/* Not much point in re-trying */

            if (ucb->ucb$l_asc==0x3A)		/* No medium in drive */
                return( status );		/* Not much point in re-trying */

            if (status==SS$_BADPARAM)		/* Bad parameter (e.g., LBN out of range)? */
                return( status );		/* We won't retry that either */

            if (status==SS$_VOLINV)		/* Did the volume go invalid? */
                return( status );		/* We won't retry that either */
						/* (A retry might erroneously succeed!) */

            if (status==SS$_WRITLCK)		/* Medium appers to be write-locked */
                return( status );		/* Not much point in re-trying */

            if (xfer_cnt == 0)			/* If no data was transfered... */
              {
                retry_cnt++;			/* Update retry count */
                if (retry_cnt == MAX_RETRY/2)	/* Halfway through the retries? */
                  {
                    BPTRACE( 0x09010000 );	/* BREAK: write wants to do reset */
                    reset_ctrl( ucb );		/* If so, reset things */
                      }
                if (retry_cnt > MAX_RETRY)	/* Were there too many retries yet? */
                    return( status );		/* Yes, exit with error */
                  }
              }
        else         
          {
            if (xfer_cnt > 0)			/* Was any data transferred ? */
                retry_cnt = 0;			/* Clear retry count on each success */
              }

        byte_cnt = xfer_cnt << BLK_SHIFT;	/* Compute byte count */
        if (byte_cnt > ucb->ucb$l_bcr)		/* Check if the transfer exceeded the user's desired bytecount */
            byte_cnt = ucb->ucb$l_bcr;		/* If so, minimize it to the user's actual request */

        ucb->ucb$l_bcr -= byte_cnt;		/* Update byte count remaining */
        ucb->ucb$l_media.lbn += xfer_cnt;	/* Update the LBN */
        blks_xfrd += xfer_cnt;			/* Update the for-loop index */
          }

    return( SS$_NORMAL );				/* Return to caller with success */

      }



/* WRITE_DISPATCHER - Figure out which routine to use for this segment
 *
 * We have (up to) six different handlers for doing the actual writes.
 * Based on three flags, dispatch to one of them.
 *
 * Input:
 *      ucb           pointer to UCB
 *      xfer_req      number of blocks remaining to transfer
 *
 * Output:
 *      xfer_cnt      count of blocks actually transferred
 *      status value
 *
 * Note:
 *
 *   You can modify this routine to select, on a case-by-case basis,
 *   which transfers will be done using DMA.
 *
 */

int write_dispatcher( DQ_UCB *ucb, int xfer_req, int *xfer_cnt )

  {

    int dispatch;


    dispatch = (ucb->ucb$l_atapi_flag    << 2)		/* Decide which routine to use */
             + (ucb->ucb$l_2K_flag       << 1)		/*   :   */
             + (ucb->ucb$l_drive_dma_capable );		/*   :   */

    switch (dispatch)					/* Switch to the appropriate handler */
      {

        case (0x0):
          return write_ata_seg_pio(   ucb, xfer_req, xfer_cnt );	/* ATA, 512-byte sectors, via PIO */

        case (0x1):
          return write_ata_seg_dma(   ucb, xfer_req, xfer_cnt );	/* ATA, 512-byte sectors, via DMA */

        case (0x4):
          return write_atapi_512_seg( ucb, xfer_req, xfer_cnt, FALSE );	/* ATAPI, 512-byte sectors, via PIO */

        case (0x5):
          return write_atapi_512_seg( ucb, xfer_req, xfer_cnt, TRUE );	/* ATAPI, 512-byte sectors, via DMA */

        case (0x6):
          return write_atapi_2K_seg(  ucb, xfer_req, xfer_cnt, FALSE );	/* ATAPI, 2KB sectors, via PIO */

        case (0x7):
          return write_atapi_2K_seg(  ucb, xfer_req, xfer_cnt, TRUE );	/* ATAPI, 2KB sectors, via DMA */

        default:							/* Unexpected case */
          break;							/* Fall into the bugcheck... */

          }

    //bug_check( INCONSTATE, FATAL, COLD );				/* So be it */
    panic(" INCONSTATE, FATAL, COLD \n");				/* So be it */
    return( SS$_ABORT );						/* (You should live so long as to get here) */

      }



/* WRITE_ATA_SEG_PIO - Write one segment to an ATA drive using PIO
 *
 * This routine performs the write of a single I/O segment.  Each
 * segment is a single read command of not more the MAX_ATA_XFER sectors.
 * The overall read I/O routine calls this routine for each of the
 * segments until the entire read is completed.
 *
 * Input:
 *      ucb             pointer to UCB
 *      xfer_req        number of blocks remaining to transfer
 *
 * Output:
 *      xfer_cnt        count of blocks actually transferred
 *      status value
 *
 * Note:
 *
 *   o Some drives sometimes give the interrupt *VERY* quickly,
 *     before I can get back to the WFIKPCH. (This probably occurs
 *     when cached data is available in the drive.) I handle this
 *     by caching the fact that an as-yet-unsolicited interrupt
 *     occurred.
 *
 */

int write_ata_seg_pio( DQ_UCB *ucb, int xfer_req, int *xfer_cnt )

  {

    int   buf_ofs;				/* Offset into user buffer */
    BYTE *buffer;				/* Pointer to our transfer buffer */
    int   byte_cnt;				/* Count of bytes to be transferred */
    int   cyl;					/* Cylinder number and components */
    int   drv_head;				/* Drive drive/head register */
    int   drverr;				/* Drive error register */
    int   drvsts;				/* Drive status register */
    int   head;					/* Head number */
    int   idx;					/* Zero fill index */
    int   orig_ipl;				/* Original IPL */
    int   remainder;				/* Bytes left at end of block */
    int   sec;					/* Sector number and count */
    int   status;				/* Routine status value */
    BYTE  *user_va;				/* Mapped user buffer address */

    TRACE( 0x09100000 + xfer_req );		/* WRITE_ATA_SEG_PIO starting */
    ucb->ucb$l_unsolicited_int = 0;		/* Forget any pending unsolicited interrupts */

    if (xfer_req > MAX_ATA_XFER)		/* Check for too large a transfer */
        xfer_req = MAX_ATA_XFER;		/*  and limit it if so */

    buffer = (BYTE *) ucb->ucb$ps_xfer_buffer;	/* Initialize our buffer pointer */
    *xfer_cnt = 0;				/* Clear count of blocks written */
    drv_head = ucb->ucb$l_drv_head;		/* Get base drive info */
    if (ucb->ucb$l_drive_lba_capable)		/* If LBA mode, ... */
        drv_head |= DRVHD_M_LBA;		/*  ... set the LBA bit */
    compute_address( ucb, &sec, &head, &cyl );	/* Compute the address */

						/* Move the data segment from the user */
    byte_cnt = xfer_req << BLK_SHIFT;		/* Compute byte count */
    if (byte_cnt > ucb->ucb$l_bcr)		/* Check for too large */
        byte_cnt = ucb->ucb$l_bcr;		/* Minimize it */

    buf_ofs = (ucb->ucb$l_media.lbn - ucb->ucb$l_org_media) * BLK_SIZE_512;
						/* Calculate the offset (so far) into the user's buffer */
    user_va = map_user_buffer( ucb, buf_ofs, byte_cnt );
						/* Map that part of the user buffer */
    TRACE( 0x09110000 + byte_cnt );		/* WRITE_ATA_SEG_PIO moving bytes from the user */
    memcpy( buffer, user_va, byte_cnt );	/* Copy the user data to our buffer */

						/* If less than a full block, then zero the remainder */
    remainder = byte_cnt & BLK_MASK;		/* Compute remainder */
    if (remainder > 0)				/* Is there any? */
      {						/* If so, then... */
        remainder = BLK_SIZE_512 - remainder;	/* Compute bytes left */
        for (idx=0; idx < remainder; idx++)	/* For each additional byte */
            buffer[byte_cnt+idx]=0;		/* Zero the byte */
          }

/*
 * Take out the device lock and raise IPL
 * Write the registers
 * Then issue the command
 *
 */

    device_lock( baseucb.ucb$l_dlck, RAISE_IPL, &orig_ipl );
						/* Take out the device lock for the first sector */
    out( ucb, WT_DRV_HD, drv_head|head );	/* Select drive and head */
    out( ucb, WT_SEC_CNT, xfer_req );		/* Ask for "n" sectors */
    out( ucb, WT_SECTOR, sec );			/* Put in the sector number */
    out( ucb, WT_CYL_LO, cyl );			/* Low order cylinder bits */
    out( ucb, WT_CYL_HI, cyl>>8 );		/* High order cylinder bits */
    out( ucb, WT_CMD, CMD_ATA_WRITE_SECS );	/* Attempt to write the sector */

    for (;;)					/* Do forever (for each sector in the transfer request)... */

      {

        status = wait_drq( ucb );		/* Wait for data request */
        if ( $FAIL( status ) )			/* Check for error */
          {					/* If any, then... */
            device_unlock( baseucb.ucb$l_dlck, orig_ipl, SMP_RESTORE );
						/* Release the device lock */
            return( status );			/* And return failing status */
              }

        move_sec_to_drive( ucb, &buffer[(*xfer_cnt)*BLK_SIZE_512], BLK_SIZE_512 );
						/* Move the sector from our transfer buffer to the drive */

        status = dq_wfikpch( ucb->ucb$ps_kpb, orig_ipl, 8 );
						/* Wait for the interrupt */
        if ( $FAIL( status ) )			/* Any error (timeout, etc.) ? */
            return( status );			/* If so, return with status */

        drvsts = inp( ucb, RD_ALT_STS );	/* Get the status byte */
        if ( IS_SET( drvsts, STS_M_ERR ) )	/* Check the status */
          {					/* If any errors, then... */
            drverr = inp( ucb, RD_ERROR );	/* Get the error byte */
            BPTRACE( 0x09120000 );		/* BREAK: Drive error during WRITE_ATA_SEG_PIO */
            return( SS$_DRVERR );		/* Return with error status */
              }

        *xfer_cnt += 1;				/* Count a block written */

        if (*xfer_cnt >= xfer_req )		/* Finished? */
            break;				/* If so, break out of the do-forever loop */

    device_lock( baseucb.ucb$l_dlck, RAISE_IPL, &orig_ipl );
						/* Else take out the device lock and go 'round again */

          }					/* Next sector in the do-forever loop */

    return( SS$_NORMAL );			/* Return to caller succeeding */

      }



/* WRITE_ATA_SEG_DMA - Write one segment to an ATA drive using DMA
 *
 * This routine performs the write of a single I/O segment.  Each
 * segment is a single read command of not more the MAX_ATA_XFER sectors.
 * The overall read I/O routine calls this routine for each of the
 * segments until the entire read is completed.
 *
 * Input:
 *      ucb             pointer to UCB
 *      xfer_req        number of blocks remaining to transfer
 *
 * Output:
 *      xfer_cnt        count of blocks actually transferred
 *      status value
 *
 * Note:
 *
 *   o Some drives sometimes give the interrupt *VERY* quickly,
 *     before I can get back to the WFIKPCH. (This probably occurs
 *     when cached data is available in the drive.) I handle this
 *     by caching the fact that an as-yet-unsolicited interrupt
 *     occurred.
 *
 */

int write_ata_seg_dma( DQ_UCB *ucb, int xfer_req, int *xfer_cnt )

  {

    int   buf_ofs;				/* Offset into user buffer */
    BYTE *buffer;				/* Pointer to our transfer buffer */
    int   byte_cnt;				/* Count of bytes to be transferred */
    int   cyl;					/* Cylinder number and components */
    int   drv_head;				/* Drive drive/head register */
    int   drverr;				/* Drive error register */
    int   drvsts;				/* Drive status register */
    int   head;					/* Head number */
    int   idx;					/* Zero fill index */
    int   orig_ipl;				/* Original IPL */
    int   remainder;				/* Bytes left at end of block */
    int   sec;					/* Sector number and count */
    int   status;				/* Routine status value */
    BYTE  *user_va;				/* Mapped user buffer address */

    TRACE( 0x09800000 + xfer_req );		/* WRITE_ATA_SEG_DMA starting */
    ucb->ucb$l_unsolicited_int = 0;		/* Forget any pending unsolicited interrupts */
    *xfer_cnt = 0;				/* Consider none of the blocks written */

    if (xfer_req > MAX_ATA_XFER)		/* Check for too large a transfer */
        xfer_req = MAX_ATA_XFER;		/*  and limit it if so */

    buffer = (BYTE *) ucb->ucb$ps_xfer_buffer;	/* Initialize our buffer pointer */
    drv_head = ucb->ucb$l_drv_head;		/* Get base drive info */
    if (ucb->ucb$l_drive_lba_capable)		/* If LBA mode, ... */
        drv_head |= DRVHD_M_LBA;		/*  ... set the LBA bit */
    compute_address( ucb, &sec, &head, &cyl );	/* Compute the address */


/* Later, for unbuffered DMA, skip this and set map registers instead */
/*   :   */
/*   :   */
						/* Move the data segment from the user */
    byte_cnt = xfer_req << BLK_SHIFT;		/* Compute byte count */
    if (byte_cnt > ucb->ucb$l_bcr)		/* Check for too large */
        byte_cnt = ucb->ucb$l_bcr;		/* Minimize it */

    buf_ofs = (ucb->ucb$l_media.lbn - ucb->ucb$l_org_media) * BLK_SIZE_512;
						/* Calculate the offset (so far) into the user's buffer */
    user_va = map_user_buffer( ucb, buf_ofs, byte_cnt );
						/* Map that part of the user buffer */
    TRACE( 0x09810000 + byte_cnt );		/* WRITE_ATA_SEG_DMA moving bytes from the user */
    memcpy( buffer, user_va, byte_cnt );	/* Copy the user data to our buffer */

						/* If less than a full block, then zero the remainder */
    remainder = byte_cnt & BLK_MASK;		/* Compute remainder */
    if (remainder > 0)				/* Is there any? */
      {						/* If so, then... */
        remainder = BLK_SIZE_512 - remainder;	/* Compute bytes left */
        for (idx=0; idx < remainder; idx++)	/* For each additional byte */
            buffer[byte_cnt+idx]=0;		/* Zero the byte */
          }

/*   :   */
/*   :   */
/* Later, for unbuffered DMA, skip this and set map registers instead */


/*
 * Take out the device lock and raise IPL
 * Write the registers
 * Then issue the command
 *
 */

    load_prdt( ucb );				/* Load the PRDT */

    device_lock( baseucb.ucb$l_dlck, RAISE_IPL, &orig_ipl );
						/* Take out the device lock so we can write the registers */

    out( ucb, WT_DMA_CMD, DMA_CMD_M_OUTBOUND | DMA_CMD_M_INACTIVE );
						/* Make sure the DMA controller is outbound    */
						/*   (that is, reading memory -> writing disk) */
						/*   but not active yet                        */
    out( ucb, WT_DMA_AD0, ( ( (UINT) ucb->ucb$l_prdt_phy )      ) & 0xFF );
    out( ucb, WT_DMA_AD1, ( ( (UINT) ucb->ucb$l_prdt_phy ) >> 8 ) & 0xFF );
    out( ucb, WT_DMA_AD2, ( ( (UINT) ucb->ucb$l_prdt_phy ) >>16 ) & 0xFF );
    out( ucb, WT_DMA_AD3, ( ( (UINT) ucb->ucb$l_prdt_phy ) >>24 ) & 0xFF );
						/* Point the controller to the PCI address of our PRDT table */
    out( ucb, WT_DMA_STS, DMA_STS_M_DRV1 | DMA_STS_M_DRV0 | DMA_STS_M_INT | DMA_STS_M_ERR );
						/* For now, set both drives as DMA-capable */
						/* Write "1"s to INT and ERR to clear them in case they're set */
    out( ucb, WT_DRV_HD, drv_head|head );	/* Select drive and head */
    out( ucb, WT_SEC_CNT, xfer_req );		/* Ask for "n" sectors */
    out( ucb, WT_SECTOR, sec );			/* Put in the sector number */
    out( ucb, WT_CYL_LO, cyl );			/* Low order cylinder bits */
    out( ucb, WT_CYL_HI, cyl>>8 );		/* High order cylinder bits */
    out( ucb, WT_CMD, CMD_ATA_WRITE_DMA );	/* Attempt to write the sector(s) */
    out( ucb, WT_DMA_CMD, DMA_CMD_M_OUTBOUND | DMA_CMD_M_ACTIVE );
						/* Set the DMA controller outbound            */
						/*   (that is, reading memory-> writing disk) */
						/*   and active                               */

    status = dq_wfikpch( ucb->ucb$ps_kpb, orig_ipl, 9 );
						/* Wait for the interrupt */
    if ( $FAIL( status ) )			/* Any error (timeout, etc.) ? */
        return( status );			/* If so, return with status */

    drvsts = inp( ucb, RD_ALT_STS );		/* Get the status byte */

    if ( IS_SET( drvsts, STS_M_ERR ) )		/* Check the status (saved from above) */
      {						/* If any errors, then... */
        drverr = inp( ucb, RD_ERROR );		/* Get the error byte */
        BPTRACE( 0x09820000 );			/* BREAK: Drive error during WRITE_ATA_SEG_PIO */
        return( SS$_DRVERR );			/* Return with error status */
          }

    device_lock( baseucb.ucb$l_dlck, RAISE_IPL, &orig_ipl );
						/* Take out the deviced lock */
    out( ucb, WT_DMA_CMD, DMA_CMD_M_INACTIVE );	/* Set the DMA controller inactive */
    device_unlock( baseucb.ucb$l_dlck, orig_ipl, SMP_RESTORE );
						/* Return the deviced lock */

    *xfer_cnt = xfer_req;			/* Consider all of the blocks written */

    return( SS$_NORMAL );			/* Return to caller succeeding */

      }



/* WRITE_ATAPI_512_SEG - Write one segment to a 512-byte-sector ATAPI drive
 *
 * This routine performs the write of a single I/O segment.
 * Each segment is a single write command of not more the MAX_ATAPI_512_XFER
 * sectors. The overall write I/O routine calls this routine for each
 * of the segments until the entire write is completed.
 *
 * Input:
 *      ucb           pointer to UCB
 *      xfer_req      number of blocks remaining to transfer
 *      buffer        address of buffer to transfer data from
 *      dma_flag      whether or not to use DMA
 *
 *
 * Output:
 *      xfer_cnt      count of blocks actually transferred
 *      status value
 *
 */

int write_atapi_512_seg( DQ_UCB *ucb, int xfer_req, int *xfer_cnt, int dma_flag )

  {

    int   buf_ofs;				/* Offset into user buffer */
    BYTE  *buffer;				/* Pointer to our transfer buffer */
    int   byte_cnt;				/* Count of bytes to be transferred */
    int   idx;					/* Zero fill index */
    int   offset;				/* Offset within 2K block */
    BYTE  *packet;				/* The packet bytes within the UCB */
    int   *packetl;				/* The packet (as longwords) within the UCB */
    int   remainder;				/* Bytes left at end of block */
    int   status;				/* Routine status value */
    BYTE  *user_va;				/* Mapped user buffer address */
    int   orig_ipl;				/* Original IPL */


    TRACE( 0x09200000 + xfer_req );		/* WRITE_ATAPI_512_SEG starting */

    if (xfer_req > MAX_ATAPI_512_XFER)		/* Check for too large a transfer */
        xfer_req = MAX_ATAPI_512_XFER;		/*  and limit it if so */

    buffer = (BYTE *) ucb->ucb$ps_xfer_buffer;	/* Initialize our buffer pointer */
    byte_cnt = 0;				/* Clear count of bytes transferred */

						/* Move the data segment from the user */
    byte_cnt = xfer_req << BLK_SHIFT;		/* Compute byte count */
    if (byte_cnt > ucb->ucb$l_bcr)		/* Check for too large */
        byte_cnt = ucb->ucb$l_bcr;		/* Minimize it */

    buf_ofs = (ucb->ucb$l_media.lbn - ucb->ucb$l_org_media) * BLK_SIZE_512;
						/* Calculate the offset (so far) into the user's buffer */
    user_va = map_user_buffer( ucb, buf_ofs, byte_cnt );
						/* Map that part of the user buffer */
    TRACE( 0x09210000 + byte_cnt );		/* WRITE_ATA_SEG_PIO moving bytes from the user */
    memcpy( buffer, user_va, byte_cnt );	/* Copy the user data to our buffer */

						/* If less than a full block, then zero the remainder */
    remainder = byte_cnt & BLK_MASK;		/* Compute remainder */
    if (remainder > 0)				/* Is there any? */
      {						/* If so, then... */
        remainder = BLK_SIZE_512 - remainder;	/* Compute bytes left */
        for (idx=0; idx < remainder; idx++)	/* For each additional byte */
            buffer[byte_cnt+idx]=0;		/* Zero the byte */
          }

    packetl = (int *) ucb->ucb$b_packet;	/* Bind onto packet in the UCB as a longword array */
    packetl[0] = 0;				/* Clear the packet */
    packetl[1] = 0;				/*   :   */
    packetl[2] = 0;				/*   :   */

    packet  = (BYTE *) ucb->ucb$b_packet;	/* Bind onto packet in the UCB as a byte array */
    packet[0]  = CMD_ATAPI_WRITE_12;		/* Write(12) command */
/*  packet[1]  = 0x00;				/@ (Reserved) */
/*  packet[2]  = 0x00;				/@ Address MSB (filled in momentarily) */
/*  packet[3]  = 0x00;				/@   :      :   */
/*  packet[4]  = 0x00;				/@   :      :   */
/*  packet[5]  = 0x00;				/@   :     LSB  */
    packet[6]  = xfer_req>>24;			/* Transfer length MSB in blocks */
    packet[7]  = xfer_req>>16;			/*   :              :  */
    packet[8]  = xfer_req>>8;			/*   :              :  */
    packet[9]  = xfer_req;			/*   :             LSB */
/*  packet[10] = 0x00;				/@ (Reserved) */
/*  packet[11] = 0x00;				/@ (Reserved) */
    offset     = fill_packet_w_adx( ucb );	/* Fill the packet address cells*/

    status = atapi_packet_command( ucb, buffer, xfer_req, xfer_cnt, dma_flag );
						/* Do the common packet-command processing using appropriate mode */

    if (dma_flag)				/* Using DMA? */
      {						/* If so, then... */
        device_lock( baseucb.ucb$l_dlck, RAISE_IPL, &orig_ipl );
						/* Take out the deviced lock */
        out( ucb, WT_DMA_CMD, DMA_CMD_M_INACTIVE ); /* Set the DMA controller inactive */
        device_unlock( baseucb.ucb$l_dlck, orig_ipl, SMP_RESTORE );
          }					/* Return the deviced lock */

    if (status == SS$_DRVERR)			/* Did it result in a drive error? */
      {						/* If so, then... */
        atapi_request_sense( ucb, (BYTE *) ucb->ucb$ps_sense_buffer );
						/* Get error info from drive */
        status = atapi_xlate_error_to_vms( ucb );
						/* Turn it into a VMS error code */
        return( status );			/* and exit */
          }

    if ( $FAIL( status ) )			/* Check for any other error */
        return( status );			/*  and exit if so */

    return( SS$_NORMAL );			/* Return to caller */

      }



/* WRITE_ATAPI_2K_SEG - Write one segment to a 2Kbyte-sector ATAPI drive
 *           
 * This routine would performs the write of a single I/O segment, but punts
 * because we don't yet know how to do the necessary Read/Modify/write(s) that
 * would be required to write VMS-sized 512-byte-blocks within 2Kbyte sectors.
 *
 * Input:
 *      ucb           pointer to UCB
 *      xfer_req      number of blocks remaining to transfer
 *      dma_flag      whether or not to use DMA (a dummy parameter right now)
 *
 * Output:
 *      xfer_cnt      count of blocks actually transferred (unused)
 *      status value
 *
 */

int write_atapi_2K_seg( DQ_UCB *ucb, int xfer_req, int *xfer_cnt, int dma_flag )

  {

    TRACE( 0x09300000 + xfer_req );		/* WRITE_ATAPI_2K_SEG_PIO starting */

    return( SS$_WRITLCK );			/* We don't know how to write 2K (CD-ROM/DVD-ROM sized) blocks? */
						/* (Presently, we'd need a read-modify-write) */
      }



/* ATAPI_READ_CAPACITY - Read the drive capacity and bytes/sector
 *
 * Input:
 *      ucb           pointer to UCB
 *      buffer        address of buffer to transfer data into
 *
 * Output:
 *      status value
 *      Uninterpreted capacity data in the designated buffer
 *
 */

int atapi_read_capacity( DQ_UCB *ucb, BYTE *buffer )

  {

    BYTE  *packet;				/* The packet bytes within the UCB */
    int   *packetl;				/* The packet (as longwords) within the UCB */
    int    xfer_cnt;				/* Count of sectors actually transferred (dummy here) */


    packetl = (int *) ucb->ucb$b_packet;	/* Bind onto packet in the UCB as a longword array */
    packetl[0] = 0;				/* Clear the packet */
    packetl[1] = 0;				/*   :   */
    packetl[2] = 0;				/*   :   */

    packet = (BYTE *) ucb->ucb$b_packet;	/* Bind onto packet in the UCB as a byte array */
    packet[0]  = CMD_ATAPI_READ_CAPACITY;	/* Read capacity command */
/*  packet[1]  = 0x00;				/@ (Reserved) */
/*  packet[2]  = 0x00;				/@ Address MSB */
/*  packet[3]  = 0x00;				/@   :      :  */
/*  packet[4]  = 0x00;				/@   :      :  */
/*  packet[5]  = 0x00;				/@   :     LSB */
/*  packet[6]  = 0x00;				/@ (Reserved) */
/*  packet[7]  = 0x00;				/@ (Reserved) */
/*  packet[8]  = 0x00;				/@ (Reserved) */
/*  packet[9]  = 0x00;				/@ (Reserved) */
/*  packet[10] = 0x00;				/@ (Reserved) */
/*  packet[11] = 0x00;				/@ (Reserved) */

    return( atapi_packet_command( ucb, buffer, 0, &xfer_cnt, FALSE ) );
						/* Do the common packet-command processing using PIO */
      }

   
  
/* ATAPI_REQUEST_SENSE - Get the sense keys from the drive
 *
 * Input:
 *      ucb           pointer to UCB
 *      buffer        address of buffer to transfer data into
 *
 * Output:
 *      status value
 *      sense, asc, and ascq fields in ucb updated
 *
 */

int atapi_request_sense( DQ_UCB *ucb, BYTE *buffer )

  {

    BYTE  *packet;				/* The packet bytes within the UCB */
    int   *packetl;			       	/* The packet (as longwords) within the UCB */
    int    status;				/* Routine return status */
    int    xfer_cnt;				/* Count of blocks actually transferred (dummy here) */


    packetl = (int *) ucb->ucb$b_packet;	/* Bind onto packet in the UCB as a longword array */
    packetl[0] = 0;				/* Clear the packet */
    packetl[1] = 0;				/*   :   */
    packetl[2] = 0;				/*   :   */

    packet = (BYTE *) ucb->ucb$b_packet;	/* Bind onto packet in the UCB as a byte array */
    packet[0]  = CMD_ATAPI_REQUEST_SENSE;	/* Request_Sense command */
/*  packet[1]  = 0x00;				/@ (Reserved) */
/*  packet[2]  = 0x00;				/@ (Reserved) */
/*  packet[3]  = 0x00;				/@ (Reserved) */
    packet[4]  =   18;				/* Allocation Length */
/*  packet[5]  = 0x00;				/@ (Reserved) */
/*  packet[6]  = 0x00;				/@ (Reserved) */
/*  packet[7]  = 0x00;				/@ (Reserved) */
/*  packet[8]  = 0x00;				/@ (Reserved) */
/*  packet[9]  = 0x00;				/@ (Reserved) */
/*  packet[10] = 0x00;				/@ (Reserved) */
/*  packet[11] = 0x00;				/@ (Reserved) */
            
    status = atapi_packet_command( ucb, buffer, 0, &xfer_cnt, FALSE );
						/* Do the common packet-command processing using PIO */

    if ( $FAIL( status ) )			/* Check for any other error */
        return( status );			/*  and exit if so */

    TRACE( 0x0C000000    + (buffer[2] & 0x0F) );/* ATAPI_REQUEST_SENSE storing the sense_key */
    ucb->ucb$l_sense_key = (buffer[2] & 0x0F);	/* Save sense key */
    TRACE( 0x0C010000    + buffer[12] );	/* ATAPI_REQUEST_SENSE storing the ASC */
    ucb->ucb$l_asc       = buffer[12];		/* Save additional sense code */
    TRACE( 0x0C020000    + buffer[13] );	/* ATAPI_REQUEST_SENSE storing the ASC Qualifier */
    ucb->ucb$l_ascq      = buffer[13];		/* Save additional sense code qualifier */

    return( status );				/* And return with status */
    
      }


  
/* ATAPI_PACKET_COMMAND - Do the common ATAPI packet command processing using desired mode
 *
 * Input:
 *      ucb           pointer to UCB
 *      buffer        address of buffer to transfer data into
 *      xfer_req      number of blocks remaining to transfer (NOTE: 512-byte blocks! Not sectors!)
 *      dma_flag      whether or not to us DMA
 *
 *
 * Output:
 *      vms status value
 *      xfer_cnt      count of blocks actually transferred.  For IO$_DIAGNOSE,
 *                    count of BYTES actually transferred.
 *
 *
 * Notes:
 *
 *    There are differences in the way drives from different
 *    vendors operate during the Packet command. In particular:
 *
 *      - Some drives (including the Toshiba drives) don't give
 *        an interrupt as DRQ asserts to request the command packet.
 *        Per the ATAPI spec dated 6/95, section 4.7, item 4, this
 *        interrupt is optional. The code handles this situation by
 *        explicitly waiting for DRQ to assert.
 *
 *      - The Sony drives don't seem to (always? ever?) give the
 *        expected interrupt as DRQ asserts to request that we read
 *        data from the drive. It may be that the drive is already
 *        ready with cached data and I'm squashing the 'rupt with
 *        the read of the STATUS (STS) register. In any case, I
 *        handle this by checking for the drive to already be
 *        non-busy and DRQ prior to waiting for the interrupt.
 *
 *      - Some drives sometimes give the interrupt *VERY* quickly,
 *        before I can get back to the WFIKPCH. (This probably occurs
 *        when cached data is available in the drive.) I handle this
 *        by caching the fact that an as-yet-unsolicited interrupt
 *        occurred.
 * 
 */                                       

int atapi_packet_command( DQ_UCB *ucb, BYTE *buffer, int xfer_req, int *xfer_cnt, int dma_flag )

  {
                                
    int   drv_head;					/* Drive drive/head register */
    int   status;					/* Routine status value */
    int   orig_ipl;					/* Original IPL */
    int   reason;					/* Drive "interrupt reason" */
    int   drvsts;    					/* Drive status register */
    int   drverr;					/* Drive error register */
    int   drvdrq;					/* Drive DRQ bit from STS/ALTSTS */
    int   drvbytcnt;					/* Drive byte count */
    int   buffer_size;					/* Size of the target buffer */
    IRP   *irp;

    TRACE( 0x02000000 );				/* ATAPI_PACKET_COMMAND_DMA starting */

    ucb->ucb$l_unsolicited_int = 0;			/* Forget any pending unsolicited interrupts       */
    reason    = 0xDEADDEAD;				/* Invalidate this, just in case anyone's looking  */
    *xfer_cnt = 0;					/* Consider none of the blocks transfered          */
    buffer_size = 0;					/* Set this to a default that's guaranteed to fail */
    if (buffer == (BYTE *) ucb->ucb$ps_xfer_buffer)	/* Pointing to the transfer buffer?                */
        buffer_size = XFER_BUFFER_SIZE;			/* If so, remember that size                       */
    if (buffer == (BYTE *) ucb->ucb$ps_sense_buffer)	/* Pointing to the sense buffer?                   */
        buffer_size = SENSE_BUFFER_SIZE;		/* If so, remember that size                       */
							/* Else we don't know any other buffer sizes       */
							/*    -- leave it zero                             */

    status = wait_ready( ucb );				/* Wait for drive to be ready */
    if ( $FAIL( status ) )				/* Check the status for failure */
        return( status );				/* Return with error */

    drv_head = ucb->ucb$l_drv_head;			/* Get base drive info */
    if (ucb->ucb$l_drive_lba_capable)			/* If LBA mode, ... */
        drv_head |= DRVHD_M_LBA;			/*  ... set the LBA bit */

/*
 * Take out the device lock and raise IPL
 * Write the registers
 * Then issue the packet command
 * Then follow the drive's lead as to what to do next
 *
 */         

    device_lock( baseucb.ucb$l_dlck, RAISE_IPL, &orig_ipl );
							/* Take out the device lock so we can write the registers */
    out( ucb, WT_DRV_HD, drv_head );			/* Select drive, ignore head               */
    if (dma_flag)
      {
        load_prdt( ucb );				/* Load the PRDT */
							/* Make sure the DMA controller is inbound     */
							/*   (that is, reading disk -> writing memory) */
							/*   but not active yet                        */
        out( ucb, WT_DMA_CMD, DMA_CMD_M_INBOUND | DMA_CMD_M_INACTIVE );
							/* For now, set both drives as DMA-capable */
							/* Write "1"s to INT and ERR to clear them in case they're set */
        out( ucb, WT_DMA_STS, DMA_STS_M_DRV1 | DMA_STS_M_DRV0 | DMA_STS_M_INT | DMA_STS_M_ERR );
							/* Point the controller to the PCI address of our PRDT table */
	outl(ucb, WTL_DMA_AD0, (UINT) ucb->ucb$l_prdt_phy );


          }


    if (dma_flag)
        out( ucb, WT_FEATURES, 0x01 );			/* No Overlap (bit <1>), Yes DMA (bit <0>) */
    else
        out( ucb, WT_FEATURES, 0x00 );			/* No Overlap (bit <1>), No DMA (bit <0>) */
    out( ucb, WT_CYL_LO, buffer_size );			/* Low order cylinder bits/bytecount       */
    out( ucb, WT_CYL_HI, buffer_size>>8 );		/* High order cylinder bits/bytecount      */
    out( ucb, WT_CMD, CMD_ATA_PACKET_CMD );		/* Issue the "packet" command              */
    device_unlock( baseucb.ucb$l_dlck, orig_ipl, SMP_RESTORE ); /* release and let ints happen */

    status = wait_drq( ucb );				/* Explicitly wait for DRQ (Toshiba fix) */
    if ( $FAIL( status ) )				/* Check for error */
      {							/* If any, then... */
        BPTRACE( 0x02010000 );				/* BREAK: WAIT_DRQ() failed during atapi_packet_command */
        return( SS$_CTRLERR );				/*  and return complaining */
          }

    device_lock( baseucb.ucb$l_dlck, RAISE_IPL, &orig_ipl );
    for (;;)						/* Now, forever process based on the drive's requests */

      {

        drvsts = inp( ucb, RD_ALT_STS );		/* Read status byte */

        if (    (dma_flag)				/* If we're doing a DMA transfer                   */
             && ( (reason & ~STS_M_DRQ)==0x01) )	/*   and the last state was "Get Packet"           */
          {						/*                                                 */
            TRACE( 0x02060000 );			/*   ATAPI quashing interrupt-bypass 'cause of DMA */
            drvsts = 0;					/*   then quash the captured status byte so we     */
              }						/*   don't allow the interrupt to be bypassed      */

        if (    ( (drvsts & STS_M_BSY) == 0 )		/* Is the drive already idle? */
             && ( (drvsts & STS_M_DRQ) != 0 ) )		/*    and waiting with DRQ asserted? */
          {						/* If so, bypass WFIKPCH, etc. */
            TRACE( 0x02020000 );			/* ATAPI taking the already-DRQ WFIKPCH bypass */
            ucb->ucb$l_unsolicited_int = 0;		/* Forget any pending unsolicited interrupts */
            drvsts = inp( ucb, RD_STS );		/* Read status byte to quash any pending interrupts */
            device_unlock( baseucb.ucb$l_dlck, orig_ipl, SMP_RESTORE );
              }						/* And release the device lock */
        else
          {						/* Else wait for an interrupt... */

	    if (dma_flag)
		{
		out( ucb, WT_DMA_CMD, DMA_CMD_M_INBOUND | DMA_CMD_M_ACTIVE );
							    /* Set the DMA controller inbound              */
							    /*   (that is, reading disk -> writing memory) */
							    /*   and active                                */
		}
            status = dq_wfikpch( ucb->ucb$ps_kpb, orig_ipl, 11 );
							/* Wait for the interrupt */
            if ( $FAIL( status ) )			/* Any error (timeout, etc.) ? */
              {
		if (dma_flag)
		    {
		    device_lock( baseucb.ucb$l_dlck, RAISE_IPL, &orig_ipl );
		    out( ucb, WT_DMA_CMD, DMA_CMD_M_INACTIVE );	/* assure dma aborted */
		    device_unlock( baseucb.ucb$l_dlck, orig_ipl, SMP_RESTORE );
		    }
		if (status == SS$_TIMEOUT)
		   {
		   reset_ctrl(ucb);
		   }
                BPTRACE( 0x020F0000 );			/* BREAK: ATAPI is handling a WFIPTCH error */
                return( status );			/* If so, return with status */
                  }

            drvsts = inp( ucb, RD_ALT_STS );		/* Get the status byte */

              }

        if ( IS_SET( drvsts, STS_M_ERR ) )		/* Any errors?                                      */
          {						/* If so, then ...                                  */
            drverr = inp( ucb, RD_ERROR );		/* Get the error byte                               */
            BPTRACE( 0x02030000 + drverr );		/* BREAK: ATAPI drive error, "sense_key" stored     */
            ucb->ucb$l_sense_key = drverr;		/* Save latest sense key                            */
							/*   (Note: Raw register -- not in justified form!) */
            return( SS$_DRVERR );			/* Caller may fill in more detail later             */
              }

        drvdrq = ( drvsts & STS_M_DRQ );		/* Get DRQ bit (val=0x08) */
        reason = inp( ucb, RD_SEC_CNT );		/* See what the drive wants */
        reason &= 0x07;					/* Keep just [0:0:0:0:0:RLS:IO:CoD] */
//#saythis "Inoring REL for now..."
        reason &= 0x03;					/* Throw away RELease, too. */
        reason |= drvdrq;				/* 'OR' in the DRQ bit */
							/*  [0:0:0:0:DRQ:RLS:IO:CoD] */

        switch (reason)					/* Dispatch based on that combined reason */
          {


            case (0x00):				/* Write-data (and no DRQ) to drive */
              {						/* *THAT* would be a surprise! */
                BPTRACE( 0x02040000 );			/* BREAK: ATAPI error: "Write-data" requested w/o DRQ */
                return( SS$_DRVERR );			/* Then, make that an error */
                break;
                  }

            case (STS_M_DRQ+0x00):			/* Write data to drive */
              {
                TRACE( 0x02040002 );			/* ATAPI Write-Data phase */

                drvbytcnt =   inp( ucb, RD_CYL_LO )	/* Get the bytecount now desired by the drive           */
                            | inp( ucb, RD_CYL_HI )<<8;	/*    :                                                 */
                if (drvbytcnt > buffer_size)		/* Is it too big to transfer?                           */
                  {					/* If so, then...                                       */
                    BPTRACE( 0x02040008 );		/* BREAK: ATAPI error: Bytecount mismatch on write-data */
                    return( SS$_DRVERR );		/* Make that an error instead of possibly               */
							/*   over-running our buffer                            */
                      }
                /* For DIAGNOSE operations, return count of BYTES tranfered. 
                 * For all other operations, return count of BLOCKS transfered.
                 */

                irp = baseucb.ucb$l_irp;
                if (irp->irp$l_func == IO$_DIAGNOSE) {
                    /* Move the sector from our transfer buffer to the drive.
                     * Note that 1 was added to drvbytcnt to force odd byte
                     * transfers to be rounded up to the next word.
                     */
                    move_sec_to_drive( ucb, buffer + *xfer_cnt, drvbytcnt+1 );
                    *xfer_cnt += drvbytcnt;		/* Count of bytes transfered */
                } else { 
                    move_sec_to_drive( ucb, &buffer[(*xfer_cnt)*BLK_SIZE_512], drvbytcnt );
							/* Move the sector from our transfer buffer to the drive */
                    *xfer_cnt += ( (drvbytcnt + BLK_SIZE_512 - 1) >> BLK_SHIFT );
							/* Count blocks written */
		}
                device_lock( baseucb.ucb$l_dlck, RAISE_IPL, &orig_ipl );
   							/* Take the device lock again */
							/* As we go back to WFIKPCH again */
                break;
                  }



            case (0x01):				/* Command packet wanted but no DRQ */
              {						/* *THAT* would be a surprise! */
                BPTRACE( 0x02040001 );			/* BREAK: ATAPI error: "Cmd Pkt wanted" w/o DRQ */
                return( SS$_DRVERR );			/* Then, make that an error */
                  }
                                       
            case (STS_M_DRQ+0x01):			/* Command packet wanted */
              {
                BYTE  *packet;				/* The packet bytes within the UCB */

                TRACE( 0x02040009 );			/* ATAPI Cmd Pkt Wanted phase */

                device_lock( baseucb.ucb$l_dlck, RAISE_IPL, &orig_ipl );
								/* Take the device lock again */
                packet = (BYTE *) ucb->ucb$b_packet;		/* Bind onto packet in the UCB as a byte array */
                outw_t( ucb, WT_DATA, packet[ 0]|(packet[ 1]<<8) );/* Push out packet to the drive */
                outw_t( ucb, WT_DATA, packet[ 2]|(packet[ 3]<<8) );/*   :   */
                outw_t( ucb, WT_DATA, packet[ 4]|(packet[ 5]<<8) );/*   :   */
                outw_t( ucb, WT_DATA, packet[ 6]|(packet[ 7]<<8) );/*   :   */
                outw_t( ucb, WT_DATA, packet[ 8]|(packet[ 9]<<8) );/*   :   */
                outw_t( ucb, WT_DATA, packet[10]|(packet[11]<<8) );/*   :   */
                break;						/* And go back to WFIKPCH again */
                  }



            case (0x02):				/* "Get data" without DRQ */
              {						/* *THAT* would be a surprise! */
                BPTRACE( 0x02040002 );			/* BREAK: ATAPI error: "Get data" w/o DRQ */
                return( SS$_DRVERR );			/* Then, make that an error */
                break;
                  }

            case (STS_M_DRQ+0x02):			/* Get the data from the silo                         */
              {						/* and into the transfer buffer                       */
                                                        /*                                                    */
                TRACE( 0x0204000A );			/* ATAPI Get-Data phase                               */
                if (dma_flag)				/* Doing a DMA transfer?                              */
                  {					/* If so, we shouldn't be here -- this is a bad thing!*/
                    BPTRACE( 0x02060001 );		/* ATAPI Get-Data phase quashed 'cause of DMA         */
                      }                                 /*                                                    */
                else					/* We're doing PIO -- go ahead and get the data       */
                  {                                     /*                                                    */
                    drvbytcnt =   inp( ucb, RD_CYL_LO )	/* Get the bytecount now desired by the drive         */
                              | inp( ucb, RD_CYL_HI )<<8;/*    :                                              */
                  if (drvbytcnt > buffer_size)		/* Is it too big to transfer?                         */
                    {					/* If so, then...                                     */
                      BPTRACE( 0x02050000 );		/* BREAK: ATAPI error: Bytecount mismatch on get_data */
                      return( SS$_DRVERR );		/* Make that an error instead of possibly             */
                        }				/*   over-running our buffer                          */
                                                        /*                                                    */

                  /* For DIAGNOSE operations, return count of BYTES tranfered. 
                   * For all other operations, return count of BLOCKS transfered.
                   */

                  irp = baseucb.ucb$l_irp;
                  if (irp->irp$l_func == IO$_DIAGNOSE) { 
                    /* Move the sector from the drive to our xfer buffer.
                     * Note that 1 was added to drvbytcnt to force odd byte
                     * transfers to be rounded up to next word.
                     */
                    move_sec_from_drive( ucb, buffer+*xfer_cnt, drvbytcnt+1);
                    *xfer_cnt += drvbytcnt;		/* Count of bytes transfered */
                  } else {
                     move_sec_from_drive( ucb, &buffer[(*xfer_cnt)*BLK_SIZE_512], drvbytcnt );
							/* Move the sector from the drive to our xfer buffer  */
                     *xfer_cnt += ( (drvbytcnt + BLK_SIZE_512 - 1) >> BLK_SHIFT );
                       					/* Count blocks read                                  */
                  }
                  device_lock( baseucb.ucb$l_dlck, RAISE_IPL, &orig_ipl );
							/* Take the device lock again                         */
							/* As we go back to WFIKPCH again                     */
                    }

                break;
                  }



            case (0x03):				/* Reason 0x03 *WITHOUT* DRQ indicates completion */
              {
                TRACE( 0x02040003 );			/* ATAPI Normal Completion phase */
                if (dma_flag)				/* Using DMA? If so, then... */
                    *xfer_cnt = xfer_req;		/* Consider all of the blocks transfered */
                return( SS$_NORMAL );			/* So break out of the forever loop */
                break;					/* (Break for safety) */
                  }

            case (STS_M_DRQ+0x03):			/* DRQ + Message from drive (future feature) */
              {						/* *THAT* would be a surprise! */
                BPTRACE( 0x0204000B );			/* BREAK: ATAPI error: "Message" from drive (with DRQ) */
                return( SS$_DRVERR );			/* Then, make that an error */
                break;
                  }



            case (0x04):				/* "Release" without DRQ */
              {						/* *THAT* would be a surprise! */
                BPTRACE( 0x02040004 );			/* BREAK: ATAPI error: "Release" from drive (w/o DRQ) */
                return( SS$_DRVERR );			/* Then, make that an error */
                break;
                  }

            case (STS_M_DRQ+0x04):			/* "Release" with DRQ */
              {						/* Either way, it's surprising */
                BPTRACE( 0x0204000C );			/* BREAK: ATAPI error: "Release" from drive (with DRQ) */
                return( SS$_DRVERR );			/* Then, make that an error */
                break;
                  }



            case (0x05):				/* Undefined reason 0x05 */
              {						/* *THAT* would be a surprise! */
                BPTRACE( 0x02040005 );			/* BREAK: ATAPI error: Reason=0x05 (w/o DRQ) */
                return( SS$_DRVERR );			/* Then, make that an error */
                break;
                  }

            case (STS_M_DRQ+0x05):			/* Undefined reason 0x05 + DRQ */
              {						/* *THAT* would be a surprise! */
                BPTRACE( 0x0204000D );			/* BREAK: ATAPI error: Reason=0x5 (with DRQ) */
                return( SS$_DRVERR );			/* Then, make that an error */
                break;
                  }



            case (0x06):				/* Undefined reason 0x06 */
              {						/* *THAT* would be a surprise! */
                BPTRACE( 0x02040006 );			/* BREAK: ATAPI error: Reason=0x6 (w/o DRQ) */
                return( SS$_DRVERR );			/* Then, make that an error */
                break;
                  }

            case (STS_M_DRQ+0x06):			/* Undefined reason DRQ + 0x06 */
              {						/* *THAT* would be a surprise! */
                BPTRACE( 0x0204000E );			/* BREAK: ATAPI error: Reason=0x6 (with DRQ) */
                return( SS$_DRVERR );			/* Then, make that an error */
                break;
                  }



            case (0x07):				/* Undefined reason 0x07 */
              {						/* *THAT* would be a surprise! */
                BPTRACE( 0x02040007 );			/* BREAK: ATAPI error: Reason=0x7 (w/o DRQ) */
                return( SS$_DRVERR );			/* Then, make that an error */
                break;
                  }

            case (STS_M_DRQ+0x07):			/* Undefined reason DRQ + 0x07 */
              {						/* *THAT* would be a surprise! */
                BPTRACE( 0x0204000F );			/* BREAK: ATAPI error: Reason=0x7 (with DRQ) */
                return( SS$_DRVERR );			/* Then, make that an error */
                break;
                  }



            default:					/* Out-of-range combination? */
              {						/* *THAT* would *REALLY* be a surprise! */
                BPTRACE( 0x020400FF );			/* BREAK: ATAPI error: Out-of-bounds Reason */
                //bug_check( INCONSTATE, FATAL, COLD );	/* So be it */
                panic(" INCONSTATE, FATAL, COLD \n");	/* So be it */
                return( SS$_DRVERR );			/* Then, make that an error */
                break;
                  }

              }						/* End of the switch */

          }						/* End of the forever loop */

      }							/* Never gets here */



/* ATAPI_XLATE_ERROR_TO_VMS - Map the sense keys to a VMS error code
 *
 * Input:
 *      sense_buffer     address of buffer to transfer data from
 *
 * Output:
 *      vms status value
 *
 */

int atapi_xlate_error_to_vms( DQ_UCB *ucb )
  {
                   
    int asc;
    int ascq;


    asc  = ucb->ucb$l_asc;			/* Get additional sense code */
    ascq = ucb->ucb$l_ascq;			/* Get additional sense code qualifier */

    if ( (asc==0x04) && (ascq==0x01) )		/* "Logical unit is in process of becoming ready" */
        return( SS$_MEDOFL );			/*   becomes "Medium is offline" */
						/* Higher-level code will handle this. */

						/* More inclusively, */
    if (asc==0x04)				/* Various "Logical unit not ready" errors */
        return( SS$_MEDOFL );			/*   all become "Medium is offline" */

    if (asc==0x21)				/* "Logical block address out of range" */
        return( SS$_BADPARAM );			/*   becomes "Bad Parameter" */

    if (asc==0x28)				/* "Medium may have changed" */
      {						/*   becomes "Volume is not software enabled" */
        baseucb.ucb$v_valid = 0;		/* Also clear the VALID bit and */
        return( SS$_VOLINV );			/* And return the status */
          }

    if (asc==0x29)				/* Various "Reset occurred" errors */
        return( SS$_MEDOFL );			/*   become "Medium is offline" */

    if (asc==0x30)				/* Various "Incompatible medium" errors */
        return( SS$_MEDOFL );			/*   become "Medium is offline" */

    if (asc==0x3A)				/* Various "Medium not present" errors */
        return( SS$_MEDOFL );			/*   become "Medium is offline" */

    BPTRACE( 0x02060000 );			/* BREAK: Untranslated sense key during atapi_xlate_error_to_vms */
    return( SS$_DRVERR );			/* All else defaults to a nice, safe, disaster */
						/*   "%SYSTEM-W-DRVERR, fatal drive error" */

      }



/* COMPUTE_ADDRESS - This routine is used to compute the head, sector
 * and track information from a logical block number.
 *
 * Note that on IDE disks, sector numbers start at 1.  Head and
 * cylinder numbers start at 0.
 *
 * LBA mode addressing is handled if the LBA flag is set.  In LBA mode,
 * the address is still returned in the sec, head and cyl locations,
 * but it is simply the sections of the LBA.  The callers of this
 * routine simply write these value to the registers, so this all
 * works just fine.
 *
 * Input:
 *      ucb     pointer to the UCB
 *
 * Output:
 *              CHS mode                LBA mode
 *      sec     sector number           LBA[0:7]
 *      head    head number             LBA[24:27]
 *      cyl     cylinder number         LBA[8:23]
 *
 */

void compute_address( DQ_UCB *ucb, int *sec, int *head, int *cyl )

  {

    if (ucb->ucb$l_drive_lba_capable)			/* LBA or CSH mode? */

      {							/* LBA mode... */
        *sec  =  ucb->ucb$l_media.lbn        & 0x00FF;	/* Bits 0-7 */
        *cyl  = (ucb->ucb$l_media.lbn >> 8)  & 0xFFFF;	/* Bits 8 - 23 */
        *head = (ucb->ucb$l_media.lbn >> 24) & 0x000F;	/* Bits 24 - 27 */
          }

    else

      {							/* CSH mode... */
        int temp;
        *sec  = ucb->ucb$l_media.lbn % baseucb.ucb$b_sectors + 1;
        temp  = ucb->ucb$l_media.lbn / baseucb.ucb$b_sectors;
        *head = temp % baseucb.ucb$b_tracks;
        *cyl  = temp / baseucb.ucb$b_tracks;
          }

      }



/* FILL_PACKET_W_ADX - This routine is used to fill the address field
 *                     in the packet based on the logical blck number
 *
 * Input:
 *      ucb     pointer to the UCB
 *
 * Output:
 *      packet  certain field in the packet are filled-in
 *
 *
 * Notes:
 *
 *   o Packets always use SCSI-style (~LBA) addressing
 *
 *   o In packets, the MSB comes first
 *
 */

int fill_packet_w_adx( DQ_UCB *ucb )

  {

    int cd_rom_lbn;
    int offset;
    BYTE  *packet;				/* The packet bytes within the UCB */


    packet = (BYTE *) ucb->ucb$b_packet;	/* Bind onto packet in the UCB as a byte array */

    cd_rom_lbn = ucb->ucb$l_media.lbn;		/* Get desired LBN */
    offset     = 0;				/* Assume no offset in buffer */

    if (ucb->ucb$l_2K_flag)			/* 2K blocks on this device? */
      {						/* If so, then... */
        offset     = cd_rom_lbn & 0x03;		/* Calculate offset within buffer */
        cd_rom_lbn = cd_rom_lbn>>2;		/* Then divide LBN by 4 */
          }

    packet[2] = (cd_rom_lbn >> 24) & 0x00FF;	/* LBN bits [24:31] */
    packet[3] = (cd_rom_lbn >> 16) & 0x00FF;	/* LBN bits [16:23] */
    packet[4] = (cd_rom_lbn >>  8) & 0x00FF;	/* LBN bits  [8:15] */
    packet[5] = (cd_rom_lbn      ) & 0x00FF;	/* LBN bits  [0:7]  */

    return( offset );				/* Return the offset, if any */

      }



/* LOAD_PRDT - This routine is used to load the PRDT with 8 pointers
 *             pointing to the 8 PCI pages that map through to our
 *             transfer buffer.
 *
 * Input:
 *      ucb        pointer to the UCB
 *
 * Output:
 *      none
 *
 *
 * Note:
 *
 *   Right now, we just load the PRDT with (essentially) static data;
 *   this could easily be done at unit_init_fork time. But when we get
 *   to the point of doing direct DMA, this routine could get a lot
 *   more sophisticated (see the "More PRDT Fun" note in the beginning
 *   comments). At that point, the work would definitely need to be done
 *   at the point where it is done now, namely, the beginning of each
 *   transfer.
 *
 */

void load_prdt( DQ_UCB *ucb )

  {

    int   i;					/* Loop counter */
    int   page_base_adx;			/* Starting address of this PCI page */
    PRDT  *prdt_tbl;				/* PRDT table */


    prdt_tbl = (PRDT *) ucb->ucb$l_prdt;	/* Bind onto the PRDT as a vector of PRDT entries         */
    page_base_adx = (UINT) ucb->ucb$l_xfer_phy;	/* Get the beginning PCI address of the xfer_buffer       */

    for ( i=0; i<XFER_BUFFER_MAP_PAGES; i++ )	/* Now, for each of the 8 pages in our transfer buffer... */
      {
        prdt_tbl[i].phys_adx = page_base_adx;	/* Load the physical address field                        */
        prdt_tbl[i].count    = MMG$GL_PAGE_SIZE;/* Load the bytecount field with 8K bytes                 */
        prdt_tbl[i].flags    = 0x0;		/* Clear the End-of-Table marker in this PRDT entry       */
        page_base_adx += prdt_tbl[i].count;	/* Bump the PCI address onwards for the next pass         */
          }					/* Next page                                              */

    prdt_tbl[i-1].flags    = DMA_PRDT_M_EDT;	/* Set the End-of-Table marker in the last PRDT entry     */

      }



/* MOVE_SEC_FROM_DRIVE - This routine is used to move a sector from the
 * disk drive on a READ operation.
 *
 * Input:
 *      ucb        pointer to the UCB
 *      buffer     pointer to buffer to place data
 *      bytecount  number of bytesto read from drive
 *
 * Output:
 *      buffer     updated buffer pointer
 *      none
 *
 *
 * Note:
 *
 *   Right now, we just get data words in from the drive as fast
 *   as our little Alpha Processor will let us. Some day, with a
 *   faster processor, it may be necessary to actually poll
 *   whether DRQ is asserted before getting each word in from
 *   the drive.
 *
 *   (DRQ is constantly asserted throughout the duration of this routine.)
 *
 */

void move_sec_from_drive( DQ_UCB *ucb, BYTE *buffer, int bytecount )

  {

    int   i;					/* Loop counter */
    WORD *w_buffer;				/* Point to the buffer as words */


    TRACE( 0x0A000000 + bytecount );		/* MOVE_SEC_FROM_DRIVE starting */

    w_buffer = (WORD *) buffer;			/* Bind onto the buffer as words */

    for (i=0; i<(bytecount>>1); i++)		/* For all the requested words... */
      {
						/* Future DRQ test goes here */
        w_buffer[i] = inpw( ucb, RD_DATA );	/* Get and enbuffer the data word */
          }

      }



/* MOVE_SEC_TO_DRIVE - This routine is used to move a sector to the
 * disk drive on a WRITE operation.
 *
 * Input:
 *      ucb        pointer to the UCB
 *      buffer     pointer to buffer to read data
 *      bytecount  number of bytes to write to drive.
 *
 * Output:
 *      buffer     updated buffer pointer
 *      none
 *
 *
 * Note:
 *
 *   Right now, we just push data words out to the drive as fast
 *   as our little Alpha Processor will let us. Some day, with a
 *   faster processor, it may be necessary to actually poll
 *   whether DRQ is asserted before pushing each word out to
 *   the drive.
 *
 *   (DRQ is constantly asserted throughout the duration of this routine.)
 *
 */

void move_sec_to_drive( DQ_UCB *ucb, BYTE *buffer, int bytecount )

  {

    int   i;					/* Loop counter */
    volatile int   j;				/* Delay counter */
    WORD *w_buffer;				/* Point to the buffer as words */


    TRACE( 0x0B000000 + bytecount );		/* MOVE_SEC_TO_DRIVE starting */

    w_buffer = (WORD *) buffer;			/* Bind onto the buffer as words */

    for (i=0; i<(bytecount>>1); i++)		/* For all the requested words... */
      {
						/* Future DRQ test goes here */
        outw( ucb, WT_DATA, w_buffer[i] );	/* Write out the data word */

//#saythis "Delay kludge for Brick..."
        for (j=0; j<100; j++)			/* Kill some time */
          {}

          }

      }



/* MAP_USER_BUFFER - this routine is used to directly map a section of
 * the users buffer.
 *
 * Input:
 *      ucb     pointer to the UCB
 *      offset  offset to start of buffer to be mapped
 *      length  total length to map
 *
 * Output:
 *      user_va
 *      returned address of mapped buffer
 *
 */

BYTE *map_user_buffer( DQ_UCB *ucb, int offset, int length )
        
  {

    int     pfn;			   	/* PFN */
    int     first_pte;				/* First PTE number */
    int     pte_cnt;				/* Number of pages to map */
    int     i;					/* Loop counter */
    int     byte_ofs;				/* Byte offset */
    PTE     *user_pte;				/* Current PTE pointer */
    BYTE    *s0_va;				/* Current S0 address */
    PTE     *s0_pte;				/* Current S0 PTE address */
    BYTE    *user_va;				/* Mapped buffer address */
    uint64  *clr_pte;				/* Pointer used to clear PTE */

#define PTE_BITS PTE$C_KOWN + PTE$C_KW + PTE$M_VALID + PTE$M_ASM

/* Calculate sizes, base PTE addresses and such */

    offset += baseucb.ucb$l_boff;		/* Compute true offset from page */
    byte_ofs = offset & mmg$gl_bwp_mask;	/* Compute byte offset in page */
    first_pte = (offset >> MMG$GL_VPN_TO_VA) * PTE$C_BYTES_PER_PTE;
						/* Compute PTE offset */
    pte_cnt = ( ( (offset & mmg$gl_bwp_mask) + length) + mmg$gl_bwp_mask) >> MMG$GL_VPN_TO_VA;
						/* Compute page count */
    user_pte = (PTE *) ( (int) baseucb.ucb$l_svapte + first_pte);
						/* Compute first PTE address */
    s0_va   = ucb->ucb$ps_s0_va;		/* S0 address of mapped region */
    s0_pte  = ucb->ucb$ps_s0_svapte;		/* Get S0 PTE address */

/* Loop over all of the PTEs and set them to double map the user buffer */

    for (i=0; i<pte_cnt; i++)
      {
        if (user_pte->pte$v_valid)		/* Check for VALID user PTE */
            pfn = user_pte->pte$v_pfn;		/* It is - get copy of PFN */
        else
            pfn = ioc_std$ptetopfn( user_pte );	/* Find PFN the hard way */

/* The following should be set field by field, but PTEDEF
 * doesn't have a proper definition for this, and frankly
 * it's a pain !  So, define some bits and use them directly.
 *
 *
 *      s0_pte->pte$v_own = PTE$C_KOWN;		/@ Owner = Kernel @/
 *      s0_pte->pte$v_prot = PTE$C_KW;		/@ Protection = Kernel Write @/
 *      s0_pte->pte$v_valid = 1;		/@ Valid page @/
 *      s0_pte->pte$v_asm = 1;			/@ Address space match @/
 */

        clr_pte = (void *) s0_pte;		/* Point to the PTE */
        *clr_pte = PTE_BITS;			/* Clear the PTE and set constant bits */
        s0_pte->pte$v_pfn = pfn;		/* Now, include the PFN */
        mmg$tbi_single( s0_va );		/* Invalidate the address */
        s0_va += MMG$GL_PAGE_SIZE;		/* Point to the next page */
        s0_pte++;				/* Point to next S0 PTE */
        user_pte++;				/* Point to next user PTE */
          }

/* Now, make a guard page */

    clr_pte = (void *) s0_pte;			/* Get PTE address */
    *clr_pte= 0;				/*  and clear it */
    mmg$tbi_single( s0_va );			/* Invalidate the address */

/* Return the S0 VA of the user buffer */

    user_va = (BYTE *) ( (int) ucb->ucb$ps_s0_va + byte_ofs );
    return( user_va );				/* Return with the address */

      }



/* UNLOAD - Perform IO$_UNLOAD driver function
 *
 * Input:
 *      ucb     pointer to UCB
 *
 * Output:
 *      status value
 *              SS$_NORMAL -- function completed successfully
 *
 */

int unload( DQ_UCB *ucb )

  {

    baseucb.ucb$v_valid = 0;			/* Clear the VALID bit */
    return( SS$_NORMAL );			/* Return with success */

      }



/* WAIT_READY - Wait Until The Drive Is Ready.  This means that the
 * BSY status bit is clear and the DRDY status bit is set.
 *
 * Input:
 *      ucb     pointer to UCB
 *
 * Output:
 *      status value
 *              SS$_NORMAL ----- function completed successfully
 *              SS$_DEVACTIVE -- BUSY never cleared
 *
 * Note:
 *
 *   If DRV_HD is currently pointing at a non-existent drive (because,
 *   perhaps, the user last tried to mount a non-existent device),
 *   all the registers (including ALT_STS) tend to say "0xFF". Rather
 *   than wait forever for this fake "busy" signal to clear, we just
 *   barge ahead and select the drive we really want to talk to.
 *
 */

int wait_ready( DQ_UCB *ucb )

  {

    int   status;				/* Routine status value */
    int   drvsts;				/* Drive status register */
    int   cyl_hi;				/* High-order byte of cylinder address */
    int   cyl_lo;				/* Low-order byte of cylinder address */


    TRACE( 0x03100000 );			/* WAIT_READY starting */

    drvsts = inp( ucb, RD_ALT_STS );		/* Get the status byte */
    if (drvsts!=0xFF)				/* If it looks like a real drive, then */
      {
        if ( IS_SET( drvsts, STS_M_BSY ) )	/* Is the drive busy? */
          {					/* If so, then... */
            status = wait_busy( ucb );		/* Make sure BUSY is clear on the current drive */
            if ( $FAIL( status ) )		/* Check status for error */
              {
                TRACE( 0x03110000 );		/* WAIT_BUSY failed for WAIT_READY before drive selection */
                return( status );		/* Exit with the error code */
                  }
              }
          }

    out( ucb, WT_DRV_HD, ucb->ucb$l_drv_head );	/* Select the drive we really want (and head 0) */

    drvsts = inp( ucb, RD_ALT_STS );		/* Get the status byte */
    if (drvsts==0xFF)				/* If it looks like a real drive, then */
      {
        TRACE( 0x03120000 );			/* WAIT_READY trying to select an apparently-nonexistent drive */
        return( SS$_DEVOFFLINE );		/* Exit with the error code */
          }

    if ( IS_SET( drvsts, STS_M_BSY ) )		/* Is the newly-selected drive busy? */
      {						/* If so, then... */
        status = wait_busy( ucb );		/* Make sure BUSY is clear on this drive, too */
        if ( $FAIL( status ) )			/* Check status for error */
          {
            TRACE( 0x03130000 );		/* WAIT_BUSY failed for WAIT_READY after drive selection */
            return( status );			/* Exit with the error code */
              }
          }

    if ( IS_SET( drvsts, STS_M_DRDY ) )		/* Check for drive READY */
      {
        TRACE( 0x03140000 );			/* WAIT_READY succeeding */
        return( SS$_NORMAL );			/* Return succeeding if ready */
          }

						/* Collect the other two pieces of the ATAPI signature */
    cyl_hi = inp( ucb, RD_CYL_HI );		/* Read high order cylinder bits */
    cyl_lo = inp( ucb, RD_CYL_LO );		/* Read low order cylinder bits */
    if ( (drvsts==ATAPI_SIG_STS) && (cyl_hi==ATAPI_SIG_CYL_HI) && (cyl_lo==ATAPI_SIG_CYL_LO) )
      {
        TRACE( 0x03150000 );			/* WAIT_READY barging ahead on ATAPI signature */
        return( SS$_NORMAL );			/* If we see ATAPI signature, barge ahead w/o ready */
          }

    if ( (drvsts==ATAPI_SIG_STSE) && (cyl_hi==ATAPI_SIG_CYL_HI) && (cyl_lo==ATAPI_SIG_CYL_LO) )
      {
        TRACE( 0x03160000 );			/* WAIT_READY barging ahead on ATAPI signature (w/ error bit) */
        return( SS$_NORMAL );			/* If we see ATAPI signature, barge ahead w/o ready */
          }

    TRACE( 0x03170000 );			/* WAIT_READY failing on non-ready drive */
    return( SS$_DEVACTIVE );			/* Otherwise, exit with failure if */
						/*   not ready and not ATAPI signature */
      }



/* WAIT_BUSY - Wait for BSY to be clear
 *
 * Input:
 *      ucb     pointer to UCB
 *
 * Output:
 *      status value
 *
 */

int wait_busy( DQ_UCB *ucb )

  {

    int      status;				/* Routine status value */
    int     drvsts;				/* Drive status register */
    __int64  delta_time;			/* Timedwait delta time */
    __int64  end_value;				/* Timedwait end value */

/* Check to see if the drive is ready right now */

    TRACE( 0x03200000 );			/* WAIT_BUSY starting */

    drvsts = inp( ucb, RD_ALT_STS );		/* Get the status byte */
    if ( IS_CLEAR( drvsts, STS_M_BSY ) )	/* If not busy, then */
      {
        TRACE( 0x03210000 );			/* WAIT_BUSY normal exit -- was already not-busy */
        return( SS$_NORMAL );			/* Drive is ready - exit */
          }

/* Drive is busy - wait a bit for it */

/* Set up the timedwait */

    delta_time = DRQ_TIME;			/* Set DRQ wait time */
    status = exe$timedwait_setup( &delta_time, &end_value );
    if ( $FAIL( status) )			/* Check for success */
      {
        TRACE( 0x03220000 );			/* WAIT_BUSY exe$timedwait_setup failure */
        return( status );			/* Return with the failure status */
          }

/* Spin until ready or timeout */

    while ( ( status=exe$timedwait_complete( &end_value ) ) == SS$_CONTINUE)
      {
        drvsts = inp( ucb, RD_ALT_STS );	/* Read status byte */
        if ( IS_CLEAR( drvsts, STS_M_BSY ) )	/* Check for it to be clear */
          {
            TRACE( 0x03230000 );		/* WAIT_BUSY "became not-busy" success */
            return( SS$_NORMAL );		/* BUSY is clear - exit */
              }
          }

/* Ok - still not ready.  Let's reset the controller and try again */

    BPTRACE( 0x03240000 );			/* BREAK: wait_busy wants to do reset */
    reset_ctrl( ucb );				/* Attempt a reset */
    drvsts = inp( ucb, RD_ALT_STS );		/* Get the status byte */
    if ( IS_CLEAR( drvsts, STS_M_BSY ) )	/* If not busy, then */
      {
        TRACE( 0x03250000 );			/* WAIT_BUSY "became not-busy" after reset */
        return( SS$_NORMAL );			/*  return with success */
          }
    else
      {
        TRACE( 0x03260000 );			/* WAIT_BUSY "still busy" after reset failure */
        return( SS$_CTRLERR );			/* Exit with controller error */
          }
      }



/* WAIT_DRQ - Wait for DRQ to be set and BSY to be clear
 *
 * Input:
 *      ucb     pointer to UCB
 *
 * Output:
 *      status value
 *
 */

int wait_drq( DQ_UCB *ucb )

  {

    int      status;				/* Routine status value */
    int      drvsts;				/* Drive status register */
    __int64  delta_time;			/* Timedwait delta time */
    __int64  end_value;				/* Timedwait end value */

/* Check to see if the drive is ready right now */

    TRACE( 0x03300000 );			/* WAIT_DRQ starting */

    drvsts = inp( ucb, RD_ALT_STS );		/* Get the status byte */
    if ( IS_CLEAR( drvsts, STS_M_BSY ) )	/* Is the drive busy? */
      {						/* If not, then... */
        if ( IS_SET( drvsts, STS_M_DRQ ) )	/*  get the DRQ bit */
          {
            TRACE( 0x03310000 );		/* WAIT_DRQ normal exit -- was already DRQ */
            return( SS$_NORMAL );		/* Drive is ready and DRQ is set -- exit */
              }
          }

/* Drive is busy or DRQ not set - wait a bit for it */

/* Set up the timedwait */

    delta_time = DRQ_TIME;			/* Set DRQ wait time */
    status = exe$timedwait_setup( &delta_time, &end_value );
    if ( $FAIL( status ) )			/* Check for success */
      {
        TRACE( 0x03320000 );			/* WAIT_DRQ exe$timedwait_setup failure */
        return( status );			/* Return with the failure status */
          }

/* Spin until ready or timeout */

    while ( (status=exe$timedwait_complete( &end_value ) ) == SS$_CONTINUE)
      {
        drvsts = inp( ucb, RD_ALT_STS );	/* No, so read status byte */
        if ( IS_CLEAR( drvsts, STS_M_BSY ) )
          {
            if ( IS_SET( drvsts, STS_M_DRQ ) )
              {
                TRACE( 0x03330000 );		/* WAIT_DRQ "became ready" success */
                return( SS$_NORMAL );		/* Looks ok - set new status */
                  }
              }
          }

    TRACE( 0x03340000 );			/* WAIT_DRQ ending with TIMEOUT waiting for DRQ */
    return( status );				/* Return with status code */

      }




/* DQ_WFIKPCH - Wait for Interrupt and Keep Channel (opionally w/Histogram)
 *
 * This routine is a jacket around the normal ioc$kp_wfikpch.
 * This routine may also keep the time completion histogram up to date.
 *
 * Input:
 *      kpb        pointer to the KPB
 *      orig_ipl   IPL to restore to when releasing the device lock
 *      erl_param  An arbitrary parameter passed from our caller
 *                 which will be passed onto dumpreg if we timeout.
 *                 We only use the param to identify (for posterity)
 *                 who called us.
 *
 * Output:
 *      status value
 *
 */

int dq_wfikpch( KPB *kpb, int orig_ipl, int erl_param )

  {

    DQ_UCB      *ucb;				/* Pointer to UCB */
    int         status;				/* Returned routine status */
    extern int  EXE$GL_ABSTIM;			/* Current time (seconds) */
    int         time;				/* Starting time, later, Elapsed time */


    ucb    = (DQ_UCB *) kpb->kpb$ps_ucb;	/* Get UCB pointer */

    TRACE( 0x03400000 );			/* DQ_WFIKPTCH starting */

    inp( ucb, RD_ALT_STS );			/* Get the status byte (just for tracing's benefit) */

    if (ucb->ucb$l_unsolicited_int!=0)		/* Is an unsolicited interrupt already pending? */
      {						/* If so, bypass WFIKPCH, etc. */
        TRACE( 0x03410000 );			/* DQ_WFIKPCH taking the pending-unsolicited-interrupt bypass */
        ucb->ucb$l_unsolicited_int = 0;		/* Forget any pending unsolicited interrupts */
        inp( ucb, RD_STS );			/* Read status byte to quash any pending interrupts */
        device_unlock( baseucb.ucb$l_dlck, orig_ipl, SMP_RESTORE );
						/* Release the device lock */
        return( SS$_NORMAL );			/* And return succeeding */
          }

						/* Else we'll wait for an interrupt... */

    time = EXE$GL_ABSTIM;			/* Get the current time */
    status = ioc$kp_wfikpch( kpb, TIMEOUT_TIME, orig_ipl );
    time = EXE$GL_ABSTIM - time;		/* Calculate elapsed time */
    TRACE( 0x03420000 + time );			/* IOC$KP_WFIKPTCH end */
    ucb->ucb$l_int_hist[time]++;		/* Bump a histogram entry */

    if (status == SS$_TIMEOUT)			/* Interrupt timeout? */
      {						/* If so, then... */
        ucb->ucb$l_int_tmo++;			/* Bump the explicit timeout indicator */
        erl_std$devictmo( erl_param, (UCB *) ucb );
						/* Handle the device timeout */
        exe$kp_fork( ucb->ucb$ps_kpb, (FKB *) ucb );
						/* Fork, and... */
        BPTRACE( 0x03430000 );			/* BREAK: WFIKPCH timeout */
        return( status );			/* Return with status intact */
          }

    if ( $FAIL( status ) )			/* Any other WFIKPCH error? */
      {						/* If so, then... */
        device_unlock( baseucb.ucb$l_dlck, orig_ipl, SMP_RESTORE );
        BPTRACE( 0x03440000 );			/* BREAK: Non-timeout WFIKPCH error */
        return( status );			/*  and return with status */
          }

						/* All is well after the 'rupt... */

    ucb->ucb$l_unsolicited_int = 0;		/* Forget any pending unsolicited interrupts */
    status = exe$kp_fork( ucb->ucb$ps_kpb, (FKB *) ucb );
						/* Drop back to fork IPL */
    return( status );				/* and return with status */

      }



/* RESET_CTRL - Reset the controller
 *
 * This routine issues a RESET to the controller. It does this for
 * ATA devices by using the RESET bit and for ATAPI devices by using
 * the ATAPI Software REset command.
 *
 * It then waits for the BUSY bit to clear.  If the BUSY bit isn't
 * cleared within a certain time, we decide that the controller is dead.
 *
 * Input:
 *      ucb     pointer to UCB
 *
 * Output:
 *      status value
 *              SS$_NORMAL --- successful reset
 *              SS$_CTRLERR -- controller failed to RESET
 *
 */

int reset_ctrl( DQ_UCB *ucb )

  {

    int   orig_ipl;				/* Original IPL */
    int   status;				/* Routine status value */
    int   drv_head;				/* Drive drive/head register */
    int   drvsts;				/* Drive status register */
    int   loop;					/* Loop counter */


    TRACE( 0x00070000 );			/* RESET_CTRL starting */
    ucb->ucb$l_resets++;			/* Count a reset issued by us */

    device_lock( baseucb.ucb$l_dlck, RAISE_IPL, &orig_ipl );
						/* Take out the device lock */

    if (ucb->ucb$l_atapi_flag==0)		/* ATAPI flag clear? */
      {						/* If so, ATA RESET -- Use reset bit */
        out( ucb, WT_DEV_CTL, (CTL_M_SRST | CTL_M_nIEN) );
						/* Set the Reset + no_ints bits */
        out( ucb, WT_DEV_CTL, 0x00 );		/* Cear the Reset + no_ints bits  */
          }
    else
      {						/* ATAPI RESET -- Use reset command */

        drv_head = ucb->ucb$l_drv_head;		/* Get base drive info */
        if (ucb->ucb$l_drive_lba_capable)	/* If LBA mode, ... */
            drv_head |= DRVHD_M_LBA;		/*  ... set the LBA bit */
        out( ucb, WT_DRV_HD, drv_head );	/* Select drive, ignore head */
        out( ucb, WT_CMD, CMD_ATA_ATAPI_SOFT_RESET );
          }					/* Issue the ATAPI reset command */

    device_unlock( baseucb.ucb$l_dlck, orig_ipl, SMP_RESTORE );
						/* And release the device lock */

    for (loop=0; loop<RESET_TIME>>1; loop++)	/* Now wait for a few seconds... */
      {
        status = sleep( ucb, 2 );		/* Sleep a bit (up to two seconds) */
						/* (First sleep allows drive to go busy) */
        if ( $FAIL( status ) )			/* Check the KP status */
            return( status );			/* Failed - exit w/error */
        drvsts = inp( ucb, RD_ALT_STS );	/* Get the status byte */
        if ( IS_CLEAR( drvsts, STS_M_BSY ) )	/* If not busy, then */
            return( SS$_NORMAL );		/* Drive is ready - exit */
          }

    return( SS$_CTRLERR );			/* It never became ready again -- punt */

      }



/* SLEEP - Kill some time
 *
 *
 * Usage:
 *      SLEEP (seconds)
 *
 * Input:
 *      ucb      pointer to UCB
 *      seconds  seconds to sleep
 *
 * Output:
 *      none
 *
 * Return value:
 *      status of the exe$kp_fork_wait call
 *
 */

int sleep( DQ_UCB *ucb, int seconds )

  {

    int loop;
    int status;


    TRACE( 0x00080000 + seconds );		/* SLEEP starting */

    for (loop=0; loop<seconds; loop++)
      {
        status = exe$kp_fork_wait( ucb->ucb$ps_kpb, (FKB *) ucb );
        if ( $FAIL( status ) )			/* Check the KP status */
            return( status );			/* Failed - exit w/error */
          }

    return( SS$_NORMAL );

     }



/* ISR - Interrupt Service Routine
 *
 *
 *
 * Usage:
 *      ISR (idb)
 *
 * Input:
 *      idb     pointer to IDB
 *
 * Output:
 *      none
 *
 * Return value:
 *      none
 *
 */

void isr( IDB *idb )

  {

    DQ_UCB *ucb;				/* Pointer to the UCB */
    int    dummy;				/* Place to dump STATUS */


/* Get pointer to the UCB;  If null, then there is none and we just exit */

    ucb = (DQ_UCB *) idb->idb$ps_owner;		/* Get UCB address from the IDB */
    if (ucb == NULL)
        return;					/* Unowned and unexpected - dismiss */
    ucb->ucb$l_total_ints++;			/* Increment interrupt count */

/* There's an owner.  If the interrupt is expected, then restart the KP */

    device_lock( baseucb.ucb$l_dlck, NORAISE_IPL, NOSAVE_IPL );
						/* Acquire the device lock */

    if (baseucb.ucb$v_int)			/* Is this an expected interrupt?            */
      {						/* If so, then...                            */
        TRACE( 0x0E000000 );			/* Expected interrupt                        */
        baseucb.ucb$v_int = 0;			/* Clear "interrupt expected"                */
        baseucb.ucb$v_tim = 0;			/* Clear TIMEOUT expected bit                */
        fork( (void (*)()) exe$kp_restart, ucb->ucb$ps_kpb, SS$_NORMAL, ucb );
          }					/* Fork off a routine to restart the stalled */
						/*   mainline kernel process                 */
    else					/* Else unexpected interrupt...              */
      {						/*                                           */
        TRACE( 0x0E100000 );			/* Unexpected interrupt!                     */
        ucb->ucb$l_unsolicited_int = 1;		/* An unsolicited interrupt is now pending   */
        ucb->ucb$l_unsol_ints++;		/* Increment unsolicited interrupt count     */
          }

    dummy = inp( ucb, RD_STS );			/* Read STATUS to acknowledge the interrupt  */

    device_unlock( baseucb.ucb$l_dlck, NOLOWER_IPL, SMP_RESTORE );
						/* Release the device lock */
    TRACE( 0x0E200000 );			/* ISR ending */
    return;					/* Return to the interrupt dispatcher */

      }



/* INP - This routine is used to read a byte from a CSR.
 *
 * Input:
 *      ucb     pointer to the UCB
 *      reg     register index
 *
 * Output:
 *      none
 *
 * Return value:
 *      byte of data read from the CSR
 *
 */

BYTE inp( DQ_UCB *ucb, int reg )

  {

    CRAM  *cram_ptr;				/* Pointer to CRAM */
    int   status;				/* Routine status */
    BYTE  data;					/* Data byte */

    cram_ptr = ucb->ucb$ps_crams[reg];		/* Point to the CRAM */
    status   = ioc$cram_io( cram_ptr );		/* Read the byte */
    data = (cram_ptr->cram$q_rdata >> cram_init[reg].shift) & 0xFF;

    TRACE( 0x05000000 + (reg<<16) + data );	/* Byte read */

    return( data );				/* Return the value */

      }



/* INPW - This routine is used to read a word from a CSR.
 *
 * Input:
 *      ucb     pointer to the UCB
 *      reg     register index
 *
 * Output:
 *      none
 *
 * Return value:
 *      word of data read from the CSR
 *
 */

WORD inpw( DQ_UCB *ucb, int reg )

  {

    CRAM  *cram_ptr;				/* Pointer to the CRAM */
    int   status;				/* Routine status value */
    WORD  data;					/* Data value */

    cram_ptr = ucb->ucb$ps_crams[reg];		/* Point to CRAM */
    status   = ioc$cram_io( cram_ptr );		/* Read the word */
    data = cram_ptr->cram$q_rdata >> cram_init[reg].shift & 0xFFFF;

#ifdef TRACE_DATA_TOO
    TRACE( 0x05400000 + (reg<<16) + data );	/* Word read */
#endif

    return( data );				/* Send back the data */

      }



/* OUT - This routine is used to write a byte to a CSR.
 *
 * Input:
 *      ucb     pointer to the UCB
 *      reg     register index
 *      data    data byte to be written to the CSR
 *
 * Output:
 *      none
 *
 */

void out( DQ_UCB *ucb, int reg, BYTE data )

  {

    CRAM  *cram_ptr;				/* Pointer to the CRAM */
    int   status;				/* Returned status */

    cram_ptr = ucb->ucb$ps_crams[reg];		/* Get correct CRAM */
    cram_ptr->cram$q_wdata = data << cram_init[reg].shift;
						/* Position data */
    status   = ioc$cram_io( cram_ptr );		/* Perform the write */

    TRACE( 0x06000000 + (reg<<16) + data );	/* Byte written */

      }



/* OUTW - This routine is used to write a word of data to a CSR.
 *
 * Input:
 *      ucb     pointer to the UCB
 *      reg     register index
 *      data    data word to be written to the CSR
 *
 * Output:
 *      none
 *
 */

void outw( DQ_UCB *ucb, int reg, WORD data )

  {

    CRAM  *cram_ptr;				/* Pointer to CRAM */
    int   status;				/* Routine status */

    cram_ptr = ucb->ucb$ps_crams[reg];		/* Point to the CRAM */
    cram_ptr->cram$q_wdata = data << cram_init[reg].shift;
						/* Position the data */
    status   = ioc$cram_io( cram_ptr );		/* Write the word */

#ifdef TRACE_DATA_TOO
    TRACE( 0x06400000 + (reg<<16) + data );	/* Word written */
#endif

      }



/* OUTW_T - This routine is used to write a word of ATAPI packet data to a CSR.
 *
 * Input:
 *      ucb     pointer to the UCB
 *      reg     register index
 *      data    data word to be written to the CSR
 *
 * Output:
 *      none
 *
 *
 * Note:
 *
 *   This routine only exists because it provides an unconditional
 *   trace of the outw() calls that write the ATAPI packet, even if
 *   TRACE_DATA_TOO isn't defined.
 *
 */

void outw_t( DQ_UCB *ucb, int reg, WORD data )

  {

    CRAM  *cram_ptr;				/* Pointer to CRAM */
    int   status;				/* Routine status */

    cram_ptr = ucb->ucb$ps_crams[reg];		/* Point to the CRAM */
    cram_ptr->cram$q_wdata = data << cram_init[reg].shift;
						/* Position the data */
    status   = ioc$cram_io( cram_ptr );		/* Write the word */

    TRACE( 0x06800000 + (reg<<16) + data );	/* ATAPI packet word written */

      }

/* OUTL - This routine is used to write a longword of data to a CSR.
 *
 * Input:
 *      ucb     pointer to the UCB
 *      reg     register index
 *      data    data to be written to the CSR
 *
 * Output:
 *      none
 *
 */

void outl( DQ_UCB *ucb, int reg, int data )

  {

    CRAM  *cram_ptr;				/* Pointer to CRAM */
    int   status;				/* Routine status */

    cram_ptr = ucb->ucb$ps_crams[reg];		/* Point to the CRAM */
    cram_ptr->cram$q_wdata = data << cram_init[reg].shift;
						/* Position the data */
    status   = ioc$cram_io( cram_ptr );		/* Write the word */

#ifdef TRACE_DATA_TOO
    TRACE( 0x06440000 + (reg<<16) + data );	/* Word written */
#endif

      }

/*+++
 * DV_QSRV_HELPER - Process QIOServer Control and Complex I/O Functions
 *
 * Description
 *
 *	This routine is called by QIOServer's kernal mode components based upon
 *	the settings of various QIOServer related IRP$L_STS2 bits.  The relationship
 *	between these bits and the actions taken are as follows:
 *
 *		On the client side:
 *
 *			- Before sending the IRP to the server:
 *
 *				If IRP$V_QSVD is set, called with the
 *				QSRV_HLPR_ACT$C_C_PREP action flag; otherwise
 *				not called.  For DQ, it is requested only
 *				for IO$_PACKACK handling.  It generates the
 *				intermediate buffer required to pass the
 *				UCB field data back.
 *
 *			- After IRP is returned from the server:
 *
 *				If IRP$V_QSVD is set, called with the
 *				QSRV_HLPR_ACT$C_C_POST action flag; otherwise
 *				not called.  For DQ, it is requested only
 *				for IO$_PACKACK handling.  It updates the
 *				client UCB fields from data passed back in the
 *				intermediate buffer, and then deletes the 
 *				buffer.
 *
 *			- Any time QIOServer modifications need to be backed out,
 *			  such as connection loss and possible path switch:
 *
 *				If any QIOServer bit is set, called with the
 *				QSRV_HLPR_ACT$C_C_CLNUP action flag; otherwise
 *				not called.  For DQ, this causes the
 *				intermediate buffer to be de-allocated and the
 *				IRP returned to its natural state so that it
 *				could be re-queued to a non-QIOServer path.
 *
 *		On the server side:
 *
 *			- Before sending the IRP to the server's driver:
 *
 *				If IRP$V_QRQT_SRVR_HLPR is set, called with the
 *				QSRV_HLPR_ACT$C_S_PREP action flag; otherwise
 *				not called.  For DQ, it is requested only
 *				for IO$_PACKACK handling.  If the
 *				IO$M_QSRV_CLIENT_UPDATE flag is set, then it
 *				causes the IRP to skip the actual driver call.
 *
 *			- After IRP is returned from the server's driver:
 *
 *				If IRP$V_QRQT_SRVR_HLPR is set, called with the
 *				QSRV_HLPR_ACT$C_S_POST action flag; otherwise
 *				not called.  For DQ, it is requested only
 *				for IO$_PACKACK handling.  It loads the
 *				intermediate data with the server side UCB's
 *				state.
 *
 *			- Any time QIOServer modifications need to be backed out,
 *			  such as connection loss and possible path switch:
 *
 *				If any QIOServer bit is set, called with the
 *				QSRV_HLPR_ACT$C_S_CLNUP action flag; otherwise
 *				not called.
 *
 * Inputs:
 *
 *	ACTION FLAG
 *	IRP address
 *	address of I/O status 1
 *	address of I/O status 2
 *
 * Outputs:
 *
 *	None
 *
 * Implicit Outputs:
 *
 *	- As directed by the action flag
 *
 * Returns:
 *
 *	SS$_NORMAL	- Processing has been completed and KClient can
 *			  finish the request.
 *
 *	SS$_INUSE	- The request will be completed later by this
 *			  routine.  KClient is relieved of any further
 *			  responsibility for this request.
 *
 * N.B.
 *
 *	On the client side, the helper routine may take complete control of
 *	the IRP by returning the SS$_INUSE condition code.  If a fork is
 *	required, then the routine must return the SS$_INUSE condition code
 *	and handle any other further action.  The QIOServer kernel client
 *	will delete any references to the IRP.  Re-initiating the IRP will
 *	send it back through the QIOServer control stream.
 *
 *	On the server side, there is no ability to take complete control of
 *	the IRP and no fork processing is allowed.
 *---
 */

int dq_qsrv_helper(	int action,
			IRP *irp,
			uint32 *iost1,
			uint32 *iost2 )
    {
    BUFIO		*buffer;
    DT_UCB		*dt_ucb;
    DTN			*dtn;
    QSRV_PACKACK	*qp;
    DTN			*scratch_dtn;
    __int32		size;
    unsigned int	status;
    UCB			*ucb;


    switch ( action )
        {
        case QSRV_HLPR_ACT$C_C_PREP:

	    if ( irp->irp$v_qcntrl )
		{
		if ( irp->irp$v_fcode == IO$_PACKACK )
		    {

		    /* Allocate a non-paged pool buffer for the PACKACK return data */

		    status = exe_std$alononpaged( BUFIO$K_HDRLEN32 + QSRV_PACKACK$C_LENGTH, &size, ( void * ) &buffer );
		    if ( $FAIL( status ) )
			{
			*iost1 = SS$_INSFMEM;
			*iost2 = 0;
			return ( SS$_INSFMEM );
			}

		    /* Initialize the buffer */

		    irp->irp$l_boff = size;
		    buffer->bufio$w_size = size;
		    irp->irp$ps_bufio_pkt = buffer;
		    buffer->bufio$ps_uva32 = NULL;
		    buffer->bufio$b_type = DYN$C_BUFIO;
		    buffer->bufio$ps_pktdata = ( char * ) buffer + BUFIO$K_HDRLEN32;
		    qp = ( QSRV_PACKACK * ) buffer->bufio$ps_pktdata;
		    qp->qsrv_packack$l_controls = 0;
		    if ( irp->irp$v_bufio )
			qp->qsrv_packack$v_orig_bufio = 1;
		    irp->irp$v_bufio = 1;
		    irp->irp$l_bcnt = QSRV_PACKACK$C_LENGTH;
		    }
		}
	    return ( SS$_NORMAL );
	    break;

        case QSRV_HLPR_ACT$C_C_POST:

	    if ( irp->irp$v_qcntrl )
		{
		if ( irp->irp$v_fcode == IO$_PACKACK )
		    {

		    /* If this is an IO$_PACKACK, then set things up */

		    ucb = irp->irp$l_ucb;
		    dt_ucb = ( DT_UCB * ) irp->irp$l_ucb;
		    if ( irp->irp$ps_bufio_pkt != 0 )
			{
			buffer = irp->irp$ps_bufio_pkt;
			qp = ( QSRV_PACKACK * ) buffer->bufio$ps_pktdata;
			*iost1 &= 0xffff;
			*iost2 = 0;
			irp->irp$v_bufio = 0;
			if ( qp->qsrv_packack$v_orig_bufio )
			    irp->irp$v_bufio = 1;
			if ( qp->qsrv_packack$v_data_valid )
			    {

			    /* If we have valid data, then update the UCB */

			    ucb->ucb$b_devclass = qp->qsrv_packack$b_devclass;
			    ucb->ucb$b_devtype = qp->qsrv_packack$b_devtype;
			    ucb->ucb$q_devdepend = qp->qsrv_packack$q_devdepend;
			    ucb->ucb$q_devdepend2 = qp->qsrv_packack$q_devdepend2;
			    ucb->ucb$l_devsts = qp->qsrv_packack$l_devsts;
			    ucb->ucb$l_media_id = qp->qsrv_packack$l_media_id;
			    dt_ucb->ucb$l_maxblock = qp->qsrv_packack$l_maxblock;
			    dt_ucb->ucb$l_maxbcnt = qp->qsrv_packack$l_maxbcnt;
			    dt_ucb->ucb$l_alloclass = qp->qsrv_packack$l_alloclass;
			    dtn = ( DTN * ) &qp->qsrv_packack$r_dtn;
			    if ( ( ucb->ucb$l_devchar2 & DEV$M_DTN ) != 0 )
				ioc$remove_device_type( ucb );
			    if ( ( qp->qsrv_packack$l_devchar2 & DEV$M_DTN ) != 0 )
				ioc$add_device_type( &dtn->dtn$t_dtname_str[0],
						     dtn->dtn$ib_dtname_len,
						     ucb,
						     &scratch_dtn );
			    ucb->ucb$l_sts &= ~(	UCB$M_ONLINE|
							UCB$M_VALID);
			    qp->qsrv_packack$l_sts &= (	UCB$M_ONLINE|
							UCB$M_VALID);
			    ucb->ucb$l_sts |= qp->qsrv_packack$l_sts;
			    }
			exe_std$deanonpaged( irp->irp$ps_bufio_pkt );
			irp->irp$ps_bufio_pkt = NULL;
			irp->irp$l_boff = irp->irp$l_bcnt = 0;
			}
		    return ( SS$_NORMAL );
		    }
		return ( SS$_NORMAL );
		}
	    return ( SS$_NORMAL );

        case QSRV_HLPR_ACT$C_C_CLNUP:

	    if ( irp->irp$v_fcode == IO$_PACKACK )
		{

		/* If this is an IO$_PACKACK, then set things up */

		if ( irp->irp$ps_bufio_pkt != 0 )
		    {
		    buffer = irp->irp$ps_bufio_pkt;
		    qp = ( QSRV_PACKACK * ) buffer->bufio$ps_pktdata;
		    irp->irp$v_bufio = 0;
		    if ( qp->qsrv_packack$v_orig_bufio )
			irp->irp$v_bufio = 1;
		    exe_std$deanonpaged( irp->irp$ps_bufio_pkt );
		    irp->irp$ps_bufio_pkt = NULL;
		    irp->irp$l_boff = irp->irp$l_bcnt = 0;
		    }
		}
	    return ( SS$_NORMAL );

        case QSRV_HLPR_ACT$C_S_PREP:

	    if ( irp->irp$v_qcntrl )
		{
		if ( irp->irp$v_func == IO$_PACKACK &&
		     ( irp->irp$l_func & IO$M_QSRV_CLIENT_UPDATE ) != 0 )
		    {
		    *iost1 = SS$_NORMAL;
		    *iost2 = 0;
		    return ( 0 );
		    }
		}
	    return ( SS$_NORMAL );

        case QSRV_HLPR_ACT$C_S_POST:

	    if ( irp->irp$v_qcntrl )
		{
		if ( irp->irp$v_fcode == IO$_PACKACK )
		    {
		    ucb = irp->irp$l_ucb;
		    dt_ucb = ( DT_UCB * ) irp->irp$l_ucb;
		    if ( irp->irp$ps_bufio_pkt != 0 )
			{
			buffer = irp->irp$ps_bufio_pkt;
			qp = ( QSRV_PACKACK * ) buffer->bufio$ps_pktdata;
			qp->qsrv_packack$v_data_valid = 1;
			qp->qsrv_packack$l_devchar2 = ucb->ucb$l_devchar2;
			qp->qsrv_packack$b_devclass = ucb->ucb$b_devclass;
			qp->qsrv_packack$b_devtype = ucb->ucb$b_devtype;
			qp->qsrv_packack$q_devdepend = ucb->ucb$q_devdepend;
			qp->qsrv_packack$q_devdepend2 = ucb->ucb$q_devdepend2;
			qp->qsrv_packack$l_devsts = ucb->ucb$l_devsts;
			qp->qsrv_packack$l_sts = ucb->ucb$l_sts;
			qp->qsrv_packack$l_media_id = ucb->ucb$l_media_id;
			qp->qsrv_packack$l_maxblock = dt_ucb->ucb$l_maxblock;
			qp->qsrv_packack$l_maxbcnt = dt_ucb->ucb$l_maxbcnt;
			qp->qsrv_packack$l_alloclass = dt_ucb->ucb$l_alloclass;
			if ( ( ucb->ucb$l_devchar2 & DEV$M_DTN ) != 0 )
			    dtn = ( DTN * ) &qp->qsrv_packack$r_dtn;
			    memcpy( &dtn->dtn$t_dtname,
				    &( ( DTN * )( ucb->ucb$ps_dtn ) )->dtn$t_dtname,
				    sizeof ( ( ( DTN * )( ucb->ucb$ps_dtn ) )->dtn$t_dtname ) );
			*iost1 &= 0xffff;
			*iost1 |= QSRV_PACKACK$C_LENGTH<<16;
		        *iost2 = 0;
			}
		    }
		}
	    return ( SS$_NORMAL );
        case QSRV_HLPR_ACT$C_S_CLNUP:

	    return ( SS$_NORMAL );
        }
    }
