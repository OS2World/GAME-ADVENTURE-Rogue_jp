/*
 * os2cur.c - curses emulation for OS/2
 */

#ifdef  CURSES

#define INCL_BASE
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include "rogue.h"

/*
 * Buffer for Screen Data
 */
#ifdef USE_COLOR
    /*
     * for COLOR, upper 8 bit is color attributes and lower 8 bit
     * contains char code.  
     *
     * NOTE :
     *      color attribute is not comatible with OS/2 VIO.
     */
short terminal[DROWS][DCOLS];
short buffer[DROWS][DCOLS];
#define STANDOUT    0x0800
#define BASECOLOR   0x0700
#define ALLCOLOR    0x0f00
#define NOCOLOR     0x00ff
#define DEFCOLOR    0x0000      /* Default Color */
#else
    /*
     * for non-COLOR, 8-th bit contains standout flag
     */
char terminal[DROWS][DCOLS];
char buffer[DROWS][DCOLS];
#define ST_MASK 0x80
#endif

boolean screen_dirty;
boolean lines_dirty[DROWS];
#ifdef USE_COLOR
short buf_attr = DEFCOLOR ;
short term_attr = 0;
extern boolean do_color;
#else
boolean buf_stand_out = 0;
boolean term_stand_out = 0;
#endif

int LINES = DROWS, COLS = DCOLS;
WINDOW scr_buf;
WINDOW *curscr = &scr_buf;

short cur_row, cur_col;

static  void    os2clear(void)   ;
static  void    os2refresh(void) ;
static  void    put_cursor(int row, int col) ;
static  void    put_st_char(int ch) ;

int     refresh(void)
{
    int i, j ;
    short old_row, old_col ;
#ifdef USE_COLOR
    short *bp, *tp;
#else
    char *bp, *tp;
#endif

    if (screen_dirty) {
	old_row = curscr->_cury ;
	old_col = curscr->_curx ;
	for (i = 0; i < DROWS; i++) {
            if (lines_dirty[i]) {
		bp = buffer[i]   ;
		tp = terminal[i] ;
		for (j = 0; j < DCOLS; j++) {
	            if (*bp != *tp) {
		        put_cursor(i, j) ;
			put_st_char(*tp++ = *bp++);
			cur_col++;
		    } else {
			bp++;
			tp++;
		    }
		}
		lines_dirty[i] = 0;
	    }
	}
	put_cursor(old_row, old_col);
	screen_dirty = 0;
    }
    os2refresh() ;
    return 0 ;
}

int     wrefresh(WINDOW *scr)
{
    short i ;
    short col ;
#ifdef USE_COLOR
    short *p ;
#else
    char *p;
#endif

    os2clear() ;
    cur_row = cur_col = 0;

    for (i = 0; i < DROWS; i++) {
	p = buffer[i];
	col = 0;
	while (col < DCOLS) {
            while ((col < DCOLS) && (p[col] == ' ')) {
		col++;
	    }
	    if (col < DCOLS) {
		put_cursor(i, col);
	    }
	    while ((col < DCOLS) && (p[col] != ' ')) {
		put_st_char((int) p[col++]);
		cur_col++;
	    }
	}
    }
    put_cursor(curscr->_cury, curscr->_curx);
    os2refresh() ;
    return 0 ;
}

int     move(short row, short col)
{
    curscr->_cury = row ;
    curscr->_curx = col ;
    screen_dirty = 1 ;
    return 0 ;
}

#ifdef USE_COLOR
int     addch(int ch)
{
    short   row, col ;

    if (ch == '\t') {
    	col = curscr->_curx % 8 ;
	while (col++ != 8) {
	    addch(' ') ;
	}
	return ;
    }
    row = curscr->_cury   ;
    col = curscr->_curx++ ;
    if (buf_attr && (ch & 0xff00) == 0) {
	ch |= buf_attr ;
    }
    if (!do_color) {
	ch &= ~BASECOLOR ;
    }
    buffer[row][col] = (short) ch ;
    lines_dirty[row] = 1 ;
    screen_dirty = 1 ;
}
#else
int     addch(int ch)
{
    short   row, col ;

    if (ch == '\t') {
	col = curscr->_curx % 8 ;
	while (col++ != 8) {
            addch(' ') ;
        }
	return ;
    }
    row = curscr->_cury   ;
    col = curscr->_curx++ ;
    if (buf_stand_out) {
    	ch |= ST_MASK ;
    }
    buffer[row][col] = (char) ch ;
    lines_dirty[row] = 1 ;
    screen_dirty = 1 ;
}
#endif

int     addstr(char *str)
{
    if (str == NULL) {
        return 0 ;
    }
    while (*str) {
        addch((int) (*str++ & 0xff)) ;
    }
}

int     mvaddch(short row, short col, int ch)
{
    move(row, col) ;
    addch(ch) ;
}

int     mvaddstr(short row, short col, char *str)
{
    move(row, col) ;
    addstr(str) ;
}

int     mvinch(short row, short col)
{
    move(row, col) ;
#ifdef USE_COLOR
    return((int) buffer[row][col] & NOCOLOR) ;
#else
    return((int) buffer[row][col]) ;
#endif
}
    
int     clear_buffers(void)
{
    int i, j ;
#ifdef USE_COLOR
    short *bufp, *termp ;
#else
    char *bufp, *termp ;
#endif

    screen_dirty = 0 ;

    for (i = 0 ; i < DROWS ; i++) {
	lines_dirty[i] = 0 ;
	bufp = buffer[i] ;
	termp = terminal[i] ;
	for (j = 0 ; j < DCOLS ; j++) {
	    *bufp++ = *termp++ = ' ' ;
	}
    }
    return 0 ;
}

int     clear(void)
{
    os2clear() ;

    cur_row = cur_col = 0 ;
    move(0, 0) ;
    clear_buffers() ;
    return 0 ;
}

int     clrtoeol(void)
{
    short row, col ;

    row = curscr->_cury;

    for (col = curscr->_curx ; col < DCOLS ; col++) {
	buffer[row][col] = ' ' ;
    }
    lines_dirty[row] = 1 ;
}

#ifdef USE_COLOR
int     repaint_screen(void)
{
    int     col, row ;
    short   ch ;

    for (row = MIN_ROW ; row < DROWS - 1 ; row++) {
	for (col = 0 ; col < DCOLS ; col++) {
            if ((ch = mvinch(row, col)) != ' ') {
		mvaddch(row, col, colored(ch)) ;
            }
        }
    }
    refresh() ;
    return 0 ;
}
#endif

#ifdef USE_COLOR
int     color(short col)
{
    buf_attr = (col << 8) ;
}

int     standout(void)
{
    buf_attr |= STANDOUT ;
}

int     standend(void)
{
    buf_attr = DEFCOLOR ;
}
#else
int     standout(void)
{
    buf_stand_out = 1 ;
}

int     standend(void)
{
    buf_stand_out = 0 ;
}
#endif

#ifdef JAPAN
int     touch(int row, int bcol, int ecol)
{
    int i ;

    for (i = bcol ; i <= ecol ; i++) {
	terminal[row][i] = 0 ;
    }
    lines_dirty[row] = 1 ;
    return 0 ;
}
#endif /*JAPAN*/

/*
 * Here, OS/2 Specific Drawing Functions
 */

static  VIOMODEINFO     ScrModeSav ;

static  VIOCURSORINFO   CursSav ;
static  VIOCURSORINFO   CursOn  ;
static  VIOCURSORINFO   CursOff ;

static  KBDINFO     KeyModeSav = { sizeof(KBDINFO), 0, 0 } ;

static  PUSHORT LvbBase ;
static  USHORT  LvbLeng ;
static  USHORT  LvbTop  ;
static  USHORT  LvbBot  ;

static  int     CurRow, CurCol ;

int     initscr(void)
{
    KbdGetStatus(&KeyModeSav, 0) ;

    /*
     * Get Current Screen Size 
     */

    ScrModeSav.cb = sizeof(ScrModeSav) ;
    VioGetMode(&ScrModeSav, 0) ;
    LINES = ScrModeSav.row ;
    COLS  = ScrModeSav.col ;

    /*
     * Cursor Control
     */

    VioGetCurType(&CursSav, 0) ;
    memcpy(&CursOn,  &CursSav, sizeof(VIOCURSORINFO)) ;
    memcpy(&CursOff, &CursSav, sizeof(VIOCURSORINFO)) ;
#if 0
    CursOff.attr = -1 ;
    VioSetCurType(&CursOff, 0) ;
#endif

    /*
     * Clear Screen 
     */
    os2clear() ;

    /*
     * Setup Logical Video Buffer
     */

    VioGetBuf((PULONG) &LvbBase, &LvbLeng, 0) ;
    LvbBase = (PUSHORT) _emx_16to32((_far16ptr) LvbBase) ;
    
    LvbTop = LvbLeng ;
    LvbBot = 0 ;

    return 0 ;
}

int     crmode(void)
{
    return 0 ;
}

int     noecho(void)
{
    return 0 ;
}

int     nonl(void)
{
    return 0 ;
}

int     endwin(void)
{
    /*
     * Restore Cursor
     */
    VioSetCurType(&CursSav, 0) ;
    VioSetCurPos(LINES - 1, 0, 0) ;

    /*
     * Restore Kbd Mode
     */
    KbdSetStatus(&KeyModeSav, 0) ;
    
    return 0 ;
}

static  USHORT  os2attr[] = {
/* WHITE    */  0x7000,
/* RED      */  0x7400,
/* GREEN    */  0x7200,
/* YELLOW   */  0x7e00,
/* BLUE     */  0x7100,
/* MAGENTA  */  0x7500,
/* CYAN     */  0x7300,
/* BLACK    */  0x7800,
/* RWHITE   */  0x7f00,
/* RRED     */  0x7c00,
/* RGREEN   */  0x7a00,
/* RYELLOW  */  0x7e00,
/* RBLUE    */  0x7900,
/* RMAGENTA */  0x7d00,
/* RCYAN    */  0x7b00,
/* RBLACK   */  0x7800
} ;

static  void    os2clear(void)
{
    USHORT  space = 0x7020 ;
    VioScrollUp(0, 0, -1, -1, -1, (PUCHAR) &space, 0) ;
    VioSetCurPos(0, 0, 0) ;
}

static  void    os2refresh(void)
{
    USHORT  from, leng ;
    
    if (LvbTop < LvbBot) {
        from = LvbTop * 2 ;
	leng = (LvbBot - LvbTop + 1) * 2 ;
	VioShowBuf(from, leng, 0) ;
    }
    VioSetCurPos(CurRow, CurCol, 0) ;
    LvbTop = LvbLeng ;
    LvbTop = 0 ;
}

static  void    put_cursor(int row, int col)
{
    CurRow = cur_row = row ;
    CurCol = cur_col = col ;
    VioSetCurPos(row, col, 0) ;
}

static  void    put_st_char(int ch)
{
    USHORT  pos, cell ;

#ifdef  USE_COLOR
    cell = os2attr[(ch & ALLCOLOR) >> 8] ;
    cell |= (ch & NOCOLOR) ;
#else
    cell = (ch & ST_MASK) ? os2attr[RWHITE] : os2attr[WHITE] ;
    cell |= (ch & 0x7f) ;
#endif
    pos = CurCol + COLS * CurRow ;
    LvbTop = LvbTop < pos ? LvbTop : pos ;
    LvbBase[pos] = cell ;
    LvbBot = LvbBot > pos ? LvbBot : pos ;
    CurCol += 1 ;
}

#endif  /* CURSES */
