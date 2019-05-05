/*
 * message.c
 *
 * This source herein may be modified and/or distributed by anybody who
 * so desires, with the following restrictions:
 *    1.)  No portion of this notice shall be removed.
 *    2.)  Credit shall not be taken for the creation of this source.
 *    3.)  This code is not to be traded, sold, or used for personal
 *         gain or profit.
 *
 */

#include <stdio.h>
#include "rogue.h"

#define	CTRL(c)	((c) & 037)

char msg_line[DCOLS] = "";
short msg_col = 0;
boolean msg_cleared = 1;
char hunger_str[8] = "";

extern boolean cant_int, did_int, interrupted, save_is_interactive;
extern short add_strength;
extern short cur_level;

message(msg, intrpt)
char *msg;
boolean intrpt;
{
	if (!save_is_interactive) {
		return;
	}
	if (intrpt) {
		interrupted = 1;
		md_slurp();
	}
	cant_int = 1;

	if (!msg_cleared) {
		mvaddstr(MIN_ROW-1, msg_col, MORE);
		refresh();
		wait_for_ack();
		check_message();
	}
	(void) strcpy(msg_line, msg);
	mvaddstr(MIN_ROW-1, 0, msg);
	addch(' ');
	refresh();
	msg_cleared = 0;
	msg_col = strlen(msg);

	cant_int = 0;
	if (did_int) {
		did_int = 0;
		onintr();
	}
}

remessage()
{
	if (msg_line[0]) {
		message(msg_line, 0);
	}
}

check_message()
{
	if (msg_cleared) {
		return;
	}
	move(MIN_ROW-1, 0);
	clrtoeol();
	refresh();
	msg_cleared = 1;
}

get_direction()
{
	int dir;

#ifdef JAPAN
	message("ï˚äpÇÕÅH", 0);
#else
	message("Direction?", 0);
#endif
	while (!is_direction(dir = rgetchar()))
		sound_bell();
	check_message();
	return dir;
}

get_input_line(prompt, insert, buf, if_cancelled, add_blank, do_echo)
char *prompt, *buf, *insert, *if_cancelled;
boolean add_blank, do_echo;
{
	int n;

	n = do_input_line(1, 0, 0, prompt, insert,
			buf, if_cancelled, add_blank, do_echo, 0);
	return ((n < 0)? 0: n);
}

input_line(row, col, insert, buf, ch)
int row, col, ch;
char *insert, *buf;
{
	return (do_input_line(0, row, col, "", insert, buf, "", 0, 1, ch));
}

do_input_line(is_msg, row,col, prompt, insert, buf, if_cancelled, add_blank, do_echo, first_ch)
boolean is_msg;
int row, col;
char *prompt, *buf, *insert, *if_cancelled;
boolean add_blank, do_echo;
int first_ch;
{
	short ch;
	short i = 0, n = 0;
#ifdef JAPAN
	short k;
	char kanji[MAX_TITLE_LENGTH];
#endif

	if (is_msg) {
		message(prompt, 0);
		n = strlen(prompt) + 1;
	} else
		mvaddstr(row, col, prompt);

	if (insert[0]) {
		mvaddstr(row, col+n, insert);
		(void) strcpy(buf, insert);
		i = strlen(insert);
#ifdef JAPAN
		k = 0;
		while (k < i) {
			ch = insert[k];
			if (ch>=0x81 && ch<=0x9f || ch>=0xe0 && ch<=0xfc) {
				kanji[k] = kanji[k+1] = 1;
				k += 2;
			} else {
				kanji[k] = 0;
				k++;
			}
		}
#endif
		move(row, col+n+i);
		refresh();
	}

#ifdef JAPAN
	for (;;) {
		if (first_ch) {
			ch = first_ch;
			first_ch = 0;
		} else
			ch = rgetchar();
		if (ch == '\r' || ch == '\n' || ch == CANCEL)
			break;
		if ((ch == '\b') && (i > 0)) {
			i -= kanji[i-1]? 2: 1;
			if (do_echo) {
				mvaddstr(row, col+n+i, "  ");
				move(row, col+n+i);
			}
		} else if ((ch>=' ' && ch<='~' || ch >= 0xa0 && ch <= 0xde)
						&& (i < MAX_TITLE_LENGTH-2)) {
			if ((ch != ' ') || (i > 0)) {
				buf[i] = ch;
				kanji[i] = 0;
				if (do_echo)
					addch(ch);
				i++;
			}
		} else if ((ch>=0x81 && ch<=0x9f || ch>=0xe0 && ch<=0xfc)
						&& (i < MAX_TITLE_LENGTH-3)) {
			buf[i] = ch;
			buf[i+1] = rgetchar();
			kanji[i] = kanji[i+1] = 1;
			if (do_echo) {
				addch(buf[i]);
				addch(buf[i+1]);
			}
			i += 2;
		}
		refresh();
	}
	if (is_msg)
		check_message();
	while ((i > 0) && (buf[i-1] == ' ') && (kanji[i-1] == 0))
		i--;
	if (add_blank)
		buf[i++] = ' ';

#else /*JAPAN*/

	while (((ch = rgetchar()) != '\r') && (ch != '\n') && (ch != CANCEL)) {
		if ((ch >= ' ') && (ch <= '~') && (i < MAX_TITLE_LENGTH-2)) {
			if ((ch != ' ') || (i > 0)) {
				buf[i++] = ch;
				if (do_echo) {
					addch(ch);
				}
			}
		}
		if ((ch == '\b') && (i > 0)) {
			i--;
			if (do_echo) {
				mvaddch(row, col+n+i, ' ');
				move(row, col+n+i);
			}
		}
		refresh();
	}
	if (is_msg)
		check_message();
	if (add_blank) {
		buf[i++] = ' ';
	} else {
		while ((i > 0) && (buf[i-1] == ' ')) {
			i--;
		}
	}
#endif /*JAPAN*/

	buf[i] = 0;

	if ((ch == CANCEL) || (i == 0) || ((i == 1) && add_blank)) {
		if (is_msg && if_cancelled) {
			message(if_cancelled, 0);
		}
		return ((ch == CANCEL)? -1: 0);
	}
	return (i);
}

rgetchar()
{
	register ch;

	for(;;) {
#ifdef  OS2EMX
                ch = os2getchar() ;
#else
		ch = getchar();
#endif
		switch(ch) {
		case '\022':
			wrefresh(curscr);
			break;
#ifdef UNIX_BSD4_2
		case '\032':
			/* putstr(CL); */
			fflush(stdout);
			tstp();
			break;
#endif
#ifndef ORIGINAL
		/*
		 * can't use X for save_screen purpose
		 * because 2nd byte of kanji might be an 'X'!
		 */
		case CTRL('D'):
#else
		case 'X':
#endif
			save_screen();
			break;
		default:
			return(ch);
		}
	}
}
/*
Level: 99 Gold: 999999 Hp: 999(999) Str: 99(99) Arm: 99 Exp: 21/10000000 Hungry
äK: 99 ã‡âÚ: 999999 ëÃóÕ: 999(999) ã≠Ç≥: 99(99) éÁîı: 99 åoå±: 21/10000000 ãÛï†
0    5    1    5    2    5    3    5    4    5    5    5    6    5    7    5
*/

print_stats(stat_mask)
register stat_mask;
{
	char buf[16];
	boolean label;
	int row = DROWS - 1;

	label = (stat_mask & STAT_LABEL) ? 1 : 0;

	if (stat_mask & STAT_LEVEL) {
		if (label) {
#ifdef JAPAN
			mvaddstr(row, 0, "äK: ");
#else
			mvaddstr(row, 0, "Level: ");
#endif
		}
		/* max level taken care of in make_level() */
		sprintf(buf, "%d", cur_level);
#ifdef JAPAN
		mvaddstr(row, 4, buf);
#else
		mvaddstr(row, 7, buf);
#endif
		pad(buf, 2);
	}
	if (stat_mask & STAT_GOLD) {
		if (label) {
			if (rogue.gold > MAX_GOLD) {
				rogue.gold = MAX_GOLD;
			}
#ifdef JAPAN
			mvaddstr(row, 7, "ã‡âÚ: ");
#else
			mvaddstr(row, 10, "Gold: ");
#endif
		}
		sprintf(buf, "%ld", rogue.gold);
#ifdef JAPAN
		mvaddstr(row, 13, buf);
#else
		mvaddstr(row, 16, buf);
#endif
		pad(buf, 6);
	}
	if (stat_mask & STAT_HP) {
		if (label) {
#ifdef JAPAN
			mvaddstr(row, 20, "ëÃóÕ: ");
#else
			mvaddstr(row, 23, "Hp: ");
#endif
			if (rogue.hp_max > MAX_HP) {
				rogue.hp_current -= (rogue.hp_max - MAX_HP);
				rogue.hp_max = MAX_HP;
			}
		}
		sprintf(buf, "%d(%d)", rogue.hp_current, rogue.hp_max);
#ifdef JAPAN
		mvaddstr(row, 26, buf);
#else
		mvaddstr(row, 27, buf);
#endif
		pad(buf, 8);
	}
	if (stat_mask & STAT_STRENGTH) {
		if (label) {
#ifdef JAPAN
			mvaddstr(row, 35, "ã≠Ç≥: ");
#else
			mvaddstr(row, 36, "Str: ");
#endif
		}
		if (rogue.str_max > MAX_STRENGTH) {
			rogue.str_current -= (rogue.str_max - MAX_STRENGTH);
			rogue.str_max = MAX_STRENGTH;
		}
		sprintf(buf, "%d(%d)", (rogue.str_current + add_strength),
			rogue.str_max);
		mvaddstr(row, 41, buf);
		pad(buf, 6);
	}
	if (stat_mask & STAT_ARMOR) {
		if (label) {
#ifdef JAPAN
			mvaddstr(row, 48, "éÁîı: ");
#else
			mvaddstr(row, 48, "Arm: ");
#endif
		}
		if (rogue.armor && (rogue.armor->d_enchant > MAX_ARMOR)) {
			rogue.armor->d_enchant = MAX_ARMOR;
		}
		sprintf(buf, "%d", get_armor_class(rogue.armor));
#ifdef JAPAN
		mvaddstr(row, 54, buf);
#else
		mvaddstr(row, 53, buf);
#endif
		pad(buf, 2);
	}
	if (stat_mask & STAT_EXP) {
		if (label) {
#ifdef JAPAN
			mvaddstr(row, 57, "åoå±: ");
#else
			mvaddstr(row, 56, "Exp: ");
#endif
		}
		/*  Max exp taken care of in add_exp() */
		sprintf(buf, "%d/%ld", rogue.exp, rogue.exp_points);
#ifdef JAPAN
		mvaddstr(row, 63, buf);
#else
		mvaddstr(row, 61, buf);
#endif
		pad(buf, 11);
	}
	if (stat_mask & STAT_HUNGER) {
#ifdef JAPAN
#ifdef CURSES
#ifdef USE_COLOR
		extern short terminal[DROWS][DCOLS];
#else
		extern char terminal[DROWS][DCOLS];
#endif
		terminal[row][75] = ' ';	/* magic cookie */
#endif
		mvaddstr(row, 75, hunger_str);
		clrtoeol();
#else
		mvaddstr(row, 73, hunger_str);
		clrtoeol();
#endif
	}
	refresh();
}

pad(s, n)
char *s;
short n;
{
	short i;

	for (i = strlen(s); i < n; i++) {
		addch(' ');
	}
}

save_screen()
{
	FILE *fp;
	short i, j;
	char buf[DCOLS+2];
	boolean found_non_blank;

#ifdef UNIX
	if ((fp = fopen("rogue.screen", "w")) != NULL) {
#endif
#if defined(MSDOS) || defined(OS2EMX)
	if ((fp = fopen("rogue.scr", "w")) != NULL) {
#endif
		for (i = 0; i < DROWS; i++) {
			found_non_blank = 0;
			for (j = (DCOLS - 1); j >= 0; j--) {
				buf[j] = mvinch(i, j);
				if (!found_non_blank) {
					if ((buf[j] != ' ') || (j == 0)) {
						buf[j + ((j == 0) ? 0 : 1)] = 0;
						found_non_blank = 1;
					}
				}
			}
			fputs(buf, fp);
			putc('\n', fp);
		}
		fclose(fp);
	} else {
		sound_bell();
	}
}

#ifndef OS2EMX
sound_bell()
{
	putchar(7);
	fflush(stdout);
}
#endif

boolean
is_digit(ch)
short ch;
{
	return(boolean)((ch >= '0') && (ch <= '9'));
}

r_index(str, ch, last)
char *str;
int ch;
boolean last;
{
	int i;

	if (last) {
		for (i = strlen(str) - 1; i >= 0; i--) {
			if (str[i] == ch) {
				return(i);
			}
		}
	} else {
		for (i = 0; str[i]; i++) {
			if (str[i] == ch) {
				return(i);
			}
		}
	}
	return(-1);
}
