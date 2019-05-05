/*
 * os2kbd.c - KBD control functions for JROGUE
 */

#define INCL_BASE
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "rogue.h"

int     os2getchar(void)
{
    KBDKEYINFO  key ;
    
    KbdCharIn(&key, 0, 0) ;

    return (int) key.chChar ;
}

/* md_control_keyboard():
 *
 * This routine is much like md_cbreak_no_echo_nonl() below.  It sets up the
 * keyboard for appropriate input.  Specifically, it prevents the tty driver
 * from stealing characters.  For example, ^Y is needed as a command
 * character, but the tty driver intercepts it for another purpose.  Any
 * such behavior should be stopped.  This routine could be avoided if
 * we used RAW mode instead of CBREAK.  But RAW mode does not allow the
 * generation of keyboard signals, which the program uses.
 *
 * The parameter 'mode' when true, indicates that the keyboard should
 * be set up to play rogue.  When false, it should be restored if
 * necessary.
 *
 * This routine is not strictly necessary and may be stubbed.  This may
 * cause certain command characters to be unavailable.
 */

static  KBDINFO     KeyModeNew = { sizeof(KBDINFO), 0, 0 } ;

#define ECHO_MASK   0x0003
#define ECHO_ON     0x0001
#define ECHO_OFF    0x0002

int md_control_keyboard(boolean mode)
{
    KbdGetStatus(&KeyModeNew, 0) ;
    KeyModeNew.fsMask &= ~ECHO_MASK ;
    if (mode) {
	KeyModeNew.fsMask |= ECHO_OFF ;
    } else {
	KeyModeNew.fsMask |= ECHO_ON  ;
    }
    KbdSetStatus(&KeyModeNew, 0) ;
}

/* md_slurp:
 *
 * This routine throws away all keyboard input that has not
 * yet been read.  It is used to get rid of input that the user may have
 * typed-ahead.
 *
 * This function is not necessary, so it may be stubbed.  The might cause
 * message-line output to flash by because the game has continued to read
 * input without waiting for the user to read the message.  Not such a
 * big deal.
 */

int md_slurp(void)
{
    return KbdFlushBuffer(0) ;
}

/* md_heed_signals():
 *
 * This routine tells the program to call particular routines when
 * certain interrupts/events occur:
 *
 *      SIGINT: call onintr() to interrupt fight with monster or long rest.
 *      SIGQUIT: call byebye() to check for game termination.
 *      SIGHUP: call error_save() to save game when terminal hangs up.
 *
 *		On VMS, SIGINT and SIGQUIT correspond to ^C and ^Y.
 *
 * This routine is not strictly necessary and can be stubbed.  This will
 * mean that the game cannot be interrupted properly with keyboard
 * input, this is not usually critical.
 */

int     md_heed_signals(void)
{
    signal(SIGINT, onintr)  ;
    signal(SIGQUIT, byebye) ;
    return 0 ;
}

/* md_ignore_signals():
 *
 * This routine tells the program to completely ignore the events mentioned
 * in md_heed_signals() above.  The event handlers will later be turned on
 * by a future call to md_heed_signals(), so md_heed_signals() and
 * md_ignore_signals() need to work together.
 *
 * This function should be implemented or the user risks interrupting
 * critical sections of code, which could cause score file, or saved-game
 * file, corruption.
 */

int     md_ignore_signals(void)
{
    signal(SIGQUIT, SIG_IGN) ;
    signal(SIGINT, SIG_IGN)  ;
    return 0 ;
}

int     md_reset_signals(void)
{
    signal(SIGQUIT, SIG_DFL) ;
    signal(SIGINT, SIG_DFL)  ;
    return 0 ;
}

