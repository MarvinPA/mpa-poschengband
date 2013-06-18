#include "angband.h"

#define MON_MARILITH 940 /* TODO: 940 is actually a Lesser Balrog */

static cptr _desc = 
	"Demons are powerful servants of evil and come in many forms. Being monsters, they "
	"may not choose a normal class. Instead, they rely on their devilish powers or their "
	"brutish strength to survive.\n \n"
	"Balrogs are powerful in melee and have access to a wide array of demonic spells "
	"of great power. Their devilish offense is unrivaled among all the monster races and "
	"at high levels they may unleash deadly hellfire upon their foes. Their summoning talents "
	"are also very good, but summoning a high demon requires a human sacrifice. "
	"Intelligence determines their casting abilities. Eventually, balrogs become "
	"immune to the charge draining attacks of their enemies.\n \n"
	"Tanar'ri are a bit weaker then Balrogs, but eventually evolve into multi-armed demons "
	"capable of attacking their enemies with many weapons. Their devilish powers are influenced "
	"by their intelligence.\n \n"
	"Cyberdemons are evil constructions. Big, slow and stupid, they possess unsurpassable firepower. "
	"Their cyberpowers are influenced by their constitution.\n \n"
	"Servants of Khorne come in many forms and are powerful demons in melee. They lack special "
	"powers.\n \n"
	"All demon races cannot eat normal food, but must feast upon the remains of their human "
	"enemies. They are unaffected by the Eldritch Horror.";

static equip_template_t _marilith_template = {12, {
	{EQUIP_SLOT_WEAPON_SHIELD, "Arm", 0},
	{EQUIP_SLOT_WEAPON_SHIELD, "Arm", 1},
	{EQUIP_SLOT_RING, "Ring", 0},
	{EQUIP_SLOT_RING, "Ring", 1},
	{EQUIP_SLOT_GLOVES, "Gloves", 0}, 
	{EQUIP_SLOT_WEAPON_SHIELD, "Arm", 2},
	{EQUIP_SLOT_WEAPON_SHIELD, "Arm", 3},
	{EQUIP_SLOT_GLOVES, "Gloves", 2}, 
	{EQUIP_SLOT_WEAPON_SHIELD, "Arm", 4},
	{EQUIP_SLOT_WEAPON_SHIELD, "Arm", 5},
	{EQUIP_SLOT_AMULET, "Neck", 0},
	{EQUIP_SLOT_LITE, "Light", 0},
}};

static equip_template_t _fleshhound_template = {10, { 
	{EQUIP_SLOT_RING, "Ring", 0},
	{EQUIP_SLOT_RING, "Ring", 0},
	{EQUIP_SLOT_RING, "Ring", 0},
	{EQUIP_SLOT_RING, "Ring", 0},
	{EQUIP_SLOT_AMULET, "Amulet", 0},
	{EQUIP_SLOT_LITE, "Light", 0},
	{EQUIP_SLOT_BODY_ARMOR, "Body", 0},
	{EQUIP_SLOT_CLOAK, "Cloak", 0},
	{EQUIP_SLOT_HELMET, "Helm", 0},
	{EQUIP_SLOT_BOOTS, "Boots", 0}, /* Back paws only ... */
}};

static equip_template_t _bloodthirster_template = {12, { 
	{EQUIP_SLOT_WEAPON_SHIELD, "Arm", 0}, /* Front paws */
	{EQUIP_SLOT_WEAPON_SHIELD, "Arm", 1},
	{EQUIP_SLOT_RING, "Ring", 0},
	{EQUIP_SLOT_RING, "Ring", 0},
	{EQUIP_SLOT_RING, "Ring", 1},
	{EQUIP_SLOT_RING, "Ring", 1},
	{EQUIP_SLOT_AMULET, "Amulet", 0},
	{EQUIP_SLOT_LITE, "Light", 0},
	{EQUIP_SLOT_BODY_ARMOR, "Body", 0},
	{EQUIP_SLOT_CLOAK, "Cloak", 0},
	{EQUIP_SLOT_HELMET, "Helm", 0},
	{EQUIP_SLOT_BOOTS, "Boots", 0}, /* Back paws only */
}};


static caster_info * _caster_info(void)
{
	static caster_info me = {0};
	static bool init = FALSE;
	if (!init)
	{
		me.magic_desc = "devilish power";
		me.which_stat = A_INT;
		me.weight = 750;
		me.options = CASTER_ALLOW_DEC_MANA;
		init = TRUE;
	}
	return &me;
}

/******************************************************************************
 *                                20            30            40
 * Servant of Khorne: Bloodletter -> Fleshhound -> Juggernaut -> Bloodthirster
 ******************************************************************************/
static void _khorne_birth(void) 
{ 
	object_type	forge;

	p_ptr->current_r_idx = MON_BLOODLETTER_KHORNE;

	object_prep(&forge, lookup_kind(TV_RING, SV_RING_DAMAGE));
	forge.to_d = 6;
	add_outfit(&forge);
	
	object_prep(&forge, lookup_kind(TV_SOFT_ARMOR, SV_LEATHER_SCALE_MAIL));
	add_outfit(&forge);

	object_prep(&forge, lookup_kind(TV_SWORD, SV_BLADE_OF_CHAOS));
	forge.name2 = EGO_SHARPNESS;
	forge.pval = 1;
	forge.to_h = 0;
	forge.to_d = 0;
	forge.ds = 6;
	forge.weight = 220;
	add_outfit(&forge);
}

static void _khorne_calc_innate_attacks(void) 
{
	if (p_ptr->current_r_idx == MON_FLESHHOUND_KHORNE)
	{
		int l = p_ptr->lev;
		int to_d = l/10 + l*l/500 + l*l*l/25000;
		int to_h = l/5 + l*l/250 + l*l*l/12500;

		/* Claws */
		{
			innate_attack_t	a = {0};

			a.dd = 5;
			a.ds = 2;
			a.to_d += to_d;
			a.to_h += to_h;

			a.weight = 100;
			calc_innate_blows(&a, 2);
			a.msg = "You claw %s.";
			a.name = "Claw";

			p_ptr->innate_attacks[p_ptr->innate_attack_ct++] = a;
		}
		/* Bite */
		{
			innate_attack_t	a = {0};

			a.dd = 10;
			a.ds = 2;
			a.to_d += to_d;
			a.to_h += to_h;

			a.weight = 200;

			calc_innate_blows(&a, 3);
			a.msg = "You bite %s.";
			a.name = "Bite";
			p_ptr->innate_attacks[p_ptr->innate_attack_ct++] = a;
		}
	}
	else if (p_ptr->current_r_idx == MON_JUGGERNAUT_KHORNE)
	{
		int l = p_ptr->lev;
		int to_d = l/10 + l*l/500 + l*l*l/25000;
		int to_h = l/5 + l*l/250 + l*l*l/12500;

		/* Claws */
		{
			innate_attack_t	a = {0};

			a.dd = 8;
			a.ds = 6;
			a.to_d += to_d;
			a.to_h += to_h;

			a.weight = 200;
			calc_innate_blows(&a, 4);
			a.msg = "You crush %s.";
			a.name = "Claw";

			p_ptr->innate_attacks[p_ptr->innate_attack_ct++] = a;
		}
		/* Bite */
		{
			innate_attack_t	a = {0};

			a.dd = 10;
			a.ds = 6;
			a.to_d += to_d;
			a.to_h += to_h;

			a.weight = 500;

			calc_innate_blows(&a, 2);
			a.msg = "You butt %s.";
			a.name = "Head";
			p_ptr->innate_attacks[p_ptr->innate_attack_ct++] = a;
		}
	}
}

static void _khorne_calc_bonuses(void) 
{
	p_ptr->align -= 200;

	res_add(RES_FIRE);
	res_add(RES_NETHER);
	
	p_ptr->slow_digest = TRUE;
	p_ptr->hold_life = TRUE;
	p_ptr->no_eldritch = TRUE;

	switch (p_ptr->current_r_idx)
	{
	case MON_BLOODLETTER_KHORNE:
		p_ptr->regenerate = TRUE;
		res_add(RES_COLD);
		res_add(RES_POIS);
		res_add(RES_CHAOS);
		break;
	case MON_FLESHHOUND_KHORNE:
		p_ptr->pspeed += 2;
		res_add(RES_CONF);
		res_add(RES_NEXUS);
		res_add(RES_DISEN);
		break;
	case MON_JUGGERNAUT_KHORNE:
		p_ptr->pspeed += 4;
		p_ptr->to_a += 100;
		p_ptr->dis_to_a += 100;
		p_ptr->reflect = TRUE;
		p_ptr->free_act = TRUE;
		p_ptr->see_inv = TRUE;

		res_add(RES_COLD);
		res_add(RES_ELEC);
		res_add(RES_POIS);
		res_add(RES_CONF);
		res_add(RES_FEAR);
		break;
	case MON_BLOODTHIRSTER:
		p_ptr->pspeed += 6;
		p_ptr->to_a += 50;
		p_ptr->dis_to_a += 50;
		p_ptr->regenerate = TRUE;
		p_ptr->levitation = TRUE;
		p_ptr->free_act = TRUE;
		p_ptr->see_inv = TRUE;
		res_add(RES_ACID);
		res_add(RES_COLD);
		res_add(RES_POIS);
		res_add(RES_CONF);
		res_add(RES_NEXUS);
		res_add(RES_TELEPORT);
		break;
	}
}

static void _khorne_get_flags(u32b flgs[TR_FLAG_SIZE]) 
{
	add_flag(flgs, TR_RES_FIRE);
	add_flag(flgs, TR_RES_NETHER);

	add_flag(flgs, TR_HOLD_LIFE);
	add_flag(flgs, TR_SLOW_DIGEST);

	switch (p_ptr->current_r_idx)
	{
	case MON_BLOODLETTER_KHORNE:
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_CHAOS);
		break;
	case MON_FLESHHOUND_KHORNE:
		add_flag(flgs, TR_SPEED);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_NEXUS);
		add_flag(flgs, TR_RES_DISEN);
		break;
	case MON_JUGGERNAUT_KHORNE:
		add_flag(flgs, TR_SPEED);
		add_flag(flgs, TR_REFLECT);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SEE_INVIS);

		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_ELEC);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_FEAR);
		break;
	case MON_BLOODTHIRSTER:
		add_flag(flgs, TR_SPEED);
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_RES_ACID);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_NEXUS);
		break;
	}
}

static void _khorne_gain_level(int new_level) 
{
	if (p_ptr->current_r_idx == MON_BLOODLETTER_KHORNE && new_level >= 20)
	{
		p_ptr->current_r_idx = MON_FLESHHOUND_KHORNE;
		msg_print("You have evolved into a Fleshhound of Khorne.");
		equip_on_change_race();
		p_ptr->redraw |= PR_MAP;
	}
	if (p_ptr->current_r_idx == MON_FLESHHOUND_KHORNE && new_level >= 30)
	{
		p_ptr->current_r_idx = MON_JUGGERNAUT_KHORNE;
		msg_print("You have evolved into a Juggernaut of Khorne.");
		equip_on_change_race();
		p_ptr->redraw |= PR_MAP;
	}
	if (p_ptr->current_r_idx == MON_JUGGERNAUT_KHORNE && new_level >= 40)
	{
		object_type forge;
		p_ptr->current_r_idx = MON_BLOODTHIRSTER;
		msg_print("You have evolved into a Bloodthirster.");
		equip_on_change_race();
		p_ptr->redraw |= PR_MAP;
		object_prep(&forge, lookup_kind(TV_SWORD, SV_BLADE_OF_CHAOS));
		forge.name2 = EGO_VAMPIRIC; /* Prevent ?Artifact or ?WeaponBranding */
		forge.dd = 50;
		forge.ds = 1;
		forge.weight = 500;
		forge.to_h = 15;
		forge.to_d = 15;
		add_outfit(&forge);
		p_ptr->update |= PU_BONUS;
	}
}

static race_t *_khorne_get_race_t(void)
{
	static race_t me = {0};
	static bool   init = FALSE;
	static cptr   titles[4] =  {"Bloodletter of Khorne", "Fleshhound of Khorne", "Juggernaut of Khorne", "Bloodthirster"};	
	int           rank = 0;

	if (p_ptr->lev >= 20) rank++;
	if (p_ptr->lev >= 30) rank++;
	if (p_ptr->lev >= 40) rank++;

	if (!init)
	{           /* dis, dev, sav, stl, srh, fos, thn, thb */
	skills_t bs = { 20,  20,  40,  -1,  13,   7,  70,  30};
	skills_t xs = { 12,   8,  10,   0,   0,   0,  32,   7};


		me.skills = bs;
		me.extra_skills = xs;

		me.exp = 275;
		me.infra = 5;
		me.base_hp = 42;

		me.birth = _khorne_birth;
		me.calc_innate_attacks = _khorne_calc_innate_attacks;
		me.calc_bonuses = _khorne_calc_bonuses;
		me.get_flags = _khorne_get_flags;
		me.gain_level = _khorne_gain_level;
		init = TRUE;
	}

	me.subname = titles[rank];
	me.stats[A_STR] =  3 + rank;
	me.stats[A_INT] = -5;
	me.stats[A_WIS] = -5;
	me.stats[A_DEX] =  0 + rank/3;
	me.stats[A_CON] =  2 + rank;
	me.stats[A_CHR] =  rank/3;
	me.life = 100 + 6*rank;

	switch (p_ptr->current_r_idx)
	{
	case MON_FLESHHOUND_KHORNE:
	case MON_JUGGERNAUT_KHORNE:
		me.equip_template = &_fleshhound_template;
		break;
	case MON_BLOODTHIRSTER:
		me.equip_template = &_bloodthirster_template;
		break;
	default:
		me.equip_template = NULL;
	}

	me.boss_r_idx = MON_MEPHISTOPHELES;

	return &me;
}

/******************************************************************************
 *                 10        20       30              40
 * Marilith: Manes -> Quasit -> Bodak -> Death Quasit -> Marilith
 ******************************************************************************/
static spell_info _marilith_spells[] = {
	{ 10,  3, 30, phase_door_spell},
	{ 12,  5, 35, scare_spell},
	{ 15,  7, 35, teleport_spell},
	{ 17, 10, 35, slow_spell},
	{ 20,  7, 40, fire_bolt_spell},
	{ 22,  9, 40, summon_manes_spell},
	{ 25, 16, 40, fire_ball_spell},
	{ 30, 18, 45, cause_wounds_III_spell},
	{ 32, 20, 50, amnesia_spell},
	{ 36, 70, 85, summon_demon_spell},
	{ 40, 70, 70, enchantment_spell},
	{ -1, -1, -1, NULL}
};
static int _marilith_get_spells(spell_info* spells, int max) {
	return get_spells_aux(spells, max, _marilith_spells);
}

static void _marilith_birth(void) { 
	object_type	forge;

	p_ptr->current_r_idx = MON_MANES;

	object_prep(&forge, lookup_kind(TV_RING, SV_RING_DAMAGE));
	forge.to_d = 3;
	add_outfit(&forge);
	
	object_prep(&forge, lookup_kind(TV_SOFT_ARMOR, SV_LEATHER_SCALE_MAIL));
	add_outfit(&forge);

	object_prep(&forge, lookup_kind(TV_HAFTED, SV_WHIP));
	forge.name2 = EGO_BRAND_FIRE;
	forge.dd = 1;
	forge.ds = 6;
	forge.to_h = 1;
	forge.to_d = 2;
	add_outfit(&forge);
}

static void _marilith_calc_innate_attacks(void) {
	if (p_ptr->lev >= 40)
	{
		innate_attack_t	a = {0};

		a.dd = 3;
		a.ds = 7;
		a.weight = 250;
		a.to_h = p_ptr->lev/2;

		a.effect[0] = GF_MISSILE;
		a.blows = 1;

		a.msg = "You sting %s.";
		a.name = "Tail";

		p_ptr->innate_attacks[p_ptr->innate_attack_ct++] = a;
	}
}

static void _marilith_calc_bonuses(void) {
	p_ptr->align -= 200;

	res_add(RES_FIRE);
	res_add(RES_NETHER);
	
	p_ptr->slow_digest = TRUE;
	p_ptr->hold_life = TRUE;
	p_ptr->no_eldritch = TRUE;

	switch (p_ptr->current_r_idx)
	{
	case MON_QUASIT:
		p_ptr->levitation = TRUE;
		p_ptr->see_inv = TRUE;
		break;
	case MON_BODAK:
		res_add(RES_CONF);
		res_add(RES_POIS);
		p_ptr->sh_fire = TRUE;
		p_ptr->free_act = TRUE;
		p_ptr->see_inv = TRUE;
		break;
	case MON_DEATH_QUASIT:
		res_add(RES_CONF);
		res_add(RES_POIS);
		res_add(RES_TELEPORT);
		p_ptr->pspeed += 5;
		p_ptr->levitation = TRUE;
		p_ptr->pass_wall = TRUE;
		p_ptr->no_passwall_dam = TRUE;
		p_ptr->free_act = TRUE;
		p_ptr->see_inv = TRUE;
		break;
	case MON_MARILITH:
		res_add(RES_POIS);
		res_add(RES_CONF);
		res_add(RES_CHAOS);
		p_ptr->pspeed += 5;
		p_ptr->free_act = TRUE;
		p_ptr->see_inv = TRUE;
		break;
	}
}

static void _marilith_get_flags(u32b flgs[TR_FLAG_SIZE]) {
	add_flag(flgs, TR_RES_FIRE);
	add_flag(flgs, TR_RES_NETHER);

	add_flag(flgs, TR_HOLD_LIFE);
	add_flag(flgs, TR_SLOW_DIGEST);

	switch (p_ptr->current_r_idx)
	{
	case MON_QUASIT:
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_SEE_INVIS);
		break;
	case MON_BODAK:
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_SH_FIRE);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SEE_INVIS);
		break;
	case MON_DEATH_QUASIT:
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_SPEED);
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SEE_INVIS);
		break;
	case MON_MARILITH:
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_CHAOS);
		add_flag(flgs, TR_SPEED);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SEE_INVIS);
		break;
	}
}

static void _marilith_gain_level(int new_level) {
	if (p_ptr->current_r_idx == MON_MANES && new_level >= 10)
	{
		p_ptr->current_r_idx = MON_QUASIT;
		msg_print("You have evolved into a Quasit.");
		p_ptr->redraw |= PR_MAP;
	}
	if (p_ptr->current_r_idx == MON_QUASIT && new_level >= 20)
	{
		p_ptr->current_r_idx = MON_BODAK;
		msg_print("You have evolved into a Bodak.");
		p_ptr->redraw |= PR_MAP;
	}
	if (p_ptr->current_r_idx == MON_BODAK && new_level >= 30)
	{
		p_ptr->current_r_idx = MON_DEATH_QUASIT;
		msg_print("You have evolved into a Death Quasit.");
		p_ptr->redraw |= PR_MAP;
	}
	if (p_ptr->current_r_idx == MON_DEATH_QUASIT && new_level >= 40)
	{
		p_ptr->current_r_idx = MON_MARILITH;
		p_ptr->psex = SEX_FEMALE;
		sp_ptr = &sex_info[p_ptr->psex];
		msg_print("You have evolved into a Marilith.");
		equip_on_change_race();
		p_ptr->redraw |= PR_MAP;
	}
}

static race_t *_marilith_get_race_t(void)
{
	static race_t me = {0};
	static bool   init = FALSE;
	static cptr   titles[5] =  {"Manes", "Quasit", "Bodak", "Death Quasit", "Marilith"};	
	int           rank = 0;

	if (p_ptr->lev >= 10) rank++;
	if (p_ptr->lev >= 20) rank++;
	if (p_ptr->lev >= 30) rank++;
	if (p_ptr->lev >= 40) rank++;

	if (!init)
	{           /* dis, dev, sav, stl, srh, fos, thn, thb */
	skills_t bs = { 20,  35,  36,   1,  16,  10,  56,  35};
	skills_t xs = { 12,  11,  10,   0,   0,   0,  20,  11};


		me.skills = bs;
		me.extra_skills = xs;

		me.exp = 250;
		me.infra = 5;
		me.base_hp = 30;

		me.birth = _marilith_birth;
		me.calc_innate_attacks = _marilith_calc_innate_attacks;
		me.get_spells = _marilith_get_spells;
		me.calc_bonuses = _marilith_calc_bonuses;
		me.get_flags = _marilith_get_flags;
		me.gain_level = _marilith_gain_level;
		me.caster_info = _caster_info;
		init = TRUE;
	}

	me.subname = titles[rank];
	me.stats[A_STR] =  rank;
	me.stats[A_INT] =  rank/2;
	me.stats[A_WIS] = -5;
	me.stats[A_DEX] =  rank;
	me.stats[A_CON] =  rank;
	me.stats[A_CHR] =  rank/2;
	me.life = 95 + 2*rank;

	if (p_ptr->current_r_idx == MON_MARILITH)
		me.equip_template = &_marilith_template;
	else
		me.equip_template = NULL;

	me.boss_r_idx = MON_MEPHISTOPHELES;

	return &me;
}

/******************************************************************************
 *                       40
 * Balrog: Lesser Balrog -> Greater Balrog
 ******************************************************************************/
static spell_info _balrog_spells[] = {
	{  2,  1, 20, detect_unlife_spell},
	{  3,  2, 25, evil_bless_spell},
	{  4,  5, 30, resist_fire_spell},
	{  7,  5, 45, scare_spell},
	{  9,  7, 40, fire_bolt_spell},
	{ 10,  7, 40, nether_bolt_spell},
	{ 11,  9, 35, summon_manes_spell},
	{ 20, 15, 50, plasma_bolt_spell},
	{ 25, 16, 50, fire_ball_spell},
	{ 27, 20, 60, flow_of_lava_spell},
	{ 30, 25, 60, recharging_spell},
	{ 32, 28, 70, nether_ball_spell},
	{ 34, 30, 80, plasma_ball_spell},
	{ 36, 70, 85, summon_demon_spell},
	{ 37, 40, 80, kiss_of_succubus_spell},
	{ 40, 35, 50, brain_smash_spell},
	{ 43, 90, 90, summon_greater_demon_spell},
	{ 45, 80, 85, hellfire_spell},
	{ -1, -1, -1, NULL}
};
static int _balrog_get_spells(spell_info* spells, int max) {
	return get_spells_aux(spells, max, _balrog_spells);
}
static void _balrog_birth(void) 
{ 
	object_type	forge;

	p_ptr->current_r_idx = MON_LESSER_BALROG;

	object_prep(&forge, lookup_kind(TV_RING, SV_RING_DAMAGE));
	forge.to_d = 5;
	add_outfit(&forge);
	
	object_prep(&forge, lookup_kind(TV_SOFT_ARMOR, SV_LEATHER_SCALE_MAIL));
	add_outfit(&forge);

	object_prep(&forge, lookup_kind(TV_HAFTED, SV_WHIP));
	forge.name2 = EGO_BRAND_FIRE;
	forge.dd = 2;
	forge.ds = 6;
	forge.to_h = 5;
	forge.to_d = 5;
	add_outfit(&forge);
}
static void _balrog_calc_bonuses(void) {
	p_ptr->align -= 200;

	res_add(RES_FIRE);
	res_add(RES_NETHER);
	
	p_ptr->hold_life = TRUE;
	p_ptr->no_eldritch = TRUE;
	p_ptr->pspeed += p_ptr->lev/8; /* Angels get +7 speed. Demons get +6 speed. */
	p_ptr->sh_fire = TRUE;
	
	if (p_ptr->lev >= 10) 
		p_ptr->see_inv = TRUE;

	if (p_ptr->lev >= 30)
	{
		res_add(RES_FIRE);
		res_add(RES_CHAOS);
	}

	if (p_ptr->lev >= 40)
	{
		res_add_immune(RES_FIRE);
		res_add(RES_NETHER);
		p_ptr->kill_wall = TRUE;
		p_ptr->no_charge_drain = TRUE;
	}
}
static void _balrog_get_flags(u32b flgs[TR_FLAG_SIZE]) {
	add_flag(flgs, TR_RES_FIRE);
	add_flag(flgs, TR_RES_NETHER);

	add_flag(flgs, TR_HOLD_LIFE);
	add_flag(flgs, TR_SH_FIRE);

	if (p_ptr->lev >= 8)
		add_flag(flgs, TR_SPEED);
	if (p_ptr->lev >= 10)
		add_flag(flgs, TR_SEE_INVIS);
	if (p_ptr->lev >= 30)
	{
		add_flag(flgs, TR_RES_CHAOS);
	}
}
static void _balrog_get_immunities(u32b flgs[TR_FLAG_SIZE]) {
	if (p_ptr->lev >= 40)
		add_flag(flgs, TR_RES_FIRE);
}
static void _balrog_gain_level(int new_level) {
	if (p_ptr->current_r_idx == MON_LESSER_BALROG && new_level >= 40)
	{
		p_ptr->current_r_idx = MON_GREATER_BALROG;
		msg_print("You have evolved into a Greater Balrog.");
		p_ptr->redraw |= PR_MAP;
	}
}
static race_t *_balrog_get_race_t(void)
{
	static race_t me = {0};
	static bool   init = FALSE;
	static cptr   titles[2] =  {"Lesser Balrog", "Greater Balrog"};	
	int           rank = 0;

	if (p_ptr->lev >= 40) rank++;

	if (!init)
	{           /* dis, dev, sav, stl, srh, fos, thn, thb */
	skills_t bs = { 20,  35,  40,  -2,  10,   7,  75,  30};
	skills_t xs = { 12,  11,  15,   0,   0,   0,  35,   7};

		me.skills = bs;
		me.extra_skills = xs;

		me.exp = 350;
		me.base_hp = 45;

		me.birth = _balrog_birth;
		me.get_spells = _balrog_get_spells;
		me.calc_bonuses = _balrog_calc_bonuses;
		me.get_flags = _balrog_get_flags;
		me.get_immunities = _balrog_get_immunities;
		me.gain_level = _balrog_gain_level;
		me.caster_info = _caster_info;
		init = TRUE;
	}

	me.subname = titles[rank];
	me.stats[A_STR] =  4 + 3*rank;
	me.stats[A_INT] =  3 + 2*rank;
	me.stats[A_WIS] = -10;
	me.stats[A_DEX] =  2 + 2*rank;
	me.stats[A_CON] =  4 + 2*rank;
	me.stats[A_CHR] =  2 + rank;
	me.infra = 5 + 10*rank;
	me.life = 110 + 15*rank;

	me.boss_r_idx = MON_GOTHMOG;

	return &me;
}

/******************************************************************************
 * Cyberdemon
 ******************************************************************************/
static int _rocket_amount(void)
{
	int l = p_ptr->lev;
	int pct = 15 + l/5 + l*l/250 + l*l*l/12500;
	return 25 + p_ptr->chp * pct / 100;
}

void _cyber_rocket_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Rocket");
		break;
	case SPELL_DESC:
		var_set_string(res, "Launches a powerful rocket at your opponent.");
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, _rocket_amount()));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;

		msg_print("You launch a rocket.");
		fire_rocket(GF_ROCKET, dir, _rocket_amount(), 2);

		var_set_bool(res, TRUE);
		break;
	}
	case SPELL_COST_EXTRA:
		var_set_int(res, p_ptr->lev*19/50 + p_ptr->lev*p_ptr->lev*19/2500);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static power_info _cyber_powers[] = {
	{ A_CON, {  1,  2, 30, _cyber_rocket_spell} },
	{    -1, { -1, -1, -1, NULL}}
};
static int _cyber_get_powers(spell_info* spells, int max) {
	return get_powers_aux(spells, max, _cyber_powers);
}

static void _cyber_birth(void) 
{ 
	object_type	forge;

	p_ptr->current_r_idx = MON_CYBER;

	object_prep(&forge, lookup_kind(TV_RING, SV_RING_DAMAGE));
	forge.to_d = 10;
	add_outfit(&forge);
	
	object_prep(&forge, lookup_kind(TV_SOFT_ARMOR, SV_LEATHER_SCALE_MAIL));
	add_outfit(&forge);

	object_prep(&forge, lookup_kind(TV_HAFTED, SV_GREAT_HAMMER));
	add_outfit(&forge);
}

static void _cyber_calc_bonuses(void) 
{
	int l = p_ptr->lev;
	int to_a = l/2 + l*l/100 + l*l*l/5000;

	p_ptr->to_a += to_a;
	p_ptr->dis_to_a += to_a;
	p_ptr->pspeed -= 1 + p_ptr->lev/23;

	res_add(RES_FIRE);
	res_add(RES_POIS);
/*	res_add_vuln(RES_CONF); */
	
	p_ptr->hold_life = TRUE;
	p_ptr->no_eldritch = TRUE;
	p_ptr->free_act = TRUE;
}

static void _cyber_get_flags(u32b flgs[TR_FLAG_SIZE]) 
{
	add_flag(flgs, TR_RES_FIRE);
	add_flag(flgs, TR_RES_POIS);
	add_flag(flgs, TR_SPEED);

	add_flag(flgs, TR_HOLD_LIFE);
	add_flag(flgs, TR_FREE_ACT);
}

static void _cyber_get_vulnerabilities(u32b flgs[TR_FLAG_SIZE]) 
{
/*	add_flag(flgs, TR_RES_CONF); */
}

static void _cyber_move_player(void)
{
	/* Cyberdemons move erratically (cf get_rep_dir()) and make a lot of noise */
	if (one_in_(66))
	{
		int i;
	
		msg_print("The dungeon trembles!");
		if (disturb_minor)
			disturb(0, 0);

		for (i = 1; i < m_max; i++)
		{
			monster_type *m_ptr = &m_list[i];

			if (!m_ptr->r_idx) continue;
			if (m_ptr->cdis < MAX_SIGHT * 2 && MON_CSLEEP(m_ptr))
				(void)set_monster_csleep(i, 0);
		}
	}
}

static race_t *_cyber_get_race_t(void)
{
	static race_t me = {0};
	static bool   init = FALSE;

	if (!init)
	{           /* dis, dev, sav, stl, srh, fos, thn, thb */
	skills_t bs = { 20,  18,  31,  -1,  13,   7,  75,  30};
	skills_t xs = { 12,   6,   9,   0,   0,   0,  35,   7};

		me.subname = "Cyberdemon";

		me.skills = bs;
		me.extra_skills = xs;

		me.infra = 5;
		me.life = 135;

		me.exp = 300;
		me.base_hp = 50;

		me.birth = _cyber_birth;
		me.get_powers = _cyber_get_powers;
		me.calc_bonuses = _cyber_calc_bonuses;
		me.get_flags = _cyber_get_flags;
		me.get_vulnerabilities = _cyber_get_vulnerabilities;
		me.move_player = _cyber_move_player;
		init = TRUE;
	}

	me.stats[A_STR] =  5 + p_ptr->lev/10;
	me.stats[A_INT] = -10;
	me.stats[A_WIS] = -10;
	me.stats[A_DEX] = -3;
	me.stats[A_CON] =  5 + p_ptr->lev/10;
	me.stats[A_CHR] =  0;

	me.boss_r_idx = MON_OREMORJ;

	return &me;
}

/**********************************************************************
 * Public
 **********************************************************************/
race_t *mon_demon_get_race_t(int psubrace)
{
	race_t *result = NULL;

	switch (psubrace)
	{
	case DEMON_BALROG:
		result = _balrog_get_race_t();
		break;
	case DEMON_MARILITH:
		result = _marilith_get_race_t();
		break;
	case DEMON_CYBERDEMON:
		result = _cyber_get_race_t();
		break;
	case DEMON_KHORNE:
		result = _khorne_get_race_t();
		break;
	default: /* Birth Menus */
		result = _balrog_get_race_t();
	}

	result->name = "Demon";
	result->desc = _desc;
	result->flags = RACE_IS_MONSTER | RACE_IS_DEMON | RACE_IS_NONLIVING;

	return result;
}