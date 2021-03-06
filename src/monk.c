#include "angband.h"

/* TODO: Move special py_attack code here
         Move blows calculations here
         Move posture code here */

static int _max_tries(int lvl, u32b defense)
{
    int tries = 0;
    if (defense & KAMAE_BYAKKO)
        tries = (lvl < 3 ? 1 : lvl / 3);
    else if (defense & KAMAE_SUZAKU)
        tries = 1;
    else if (defense & KAMAE_GENBU)
        tries = 1;
    else if (mystic_get_toggle() == MYSTIC_TOGGLE_OFFENSE)
        tries = 1 + lvl/4;
    else if (mystic_get_toggle() == MYSTIC_TOGGLE_DEFENSE)
        tries = 1 + lvl/15;
    else
        tries = (lvl < 7 ? 1 : lvl / 7);

    return tries;
}

static int _get_attack_idx(int lvl, u32b defense)
{
    int tries = _max_tries(lvl, defense);
    int i;
    int attack_idx = 0;
    int best_attack_idx = 0;
    int min_level;

    if (p_ptr->stun || p_ptr->confused)
        return 0; /* Punch for 1d4 */

    for (i = 0; i < tries; i++)
    {
        martial_arts *ma_ptr;
        do
        {
            attack_idx = randint0(MAX_MA);
            ma_ptr = &ma_blows[attack_idx];

            if (p_ptr->pclass == CLASS_FORCETRAINER && ma_ptr->min_level > 1) 
                min_level = ma_ptr->min_level + 3;
            else min_level = ma_ptr->min_level;
        }
        while (min_level > lvl || randint1(lvl) < ma_ptr->chance);

        if (ma_ptr->min_level > ma_blows[best_attack_idx].min_level)
            best_attack_idx = attack_idx;
    }

    return best_attack_idx;
}

typedef struct _attack_s {
    int count;
    int mul;
    int to_d;
} _attack_t;

void _get_attack_counts(int tot, _attack_t *counts, int hand)
{
    int i;
    _attack_t *_attack_ptr;
    critical_t crit;

    for (i = 0; i < MAX_MA; i++)
    {
        _attack_ptr = &counts[i];
        _attack_ptr->count = 0;
        _attack_ptr->mul = 0;
        _attack_ptr->to_d = 0;
    }

    for (i = 0; i < tot; i++)
    {
        int attack_idx = _get_attack_idx(p_ptr->lev, p_ptr->special_defense);
        martial_arts *ma_ptr = &ma_blows[attack_idx];

        _attack_ptr = &counts[attack_idx];
        _attack_ptr->count++;
        
        /* Crits depend on the attack chosen. The following won't be stable
           for attacks that occur infrequently, but hopefully things will just
           average out */
        crit = monk_get_critical(ma_ptr, hand, 0);

        if (crit.desc)
        {
            _attack_ptr->mul += crit.mul;
            _attack_ptr->to_d += crit.to_d;
        }
        else
            _attack_ptr->mul += 100;
    }
}

static int _get_weight(void)
{
    int weight = 8;
    if (p_ptr->special_defense & KAMAE_SUZAKU) weight = 4;
    if (mystic_get_toggle() == MYSTIC_TOGGLE_DEFENSE) weight = 6;
    if (mystic_get_toggle() == MYSTIC_TOGGLE_OFFENSE) weight = 10;
    if ((p_ptr->pclass == CLASS_FORCETRAINER) && (p_ptr->magic_num1[0]))
    {
        weight += (p_ptr->magic_num1[0]/30);
        if (weight > 20) weight = 20;
    }
    return weight * p_ptr->lev;
}

critical_t monk_get_critical(martial_arts *ma_ptr, int hand, int mode)
{
    int min_level = ma_ptr->min_level;
    int weight = _get_weight();

    if (mode == MYSTIC_CRITICAL)
        weight += weight/2 + 300;
    
    if (p_ptr->pclass == CLASS_FORCETRAINER) min_level = MAX(1, min_level - 3);

    return critical_norm(weight, min_level, p_ptr->weapon_info[hand].to_h, 0, 0);
}

int monk_get_attack_idx(void)
{
    return _get_attack_idx(p_ptr->lev, p_ptr->special_defense);
}

void monk_display_attack_info(doc_ptr doc, int hand)
{
    _attack_t counts[MAX_MA];
    int i;
    const int tot = 1000;
    int tot_dam = 0;
    int tot_mul = 0;
    int tot_to_d = 0;
    int blows = NUM_BLOWS(hand);
    int to_d = p_ptr->weapon_info[hand].to_d * 10;
    critical_t crit;
    doc_ptr cols[2] = {0};

    cols[0] = doc_alloc(45);
    cols[1] = doc_alloc(35);

    /* First Column */
    doc_printf(cols[0], "<color:G>%-14.14s %6s %5s %6s</color>\n", "Attack", "Dice", "Pct", "Dam");

    _get_attack_counts(tot, counts, hand);
    for (i = 0; i < MAX_MA; i++)
    {
        martial_arts *ma_ptr = &ma_blows[i];
        int dd = ma_ptr->dd + p_ptr->weapon_info[hand].to_dd;
        int ds = ma_ptr->ds + p_ptr->weapon_info[hand].to_ds;
        char tmp[20];
        int dam = dd * (ds + 1) * 10 * counts[i].count / (2 * tot);

        if (counts[i].count == 0) continue;

        tot_dam += dam;
        tot_mul += counts[i].mul;
        tot_to_d += counts[i].to_d;

        sprintf(tmp, "%dd%d", dd, ds);
        doc_printf(cols[0], "%-14.14s %6s %3d.%1d%% %3d.%1d\n",
                                ma_ptr->name, tmp,
                                counts[i].count/10, counts[i].count%10,
                                dam/10, dam%10);
    }

    doc_printf(cols[0], "<tab:8>%20s %3d.%1d\n", "Total:", tot_dam/10, tot_dam%10);

    crit.mul = tot_mul/tot;
    crit.to_d = tot_to_d*10/tot;
    doc_printf(cols[0], "<tab:8>%20s %3d.%02dx\n", "Criticals:", crit.mul/100, crit.mul%100);

    /* Account for criticals in all that follows ... */
    tot_dam = tot_dam * crit.mul/100;
    to_d = to_d + crit.to_d;
    doc_printf(cols[0], "<tab:8>%20s %3d.%1d +%3d\n", "One Strike:", tot_dam/10, tot_dam%10, to_d/10);

    /* Second Column */
    doc_insert(cols[1], "<color:y>Your Fists</color>\n");

    doc_printf(cols[1], "Number of Blows: %d.%2.2d\n", blows/100, blows%100);
    doc_printf(cols[1], "To Hit:  0  50 100 150 200 (AC)\n");
    doc_printf(cols[1], "        %2d  %2d  %2d  %2d  %2d (%%)\n",
        hit_chance(0, 0, 0), 
        hit_chance(0, 0, 50), 
        hit_chance(0, 0, 100), 
        hit_chance(0, 0, 150), 
        hit_chance(0, 0, 200)
    );

    doc_newline(cols[1]);
    doc_insert(cols[1], "<color:y>Average Damage:</color>\n");
    doc_printf(cols[1], " One Strike: %d.%1d\n", (tot_dam + to_d)/10, (tot_dam + to_d)%10);
    doc_printf(cols[1], " One Attack: %d.%1d\n", blows*(tot_dam + to_d)/1000, ((blows*(tot_dam + to_d))/100)%10);

    if (have_flag(p_ptr->weapon_info[hand].flags, TR_BRAND_ACID))
    {
        doc_printf(cols[1], " <color:r>      Acid</color>: %d.%1d\n",
            blows*(tot_dam*17/10 + to_d)/1000,
            ((blows*(tot_dam*17/10 + to_d))/100)%10);
    }
    if (have_flag(p_ptr->weapon_info[hand].flags, TR_BRAND_ELEC))
    {
        doc_printf(cols[1], " <color:r>      Elec</color>: %d.%1d\n",
            blows*(tot_dam*17/10 + to_d)/1000,
            ((blows*(tot_dam*17/10 + to_d))/100)%10);
    }
    if (have_flag(p_ptr->weapon_info[hand].flags, TR_BRAND_FIRE))
    {
        doc_printf(cols[1], " <color:r>      Fire</color>: %d.%1d\n",
            blows*(tot_dam*17/10 + to_d)/1000,
            ((blows*(tot_dam*17/10 + to_d))/100)%10);
    }
    if (have_flag(p_ptr->weapon_info[hand].flags, TR_BRAND_COLD))
    {
        doc_printf(cols[1], " <color:r>      Cold</color>: %d.%1d\n",
            blows*(tot_dam*17/10 + to_d)/1000,
            ((blows*(tot_dam*17/10 + to_d))/100)%10);
    }
    if (have_flag(p_ptr->weapon_info[hand].flags, TR_BRAND_POIS))
    {
        doc_printf(cols[1], " <color:r>      Pois</color>: %d.%1d\n",
            blows*(tot_dam*17/10 + to_d)/1000,
            ((blows*(tot_dam*17/10 + to_d))/100)%10);
    }

    doc_insert_cols(doc, cols, 2, 0);
    doc_free(cols[0]);
    doc_free(cols[1]);
}

static bool _monk_check_spell(void)
{
    if (p_ptr->pclass == CLASS_WILD_TALENT)
        return TRUE;
    if ((p_ptr->prace == RACE_MON_POSSESSOR || p_ptr->prace == RACE_MON_MIMIC) && !p_ptr->weapon_ct)
        return TRUE;
    if (!p_ptr->weapon_info[0].bare_hands)
    {
        msg_print("You need to fight bare handed.");
        return FALSE;
    }
    if (p_ptr->riding)
    {
        msg_print("You need to get off your pet.");
        return FALSE;
    }
    return TRUE;
}

static bool choose_kamae(void)
{
    char choice;
    int new_kamae = 0;
    int i;
    rect_t display = ui_menu_rect();
    char buf[80];

    if (display.cx > 40)
        display.cx = 40;

    if (p_ptr->confused)
    {
        msg_print("You are too confused.");
        return FALSE;
    }

    screen_save();

    Term_erase(display.x, display.y, display.cx);
    put_str("Choose Form: ", display.y, display.x + 1);

    Term_erase(display.x, display.y + 1, display.cx);
    put_str(" a) No form", display.y + 1, display.x + 1);

    for (i = 0; i < MAX_KAMAE; i++)
    {
        if (p_ptr->lev >= kamae_shurui[i].min_level)
        {
            sprintf(buf," %c) %-12s  %s",I2A(i+1), kamae_shurui[i].desc, kamae_shurui[i].info);
            Term_erase(display.x, display.y + 2 + i, display.cx);
            put_str(buf, display.y + 2 + i, display.x + 1);
        }
    }

    while(1)
    {
        choice = inkey();
        if (choice == ESCAPE)
        {
            screen_load();
            return FALSE;
        }
        else if ((choice == 'a') || (choice == 'A'))
        {
            if (p_ptr->action == ACTION_KAMAE)
                set_action(ACTION_NONE);
            else
                msg_print("You are not assuming a posture.");
            screen_load();
            return TRUE;
        }
        else if ((choice == 'b') || (choice == 'B'))
        {
            new_kamae = 0;
            break;
        }
        else if (((choice == 'c') || (choice == 'C')) && (p_ptr->lev > 29))
        {
            new_kamae = 1;
            break;
        }
        else if (((choice == 'd') || (choice == 'D')) && (p_ptr->lev > 34))
        {
            new_kamae = 2;
            break;
        }
        else if (((choice == 'e') || (choice == 'E')) && (p_ptr->lev > 39))
        {
            new_kamae = 3;
            break;
        }
    }
    set_action(ACTION_KAMAE);

    if (p_ptr->special_defense & (KAMAE_GENBU << new_kamae))
        msg_print("You reassume a posture.");
    else
    {
        p_ptr->special_defense &= ~(KAMAE_MASK);
        p_ptr->update |= (PU_BONUS);
        p_ptr->redraw |= (PR_STATE);
        msg_format("You assume a posture of %s form.",kamae_shurui[new_kamae].desc);
        p_ptr->special_defense |= (KAMAE_GENBU << new_kamae);
    }
    p_ptr->redraw |= PR_STATE;
    screen_load();
    return TRUE;
}

void monk_double_attack_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Double Attack");
        break;
    case SPELL_DESC:
        var_set_string(res, "Attack twice at an adjacent enemy. This action consumes double the normal energy.");
        break;
    case SPELL_CAST:
        var_set_bool(res, FALSE);
        if (_monk_check_spell())
        {
            int x, y, dir = 0;

            if (!get_rep_dir(&dir, FALSE)) return;

            y = py + ddy[dir];
            x = px + ddx[dir];
            if (cave[y][x].m_idx)
            {
                if (one_in_(2)) msg_print("Ahhhtatatatatatatatatatatatatatataatatatatattaaaaa!!!!");
                else msg_print("Oraoraoraoraoraoraoraoraoraoraoraoraoraoraoraoraora!!!!");

                py_attack(y, x, 0);
                if (cave[y][x].m_idx)
                {
                    handle_stuff();
                    py_attack(y, x, 0);
                }
            }
            else
            {
                msg_print("You don't see any monster in this direction");
                msg_print(NULL);
            }
            var_set_bool(res, TRUE);
        }
        break;
    case SPELL_ENERGY:
        var_set_int(res, 100 + ENERGY_NEED());
        break;
    default:
        default_spell(cmd, res);
        break;
    }
}

void monk_posture_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Assume a Posture");
        break;
    case SPELL_DESC:
        var_set_string(res, "");
        break;
    case SPELL_CAST:
        var_set_bool(res, FALSE);
        if ( _monk_check_spell()
          && choose_kamae() )
        {
            p_ptr->update |= (PU_BONUS);
            var_set_bool(res, TRUE);
        }
        break;
    default:
        default_spell(cmd, res);
        break;
    }
}

static int _get_powers(spell_info* spells, int max)
{
    int ct = 0;
    
    spell_info* spell = &spells[ct++];
    spell->level = 25;
    spell->cost = 0;
    spell->fail = 0;
    spell->fn = monk_posture_spell;

    spell = &spells[ct++];
    spell->level = 30;
    spell->cost = 30;
    spell->fail = calculate_fail_rate(spell->level, 80, p_ptr->stat_ind[A_STR]);
    spell->fn = monk_double_attack_spell;

    return ct;
}

static void _ac_bonus_imp(int slot)
{
    if (!equip_obj(slot))
    {
        switch (equip_slot_type(slot))
        {
        case EQUIP_SLOT_BODY_ARMOR:
            p_ptr->to_a += p_ptr->lev*3/2;
            p_ptr->dis_to_a += p_ptr->lev*3/2;
            break;
        case EQUIP_SLOT_CLOAK:
            if (p_ptr->lev > 15)
            {
                p_ptr->to_a += (p_ptr->lev - 13)/3;
                p_ptr->dis_to_a += (p_ptr->lev - 13)/3;
            }
            break;
        case EQUIP_SLOT_WEAPON_SHIELD: /* Oops: was INVEN_LARM only and "/3" ... */
            if (p_ptr->lev > 10)
            {
                p_ptr->to_a += (p_ptr->lev - 8)/6;
                p_ptr->dis_to_a += (p_ptr->lev - 8)/6;
            }
            break;
        case EQUIP_SLOT_HELMET:
            if (p_ptr->lev >= 5)
            {
                p_ptr->to_a += (p_ptr->lev - 2)/3;
                p_ptr->dis_to_a += (p_ptr->lev - 2)/3;
            }
            break;
        case EQUIP_SLOT_GLOVES:
            p_ptr->to_a += p_ptr->lev/2;
            p_ptr->dis_to_a += p_ptr->lev/2;
            break;
        case EQUIP_SLOT_BOOTS:
            p_ptr->to_a += p_ptr->lev/3;
            p_ptr->dis_to_a += p_ptr->lev/3;
            break;
        }
    }
}

void monk_ac_bonus(void)
{
    if (!(heavy_armor()))
        equip_for_each_slot(_ac_bonus_imp);
}

void monk_posture_calc_bonuses(void)
{
    int i;
    if (!heavy_armor() || p_ptr->pclass == CLASS_WILD_TALENT)
    {
        if (p_ptr->special_defense & KAMAE_BYAKKO)
        {
            p_ptr->to_a -= 40;
            p_ptr->dis_to_a -= 40;
            if (p_ptr->pclass == CLASS_WILD_TALENT)
            {
                /* Hack: This should "strengthen your attacks" */
                for (i = 0; i < MAX_HANDS; i++)
                    p_ptr->weapon_info[i].xtra_blow += 100;
            }
        }
        else if (p_ptr->special_defense & KAMAE_SEIRYU)
        {
            p_ptr->to_a -= 50;
            p_ptr->dis_to_a -= 50;
            res_add(RES_ACID);
            res_add(RES_FIRE);
            res_add(RES_ELEC);
            res_add(RES_COLD);
            res_add(RES_POIS);
            p_ptr->sh_fire = TRUE;
            p_ptr->sh_elec = TRUE;
            p_ptr->sh_cold = TRUE;
            p_ptr->levitation = TRUE;
        }
        else if (p_ptr->special_defense & KAMAE_GENBU)
        {
            p_ptr->to_a += (p_ptr->lev*p_ptr->lev)/50;
            p_ptr->dis_to_a += (p_ptr->lev*p_ptr->lev)/50;
            p_ptr->reflect = TRUE;
            for (i = 0; i < MAX_HANDS; i++)
            {
                p_ptr->weapon_info[i].xtra_blow -= 200;
                if (p_ptr->lev > 42) 
                    p_ptr->weapon_info[i].xtra_blow -= 100;
            }
        }
        else if (p_ptr->special_defense & KAMAE_SUZAKU)
        {
            p_ptr->pspeed += 10;
            for (i = 0; i < MAX_HANDS; i++)
            {
                p_ptr->weapon_info[i].to_h -= (p_ptr->lev / 3);
                p_ptr->weapon_info[i].to_d -= (p_ptr->lev / 6);

                p_ptr->weapon_info[i].dis_to_h -= (p_ptr->lev / 3);
                p_ptr->weapon_info[i].dis_to_d -= (p_ptr->lev / 6);
                p_ptr->weapon_info[i].base_blow /= 2;
            }
            p_ptr->levitation = TRUE;
        }
    }
}

void monk_posture_calc_stats(s16b stats[MAX_STATS])
{
    if (!heavy_armor() || p_ptr->pclass == CLASS_WILD_TALENT)
    {
        if (p_ptr->special_defense & KAMAE_BYAKKO)
        {
            stats[A_STR] += 2;
            stats[A_DEX] += 2;
            stats[A_CON] -= 3;
        }
        else if (p_ptr->special_defense & KAMAE_SEIRYU)
        {
        }
        else if (p_ptr->special_defense & KAMAE_GENBU)
        {
            stats[A_INT] -= 1;
            stats[A_WIS] -= 1;
            stats[A_DEX] -= 2;
            stats[A_CON] += 3;
        }
        else if (p_ptr->special_defense & KAMAE_SUZAKU)
        {
            stats[A_STR] -= 2;
            stats[A_INT] += 1;
            stats[A_WIS] += 1;
            stats[A_DEX] += 2;
            stats[A_CON] -= 2;
        }
    }
}

void monk_posture_get_flags(u32b flgs[TR_FLAG_SIZE])
{
    if (p_ptr->special_defense & KAMAE_GENBU)
        add_flag(flgs, TR_REFLECT);
    if (p_ptr->special_defense & KAMAE_SUZAKU)
    {
        add_flag(flgs, TR_LEVITATION);
        add_flag(flgs, TR_SPEED);
    }
    if (p_ptr->special_defense & KAMAE_SEIRYU)
    {
        add_flag(flgs, TR_RES_FIRE);
        add_flag(flgs, TR_RES_COLD);
        add_flag(flgs, TR_RES_ACID);
        add_flag(flgs, TR_RES_ELEC);
        add_flag(flgs, TR_RES_POIS);
        add_flag(flgs, TR_LEVITATION);
        add_flag(flgs, TR_SH_FIRE);
        add_flag(flgs, TR_SH_ELEC);
        add_flag(flgs, TR_SH_COLD);
    }
}

static void _calc_bonuses(void)
{
    monk_posture_calc_bonuses();
    if (!heavy_armor())
    {
        p_ptr->pspeed += p_ptr->lev/10;
        p_ptr->sh_retaliation = TRUE;
        if  (p_ptr->lev >= 25)
            p_ptr->free_act = TRUE;
    }
    monk_ac_bonus();
}

static void _calc_stats(s16b stats[MAX_STATS])
{
    monk_posture_calc_stats(stats);
}

static void _get_flags(u32b flgs[TR_FLAG_SIZE])
{
    monk_posture_get_flags(flgs);
    if (!heavy_armor())
    {
        add_flag(flgs, TR_SH_REVENGE);
        if (p_ptr->lev >= 10)
            add_flag(flgs, TR_SPEED);
        if (p_ptr->lev >= 25)
            add_flag(flgs, TR_FREE_ACT);
    }
}

static caster_info *_caster_info(void)
{
    static caster_info me = {0};
    static bool init = FALSE;
    if (!init)
    {
        me.magic_desc = "prayer";
        me.which_stat = A_WIS;
        me.weight = 350;
        me.min_fail = 5;
        init = TRUE;
    }
    return &me;
}

class_t *monk_get_class(void)
{
    static class_t me = {0};
    static bool init = FALSE;

    if (!init)
    {           /* dis, dev, sav, stl, srh, fos, thn, thb */
    skills_t bs = { 45,  34,  36,   5,  32,  24,  64,  60};
    skills_t xs = { 15,  11,  10,   0,   0,   0,  18,  18};

        me.name = "Monk";
        me.desc = "The Monk character class is very different from all other classes. "
                    "Their training in martial arts makes them much more powerful with "
                    "no armor or weapons. To gain the resistances necessary for "
                    "survival a monk may need to wear some kind of armor, but if the "
                    "armor he wears is too heavy, it will severely disturb his martial "
                    "arts maneuvers. As the monk advances levels, new, powerful forms "
                    "of attack become available. Their defensive capabilities increase "
                    "likewise, but if armour is being worn, this effect decreases. "
                    "Wisdom determines a Monk's spell casting ability.\n \n"
                    "The different sects of monks are devoted to different areas of "
                    "magic. They select a realm from Life, Nature, Craft, Trump and Death. "
                    "They will eventually learn all prayers in the discipline of their "
                    "choice. They have two class powers - 'Assume a Posture' and "
                    "'Double Attack'. They can choose different forms of postures in "
                    "different situations, and use powerful combinations of attacks for "
                    "the finishing blow.";

        me.stats[A_STR] =  2;
        me.stats[A_INT] = -1;
        me.stats[A_WIS] =  1;
        me.stats[A_DEX] =  3;
        me.stats[A_CON] =  2;
        me.stats[A_CHR] =  1;
        me.base_skills = bs;
        me.extra_skills = xs;
        me.life = 110;
        me.base_hp = 12;
        me.exp = 130;
        me.pets = 35;
        
        me.calc_bonuses = _calc_bonuses;
        me.calc_stats = _calc_stats;
        me.get_flags = _get_flags;
        me.caster_info = _caster_info;
        /* TODO: This class uses spell books, so we are SOL
        me.get_spells = _get_spells;*/
        me.get_powers = _get_powers;
        me.character_dump = spellbook_character_dump;
        init = TRUE;
    }

    return &me;
}
