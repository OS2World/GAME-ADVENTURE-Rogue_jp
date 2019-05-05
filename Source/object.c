/*
 * object.c
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

object level_objects;
unsigned short dungeon[DROWS][DCOLS];
short foods = 0;
short party_counter;
object *free_list = (object *) 0;

#ifdef JAPAN
char *fruit = "こけもも";

fighter rogue = {
	0, 0,		/* armor, weapon */
	0, 0,		/* rings */
	INIT_HP,	/* Hp current */
	INIT_HP,	/* Hp max */
	16, 16,		/* Str */
	{0},		/* pack */
	0,			/* gold */
	1, 0,		/* exp, exp_points */
	0, 0,		/* row, col */
	'@',		/* char */
	1250		/* moves */
};

char *po_color[14] = {
	"青い","赤い","緑の","灰色の","茶色の","透明な","ピンクの",
	"白い","紫の","黒い","黄色い","水色の","ぶどう色の","オレンジ色の"
};

char po_title[14][34], sc_title[12][34], wa_title[10][34], ri_title[11][34];

struct id id_potions[POTIONS] = {
	{100, po_title[ 0], "強さが増す", 0},
	{250, po_title[ 1], "強さが元にもどる", 0},
	{100, po_title[ 2], "体力が回復する", 0},
	{200, po_title[ 3], "体力がとても回復する", 0},
	 {10, po_title[ 4], "毒の", 0},
	{300, po_title[ 5], "経験が増す", 0},
	 {10, po_title[ 6], "目が見えなくなる", 0},
	 {25, po_title[ 7], "幻覚をおこす", 0},
	{100, po_title[ 8], "遠くの怪物がわかる", 0},
	{100, po_title[ 9], "遠くのものがわかる", 0},
	 {10, po_title[10], "頭が混乱する", 0},
	 {80, po_title[11], "空中に浮きあがる", 0},
	{150, po_title[12], "素早くなる", 0},
	{145, po_title[13], "見えないものが見える", 0}
};

struct id id_scrolls[SCROLS] = {
	{505, sc_title[ 0], "よろいを守る", 0},
	{200, sc_title[ 1], "怪物を封じこめる", 0},
	{235, sc_title[ 2], "武器に魔法をかける", 0},
	{235, sc_title[ 3], "よろいに魔法をかける", 0},
	{175, sc_title[ 4], "持ちものの種類がわかる", 0},
	{190, sc_title[ 5], "テレポートする", 0},
	 {25, sc_title[ 6], "眠りにおちる", 0},
	{610, sc_title[ 7], "怪物を近寄せない", 0},
	{210, sc_title[ 8], "のろいを解く", 0},
	{100, sc_title[ 9], "怪物を作りだす",0},
	 {25, sc_title[10], "怪物を怒らせる",0},
	{180, sc_title[11], "魔法の地図の",0}
};
struct id id_weapons[WEAPONS] = {
	{150, "弓",       "", 0},
	  {8, "投げ矢",   "", 0},
	 {15, "矢",       "", 0},
	 {27, "短剣",     "", 0},
	 {35, "手裏剣",   "", 0},
	{360, "ほこ",     "", 0},
	{470, "長い剣",   "", 0},
	{580, "大きな剣", "", 0}
};

struct id id_armors[ARMORS] = {
	{300, "革のよろい",     "", (UNIDENTIFIED)},
	{300, "かたびら",       "", (UNIDENTIFIED)},
	{400, "うろこのよろい", "", (UNIDENTIFIED)},
	{500, "鎖かたびら",     "", (UNIDENTIFIED)},
	{600, "帯金のよろい",   "", (UNIDENTIFIED)},
	{600, "延金のよろい",   "", (UNIDENTIFIED)},
	{700, "鋼鉄のよろい",   "", (UNIDENTIFIED)}
};

struct id id_wands[WANDS] = {
	 {25, wa_title[ 0], "怪物を遠くに飛ばす",0},
	 {50, wa_title[ 1], "怪物を遅くする", 0},
	 {45, wa_title[ 2], "怪物を混乱させる",0},
	  {8, wa_title[ 3], "怪物を見えなくする",0},
	 {55, wa_title[ 4], "ほかの怪物にする",0},
	  {2, wa_title[ 5], "怪物を素早くする",0},
	 {25, wa_title[ 6], "眠らせる",0},
	 {20, wa_title[ 7], "魔法のミサイルの",0},
	 {20, wa_title[ 8], "魔力を封じる",0},
	  {0, wa_title[ 9], "役に立たない",0}
};

struct id id_rings[RINGS] = {
	{250, ri_title[ 0], "身を隠す",0},
	{100, ri_title[ 1], "テレポートする", 0},
	{255, ri_title[ 2], "体力が復活する",0},
	{295, ri_title[ 3], "食べものが長持ちする",0},
	{200, ri_title[ 4], "強さが増す",0},
	{250, ri_title[ 5], "強さが減らない",0},
	{250, ri_title[ 6], "機敏になる",0},
	 {25, ri_title[ 7], "飾りものの",0},
	{300, ri_title[ 8], "見えないものが見える",0},
	{290, ri_title[ 9], "よろいが丈夫になる",0},
	{270, ri_title[10], "隠れたものを見つける",0}
};

#else /*JAPAN*/

char *fruit = "slime-mold ";

fighter rogue = {
	0, 0,		/* armor, weapon */
	0, 0,		/* rings */
	INIT_HP,	/* Hp current */
	INIT_HP,	/* Hp max */
	16, 16,		/* Str */
	{0},		/* pack */
	0,			/* gold */
	1, 0,		/* exp, exp_points */
	0, 0,		/* row, col */
	'@',		/* char */
	1250		/* moves */
};

char *po_color[14] = {
	"blue ","red ","green ","grey ","brown ","clear ","pink ",
	"white ","purple ","black ","yellow ","plaid ","burgundy ","beige "
};

char po_title[14][34], sc_title[12][34], wa_title[10][34], ri_title[11][34];

struct id id_potions[POTIONS] = {
	{100, po_title[ 0], "of increase strength ", 0},
	{250, po_title[ 1], "of restore strength ", 0},
	{100, po_title[ 2], "of healing ", 0},
	{200, po_title[ 3], "of extra healing ", 0},
	 {10, po_title[ 4], "of poison ", 0},
	{300, po_title[ 5], "of raise level ", 0},
	 {10, po_title[ 6], "of blindness ", 0},
	 {25, po_title[ 7], "of hallucination ", 0},
	{100, po_title[ 8], "of detect monster ", 0},
	{100, po_title[ 9], "of detect things ", 0},
	 {10, po_title[10], "of confusion ", 0},
	 {80, po_title[11], "of levitation ", 0},
	{150, po_title[12], "of haste self ", 0},
	{145, po_title[13], "of see invisible ", 0}
};

struct id id_scrolls[SCROLS] = {
	{505, sc_title[ 0], "of protect armor ", 0},
	{200, sc_title[ 1], "of hold monster ", 0},
	{235, sc_title[ 2], "of enchant weapon ", 0},
	{235, sc_title[ 3], "of enchant armor ", 0},
	{175, sc_title[ 4], "of identify ", 0},
	{190, sc_title[ 5], "of teleportation ", 0},
	 {25, sc_title[ 6], "of sleep ", 0},
	{610, sc_title[ 7], "of scare monster ", 0},
	{210, sc_title[ 8], "of remove curse ", 0},
	{100, sc_title[ 9], "of create monster ",0},
	 {25, sc_title[10], "of aggravate monster ",0},
	{180, sc_title[11], "of magic mapping ",0}
};

struct id id_weapons[WEAPONS] = {
	{150, "short bow ",        "", 0},
	  {8, "darts ",            "", 0},
	 {15, "arrows ",           "", 0},
	 {27, "daggers ",          "", 0},
	 {35, "shurikens ",        "", 0},
	{360, "mace ",             "", 0},
	{470, "long sword ",       "", 0},
#ifndef ORIGINAL
	{580, "two handed sword ", "", 0}
#else
	{580, "two-handed sword ", "", 0}
#endif
};

struct id id_armors[ARMORS] = {
	{300, "leather armor ", "", (UNIDENTIFIED)},
	{300, "ring mail ",     "", (UNIDENTIFIED)},
	{400, "scale mail ",    "", (UNIDENTIFIED)},
	{500, "chain mail ",    "", (UNIDENTIFIED)},
	{600, "banded mail ",   "", (UNIDENTIFIED)},
	{600, "splint mail ",   "", (UNIDENTIFIED)},
	{700, "plate mail ",    "", (UNIDENTIFIED)}
};

struct id id_wands[WANDS] = {
	  {25, wa_title[ 0], "of teleport away ",0},
	  {50, wa_title[ 1], "of slow monster ", 0},
	  {45, wa_title[ 2], "of confuse monster ",0},
	   {8, wa_title[ 3], "of invisibility ",0},
	  {55, wa_title[ 4], "of polymorph ",0},
	   {2, wa_title[ 5], "of haste monster ",0},
	  {25, wa_title[ 6], "of sleep ",0},
	  {20, wa_title[ 7], "of magic missile ",0},
	  {20, wa_title[ 8], "of cancellation ",0},
	   {0, wa_title[ 9], "of do nothing ",0}
};

struct id id_rings[RINGS] = {
	 {250, ri_title[ 0], "of stealth ",0},
	 {100, ri_title[ 1], "of teleportation ", 0},
	 {255, ri_title[ 2], "of regeneration ",0},
	 {295, ri_title[ 3], "of slow digestion ",0},
	 {200, ri_title[ 4], "of add strength ",0},
	 {250, ri_title[ 5], "of sustain strength ",0},
	 {250, ri_title[ 6], "of dexterity ",0},
	  {25, ri_title[ 7], "of adornment ",0},
	 {300, ri_title[ 8], "of see invisible ",0},
	 {290, ri_title[ 9], "of maintain armor ",0},
	 {270, ri_title[10], "of searching ",0},
};
#endif /*JAPAN*/

extern short cur_level, max_level;
extern short party_room;
extern char *error_file;
extern boolean is_wood[];

#ifdef USE_COLOR
extern boolean do_color;
extern short c_attr[];

colored(c)
register c;
{
	c &= 0xff;
	return (do_color? (c | c_attr[c]): c);
}
#endif

put_objects()
{
	short i, n;
	object *obj;

	if (cur_level < max_level) {
		return;
	}
	n = coin_toss() ? get_rand(2, 4) : get_rand(3, 5);
	while (rand_percent(33)) {
		n++;
	}
	if (cur_level == party_counter) {
		make_party();
		party_counter = next_party();
	}
	for (i = 0; i < n; i++) {
		obj = gr_object();
		rand_place(obj);
	}
	put_gold();
}

put_gold()
{
	short i, j;
	short row,col;
	boolean is_maze, is_room;

	for (i = 0; i < MAXROOMS; i++) {
		is_maze = (rooms[i].is_room & R_MAZE) ? 1 : 0;
		is_room = (rooms[i].is_room & R_ROOM) ? 1 : 0;

		if (!(is_room || is_maze)) {
			continue;
		}
		if (is_maze || rand_percent(GOLD_PERCENT)) {
			for (j = 0; j < 50; j++) {
				row = get_rand(rooms[i].top_row+1,
				rooms[i].bottom_row-1);
				col = get_rand(rooms[i].left_col+1,
				rooms[i].right_col-1);
				if ((dungeon[row][col] == FLOOR) ||
					(dungeon[row][col] == TUNNEL)) {
					plant_gold(row, col, is_maze);
					break;
				}
			}
		}
	}
}

plant_gold(row, col, is_maze)
short row, col;
boolean is_maze;
{
	object *obj;

	obj = alloc_object();
	obj->row = row; obj->col = col;
	obj->what_is = GOLD;
	obj->quantity = get_rand((2 * cur_level), (16 * cur_level));
	if (is_maze) {
		obj->quantity += obj->quantity / 2;
	}
	dungeon[row][col] |= OBJECT;
	(void) add_to_pack(obj, &level_objects, 0);
}

place_at(obj, row, col)
object *obj;
{
	obj->row = row;
	obj->col = col;
	dungeon[row][col] |= OBJECT;
	(void) add_to_pack(obj, &level_objects, 0);
}

object *
object_at(pack, row, col)
register object *pack;
short row, col;
{
	object *obj;

	obj = pack->next_object;

	while (obj && ((obj->row != row) || (obj->col != col))) {
		obj = obj->next_object;
	}
	return(obj);
}

object *
get_letter_object(ch)
{
	object *obj;

	obj = rogue.pack.next_object;

	while (obj && (obj->ichar != ch)) {
		obj = obj->next_object;
	}
	return(obj);
}

free_stuff(objlist)
object *objlist;
{
	object *obj;

	while (objlist->next_object) {
		obj = objlist->next_object;
		objlist->next_object =
			objlist->next_object->next_object;
		free_object(obj);
	}
}

#ifdef JAPAN /* for whole function name_of() */
char *
name_of(obj)
object *obj;
{
#ifndef ORIGINAL
	int i;
	static unsigned short wa[] = {
		SCROL, POTION, WAND, ARMOR, RING, AMULET
	};
	static char *na[] = {
		"巻き物", "水薬", "つえ", "よろい", "指輪", "魔除け"
	};

	if (obj->what_is == WEAPON) {
		return id_weapons[obj->which_kind].title;
	}
	if (obj->what_is == FOOD) {
		return (obj->which_kind == RATION)? "食糧": fruit;
	}
	for (i = 0; i < 6; i++) {
		if (obj->what_is == wa[i])
			return na[i];
	}
	return "？？？";
#else
	char *retstring;

	switch(obj->what_is) {
	case SCROL:
		retstring = "巻き物";
		break;
	case POTION:
		retstring = "水薬";
		break;
	case FOOD:
		if (obj->which_kind == RATION) {
			retstring = "食糧";
		} else {
			retstring = fruit;
		}
		break;
	case WAND:
		retstring = "つえ";
		break;
	case WEAPON:
		retstring = id_weapons[obj->which_kind].title;
		break;
	case ARMOR:
		retstring = "よろい";
		break;
	case RING:
		retstring = "指輪";
		break;
	case AMULET:
		retstring = "魔除け";
		break;
	default:
		retstring = "？？？";
		break;
	}
	return(retstring);
#endif /* ORIGINAL */
}

#else /*JAPAN for whole function name_of() */

char *
name_of(obj)
object *obj;
{
	char *retstring;

	switch(obj->what_is) {
	case SCROL:
		retstring = obj->quantity > 1 ? "scrolls " : "scroll ";
		break;
	case POTION:
		retstring = obj->quantity > 1 ? "potions " : "potion ";
		break;
	case FOOD:
		if (obj->which_kind == RATION) {
			retstring = "food ";
		} else {
			retstring = fruit;
		}
		break;
	case WAND:
		retstring = is_wood[obj->which_kind] ? "staff " : "wand ";
		break;
	case WEAPON:
		switch(obj->which_kind) {
		case DART:
			retstring=obj->quantity > 1 ? "darts " : "dart ";
			break;
		case ARROW:
			retstring=obj->quantity > 1 ? "arrows " : "arrow ";
			break;
		case DAGGER:
			retstring=obj->quantity > 1 ? "daggers " : "dagger ";
			break;
		case SHURIKEN:
			retstring=obj->quantity > 1?"shurikens ":"shuriken ";
			break;
		default:
			retstring = id_weapons[obj->which_kind].title;
		}
		break;
	case ARMOR:
		retstring = "armor ";
		break;
	case RING:
		retstring = "ring ";
		break;
	case AMULET:
		retstring = "amulet ";
		break;
	default:
		retstring = "unknown ";
		break;
	}
	return(retstring);
}
#endif /*JAPAN for whole function name_of() */

object *
gr_object()
{
	object *obj;

	obj = alloc_object();

	if (foods < (cur_level / 3)) {
		obj->what_is = FOOD;
		foods++;
	} else {
		obj->what_is = gr_what_is();
	}
	switch(obj->what_is) {
	case SCROL:
		gr_scroll(obj);
		break;
	case POTION:
		gr_potion(obj);
		break;
	case WEAPON:
		gr_weapon(obj, 1);
		break;
	case ARMOR:
		gr_armor(obj);
		break;
	case WAND:
		gr_wand(obj);
		break;
	case FOOD:
		get_food(obj, 0);
		break;
	case RING:
		gr_ring(obj, 1);
		break;
	}
	return(obj);
}

unsigned short
gr_what_is()
{
	register short percent;
	register int i;
	static short per[] = { 30, 60, 64, 74, 83, 88, 91 };
	static unsigned short ret[] = {
		SCROL, POTION, WAND, WEAPON, ARMOR, FOOD, RING
	};

	percent = get_rand(1, 91);

	for (i = 0;; i++) {
		if (percent <= per[i])
			return ret[i];
	}
}

gr_scroll(obj)
object *obj;
{
	short percent;
	register int i;
	static short per[SCROLS] = {
		5, 11, 16, 21, 36, 44, 51, 56, 65, 74, 80, 85
	};

	percent = get_rand(0, 85);
	obj->what_is = SCROL;
	for (i = 0;; i++) {
		if (percent <= per[i]) {
			obj->which_kind = i;
			return;
		}
	}
}

gr_potion(obj)
object *obj;
{
	short percent;
	register int i;
	static short per[POTIONS] = {
		10, 20, 30, 40, 50, 55, 65, 75, 85, 95, 105, 110, 114, 118
	};

	percent = get_rand(1, 118);
	obj->what_is = POTION;
	for (i = 0; i < POTIONS; i++) {
		if (percent <= per[i]) {
			obj->which_kind = i;
			return;
		}
	}
}

gr_weapon(obj, assign_wk)
object *obj;
int assign_wk;
{
	short i;
	short percent;
	short blessing, increment;
	static char *da[WEAPONS] = {
		"1d1", "1d1", "1d2", "1d3", "1d4", "2d3", "3d4", "4d5"
	};

	obj->what_is = WEAPON;
	if (assign_wk) {
		obj->which_kind = get_rand(0, (WEAPONS - 1));
	}
	if ((i = obj->which_kind) == ARROW || i == DAGGER ||
			i == SHURIKEN || i == DART) {
		obj->quantity = get_rand(3, 15);
		obj->quiver = get_rand(0, 126);
	} else {
		obj->quantity = 1;
	}
	obj->hit_enchant = obj->d_enchant = 0;

	percent = get_rand(1, 96);
	blessing = get_rand(1, 3);

	if (percent <= 16) {
		increment = 1;
	} else if (percent <= 32) {
		increment = -1;
		obj->is_cursed = 1;
	}
	if (percent <= 32) {
		for (i = 0; i < blessing; i++) {
			if (coin_toss()) {
				obj->hit_enchant += increment;
			} else {
				obj->d_enchant += increment;
			}
		}
	}
	obj->damage = da[obj->which_kind];
}

gr_armor(obj)
object *obj;
{
	short percent;
	short blessing;

	obj->what_is = ARMOR;
	obj->which_kind = get_rand(0, (ARMORS - 1));
	obj->class = obj->which_kind + 2;
	if ((obj->which_kind == PLATE) || (obj->which_kind == SPLINT)) {
		obj->class--;
	}
	obj->is_protected = 0;
	obj->d_enchant = 0;

	percent = get_rand(1, 100);
	blessing = get_rand(1, 3);

	if (percent <= 16) {
		obj->is_cursed = 1;
		obj->d_enchant -= blessing;
	} else if (percent <= 33) {
		obj->d_enchant += blessing;
	}
}

gr_wand(obj)
object *obj;
{
	obj->what_is = WAND;
	obj->which_kind = get_rand(0, (WANDS - 1));
	if (obj->which_kind == MAGIC_MISSILE) {
		obj->class = get_rand(6, 12);
	} else if (obj->which_kind == CANCELLATION) {
		obj->class = get_rand(5, 9);
	} else {
		obj->class = get_rand(3, 6);
	}
}

get_food(obj, force_ration)
object *obj;
boolean force_ration;
{
	obj->what_is = FOOD;

	if (force_ration || rand_percent(80)) {
		obj->which_kind = RATION;
	} else {
		obj->which_kind = FRUIT;
	}
}

put_stairs()
{
	short row, col;

	gr_row_col(&row, &col, (FLOOR | TUNNEL));
	dungeon[row][col] |= STAIRS;
}

get_armor_class(obj)
object *obj;
{
	if (obj) {
		return(obj->class + obj->d_enchant);
	}
	return(0);
}

object *
alloc_object()
{
	object *obj;

	if (free_list) {
		obj = free_list;
		free_list = free_list->next_object;
	} else if (!(obj = (object *) md_malloc(sizeof(object)))) {
#ifdef JAPAN
		message("メモリーが足りません。 ゲームをセーブします。", 0);
#else
		message("Cannot allocate object, saving game", 0);
#endif
		save_into_file(error_file);
	}
	obj->quantity = 1;
	obj->ichar = 'L';
	obj->picked_up = obj->is_cursed = 0;
	obj->in_use_flags = NOT_USED;
	obj->identified = UNIDENTIFIED;
	obj->damage = "1d1";
	return(obj);
}

free_object(obj)
object *obj;
{
	obj->next_object = free_list;
	free_list = obj;
}

make_party()
{
	short n;

	party_room = gr_room();

	n = rand_percent(99) ? party_objects(party_room) : 11;
	if (rand_percent(99)) {
		party_monsters(party_room, n);
	}
}

show_objects()
{
	object *obj;
	short mc, rc, row, col;
	object *monster;

	obj = level_objects.next_object;

	while (obj) {
		row = obj->row;
		col = obj->col;

		rc = get_mask_char(obj->what_is);

		if (dungeon[row][col] & MONSTER) {
			if (monster = object_at(&level_monsters, row, col)) {
				monster->trail_char = rc;
			}
		}
		mc = mvinch(row, col);
		if (((mc < 'A') || (mc > 'Z')) &&
			((row != rogue.row) || (col != rogue.col))) {
			mvaddch(row, col, colored(rc));
		}
		obj = obj->next_object;
	}

	monster = level_monsters.next_object;

	while (monster) {
		if (monster->m_flags & IMITATES) {
			mvaddch(monster->row, monster->col, colored(monster->disguise));
		}
		monster = monster->next_monster;
	}
}

put_amulet()
{
	object *obj;

	obj = alloc_object();
	obj->what_is = AMULET;
	rand_place(obj);
}

rand_place(obj)
object *obj;
{
	short row, col;

	gr_row_col(&row, &col, (FLOOR | TUNNEL));
	place_at(obj, row, col);

}

new_object_for_wizard()
{
	short ch, max;
#ifdef ORIGINAL
	short wk;
#endif
	object *obj;
	char buf[80];

	if (pack_count((object *) 0) >= MAX_PACK_COUNT) {
#ifdef JAPAN
		message("これ以上は物を持てない。", 0);
#else
		message("Pack full", 0);
#endif
		return;
	}
#ifdef JAPAN
	message("どんなものですか？", 0);
#else
	message("Type of object?", 0);
#endif

	while (r_index("!?:)]=/,\033", (ch = rgetchar()), 0) == -1)
		sound_bell();
	check_message();
	if (ch == '\033')
		return;

	obj = alloc_object();

	switch(ch) {
	case '!':
		obj->what_is = POTION;
		max = POTIONS - 1;
		break;
	case '?':
		obj->what_is = SCROL;
		max = SCROLS - 1;
		break;
	case ',':
		obj->what_is = AMULET;
		break;
	case ':':
		get_food(obj, 0);
		break;
	case ')':
		gr_weapon(obj, 0);
		max = WEAPONS - 1;
		break;
	case ']':
		gr_armor(obj);
		max = ARMORS - 1;
		break;
	case '/':
		gr_wand(obj);
		max = WANDS - 1;
		break;
	case '=':
		max = RINGS - 1;
		obj->what_is = RING;
		break;
	}
	if ((ch != ',') && (ch != ':')) {
#ifndef ORIGINAL
#ifdef JAPAN
		sprintf(buf, "%sの種類は？", name_of(obj));
#else
		sprintf(buf, "Which kind of %s?", name_of(obj));
#endif
		for (;;) {
			message(buf, 0);
			for (;;) {
				ch = rgetchar();
				if (ch != LIST && ch != CANCEL
						&& ch < 'a' || ch > 'a'+max)
					sound_bell();
				else
					break;
			}
			if (ch == LIST) {
				check_message();
				list_object(obj, max);
			} else
				break;
		}
		check_message();
		if (ch == CANCEL) {
			free_object(obj);
			return;
		}
		obj->which_kind = ch - 'a';
		if (obj->what_is == RING)
			gr_ring(obj, 0);
#else /*ORIGINAL*/
		if (get_input_line("Which kind?", "", buf, "", 0, 1)) {
			wk = get_number(buf);
			if ((wk >= 0) && (wk <= max)) {
				obj->which_kind = (unsigned short) wk;
				if (obj->what_is == RING) {
					gr_ring(obj, 0);
				}
			} else {
				sound_bell();
				goto GIL;
			}
		} else {
			free_object(obj);
			return;
		}
#endif /*ORIGINAL*/
	}
	get_desc(obj, buf, 1);
	message(buf, 0);
	(void) add_to_pack(obj, &rogue.pack, 1);
}

#ifndef ORIGINAL
list_object(obj, max)
object *obj;
short max;
{
	short i, j, maxlen, n;
	char descs[DROWS][DCOLS];
	short row, col;
	struct id *id;
#ifdef USE_COLOR
	char *p;
#endif
#ifdef JAPAN
	char *msg = "  ＝スペースを押してください＝";
	short len = 30;
#else
	char *msg = " --Press space to continue--";
	short len = 28;
#endif

	switch (obj->what_is) {
	case ARMOR:  id = id_armors;  break;
	case WEAPON: id = id_weapons; break;
	case SCROL:  id = id_scrolls; break;
	case POTION: id = id_potions; break;
	case WAND:   id = id_wands;   break;
	case RING:   id = id_rings;   break;
	default:     return;
	}

	maxlen = len;
	for (i = 0; i <= max; i++) {
#ifdef JAPAN
		sprintf(descs[i]," %c) %s%s",i+'a',id[i].real,name_of(obj));
#else
		sprintf(descs[i]," %c) %s%s",i+'a',name_of(obj),id[i].real);
#endif
		if ((n = strlen(descs[i])) > maxlen)
			maxlen = n;
	}
	(void) strcpy(descs[i++], msg);

	col = DCOLS - (maxlen + 2);
	for (row = 0; row < i; row++) {
		if (row > 0) {
			for (j = col; j < DCOLS; j++) {
				descs[row-1][j-col] = mvinch(row, j);
			}
			descs[row-1][j-col] = 0;
		}
		mvaddstr(row, col, descs[row]);
		clrtoeol();
	}
	refresh();
	wait_for_ack();

	move(0, 0);
	clrtoeol();
#ifdef USE_COLOR
	for (j = 1; j < i; j++) {
		move(j, col);
		for (p = descs[j-1]; *p; p++)
			addch(colored(*p));
	}
#else
	for (j = 1; j < i; j++)
		mvaddstr(j, col, descs[j-1]);
#endif
}
#endif /*ORIGINAL*/

next_party()
{
	int n;

	n = cur_level;
	while (n % PARTY_TIME) {
		n++;
	}
	return(get_rand((n + 1), (n + PARTY_TIME)));
}
