/*
 * pack.c
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

#ifdef JAPAN
char *curse_message = "呪われているので、それはできない！";
#else
char *curse_message = "You can't, it appears to be cursed";
#endif

object *
add_to_pack(obj, pack, condense)
object *obj, *pack;
{
	object *op, *p;
	
	if (condense) {
		if (op = check_duplicate(obj, pack)) {
			free_object(obj);
			return(op);
		} else {
			obj->ichar = next_avail_ichar();
		}
	}
#ifndef ORIGINAL
	for (op = pack; op->next_object; op = op->next_object) {
		if (op->next_object->what_is > obj->what_is) {
			p = op->next_object;
			op->next_object = obj;
			obj->next_object = p;
			return obj;
		}
	}
	op->next_object = obj;
	obj->next_object = 0;
	return obj;
#else
	if (pack->next_object == 0) {
		pack->next_object = obj;
	} else {
		op = pack->next_object;

		while (op->next_object) {
			op = op->next_object;
		}
		op->next_object = obj;
	}
	obj->next_object = 0;
	return(obj);
#endif
}

take_from_pack(obj, pack)
object *obj, *pack;
{
	while (pack->next_object != obj) {
		pack = pack->next_object;
	}
	pack->next_object = pack->next_object->next_object;
}

object *
pick_up(row, col, status)
short *status;
{
	object *obj;

	obj = object_at(&level_objects, row, col);
	*status = 1;

	if ((obj->what_is == SCROL) && (obj->which_kind == SCARE_MONSTER) &&
		obj->picked_up) {
#ifdef JAPAN
		message("拾いあげたとたん、巻き物はちりになってしまった。", 0);
#else
		message("The scroll turns to dust as you pick it up", 0);
#endif
		dungeon[row][col] &= (~OBJECT);
		vanish(obj, 0, &level_objects);
		*status = 0;
		if (id_scrolls[SCARE_MONSTER].id_status == UNIDENTIFIED) {
			id_scrolls[SCARE_MONSTER].id_status = IDENTIFIED;
		}
		return(0);
	}
	if (obj->what_is == GOLD) {
		rogue.gold += obj->quantity;
		dungeon[row][col] &= ~(OBJECT);
		take_from_pack(obj, &level_objects);
		print_stats(STAT_GOLD);
		return(obj);	/* obj will be free_object()ed in one_move_rogue() */
	}
	if (pack_count(obj) >= MAX_PACK_COUNT) {
#ifdef JAPAN
		message("もうこれ以上、物は持てない。", 1);
#else
		message("Pack too full", 1);
#endif
		return(0);
	}
	dungeon[row][col] &= ~(OBJECT);
	take_from_pack(obj, &level_objects);
	obj = add_to_pack(obj, &rogue.pack, 1);
	obj->picked_up = 1;
	return(obj);
}

drop()
{
	object *obj, *new;
	short ch;
	char desc[DCOLS];

	if (dungeon[rogue.row][rogue.col] & (OBJECT | STAIRS | TRAP)) {
#ifdef JAPAN
		message("そこには、すでに何かが置かれている。", 0);
#else
		message("There's already something there", 0);
#endif
		return;
	}
	if (!rogue.pack.next_object) {
#ifdef JAPAN
		message("落とす物を持っていない。", 0);
#else
		message("You have nothing to drop", 0);
#endif
		return;
	}
#ifdef JAPAN
	if ((ch = pack_letter("落とす物は？", ALL_OBJECTS)) == CANCEL) {
#else
	if ((ch = pack_letter("Drop what?", ALL_OBJECTS)) == CANCEL) {
#endif
		return;
	}
	if (!(obj = get_letter_object(ch))) {
#ifdef JAPAN
		message("それは持っていない。", 0);
#else
		message("No such item.", 0);
#endif
		return;
	}
	if (obj->in_use_flags & BEING_WIELDED) {
		if (obj->is_cursed) {
			message(curse_message, 0);
			return;
		}
		unwield(rogue.weapon);
	} else if (obj->in_use_flags & BEING_WORN) {
		if (obj->is_cursed) {
			message(curse_message, 0);
			return;
		}
		mv_aquatars();
		unwear(rogue.armor);
		print_stats(STAT_ARMOR);
	} else if (obj->in_use_flags & ON_EITHER_HAND) {
		if (obj->is_cursed) {
			message(curse_message, 0);
			return;
		}
		un_put_on(obj);
	}
	obj->row = rogue.row;
	obj->col = rogue.col;

	if ((obj->quantity > 1) && (obj->what_is != WEAPON)) {
		obj->quantity--;
		new = alloc_object();
		*new = *obj;
		new->quantity = 1;
		obj = new;
	} else {
		obj->ichar = 'L';
		take_from_pack(obj, &rogue.pack);
	}
	place_at(obj, rogue.row, rogue.col);
#ifdef JAPAN
	get_desc(obj, desc, 0);
	(void) strcat(desc, "を落とした。");
#else
	(void) strcpy(desc, "Dropped ");
	get_desc(obj, desc+8, 0);
#endif
	message(desc, 0);
	(void) reg_move();
}

object *
check_duplicate(obj, pack)
object *obj, *pack;
{
	object *op;

	if (!(obj->what_is & (WEAPON | FOOD | SCROL | POTION))) {
		return(0);
	}
	if ((obj->what_is == FOOD) && (obj->which_kind == FRUIT)) {
		return(0);
	}
	op = pack->next_object;

	while (op) {
		if ((op->what_is == obj->what_is) && 
			(op->which_kind == obj->which_kind)) {

			if ((obj->what_is != WEAPON) ||
			((obj->what_is == WEAPON) &&
			((obj->which_kind == ARROW) ||
			(obj->which_kind == DAGGER) ||
			(obj->which_kind == DART) ||
			(obj->which_kind == SHURIKEN)) &&
			(obj->quiver == op->quiver))) {
				op->quantity += obj->quantity;
				return(op);
			}
		}
		op = op->next_object;
	}
	return(0);
}

next_avail_ichar()
{
	register object *obj;
	register i;
	boolean ichars[26];

	for (i = 0; i < 26; i++) {
		ichars[i] = 0;
	}
	obj = rogue.pack.next_object;
	while (obj) {
		ichars[(obj->ichar - 'a')] = 1;
		obj = obj->next_object;
	}
	for (i = 0; i < 26; i++) {
		if (!ichars[i]) {
			return(i + 'a');
		}
	}
	return('?');
}

wait_for_ack()
{
	while (rgetchar() != ' ') ;
}

pack_letter(prompt, mask)
char *prompt;
unsigned short mask;
{
	short ch;
	unsigned short tmask = mask;

	if (!mask_pack(&rogue.pack, mask)) {
#ifdef JAPAN
		message("それは１つも持っていない。", 0);
#else
		message("Nothing appropriate", 0);
#endif
		return(CANCEL);
	}
	for (;;) {

		message(prompt, 0);

		for (;;) {
			ch = rgetchar();
			if (!is_pack_letter(&ch, &mask)) {
				sound_bell();
			} else {
				break;
			}
		}

		if (ch == LIST) {
			check_message();
			inventory(&rogue.pack, mask);
		} else {
			break;
		}
		mask = tmask;
	}
	check_message();
	return(ch);
}

take_off()
{
	char desc[DCOLS];
	object *obj;

	if (rogue.armor) {
		if (rogue.armor->is_cursed) {
			message(curse_message, 0);
		} else {
			mv_aquatars();
			obj = rogue.armor;
			unwear(rogue.armor);
#ifdef JAPAN
			get_desc(obj, desc, 0);
			(void) strcat(desc, "を脱いだ。");
#else
			(void) strcpy(desc, "Was wearing ");
			get_desc(obj, desc+12, 0);
#endif
			message(desc, 0);
			print_stats(STAT_ARMOR);
			(void) reg_move();
		}
	} else {
#ifdef JAPAN
		message("よろいを着ていない。", 0);
#else
		message("Not wearing any", 0);
#endif
	}
}

wear()
{
	short ch;
	register object *obj;
	char desc[DCOLS];

	if (rogue.armor) {
#ifdef JAPAN
		message("他のよろいを着ている。", 0);
#else
		message("You're already wearing some", 0);
#endif
		return;
	}
#ifdef JAPAN
	ch = pack_letter("どのよろいを？", ARMOR);
#else
	ch = pack_letter("Wear what?", ARMOR);
#endif

	if (ch == CANCEL) {
		return;
	}
	if (!(obj = get_letter_object(ch))) {
#ifdef JAPAN
		message("それは持っていない。", 0);
#else
		message("No such item.", 0);
#endif
		return;
	}
	if (obj->what_is != ARMOR) {
#ifdef JAPAN
		message("それはよろいではない。", 0);
#else
		message("You can't wear that", 0);
#endif
		return;
	}
	obj->identified = 1;
#ifdef JAPAN
	get_desc(obj, desc, 0);
	(void) strcat(desc, "を身につけた。");
#else
	(void) strcpy(desc, "Wearing ");
	get_desc(obj, desc + 8, 0);
#endif
	message(desc, 0);
	do_wear(obj);
	print_stats(STAT_ARMOR);
	(void) reg_move();
}

unwear(obj)
object *obj;
{
	if (obj) {
		obj->in_use_flags &= (~BEING_WORN);
	}
	rogue.armor = (object *) 0;
}

do_wear(obj)
object *obj;
{
	rogue.armor = obj;
	obj->in_use_flags |= BEING_WORN;
	obj->identified = 1;
}

wield()
{
	short ch;
	register object *obj;
	char desc[DCOLS];

	if (rogue.weapon && rogue.weapon->is_cursed) {
		message(curse_message, 0);
		return;
	}
#ifdef JAPAN
	ch = pack_letter("どの武器を？", WEAPON);
#else
	ch = pack_letter("Wield what?", WEAPON);
#endif

	if (ch == CANCEL) {
		return;
	}
	if (!(obj = get_letter_object(ch))) {
#ifdef JAPAN
		message("それは持っていない。", 0);
#else
		message("No such item.", 0);
#endif
		return;
	}
	if (obj->what_is & (ARMOR | RING)) {
#ifdef JAPAN
		sprintf(desc, "%sを武器にすることはできない。",
			((obj->what_is == ARMOR) ? "よろい" : "指輪"));
#else
		sprintf(desc, "You can't wield %s",
			((obj->what_is == ARMOR) ? "armor" : "rings"));
#endif
		message(desc, 0);
		return;
	}
	if (obj->in_use_flags & BEING_WIELDED) {
#ifdef JAPAN
		message("その武器はもう使っている。", 0);
#else
		message("In use", 0);
#endif
	} else {
		unwield(rogue.weapon);
#ifdef JAPAN
		get_desc(obj, desc, 0);
		(void) strcat(desc, "に持ちかえた。");
#else
		(void) strcpy(desc, "Wielding ");
		get_desc(obj, desc + 9, 0);
#endif
		message(desc, 0);
		do_wield(obj);
		(void) reg_move();
	}
}

do_wield(obj)
object *obj;
{
	rogue.weapon = obj;
	obj->in_use_flags |= BEING_WIELDED;
}

unwield(obj)
object *obj;
{
	if (obj) {
		obj->in_use_flags &= (~BEING_WIELDED);
	}
	rogue.weapon = (object *) 0;
}

call_it()
{
	short ch;
	register object *obj;
	struct id *id_table;
	char buf[MAX_TITLE_LENGTH+2];

#ifdef JAPAN
	ch = pack_letter("呼び名をつけるものは？",
#else
	ch = pack_letter("Call what?",
#endif
			(SCROL | POTION | WAND | RING));

	if (ch == CANCEL) {
		return;
	}
	if (!(obj = get_letter_object(ch))) {
#ifdef JAPAN
		message("それは持っていない。", 0);
#else
		message("No such item.", 0);
#endif
		return;
	}
	if (!(obj->what_is & (SCROL | POTION | WAND | RING))) {
#ifdef JAPAN
		message("それには呼び名をつけることができない。", 0);
#else
		message("Surely you already know what that's called", 0);
#endif
		return;
	}
	id_table = get_id_table(obj);

#ifdef JAPAN
	if (get_input_line("呼び名は？",
			"", buf, id_table[obj->which_kind].title, 0, 1)) {
		ch = *buf;
		if (ch >= ' ' && ch <= '~' || ch >= 0xa0 && ch <= 0xde) {
			/* alphabet or kana character; append 1 blank */
			(void) strcat(buf, " ");
		}
		id_table[obj->which_kind].id_status = CALLED;
		(void) strcpy(id_table[obj->which_kind].title, buf);
	}
#else
	if (get_input_line("Call it:",
			"", buf, id_table[obj->which_kind].title, 1, 1)) {
		id_table[obj->which_kind].id_status = CALLED;
		(void) strcpy(id_table[obj->which_kind].title, buf);
	}
#endif
}

pack_count(new_obj)
object *new_obj;
{
	object *obj;
	short count = 0;

	obj = rogue.pack.next_object;

	while (obj) {
		if (obj->what_is != WEAPON) {
			count += obj->quantity;
		} else if (!new_obj) {
			count++;
		} else if ((new_obj->what_is != WEAPON) ||
			((obj->which_kind != ARROW) &&
			(obj->which_kind != DAGGER) &&
			(obj->which_kind != DART) &&
			(obj->which_kind != SHURIKEN)) ||
			(new_obj->which_kind != obj->which_kind) ||
			(obj->quiver != new_obj->quiver)) {
			count++;
		}
		obj = obj->next_object;
	}
	return(count);
}

boolean
mask_pack(pack, mask)
object *pack;
unsigned short mask;
{
	while (pack->next_object) {
		pack = pack->next_object;
		if (pack->what_is & mask) {
			return(1);
		}
	}
	return(0);
}

is_pack_letter(c, mask)
short *c;
unsigned short *mask;
{
	switch(*c) {
	case '?': *mask = SCROL;  goto found;
	case '!': *mask = POTION; goto found;
	case ':': *mask = FOOD;   goto found;
	case ')': *mask = WEAPON; goto found;
	case ']': *mask = ARMOR;  goto found;
	case '/': *mask = WAND;   goto found;
	case '=': *mask = RING;   goto found;
	case ',': *mask = AMULET; goto found;
	default: return(*c >= 'a' && *c <= 'z' || *c == CANCEL || *c == LIST);
	}
found:
	*c = LIST;
	return(1);
}

has_amulet()
{
	return(mask_pack(&rogue.pack, AMULET));
}

kick_into_pack()
{
	object *obj;
	char *p;
	char desc[DCOLS];
	short stat;
	extern short levitate;

	if (!(dungeon[rogue.row][rogue.col] & OBJECT)) {
#ifdef JAPAN
		message("ここには、何もない。", 0);
#else
		message("Nothing here", 0);
#endif
	} else {
#ifndef ORIGINAL
		if (levitate) {
#ifdef JAPAN
			message("拾うことができない！", 0);
#else
			message("You're floating in the air!", 0);
#endif
			return 0;
		}
#endif
		if (obj = pick_up(rogue.row, rogue.col, &stat)) {
			get_desc(obj, desc, 1);
#ifdef JAPAN
			(void) strcat(desc, "を手に入れた。 ");
#endif
			if (obj->what_is == GOLD) {
				message(desc, 0);
				free_object(obj);
			} else {
				p = desc + strlen(desc);
				*p++ = '(';
				*p++ = obj->ichar;
				*p++ = ')';
				*p = 0;
				message(desc, 0);
			}
		}
		if (obj || (!stat)) {
			(void) reg_move();
		}
	}
}
