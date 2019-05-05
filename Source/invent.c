/*
 * invent.c
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

#define swap_string(x,y) {t = x; x = y; y = t;}

boolean is_wood[WANDS];

#ifdef JAPAN
char *wand_materials[WAND_MATERIALS] = {
	"鋼鉄",
	"青銅",
	"金",
	"銀",
	"銅",
	"ニッケル",
	"コバルト",
	"すず",
	"鉄",
	"マグネシウム",
	"クロム",
	"石炭",
	"プラチナ",
	"シリコン",
	"チタン",
	"チーク",
	"樫",
	"桜",
	"白樺",
	"松",
	"杉",
	"アメリカ杉",
	"バルサ",
	"象牙やし",
	"胡桃",
	"楓",
	"マホガニー",
	"楡",
	"やし",
	"柿"
};

char *gems[GEMS] = {
	"ダイヤ",
	"アクアマリン",
	"るり",
	"ルビー",
	"エメラルド",
	"サファイア",
	"紫水晶",
	"水晶",
	"虎目石",
	"オパール",
	"めのう",
	"トルコ石",
	"真珠",
	"ざくろ石"
};

char *syllables[MAXSYLLABLES] = {
	"ぶん ",
	"に ",
	"とす ",
	"ら ",
	"ぽみ ",
	"ぷ ",
	"どん ",
	"せろ ",
	"い ",
	"か ",
	"そろ ",
	"む ",
	"ろお ",
	"わ ",
	"おふ ",
	"ろん ",
	"わっと ",
	"ぴお ",
	"ぽんぽ ",
	"どどら ",
	"ひゅ ",
	"よふ ",
	"ぽた ",
	"るん ",
	"ばび ",
	"ろろり ",
	"ずず ",
	"ぐり ",
	"こも ",
	"きき ",
	"およ ",
	"いむ ",
	"にゃあ ",
	"うー ",
	"ぐふる ",
	"こぴ ",
	"すーる ",
	"ぽろん ",
	"みーる ",
	"ぽきし "
};
#else
char *wand_materials[WAND_MATERIALS] = {
	"steel ",
	"bronze ",
	"gold ",
	"silver ",
	"copper ",
	"nickel ",
	"cobalt ",
	"tin ",
	"iron ",
	"magnesium ",
	"chrome ",
	"carbon ",
	"platinum ",
	"silicon ",
	"titanium ",

	"teak ",
	"oak ",
	"cherry ",
	"birch ",
	"pine ",
	"cedar ",
	"redwood ",
	"balsa ",
	"ivory ",
	"walnut ",
	"maple ",
	"mahogany ",
	"elm ",
	"palm ",
	"wooden "
};

char *gems[GEMS] = {
	"diamond ",
	"stibotantalite ",
#ifndef ORIGINAL
	"lapis lazuli ",
#else
	"lapi-lazuli ",
#endif
	"ruby ",
	"emerald ",
	"sapphire ",
	"amethyst ",
	"quartz ",
#ifndef ORIGINAL
	"tiger eye ",
#else
	"tiger-eye ",
#endif
	"opal ",
	"agate ",
	"turquoise ",
	"pearl ",
	"garnet "
};

char *syllables[MAXSYLLABLES] = {
	"blech ",
	"foo ",
	"barf ",
	"rech ",
	"bar ",
	"blech ",
	"quo ",
	"bloto ",
	"woh ",
	"caca ",
	"blorp ",
	"erp ",
	"festr ",
	"rot ",
	"slie ",
	"snorf ",
	"iky ",
	"yuky ",
	"ooze ",
	"ah ",
	"bahl ",
	"zep ",
	"druhl ",
	"flem ",
	"behil ",
	"arek ",
	"mep ",
	"zihr ",
	"grit ",
	"kona ",
	"kini ",
	"ichi ",
	"niah ",
	"ogr ",
	"ooh ",
	"ighr ",
	"coph ",
	"swerr ",
	"mihln ",
	"poxi "
};
#endif

extern boolean wizard;

char descs[DROWS][DCOLS];	/* multi-purpose screen saver */

inventory(pack, mask)
object *pack;
unsigned short mask;
{
	object *obj;
	short i, j, maxlen, n;
	short row, col;
	char *p;
#ifdef JAPAN
	char *msg = "  ＝スペースを押してください＝";
	short len = 30;
#else
	char *msg = " --Press space to continue--";
	short len = 28;
#endif

	if (!(obj = pack->next_object)) {
#ifdef JAPAN
		message("持ちものは、１つも持っていない。", 0);
#else
		message("Your pack is empty", 0);
#endif
		return;
	}

#define	Protected(obj)	((obj->what_is & ARMOR) && obj->is_protected)
nextpage:
	i = 0;
	maxlen = len;
	while (obj && i < DROWS - 2) {
		if (obj->what_is & mask) {
			p = descs[i];
			*p++ = ' ';
			*p++ = obj->ichar;
			*p++ = Protected(obj)? '}': ')';
			*p++ = ' ';
			get_desc(obj, p, 0);
			if ((n = strlen(descs[i])) > maxlen)
				maxlen = n;
			i++;
		}
		obj = obj->next_object;
	}
	(void) strcpy(descs[i++], msg);

	if (i == 0)
		return;

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
#ifdef JAPAN
	for (j = 0; j < i; j++)
		touch(j, col, DCOLS-1);
#endif
	if (obj)
		goto nextpage;
}

mix_colors()
{
	short i, j;
	char *t;
	extern char *po_color[];

	for (i = 0; i < POTIONS; i++) {
		id_potions[i].title = po_color[i];
	}
	for (i = 0; i < POTIONS; i++) {
		j = get_rand(i, POTIONS - 1);
		swap_string(id_potions[i].title, id_potions[j].title);
	}
}

make_scroll_titles()
{
	short i, j, n, len;
	short sylls, s;

	for (i = 0; i < SCROLS; i++) {
		sylls = get_rand(2, 5);
#ifdef JAPAN
		(void) strcpy(id_scrolls[i].title, "「");
		len = 2;
		for (j = 0; j < sylls; j++) {
			s = get_rand(1, (MAXSYLLABLES-1));
			n = strlen(syllables[s]);
			if (len + n - 1 >= MAX_TITLE_LENGTH - 2)
				break;
			(void) strcat(id_scrolls[i].title, syllables[s]);
			len += n;
		}
		(void) strcpy(id_scrolls[i].title+(len-1), "」");
#else
		(void) strcpy(id_scrolls[i].title, "'");
		for (j = 0; j < sylls; j++) {
			s = get_rand(1, (MAXSYLLABLES-1));
			(void) strcat(id_scrolls[i].title, syllables[s]);
		}
		n = strlen(id_scrolls[i].title);
		(void) strcpy(id_scrolls[i].title+(n-1), "' ");
#endif
	}
}

#ifdef JAPAN /* for whole function */
get_desc(obj, desc, capitalized)
register object *obj;
register char *desc;
boolean capitalized;
{
#ifdef  JAPAN
	unsigned char *item_name, *p;
#else
	char *item_name, *p;
#endif    
	struct id *id_table;
	char more_info[32];
	short i;

	*desc = 0;
	if (obj->what_is == AMULET) {
		(void) strcpy(desc, "イェンダーの魔除け");
		return;
	}
	item_name = name_of(obj);
	if (obj->what_is == GOLD) {
		znum(desc, obj->quantity, 0);
		strcat(desc, "個の金塊");
		return;
	}
	if (obj->what_is == WEAPON && obj->quantity > 1) {
		znum(desc, obj->quantity, 0);
		strcat(desc, "本の");
	} else if (obj->what_is == FOOD) {
		znum(desc, obj->quantity, 0);
		strcat(desc, (obj->which_kind == RATION)? "袋の": "個の");
		(void) strcat(desc, item_name);
		goto ANA;
	} else if (obj->what_is != ARMOR && obj->quantity > 1) {
		znum(desc, obj->quantity, 0);
		strcat(desc, "つの");
	}
	id_table = get_id_table(obj);

	if (wizard) {
		goto ID;
	}
	if (obj->what_is & (WEAPON | ARMOR | WAND | RING)) {
		goto CHECK;
	}

	switch(id_table[obj->which_kind].id_status) {
	case UNIDENTIFIED:
CHECK:
		switch(obj->what_is) {
		case SCROL:
			(void) strcat(desc, id_table[obj->which_kind].title);
			(void) strcat(desc, "という");
			(void) strcat(desc, item_name);
			break;
		case POTION:
			(void) strcat(desc, id_table[obj->which_kind].title);
			(void) strcat(desc, item_name);
			break;
		case WAND:
		case RING:
			if (obj->identified ||
			(id_table[obj->which_kind].id_status == IDENTIFIED)) {
				goto ID;
			}
			if (id_table[obj->which_kind].id_status == CALLED) {
				goto CALL;
			}
			(void) strcat(desc, id_table[obj->which_kind].title);
			(void) strcat(desc, item_name);
			break;
		case ARMOR:
			if (obj->identified) {
				goto ID;
			}
			(void) strcpy(desc, id_table[obj->which_kind].title);
			break;
		case WEAPON:
			if (obj->identified) {
				goto ID;
			}
			(void) strcat(desc, name_of(obj));
			break;
		}
		break;
	case CALLED:
CALL:	switch(obj->what_is) {
		case SCROL:
		case POTION:
		case WAND:
		case RING:
			p = id_table[obj->which_kind].title;
			if (*desc && (*p >= ' ' && *p <= '~' ||
						*p >= 0xa0 && *p <= 0xdf))
				(void) strcat(desc, " ");
			(void) strcat(desc, p);
			(void) strcat(desc, "と呼ぶ");
			(void) strcat(desc, item_name);
			break;
		}
		break;
	case IDENTIFIED:
ID:		switch(obj->what_is) {
		case SCROL:
		case POTION:
			(void) strcat(desc, id_table[obj->which_kind].real);
			(void) strcat(desc, item_name);
			break;
		case RING:
			(void) strcat(desc, id_table[obj->which_kind].real);
			if (wizard || obj->identified) {
				if ((obj->which_kind == DEXTERITY) ||
					(obj->which_kind == ADD_STRENGTH)) {
					strcpy(more_info, "（");
					znum(more_info, obj->class, 1);
					strcat(more_info, "）");
					(void) strcat(desc, more_info);
				}
			}
			(void) strcat(desc, item_name);
			break;
		case WAND:
			(void) strcat(desc, id_table[obj->which_kind].real);
			(void) strcat(desc, item_name);
			if (wizard || obj->identified) {
				strcpy(more_info, "［");
				znum(more_info, obj->class, 0);
				strcat(more_info, "］");
				(void) strcat(desc, more_info);
			}
			break;
		case ARMOR:
			strcpy(desc, "（");
			znum(desc, obj->d_enchant, 1);
			strcat(desc, "）");
			(void) strcat(desc, id_table[obj->which_kind].title);
			strcpy(more_info, "［");
			znum(more_info, get_armor_class(obj), 0);
			strcat(more_info, "］");
			(void) strcat(desc, more_info);
			break;
		case WEAPON:
			strcat(desc, "（");
			znum(desc, obj->hit_enchant, 1);
			strcat(desc, "，");
			znum(desc, obj->d_enchant, 1);
			strcat(desc, "）");
			(void) strcat(desc, name_of(obj));
			break;
		}
		break;
	}
ANA:
	i = obj->in_use_flags;
	if (i & BEING_WIELDED) {
		p = "（使っている）";
	} else if (i & BEING_WORN) {
		p = "（着ている）";
	} else if (i & ON_LEFT_HAND) {
		p = "（左手）";
	} else if (i & ON_RIGHT_HAND) {
		p = "（右手）";
	} else {
		p = "";
	}
	(void) strcat(desc, p);
}

#else /*JAPAN for whole function get_desc */

get_desc(obj, desc, capitalized)
object *obj;
char *desc;
boolean capitalized;
{
	char *p;
	char *item_name;
	struct id *id_table;
	char more_info[32];
	short i;

	if (obj->what_is == AMULET) {
		(void) strcpy(desc, "The Amulet of Yendor ");
		if (!capitalized)
			*desc = 't';
		return;
	}
	item_name = name_of(obj);

	if (obj->what_is == GOLD) {
		sprintf(desc, "%d pieces of gold", obj->quantity);
		return;
	}

	if (obj->what_is != ARMOR) {
		if (obj->quantity == 1) {
			strcpy(desc, capitalized? "A ": "a ");
		} else {
			sprintf(desc, "%d ", obj->quantity);
		}
	}
	if (obj->what_is == FOOD) {
		if (obj->which_kind == RATION) {
			if (obj->quantity > 1) {
				sprintf(desc, "%d rations of ", obj->quantity);
			} else {
				strcpy(desc, capitalized? "Some ": "some ");
			}
		} else {
			strcpy(desc, capitalized? "A ": "a ");
		}
		(void) strcat(desc, item_name);
		goto ANA;
	}
	id_table = get_id_table(obj);

	if (wizard) {
		goto ID;
	}
	if (obj->what_is & (WEAPON | ARMOR | WAND | RING)) {
		goto CHECK;
	}

	switch(id_table[obj->which_kind].id_status) {
	case UNIDENTIFIED:
CHECK:
		switch(obj->what_is) {
		case SCROL:
			(void) strcat(desc, item_name);
#ifndef ORIGINAL
			(void) strcat(desc, "titled ");
#else
			(void) strcat(desc, "entitled: ");
#endif
			(void) strcat(desc, id_table[obj->which_kind].title);
			break;
		case POTION:
			(void) strcat(desc, id_table[obj->which_kind].title);
			(void) strcat(desc, item_name);
			break;
		case WAND:
		case RING:
			if (obj->identified ||
			(id_table[obj->which_kind].id_status == IDENTIFIED)) {
				goto ID;
			}
			if (id_table[obj->which_kind].id_status == CALLED) {
				goto CALL;
			}
			(void) strcat(desc, id_table[obj->which_kind].title);
			(void) strcat(desc, item_name);
			break;
		case ARMOR:
			if (obj->identified) {
				goto ID;
			}
			(void) strcpy(desc, id_table[obj->which_kind].title);
			break;
		case WEAPON:
			if (obj->identified) {
				goto ID;
			}
			(void) strcat(desc, name_of(obj));
			break;
		}
		break;
	case CALLED:
CALL:	switch(obj->what_is) {
		case SCROL:
		case POTION:
		case WAND:
		case RING:
			(void) strcat(desc, item_name);
			(void) strcat(desc, "called ");
			(void) strcat(desc, id_table[obj->which_kind].title);
			break;
		}
		break;
	case IDENTIFIED:
ID:		switch(obj->what_is) {
		case SCROL:
		case POTION:
			(void) strcat(desc, item_name);
			(void) strcat(desc, id_table[obj->which_kind].real);
			break;
		case RING:
			if (wizard || obj->identified) {
				if ((obj->which_kind == DEXTERITY) ||
					(obj->which_kind == ADD_STRENGTH)) {
					sprintf(more_info, "%s%d ",
						((obj->class > 0) ? "+" : ""),
						obj->class);
					(void) strcat(desc, more_info);
				}
			}
			(void) strcat(desc, item_name);
			(void) strcat(desc, id_table[obj->which_kind].real);
			break;
		case WAND:
			(void) strcat(desc, item_name);
			(void) strcat(desc, id_table[obj->which_kind].real);
			if (wizard || obj->identified) {
				sprintf(more_info, "[%d]", obj->class);
				(void) strcat(desc, more_info);
			}
			break;
		case ARMOR:
			sprintf(desc, "%s%d ",
				((obj->d_enchant >= 0) ? "+" : ""),
				obj->d_enchant);
			(void) strcat(desc, id_table[obj->which_kind].title);
			sprintf(more_info, "[%d] ", get_armor_class(obj));
			(void) strcat(desc, more_info);
			break;
		case WEAPON:
			sprintf(desc+strlen(desc), "%s%d, %s%d ",
				((obj->hit_enchant >= 0) ? "+" : ""),
				obj->hit_enchant,
				((obj->d_enchant >= 0) ? "+" : ""),
				obj->d_enchant);
			(void) strcat(desc, name_of(obj));
			break;
		}
		break;
	}
ANA:
	if (!strncmp(desc, (capitalized? "A ": "a "), 2)) {
		if (is_vowel(desc[2])) {
			for (i = strlen(desc) + 1; i > 1; i--) {
				desc[i] = desc[i-1];
			}
			desc[1] = 'n';
		}
	}
	i = obj->in_use_flags;
	if (i & BEING_WIELDED) {
		p = "(weapon in hand)";
	} else if (i & BEING_WORN) {
		p = "(being worn)";
	} else if (i & ON_LEFT_HAND) {
		p = "(on left hand)";
	} else if (i & ON_RIGHT_HAND) {
		p = "(on right hand)";
	} else {
		p = "";
	}
	(void) strcat(desc, p);
}
#endif /*JAPAN for whole function get_desc() */

get_wand_and_ring_materials()
{
	short i, j;
	char *p;
	boolean used[WAND_MATERIALS];

	for (i = 0; i < WAND_MATERIALS; i++) {
		used[i] = 0;
	}
	for (i = 0; i < WANDS; i++) {
		do {
			j = get_rand(0, WAND_MATERIALS-1);
		} while (used[j]);
		used[j] = 1;
		p = id_wands[i].title;
		(void) strcpy(p, wand_materials[j]);
#ifdef JAPAN
		(void) strcat(p, "の");
#endif
		is_wood[i] = (j > MAX_METAL);
	}
	for (i = 0; i < GEMS; i++) {
		used[i] = 0;
	}
	for (i = 0; i < RINGS; i++) {
		do {
			j = get_rand(0, GEMS-1);
		} while (used[j]);
		used[j] = 1;
		p = id_rings[i].title;
		(void) strcpy(p, gems[j]);
#ifdef JAPAN
		(void) strcat(p, "の");
#endif
	}
}

single_inv(ichar)
short ichar;
{
	short ch;
	char *p;
	object *obj;
	char desc[DCOLS];

#ifdef JAPAN
	ch = ichar? ichar: pack_letter("調べる持ちものの種類は？",ALL_OBJECTS);
#else
	ch = ichar ? ichar : pack_letter("Inventory what?", ALL_OBJECTS);
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
	p = desc;
	*p++ = ch;
	*p++ = ((obj->what_is & ARMOR) && obj->is_protected) ? '}' : ')';
	*p++ = ' ';
	get_desc(obj, p, 1);
	message(desc, 0);
}

struct id *
get_id_table(obj)
object *obj;
{
	switch(obj->what_is) {
	case SCROL:
		return(id_scrolls);
	case POTION:
		return(id_potions);
	case WAND:
		return(id_wands);
	case RING:
		return(id_rings);
	case WEAPON:
		return(id_weapons);
	case ARMOR:
		return(id_armors);
	}
	return((struct id *) 0);
}

inv_armor_weapon(is_weapon)
boolean is_weapon;
{
	if (is_weapon) {
		if (rogue.weapon) {
			single_inv(rogue.weapon->ichar);
		} else {
#ifdef JAPAN
			message("武器は使っていない。", 0);
#else
			message("Not wielding anything", 0);
#endif
		}
	} else {
		if (rogue.armor) {
			single_inv(rogue.armor->ichar);
		} else {
#ifdef JAPAN
			message("よろいは着ていない。", 0);
#else
			message("Not wearing anything", 0);
#endif
		}
	}
}

#ifndef ORIGINAL
struct	dlist {
	short	type, no;
	char	*name, *real;
#ifdef JAPAN
	char	*sub;
#endif
}	dlist[SCROLS + POTIONS + WANDS + RINGS + 4];

struct	dobj {
	short	type;
	char	ch, max;
	char	*name;
	struct	id *id;
}	dobj[] = {
#ifdef JAPAN
		{ SCROL,  '?', SCROLS,  "巻き物",  id_scrolls },
		{ POTION, '!', POTIONS, "水薬",    id_potions },
		{ WAND,   '/', WANDS,   "つえ",    id_wands   },
		{ RING,   '=', RINGS,   "指輪",    id_rings   },
#else
		{ SCROL,  '?', SCROLS,  "scroll ", id_scrolls },
		{ POTION, '!', POTIONS, "potion ", id_potions },
		{ WAND,   '/', WANDS,   "wand ",   id_wands   },
		{ RING,   '=', RINGS,   "ring ",   id_rings   },
#endif
		{ 0 }
};

discovered()
{
	short i, j, n;
	short ch, maxlen, found;
	short row, col;
	struct dlist *dp, *enddp;
	struct dobj *op;
	char *p;
#ifdef JAPAN
	char *msg = "  ＝スペースを押してください＝";
	short len = 30;
#else
	char *msg = " --Press space to continue--";
	short len = 28;
#endif

#ifdef JAPAN
	message("どの種類のものですか？", 0);
#else
	message("What type?", 0);
#endif
	while (r_index("?!/=*\033", (ch = rgetchar()), 0) == -1)
		sound_bell();
	check_message();
	if (ch == '\033')
		return;

	found = 0;
	dp = dlist;
	for (op = dobj; op->type; op++) {
		if (ch != op->ch && ch != '*')
			continue;
		for (i = 0; i < op->max; i++) {
			j = op->id[i].id_status;
			if (j == IDENTIFIED || j == CALLED) {
				dp->type = op->type;
				dp->no = i;
				dp->name = op->name;
				if (wizard || j == IDENTIFIED) {
					dp->real = op->id[i].real;
#ifdef JAPAN
					dp->sub  = "";
#endif
				} else {
					dp->real = op->id[i].title;
#ifdef JAPAN
					dp->sub  = "と呼ぶ";
#endif
				}
#ifndef JAPAN
				if (op->type == WAND && is_wood[i])
					dp->name = "staff ";
#endif
				found |= op->type;
				dp++;
			}
		}
		if ((found & op->type) == 0) {
			dp->type = op->type;
			dp->no = -1;
			dp->name = op->name;
			dp++;
		}
		dp->type = 0;
		dp++;
	}
	enddp = dp;

	if (found == 0) {
#ifdef JAPAN
		message("知っているものは、何もない。", 0);
#else
		message("Nothing discovered", 0);
#endif
		return;
	}

	dp = dlist;

nextpage:
	i = 0;
	maxlen = len;
	while (dp < enddp && i < DROWS-2) {
		p = descs[i];
		if (dp->type == 0) {
			(void) strcpy(p, "");
#ifdef JAPAN
		} else if (dp->no < 0) {
			(void) strcpy(p, "  （知っている");
			(void) strcat(p, dp->name);
			(void) strcat(p, "はない）");
		} else {
			(void) strcpy(p, "  ");
			(void) strcat(p, dp->real);
			(void) strcat(p, dp->sub);
			(void) strcat(p, dp->name);
#else
		} else if (dp->no < 0) {
			(void) strcpy(p, " Nothing about ");
			(void) strcat(p, dp->name);
			descs[i][strlen(p)-1] = 's';
		} else {
			p[0] = ' ';
			(void) strcpy(p+1, dp->name);
			(void) strcat(p, dp->real);
			p[1] -= 'a' - 'A';
#endif
		}
		if ((n = strlen(p)) > maxlen)
			maxlen = n;
		i++;
		dp++;
	}

	if (i == 0 || i == 1 && !descs[0][0]) {
		/*
		 * can be here only in 2nd pass (exactly one page)
		 */
		return;
	}

	strcpy(descs[i++], msg);
	col = DCOLS - (maxlen + 2);
	for (row = 0; row < i; row++) {
		if (row > 0) {
			for (j = col; j < DCOLS; j++)
				descs[row-1][j-col] = mvinch(row, j);
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
#ifdef JAPAN
	for (j = 0; j < i; j++)
		touch(j, col, DCOLS-1);
#endif
	if (dp < enddp)
		goto nextpage;
}
#endif /*ORIGINAL*/

#ifdef JAPAN
static char *_num[10] = { "０","１","２","３","４","５","６","７","８","９" };

znum(buf, n, plus)
char *buf;
int n, plus;
{
	char s[10], *p;

	while (*buf)
		buf++;
	if (plus && n >= 0) {
		strcpy(buf, "＋");
		buf += 2;
	}
	sprintf(s, "%d", n);
	for (p = s; *p; p++) {
		strcpy(buf, (*p == '-')? "－": _num[*p - '0']);
		buf += 2;
	}
}

lznum(buf, n, plus)
char *buf;
long n;
int plus;
{
	char s[13], *p;

	while (*buf)
		buf++;
	if (plus && n >= 0L) {
		strcpy(buf, "＋");
		buf += 2;
	}
	sprintf(s, "%ld", n);
	for (p = s; *p; p++) {
		strcpy(buf, (*p == '-')? "－": _num[*p - '0']);
		buf += 2;
	}
}
#endif
