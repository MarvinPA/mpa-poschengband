#include "angband.h"

/****************************************************************
 * Helpers
 ****************************************************************/
static int _get_toggle(void)
{
	return p_ptr->magic_num1[0];
}

static int _set_toggle(s32b toggle)
{
	int result = p_ptr->magic_num1[0];

	if (toggle == result) return result;

	p_ptr->magic_num1[0] = toggle;

	p_ptr->redraw |= PR_STATUS;
	p_ptr->update |= PU_BONUS;
	handle_stuff();

	return result;
}

int mystic_get_toggle(void)
{
	int result = TOGGLE_NONE;
	if (p_ptr->pclass == CLASS_MYSTIC && !heavy_armor())
		result = _get_toggle();
	return result;
}

static bool _do_blow(int type)
{
	int x, y;
	int dir;
	int m_idx = 0;

	/* For ergonomics sake, use currently targeted monster.  This allows
	   a macro of \e*tmaa or similar to pick an adjacent foe, while
	   \emaa*t won't work, since get_rep_dir2() won't allow a target. */
	if (use_old_target && target_okay())
	{
		y = target_row;
		x = target_col;
		m_idx = cave[y][x].m_idx;
		if (m_idx)
		{
			if (m_list[m_idx].cdis > 1)
				m_idx = 0;
			else
				dir = 5;
		}
	}

	if (!m_idx)
	{
		if (!get_rep_dir2(&dir)) return FALSE;
		if (dir == 5) return FALSE;

		y = py + ddy[dir];
		x = px + ddx[dir];
		m_idx = cave[y][x].m_idx;

		if (!m_idx)
		{
			msg_print("There is no monster there.");
			return FALSE;
		}

	}

	if (m_idx)
		py_attack(y, x, type);

	return TRUE;
}

/****************************************************************
 * Spells
 ****************************************************************/
static void _toggle_spell(int which, int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_CAST:
		var_set_bool(res, FALSE);
		if (_get_toggle() == which)
			_set_toggle(TOGGLE_NONE);
		else
			_set_toggle(which);
		var_set_bool(res, TRUE);
		break;
	case SPELL_ENERGY:
		if (_get_toggle() != which)
			var_set_int(res, 0);	/* no charge for dismissing a technique */
		else
			var_set_int(res, 100);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static void _acid_strike_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Corrosive Blow");
		break;
	case SPELL_DESC:
		var_set_string(res, "Attack an adjacent opponent with an acid blow.");
		break;
	case SPELL_CAST:
		var_set_bool(res, _do_blow(MYSTIC_ACID));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static void _cold_strike_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Icy Fists");
		break;
	case SPELL_DESC:
		var_set_string(res, "Attack an adjacent opponent with a freezing blow.");
		break;
	case SPELL_CAST:
		var_set_bool(res, _do_blow(MYSTIC_COLD));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static void _confusing_strike_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Confusing Strike");
		break;
	case SPELL_DESC:
		var_set_string(res, "Attack an adjacent opponent with confusing blows.");
		break;
	case SPELL_CAST:
		var_set_bool(res, _do_blow(MYSTIC_CONFUSE));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static void _crushing_blow_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Crushing Blow");
		break;
	case SPELL_DESC:
		var_set_string(res, "Attack an adjacent opponent with crushing blows for extra damage.");
		break;
	case SPELL_CAST:
		var_set_bool(res, _do_blow(MYSTIC_CRITICAL));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static void _defense_toggle_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Defensive Stance");
		break;
	case SPELL_DESC:
		var_set_string(res, "When using this technique, you gain increased armor class at the expense of your fighting prowess.");
		break;
	default:
		_toggle_spell(MYSTIC_TOGGLE_DEFENSE, cmd, res);
		break;
	}
}

static void _elec_strike_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Lightning Eagle");
		break;
	case SPELL_DESC:
		var_set_string(res, "Attack an adjacent opponent with a shocking blow.");
		break;
	case SPELL_CAST:
		var_set_bool(res, _do_blow(MYSTIC_ELEC));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static void _fast_toggle_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Quick Approach");
		break;
	case SPELL_DESC:
		var_set_string(res, "When using this technique, you will move with great haste.");
		break;
	default:
		_toggle_spell(MYSTIC_TOGGLE_FAST, cmd, res);
		break;
	}
}

static void _fire_strike_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Flaming Strike");
		break;
	case SPELL_DESC:
		var_set_string(res, "Attack an adjacent opponent with a flaming blow.");
		break;
	case SPELL_CAST:
		var_set_bool(res, _do_blow(MYSTIC_FIRE));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static void _killing_strike_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Touch of Death");
		break;
	case SPELL_DESC:
		var_set_string(res, "Attempt to kill an adjacent opponent with a single blow.");
		break;
	case SPELL_CAST:
		var_set_bool(res, _do_blow(MYSTIC_KILL));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static void _knockout_blow_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Knockout Blow");
		break;
	case SPELL_DESC:
		var_set_string(res, "Attempt to knockout an adjacent opponent.");
		break;
	case SPELL_CAST:
		var_set_bool(res, _do_blow(MYSTIC_KNOCKOUT));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static void _mystic_insights_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Mystic Insights");
		break;
	default:
		probing_spell(cmd, res);
		break;
	}
}

static void _offense_toggle_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Death Stance");
		break;
	case SPELL_DESC:
		var_set_string(res, "When using this technique, you concentrate all your mental efforts on offensive deadliness. As such, you become more exposed to enemy attacks.");
		break;
	default:
		_toggle_spell(MYSTIC_TOGGLE_OFFENSE, cmd, res);
		break;
	}
}

static void _poison_strike_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Serpent's Tongue");
		break;
	case SPELL_DESC:
		var_set_string(res, "Attack an adjacent opponent with a poisonous blow.");
		break;
	case SPELL_CAST:
		var_set_bool(res, _do_blow(MYSTIC_POIS));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static void _retaliate_toggle_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Aura of Retaliation");
		break;
	case SPELL_DESC:
		var_set_string(res, "When using this technique, you will retaliate when struck.");
		break;
	default:
		_toggle_spell(MYSTIC_TOGGLE_RETALIATE, cmd, res);
		break;
	}
}

static void _stealth_toggle_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Stealthy Approach");
		break;
	case SPELL_DESC:
		var_set_string(res, "When using this technique, you will gain enhanced stealth.");
		break;
	default:
		_toggle_spell(MYSTIC_TOGGLE_STEALTH, cmd, res);
		break;
	}
}

static void _stunning_blow_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Stunning Blow");
		break;
	case SPELL_DESC:
		var_set_string(res, "Attack an adjacent opponent with stunning blows.");
		break;
	case SPELL_CAST:
		var_set_bool(res, _do_blow(MYSTIC_STUN));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void _summon_hounds_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Summon Hounds");
		break;
	case SPELL_DESC:
		var_set_string(res, "Summon hounds for assistance.");
		break;
	case SPELL_CAST:
	{
		int num = 1; /* randint0(p_ptr->lev/10); */
		int ct = 0, i;
		int l = p_ptr->lev + randint1(p_ptr->lev);

		for (i = 0; i < num; i++)
		{
			ct += summon_specific(-1, py, px, l, SUMMON_HOUND, PM_FORCE_PET | PM_ALLOW_GROUP);
		}
		if (!ct)
			msg_print("No hounds arrive.");
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void _summon_spiders_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Summon Spiders");
		break;
	case SPELL_DESC:
		var_set_string(res, "Summon spiders for assistance.");
		break;
	case SPELL_CAST:
	{
		int num = 1; /* randint0(p_ptr->lev/10); */
		int ct = 0, i;
		int l = p_ptr->lev + randint1(p_ptr->lev);

		for (i = 0; i < num; i++)
		{
			ct += summon_specific(-1, py, px, l, SUMMON_SPIDER, PM_FORCE_PET | PM_ALLOW_GROUP);
		}
		if (!ct)
			msg_print("No spiders arrive.");
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}


/****************************************************************
 * Spell Table and Exports
 ****************************************************************/
static spell_info _spells[] = 
{
    /*lvl cst fail spell */
    {  1,  0,  0, samurai_concentration_spell},
	{  3,  8,  0, _fire_strike_spell}, 
	{  5,  8, 30, _summon_spiders_spell},
	{  7,  8,  0, _cold_strike_spell}, 
	{  9, 10, 30, detect_menace_spell}, 
	{ 11, 10,  0, _poison_strike_spell}, 
	{ 13, 15, 40, sense_surroundings_spell},
	{ 15,  0,  0, _stealth_toggle_spell},
	{ 17,  0,  0, _fast_toggle_spell},
	{ 19,  0,  0, _defense_toggle_spell},
	{ 21, 15, 50, _mystic_insights_spell}, 
	{ 23, 15,  0, _confusing_strike_spell}, 
	{ 25, 17,  0, _acid_strike_spell}, 
	{ 27, 20,  0, _stunning_blow_spell},
	{ 29,  0,  0, _retaliate_toggle_spell},
	{ 30, 30, 60, haste_self_spell},
	{ 32, 30, 60, resistance_spell},
	{ 33, 30,  0, _elec_strike_spell}, 
	{ 35, 20, 60, rush_attack_spell},
	{ 36, 40, 60, _summon_hounds_spell},
	{ 37,  0,  0, _offense_toggle_spell},
	{ 39, 40,  0, _knockout_blow_spell},
	{ 42, 50,  0, _killing_strike_spell},
	{ 45, 50,  0, _crushing_blow_spell},
	{ -1, -1, -1, NULL}
};

static int _get_spells(spell_info* spells, int max)
{
	return get_spells_aux(spells, max, _spells);
}
static void _calc_bonuses(void)
{
	if (!heavy_armor())
	{
		p_ptr->pspeed += p_ptr->lev/10;
		if  (p_ptr->lev >= 25)
			p_ptr->free_act = TRUE;

		switch (_get_toggle())
		{
		case MYSTIC_TOGGLE_STEALTH:
			p_ptr->skills.stl += 2 + 3 * p_ptr->lev/50;
			break;
		case MYSTIC_TOGGLE_FAST:
			p_ptr->quick_walk = TRUE;
			break;
		case MYSTIC_TOGGLE_DEFENSE:
		{
			int bonus = 10 + 40*p_ptr->lev/50;
			p_ptr->to_a += bonus;
			p_ptr->dis_to_a += bonus;
			break;
		}
		case MYSTIC_TOGGLE_OFFENSE:
		{
			int penalty = 10 + 40*p_ptr->lev/50;
			p_ptr->to_a -= penalty;
			p_ptr->dis_to_a -= penalty;
			break;
		}
		}
	}
	monk_ac_bonus();
}
static void _get_flags(u32b flgs[TR_FLAG_SIZE])
{
	if (!heavy_armor())
	{
		if (p_ptr->lev >= 10)
			add_flag(flgs, TR_SPEED);
		if (p_ptr->lev >= 25)
			add_flag(flgs, TR_FREE_ACT);
	}
}
static caster_info * _caster_info(void)
{
	static caster_info me = {0};
	static bool init = FALSE;
	if (!init)
	{
		me.magic_desc = "mystic technique";
		me.which_stat = A_CHR;
		me.weight = 350;
		init = TRUE;
	}
	return &me;
}
static void _character_dump(FILE* file)
{
	spell_info spells[MAX_SPELLS];
	int        ct = _get_spells(spells, MAX_SPELLS);

	dump_spells_aux(file, spells, ct);
}
class_t *mystic_get_class_t(void)
{
	static class_t me = {0};
	static bool init = FALSE;

	if (!init)
	{           /* dis, dev, sav, stl, srh, fos, thn, thb */
	skills_t bs = { 45,  34,  36,   5,  32,  24,  64,  60};
	skills_t xs = { 15,  11,  10,   0,   0,   0,  18,  18};

		me.name = "Mystic";
		me.desc = "Mystics are masters of bare handed fighting, like Monks. However, they "
		          "do not learn normal spells. Instead, they gain mystical powers with experience, "
				  "and these powers directly influence their martial arts. In this respect, "
				  "Mystics are similar to the Samurai. Indeed, they even concentrate to boost "
				  "their mana like the Samurai. Mystics eschew weapons of any kind and require "
				  "the lightest of armors in order to practice their martial arts. The number "
				  "of attacks are influenced by dexterity and experience level while the mystic's "
				  "mana and fail rates are influenced by charisma. Mystics are in tune with the "
				  "natural forces around them and may even call on aid when necessary. It has "
				  "been whispered that mystics have even discovered how to kill an opponent with "
				  "a single touch, though they do not share this knowledge with novices.";

		me.stats[A_STR] =  2;
		me.stats[A_INT] = -1;
		me.stats[A_WIS] = -2;
		me.stats[A_DEX] =  3;
		me.stats[A_CON] =  1;
		me.stats[A_CHR] =  2;
		me.base_skills = bs;
		me.extra_skills = xs;
		me.life = 100;
		me.base_hp = 4;
		me.exp = 130;
		me.pets = 35;
		
		me.calc_bonuses = _calc_bonuses;
		me.get_flags = _get_flags;
		me.caster_info = _caster_info;
		me.get_spells = _get_spells;
		me.character_dump = _character_dump;
		init = TRUE;
	}

	return &me;
}