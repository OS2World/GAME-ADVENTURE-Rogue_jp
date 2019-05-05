/*
 * main.c
 *
 * This source herein may be modified and/or distributed by anybody who
 * so desires, with the following restrictions:
 *    1.)  No portion of this notice shall be removed.
 *    2.)  Credit shall not be taken for the creation of this source.
 *    3.)  This code is not to be traded, sold, or used for personal
 *         gain or profit.
 *
 */

#include "rogue.h"

extern short party_room;
#ifndef ORIGINAL
extern char *nick_name;
#endif

main(argc, argv)
int argc;
char *argv[];
{
#ifndef ORIGINAL
	int first = 1;
	char buf[80];
#endif

	if (init(argc, argv)) {		/* restored game */
		goto PL;
	}

	for (;;) {
		clear_level();
		make_level();
		put_objects();
		put_stairs();
		add_traps();
		put_mons();
		put_player(party_room);
		print_stats(STAT_ALL);
#ifndef ORIGINAL
		if (first) {
			sprintf(buf,
#ifdef JAPAN
				"Ç‚Ç†ÅA%sÅB â^ñΩÇÃì¥åAÇ÷ÇÊÇ§Ç±Çª...",
#else
				"Hello %s, welcome to the Dungeons of Doom...",
#endif
				nick_name);
			message(buf, 0);
		}
	PL:
		first = 0;
#else
	PL:
#endif
		play_level();
		free_stuff(&level_objects);
		free_stuff(&level_monsters);
	}
}
