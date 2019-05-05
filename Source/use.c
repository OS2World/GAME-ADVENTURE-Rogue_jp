/*
 * use.c
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

short halluc = 0;
short blind = 0;
short confused = 0;
short levitate = 0;
short haste_self = 0;
boolean see_invisible = 0;
short extra_hp = 0;
boolean detect_monster = 0;
#ifdef JAPAN
char *strange_feeling = "不思議な気分におそわれたが、すぐに消えていった。";
#else
char *strange_feeling = "You have a strange feeling for a moment, then it passes";
#endif

extern short bear_trap;
extern char hunger_str[];
extern short cur_room;
extern long level_points[];
extern boolean being_held;
extern char *fruit, *you_can_move_again;
extern boolean sustain_strength;

#ifdef JAPAN
quaff()
{
	short ch;
	char buf[80];
	object *obj;

	ch = pack_letter("飲む水薬は？", POTION);
	if (ch == CANCEL) {
		return;
	}
	if (!(obj = get_letter_object(ch))) {
		message("それは持っていない。", 0);
		return;
	}
	if (obj->what_is != POTION) {
		message("それは水薬ではない。", 0);
		return;
	}
	switch(obj->which_kind) {
		case INCREASE_STRENGTH:
			message("何だか、力がわいてくるぞ！", 0);
			rogue.str_current++;
			if (rogue.str_current > rogue.str_max) {
				rogue.str_max = rogue.str_current;
			}
			break;
		case RESTORE_STRENGTH:
			rogue.str_current = rogue.str_max;
			message("これはうまい！ からだがポカポカしてきた。",0);
			break;
		case HEALING:
			message("気分がよくなった！", 0);
			potion_heal(0);
			break;
		case EXTRA_HEALING:
			message("とても気分がよくなった！", 0);
			potion_heal(1);
			break;
		case POISON:
			if (!sustain_strength) {
				rogue.str_current -= get_rand(1, 3);
				if (rogue.str_current < 1) {
					rogue.str_current = 1;
				}
			}
			message("この水薬は毒だった！", 0);
			if (halluc) {
				unhallucinate();
			}
			break;
		case RAISE_LEVEL:
			rogue.exp_points = level_points[rogue.exp - 1];
			add_exp(1, 1);
			break;
		case BLINDNESS:
			go_blind();
			break;
		case HALLUCINATION:
			message("ふにゃ？ 何もかもが虹色にみえるなあ？", 0);
			halluc += get_rand(500, 800);
			break;
		case DETECT_MONSTER:
			show_monsters();
			if (!(level_monsters.next_monster)) {
				message(strange_feeling, 0);
			}
			break;
		case DETECT_OBJECTS:
			if (level_objects.next_object) {
				if (!blind) {
					show_objects();
				}
			} else {
				message(strange_feeling, 0);
			}
			break;
		case CONFUSION:
			message((halluc ? "何だか酔っぱらいみたいな気分だ！"
					: "あれ？ ここはどこ？ 私はだれ？"),0);
			confuse();
			break;
		case LEVITATION:
			message("体が宙に浮いてしまった！", 0);
			levitate += get_rand(15, 30);
			being_held = bear_trap = 0;
			break;
		case HASTE_SELF:
			message("体が素早く動くようになった！", 0);
			haste_self += get_rand(11, 21);
			if (!(haste_self % 2)) {
				haste_self++;
			}
			break;
		case SEE_INVISIBLE:
			sprintf(buf, "う～む、%sジュースのような味がする。",
						fruit);
			message(buf, 0);
			if (blind) {
				unblind();
			}
			see_invisible = 1;
			relight();
			break;
	}
	print_stats((STAT_STRENGTH | STAT_HP));
#ifndef ORIGINAL
	id_potions[obj->which_kind].id_status = IDENTIFIED;
#else
	if (id_potions[obj->which_kind].id_status != CALLED) {
		id_potions[obj->which_kind].id_status = IDENTIFIED;
	}
#endif
	vanish(obj, 1, &rogue.pack);
}

#else /*JAPAN*/

quaff()
{
	short ch;
	char buf[80];
	object *obj;

	ch = pack_letter("Quaff what?", POTION);
	if (ch == CANCEL) {
		return;
	}
	if (!(obj = get_letter_object(ch))) {
		message("No such item.", 0);
		return;
	}
	if (obj->what_is != POTION) {
		message("You can't drink that", 0);
		return;
	}
	switch(obj->which_kind) {
		case INCREASE_STRENGTH:
			message("You feel stronger now, what bulging muscles!", 0);
			rogue.str_current++;
			if (rogue.str_current > rogue.str_max) {
				rogue.str_max = rogue.str_current;
			}
			break;
		case RESTORE_STRENGTH:
			rogue.str_current = rogue.str_max;
			message("This tastes great, you feel warm all over",0);
			break;
		case HEALING:
			message("You begin to feel better", 0);
			potion_heal(0);
			break;
		case EXTRA_HEALING:
			message("You begin to feel much better", 0);
			potion_heal(1);
			break;
		case POISON:
			if (!sustain_strength) {
				rogue.str_current -= get_rand(1, 3);
				if (rogue.str_current < 1) {
					rogue.str_current = 1;
				}
			}
			message("You feel very sick now", 0);
			if (halluc) {
				unhallucinate();
			}
			break;
		case RAISE_LEVEL:
			rogue.exp_points = level_points[rogue.exp - 1];
			add_exp(1, 1);
			break;
		case BLINDNESS:
			go_blind();
			break;
		case HALLUCINATION:
			message("Oh wow, everything seems so cosmic!", 0);
			halluc += get_rand(500, 800);
			break;
		case DETECT_MONSTER:
			show_monsters();
			if (!(level_monsters.next_monster)) {
				message(strange_feeling, 0);
			}
			break;
		case DETECT_OBJECTS:
			if (level_objects.next_object) {
				if (!blind) {
					show_objects();
				}
			} else {
				message(strange_feeling, 0);
			}
			break;
		case CONFUSION:
			message((halluc ? "What a trippy feeling" :
#ifndef ORIGINAL
			"Wait, what's going on here. Huh? What? Who?"), 0);
#else
			"You feel confused"), 0);
#endif
			confuse();
			break;
		case LEVITATION:
			message("You start to float in the air", 0);
			levitate += get_rand(15, 30);
			being_held = bear_trap = 0;
			break;
		case HASTE_SELF:
			message("You feel yourself moving much faster", 0);
			haste_self += get_rand(11, 21);
			if (!(haste_self % 2)) {
				haste_self++;
			}
			break;
		case SEE_INVISIBLE:
			sprintf(buf, "Hmm, this potion tastes like %sjuice",
						fruit);
			message(buf, 0);
			if (blind) {
				unblind();
			}
			see_invisible = 1;
			relight();
			break;
	}
	print_stats((STAT_STRENGTH | STAT_HP));
#ifndef ORIGINAL
	id_potions[obj->which_kind].id_status = IDENTIFIED;
#else
	if (id_potions[obj->which_kind].id_status != CALLED) {
		id_potions[obj->which_kind].id_status = IDENTIFIED;
	}
#endif
	vanish(obj, 1, &rogue.pack);
}
#endif /*JAPAN*/

#ifdef JAPAN
read_scroll()
{
	short ch;
	object *obj;
	char msg[DCOLS];

	ch = pack_letter("読む巻き物は？", SCROL);

	if (ch == CANCEL) {
		return;
	}
	if (!(obj = get_letter_object(ch))) {
		message("それは持っていない。", 0);
		return;
	}
	if (obj->what_is != SCROL) {
		message("それは巻き物ではない。", 0);
		return;
	}
	switch(obj->which_kind) {
		case SCARE_MONSTER:
			message("遠くから、狂ったような笑い声が聞こえてくる。",
			0);
			break;
		case HOLD_MONSTER:
			hold_monster();
			break;
		case ENCH_WEAPON:
			if (rogue.weapon) {
				if (rogue.weapon->what_is == WEAPON) {
					sprintf(msg,
				"手に持った%sが、ほんの少し%s輝きに包まれた。",
					name_of(rogue.weapon),
					get_ench_color());
					message(msg, 0);
					if (coin_toss()) {
						rogue.weapon->hit_enchant++;
					} else {
						rogue.weapon->d_enchant++;
					}
				}
				rogue.weapon->is_cursed = 0;
			} else {
				message("何だか、手がむずむずする。", 0);
			}
			break;
		case ENCH_ARMOR:
			if (rogue.armor) {
				sprintf(msg,
			"着ているよろいが、ほんの少し%s輝きに包まれた。",
				get_ench_color());
				message(msg, 0);
				rogue.armor->d_enchant++;
				rogue.armor->is_cursed = 0;
				print_stats(STAT_ARMOR);
			} else {
				message("何だか、体がむずむずする。", 0);
			}
			break;
		case IDENTIFY:
			message("これは持ちものを調べる巻き物だった。", 0);
			obj->identified = 1;
			id_scrolls[obj->which_kind].id_status = IDENTIFIED;
			idntfy();
			break;
		case TELEPORT:
			tele();
			break;
		case SLEEP:
			message("知らないうちに眠り込んでしまった。", 0);
			take_a_nap();
			break;
		case PROTECT_ARMOR:
			if (rogue.armor) {
			message("着ているよろいは、輝く金色の光に守られた。",
									0);
				rogue.armor->is_protected = 1;
				rogue.armor->is_cursed = 0;
			} else {
				message("何だか、顔がむずむずする。", 0);
			}
			break;
		case REMOVE_CURSE:
				message((!halluc) ?
			"ふっと、誰かに見つめられているような気がした。" :
			"ふっと、女神の手に包まれているような気がした。", 0);
			uncurse_all();
			break;
		case CREATE_MONSTER:
			create_monster();
			break;
		case AGGRAVATE_MONSTER:
			aggravate();
			break;
		case MAGIC_MAPPING:
			message("おや、この巻き物には地図が書いてある！", 0);
			draw_magic_map();
			break;
	}
#ifndef ORIGINAL
	id_scrolls[obj->which_kind].id_status = IDENTIFIED;
#else
	if (id_scrolls[obj->which_kind].id_status != CALLED) {
		id_scrolls[obj->which_kind].id_status = IDENTIFIED;
	}
#endif
	vanish(obj, (obj->which_kind != SLEEP), &rogue.pack);
}

#else /*JAPAN*/

read_scroll()
{
	short ch;
	object *obj;
	char msg[DCOLS];

	ch = pack_letter("Read what?", SCROL);

	if (ch == CANCEL) {
		return;
	}
	if (!(obj = get_letter_object(ch))) {
		message("No such item.", 0);
		return;
	}
	if (obj->what_is != SCROL) {
		message("You can't read that", 0);
		return;
	}
	switch(obj->which_kind) {
		case SCARE_MONSTER:
			message("You hear a maniacal laughter in the distance",
			0);
			break;
		case HOLD_MONSTER:
			hold_monster();
			break;
		case ENCH_WEAPON:
			if (rogue.weapon) {
				if (rogue.weapon->what_is == WEAPON) {
					sprintf(msg, "Your %sglow%s %sfor a moment",
					name_of(rogue.weapon),
					((rogue.weapon->quantity <= 1) ? "s" : ""),
					get_ench_color());
					message(msg, 0);
					if (coin_toss()) {
						rogue.weapon->hit_enchant++;
					} else {
						rogue.weapon->d_enchant++;
					}
				}
				rogue.weapon->is_cursed = 0;
			} else {
				message("Your hands tingle", 0);
			}
			break;
		case ENCH_ARMOR:
			if (rogue.armor) {
				sprintf(msg, "Your armor glows %sfor a moment",
				get_ench_color());
				message(msg, 0);
				rogue.armor->d_enchant++;
				rogue.armor->is_cursed = 0;
				print_stats(STAT_ARMOR);
			} else {
				message("Your skin crawls", 0);
			}
			break;
		case IDENTIFY:
			message("This is a scroll of identify", 0);
			obj->identified = 1;
			id_scrolls[obj->which_kind].id_status = IDENTIFIED;
			idntfy();
			break;
		case TELEPORT:
			tele();
			break;
		case SLEEP:
			message("You fall asleep", 0);
			take_a_nap();
			break;
		case PROTECT_ARMOR:
			if (rogue.armor) {
				message( "Your armor is covered by a shimmering gold shield",0);
				rogue.armor->is_protected = 1;
				rogue.armor->is_cursed = 0;
			} else {
				message("Your acne seems to have disappeared", 0);
			}
			break;
		case REMOVE_CURSE:
				message((!halluc) ?
					"You feel as though someone is watching over you" :
					"You feel in touch with the universal oneness", 0);
			uncurse_all();
			break;
		case CREATE_MONSTER:
			create_monster();
			break;
		case AGGRAVATE_MONSTER:
			aggravate();
			break;
		case MAGIC_MAPPING:
			message("This scroll seems to have a map on it", 0);
			draw_magic_map();
			break;
	}
#ifndef ORIGINAL
	id_scrolls[obj->which_kind].id_status = IDENTIFIED;
#else
	if (id_scrolls[obj->which_kind].id_status != CALLED) {
		id_scrolls[obj->which_kind].id_status = IDENTIFIED;
	}
#endif
	vanish(obj, (obj->which_kind != SLEEP), &rogue.pack);
}
#endif /*JAPAN*/

/* vanish() does NOT handle a quiver of weapons with more than one
   arrow (or whatever) in the quiver.  It will only decrement the count.
*/

vanish(obj, rm, pack)
object *obj;
short rm;
object *pack;
{
	if (obj->quantity > 1) {
		obj->quantity--;
	} else {
		if (obj->in_use_flags & BEING_WIELDED) {
			unwield(obj);
		} else if (obj->in_use_flags & BEING_WORN) {
			unwear(obj);
		} else if (obj->in_use_flags & ON_EITHER_HAND) {
			un_put_on(obj);
		}
		take_from_pack(obj, pack);
		free_object(obj);
	}
	if (rm) {
		(void) reg_move();
	}
}

potion_heal(extra)
{
	long ratio;
	short add;

	rogue.hp_current += rogue.exp;

	ratio = rogue.hp_current * 100L / rogue.hp_max;

	if (ratio >= 100L) {
		rogue.hp_max += (extra ? 2 : 1);
		extra_hp += (extra ? 2 : 1);
		rogue.hp_current = rogue.hp_max;
	} else if (ratio >= 90L) {
		rogue.hp_max += (extra ? 1 : 0);
		extra_hp += (extra ? 1 : 0);
		rogue.hp_current = rogue.hp_max;
	} else {
		if (ratio < 33L) {
			ratio = 33L;
		}
		if (extra) {
			ratio += ratio;
		}
		add = (short)(ratio * (rogue.hp_max-rogue.hp_current) / 100L);
		rogue.hp_current += add;
		if (rogue.hp_current > rogue.hp_max) {
			rogue.hp_current = rogue.hp_max;
		}
	}
	if (blind) {
		unblind();
	}
	if (confused && extra) {
		unconfuse();
	} else if (confused) {
		confused = (confused / 2) + 1;
	}
	if (halluc && extra) {
		unhallucinate();
	} else if (halluc) {
		halluc = (halluc / 2) + 1;
	}
}

idntfy()
{
	short ch;
	object *obj;
	struct id *id_table;
	char desc[DCOLS];
AGAIN:
#ifdef JAPAN
	ch = pack_letter("調べる持ちものは？", ALL_OBJECTS);
#else
	ch = pack_letter("What would you like to identify?", ALL_OBJECTS);
#endif
	if (ch == CANCEL) {
		return;
	}
	if (!(obj = get_letter_object(ch))) {
#ifdef JAPAN
		message("それは持っていない。", 0);
#else
		message("No such item, try again", 0);
#endif
		message("", 0);
		check_message();
		goto AGAIN;
	}
	obj->identified = 1;
	if (obj->what_is & (SCROL | POTION | WEAPON | ARMOR | WAND | RING)) {
		id_table = get_id_table(obj);
		id_table[obj->which_kind].id_status = IDENTIFIED;
	}
	get_desc(obj, desc, 1);
	message(desc, 0);
}

#ifdef JAPAN
eat()
{
	short ch;
	short moves;
	object *obj;
	char buf[70];

	ch = pack_letter("食べる物は？", FOOD);
	if (ch == CANCEL) {
		return;
	}
	if (!(obj = get_letter_object(ch))) {
		message("それは持っていない。", 0);
		return;
	}
	if (obj->what_is != FOOD) {
		message("それは食べられない。", 0);
		return;
	}
	if ((obj->which_kind == FRUIT) || rand_percent(60)) {
		moves = get_rand(900, 1100);
		if (obj->which_kind == RATION) {
#ifndef ORIGINAL
			if (get_rand(1, 10) == 1)
	message("う～む、まったりとしていて、それでいてこくがある。", 0);
			else
#endif
			message("ああ、おいしかった。", 0);
		} else {
			sprintf(buf, "おや、なんて甘い%sだ。", fruit);
			message(buf, 0);
		}
	} else {
		moves = get_rand(700, 900);
		message("げげっ、まずい！", 0);
		add_exp(2, 1);
	}
	rogue.moves_left /= 3;
	rogue.moves_left += moves;
	hunger_str[0] = 0;
	print_stats(STAT_HUNGER);

	vanish(obj, 1, &rogue.pack);
}

#else /*JAPAN*/

eat()
{
	short ch;
	short moves;
	object *obj;
	char buf[70];

	ch = pack_letter("Eat what?", FOOD);
	if (ch == CANCEL) {
		return;
	}
	if (!(obj = get_letter_object(ch))) {
		message("No such item.", 0);
		return;
	}
	if (obj->what_is != FOOD) {
		message("You can't eat that", 0);
		return;
	}
	if ((obj->which_kind == FRUIT) || rand_percent(60)) {
		moves = get_rand(900, 1100);
		if (obj->which_kind == RATION) {
			message("Yum, that tasted good", 0);
		} else {
			sprintf(buf, "My, that was a yummy %s", fruit);
			message(buf, 0);
		}
	} else {
		moves = get_rand(700, 900);
		message("Yuk, that food tasted awful", 0);
		add_exp(2, 1);
	}
	rogue.moves_left /= 3;
	rogue.moves_left += moves;
	hunger_str[0] = 0;
	print_stats(STAT_HUNGER);

	vanish(obj, 1, &rogue.pack);
}
#endif /*JAPAN*/

hold_monster()
{
	short i, j;
	short mcount = 0;
	object *monster;
	short row, col;

	for (i = -2; i <= 2; i++) {
		for (j = -2; j <= 2; j++) {
			row = rogue.row + i;
			col = rogue.col + j;
			if ((row < MIN_ROW) || (row > (DROWS-2)) || (col < 0) ||
				 (col > (DCOLS-1))) {
				continue;
			}
			if (dungeon[row][col] & MONSTER) {
				monster = object_at(&level_monsters, row, col);
				monster->m_flags |= ASLEEP;
				monster->m_flags &= (~WAKENS);
				mcount++;
			}
		}
	}
#ifdef JAPAN
	if (mcount == 0) {
		message("急に、体から力が抜けていくような気がした。", 0);
	} else if (mcount == 1) {
		message("怪物は動けなくなった！", 0);
	} else {
		message("怪物どもは動けなくなった！", 0);
	}
#else
	if (mcount == 0) {
		message("You feel a strange sense of loss", 0);
	} else if (mcount == 1) {
		message("The monster freezes", 0);
	} else {
		message("The monsters around you freeze", 0);
	}
#endif
}

tele()
{
	mvaddch(rogue.row, rogue.col, colored(get_dungeon_char(rogue.row, rogue.col)));

	if (cur_room >= 0) {
		darken_room(cur_room);
	}
	put_player(get_room_number(rogue.row, rogue.col));
	being_held = 0;
	bear_trap = 0;
}

hallucinate()
{
	object *obj, *monster;
	short ch;

	if (blind) return;

	obj = level_objects.next_object;

	while (obj) {
		ch = mvinch(obj->row, obj->col);
		if (((ch < 'A') || (ch > 'Z')) &&
			((obj->row != rogue.row) || (obj->col != rogue.col)))
		if ((ch != ' ') && (ch != '.') && (ch != '#') && (ch != '+')) {
			addch(colored(gr_obj_char()));
		}
		obj = obj->next_object;
	}
	monster = level_monsters.next_monster;

	while (monster) {
		ch = mvinch(monster->row, monster->col);
		if ((ch >= 'A') && (ch <= 'Z')) {
			addch(colored(get_rand('A', 'Z')));
		}
		monster = monster->next_monster;
	}
}

unhallucinate()
{
	halluc = 0;
	relight();
#ifdef JAPAN
	message("やっと、あたりの景色が元どおりになった。", 1);
#else
	message("Everything looks SO boring now", 1);
#endif
}

unblind()
{
	blind = 0;
#ifdef JAPAN
	message("暗闇のとばりが上がってゆく。", 1);
#else
	message("The veil of darkness lifts", 1);
#endif
	relight();
	if (halluc) {
		hallucinate();
	}
	if (detect_monster) {
		show_monsters();
	}
}

relight()
{
	if (cur_room == PASSAGE) {
		light_passage(rogue.row, rogue.col);
	} else {
		light_up_room(cur_room);
	}
	mvaddch(rogue.row, rogue.col, colored(rogue.fchar));
}

take_a_nap()
{
	short i;

	i = get_rand(2, 5);
	md_sleep(1);

	while (i--) {
		mv_mons();
	}
	md_sleep(1);
	message(you_can_move_again, 0);
}

go_blind()
{
	short i, j;

	if (!blind) {
#ifdef JAPAN
		message("深い暗闇のとばりがあたりをおおってゆく。", 0);
#else
		message("A cloak of darkness falls around you", 0);
#endif
	}
	blind += get_rand(500, 800);

	if (detect_monster) {
		object *monster;

		monster = level_monsters.next_monster;

		while (monster) {
			mvaddch(monster->row, monster->col, colored(monster->trail_char));
			monster = monster->next_monster;
		}
	}
	if (cur_room >= 0) {
		for (i = rooms[cur_room].top_row + 1;
			 i < rooms[cur_room].bottom_row; i++) {
			for (j = rooms[cur_room].left_col + 1;
				 j < rooms[cur_room].right_col; j++) {
				mvaddch(i, j, colored(' '));
			}
		}
	}
	mvaddch(rogue.row, rogue.col, colored(rogue.fchar));
}

char *
get_ench_color()
{
	if (halluc) {
		return(id_potions[get_rand(0, POTIONS-1)].title);
	}
#ifdef JAPAN
	return("青い");
#else
	return("blue ");
#endif
}

confuse()
{
	confused += get_rand(12, 22);
}

unconfuse()
{
	char msg[80];

	confused = 0;
#ifdef JAPAN
	if (halluc)
		sprintf(msg, "ふたたび、景色が虹色に見えてきた。");
	else
		sprintf(msg, "ようやく、頭がはっきりしてきた。");
#else
	sprintf(msg, "You feel less %s now", (halluc ? "trippy" : "confused"));
#endif
	message(msg, 1);
}

uncurse_all()
{
	object *obj;

	obj = rogue.pack.next_object;

	while (obj) {
		obj->is_cursed = 0;
		obj = obj->next_object;
	}
}
