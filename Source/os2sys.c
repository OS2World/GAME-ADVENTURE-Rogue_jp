/*
 * os2sys.c - OS/2 System Dependent Module
 *
 *  Implement part of 'machdep.c' module.
 *
 *      KBD and SCR related functions are moved to
 *          os2kbd.c
 *          os2cur.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "rogue.h"

/*
 * md_getcwd:
 *
 * get full pathname of current directory and place it into dir.
 * maximum length of pathname is given as len.
 * return dir on success, or NULL on failure.
 * OS2 version get also drive spec.
 */

char *md_getcwd(char *dir, int len)
{
    return _getcwd2(dir, len) ;
}

/*
 * md_chdir:
 *
 * change directory to dir.  also change drive in MSDOS environment.
 * return 0 on success, or -1 on failure.
 * OS2 version also changes drive.
 */

int md_chdir(char *dir)
{
    return _chdir2(dir) ;
}

/* md_get_file_id():
 *
 * This function returns an integer that uniquely identifies the specified
 * file.  It need not check for the file's existence.  In UNIX, the inode
 * number is used.
 *
 * This function need not be implemented.  To stub the routine, just make
 * it return 0.  This will make the game less able to prevent users from
 * modifying saved-game files.  This is probably no big deal.
 */

int     md_get_file_id(void)
{
    return 0 ;
}

/* md_gct(): (Get Current Time)
 *
 * This function returns the current year, month(1-12), day(1-31), hour(0-23),
 * minute(0-59), and second(0-59).  This is used for identifying the time
 * at which a game is saved.
 *
 * This function is not strictly necessary.  It can be stubbed by returing
 * zeros instead of the correct year, month, etc.  If your operating
 * system doesn't provide all of the time units requested here, then you
 * can provide only those that it does, and return zeros for the others.
 * If you cannot provide good time values, then users may be able to copy
 * saved-game files and play them.  
 */

int     md_gct(struct rogue_time *rt_buf)
{
    struct tm *t, *localtime() ;
    long seconds ;

    time(&seconds);
    t = localtime(&seconds);

    rt_buf->year   = t->tm_year    ;
    rt_buf->month  = t->tm_mon + 1 ;
    rt_buf->day    = t->tm_mday    ;
    rt_buf->hour   = t->tm_hour    ;
    rt_buf->minute = t->tm_min     ;
    rt_buf->second = t->tm_sec     ;
    
    return 0 ;
}

/* md_gfmt: (Get File Modification Time)
 *
 * This routine returns a file's date of last modification in the same format
 * as md_gct() above.
 *
 * This function is not strictly necessary.  It is used to see if saved-game
 * files have been modified since they were saved.  If you have stubbed the
 * routine md_gct() above by returning constant values, then you may do
 * exactly the same here.
 * Or if md_gct() is implemented correctly, but your system does not provide
 * file modification dates, you may return some date far in the past so
 * that the program will never know that a saved-game file being modified.  
 * You may also do this if you wish to be able to restore games from
 * saved-games that have been modified.
 */

int     md_gfmt(char *fname, struct rogue_time *rt_buf)
{
    struct stat sbuf ;
    long seconds ;
    struct tm *t ;

    stat(fname, &sbuf) ;
    seconds = (long) sbuf.st_mtime ;
    t = localtime(&seconds) ;

    rt_buf->year   = t->tm_year    ;
    rt_buf->month  = t->tm_mon + 1 ;
    rt_buf->day    = t->tm_mday    ;
    rt_buf->hour   = t->tm_hour    ;
    rt_buf->minute = t->tm_min     ;
    rt_buf->second = t->tm_sec     ;
    
    return 0 ;
}

/* md_df: (Delete File)
 *
 * This function deletes the specified file, and returns true (1) if the
 * operation was successful.  This is used to delete saved-game files
 * after restoring games from them.
 *
 * Again, this function is not strictly necessary, and can be stubbed
 * by simply returning 1.  In this case, saved-game files will not be
 * deleted and can be replayed.
 */

boolean md_df(char *fname)
{
    if (unlink(fname)) {
        return 0 ;
    } else {
        return 1 ;
    }
}

/* md_getenv()
 *
 * This routine gets certain values from the user's environment.  These
 * values are strings, and each string is identified by a name.  The names
 * of the values needed, and their use, is as follows:
 *
 *   TERMCAP
 *     The name of the users's termcap file, NOT the termcap entries
 *     themselves.  This is used ONLY if the program is compiled with
 *     CURSES defined (-DCURSES).  Even in this case, the program need
 *     not find a string for TERMCAP.  If it does not, it will use the
 *     default termcap file as returned by md_gdtcf();
 *   TERM
 *     The name of the users's terminal.  This is used ONLY if the program
 *     is compiled with CURSES defined (-DCURSES).  In this case, the string
 *     value for TERM must be found, or the routines in curses.c cannot
 *     function, and the program will quit.
 *   ROGUEOPTS
 *     A string containing the various game options.  This need not be
 *     defined.
 *   HOME
 *     The user's home directory.  This is only used when the user specifies
 *     '~' as the first character of a saved-game file.  This string need
 *     not be defined.
 *
 * If your system does not provide a means of searching for these values,
 * you will have to do it yourself.  None of the values above really need
 * to be defined except TERM when the program is compiled with CURSES
 * defined.  In this case, as a bare minimum, you can check the 'name'
 * parameter, and if it is "TERM" find the terminal name and return that,
 * else return zero.  If the program is not compiled with CURSES, you can
 * get by with simply always returning zero.  Returning zero indicates
 * that their is no defined value for the given string.
 */

char *md_getenv(char *name)
{
    return getenv(name) ;
}

/* md_malloc()
 *
 * This routine allocates, and returns a pointer to, the specified number
 * of bytes.  This routines absolutely MUST be implemented for your
 * particular system or the program will not run at all.  Return zero
 * when no more memory can be allocated.
 */

char *md_malloc(int n)
{
    return malloc(n) ;
}

/* md_gln: (Get login name)
 *
 * This routine returns the login name of the user.  This string is
 * used mainly for identifying users in score files.
 *
 * A dummy string may be returned if you are unable to implement this
 * function, but then the score file would only have one name in it.
 */

char    *md_gln(void)
{
    char *t ;

    if ((t = md_getenv("USER")) == NULL) {
#ifdef JAPAN
    	t = "êÌém";
#else
	t = "Fighter";
#endif
    }
    return (t);
}

/* md_sleep:
 *
 * This routine causes the game to pause for the specified number of
 * seconds.
 *
 * This routine is not necessary at all, and can be stubbed with no ill
 * effects.
 */

int     md_sleep(int nsecs)
{
    (void) sleep(nsecs) ;
    return 0 ;
}

/* md_gseed() (Get Seed)
 *
 * This function returns a seed for the random number generator (RNG).  This
 * seed causes the RNG to begin generating numbers at some point in it's
 * sequence.  Without a random seed, the RNG will generate the same set
 * of numbers, and every game will start out exactly the same way.  A good
 * number to use is the process id, given by getpid() on most UNIX systems.
 *
 * You need to find some single random integer, such as:
 *   process id.
 *   current time (minutes + seconds) returned from md_gct(), if implemented.
 *   
 * It will not help to return "get_rand()" or "rand()" or the return value of
 * any pseudo-RNG.  If you don't have a random number, you can just return 1,
 * but this means your games will ALWAYS start the same way, and will play
 * exactly the same way given the same input.
 */

int     md_gseed(void)
{
    return getpid() ;
}

/* md_exit():
 *
 * This function causes the program to discontinue execution and exit.
 * This function must be implemented or the program will continue to
 * hang when it should quit.
 */

int     md_exit(int status)
{
    if (org_dir && *org_dir) {
	md_chdir(org_dir) ;
    }
    exit(status) ;
    return 0 ;
}


int     sound_bell(void)
{
    DosBeep(440, 50) ;
}
