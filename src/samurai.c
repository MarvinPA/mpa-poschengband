#include "angband.h"

/***********************************************************************
 * Samurai Spells (Hissatsu)
 ***********************************************************************/
cptr do_hissatsu_spell(int spell, int mode)
{
    bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
    bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
    bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

    int dir;
    int plev = p_ptr->lev;

    switch (spell)
    {
    case 0:
        if (name) return "Tobi-Izuna";
        if (desc) return "Attacks a two squares distant monster.";
        if (cast)
        {
            project_length = 2;
            if (!get_aim_dir(&dir)) return NULL;

            project_hook(GF_ATTACK, dir, HISSATSU_2, PROJECT_STOP | PROJECT_KILL);
        }
        break;

    case 1:
        if (name) return "3-Way Attack";
        if (desc) return "Attacks in 3 directions in one time.";
        if (cast)
        {
            int cdir;
            int y, x;

            if (!get_rep_dir2(&dir)) return NULL;
            if (dir == 5) return NULL;

            for (cdir = 0;cdir < 8; cdir++)
            {
                if (cdd[cdir] == dir) break;
            }

            if (cdir == 8) return NULL;

            y = py + ddy_cdd[cdir];
            x = px + ddx_cdd[cdir];
            if (cave[y][x].m_idx)
                py_attack(y, x, 0);
            else
                msg_print("You attack the empty air.");
            y = py + ddy_cdd[(cdir + 7) % 8];
            x = px + ddx_cdd[(cdir + 7) % 8];
            if (cave[y][x].m_idx)
                py_attack(y, x, 0);
            else
                msg_print("You attack the empty air.");
            y = py + ddy_cdd[(cdir + 1) % 8];
            x = px + ddx_cdd[(cdir + 1) % 8];
            if (cave[y][x].m_idx)
                py_attack(y, x, 0);
            else
                msg_print("You attack the empty air.");
        }
        break;

    case 2:
        if (name) return "Boomerang";
        if (desc) return "Throws current weapon. And it'll return to your hand unless failed.";
        if (cast)
        {
            if (!do_cmd_throw_aux(1, TRUE, 0)) return NULL;
        }
        break;

    case 3:
        if (name) return "Burning Strike";
        if (desc) return "Attacks a monster with more damage unless it has resistance to fire.";
        if (cast)
        {
            int y, x;

            if (!get_rep_dir2(&dir)) return NULL;
            if (dir == 5) return NULL;

            y = py + ddy[dir];
            x = px + ddx[dir];

            if (cave[y][x].m_idx)
                py_attack(y, x, HISSATSU_FIRE);
            else
            {
                msg_print("There is no monster.");
                return NULL;
            }
        }
        break;

    case 4:
        if (name) return "Detect Ferocity";
        if (desc) return "Detects all monsters except mindless in your vicinity.";
        if (cast)
            detect_monsters_mind(DETECT_RAD_DEFAULT);
        break;

    case 5:
        if (name) return "Strike to Stun";
        if (desc) return "Attempts to stun a monster in the adjacent.";
        if (cast)
        {
            int y, x;

            if (!get_rep_dir2(&dir)) return NULL;
            if (dir == 5) return NULL;

            y = py + ddy[dir];
            x = px + ddx[dir];

            if (cave[y][x].m_idx)
                py_attack(y, x, HISSATSU_MINEUCHI);
            else
            {
                msg_print("There is no monster.");
                return NULL;
            }
        }
        break;

    case 6:
        if (name) return "Counter";
        if (desc) return "Prepares to counterattack. When attack by a monster, strikes back using SP each time.";
        if (cast)
        {
            if (p_ptr->riding)
            {
                msg_print("You cannot do it when riding.");
                return NULL;
            }
            msg_print("You prepare to counter blow.");
            p_ptr->counter = TRUE;
        }
        break;

    case 7:
        if (name) return "Harainuke";
        if (desc) return "Attacks monster with your weapons normally, then move through counter side of the monster.";
        if (cast)
        {
            int y, x;
            if (p_ptr->riding)
            {
                msg_print("You cannot do it when riding.");
                return NULL;
            }
    
            if (!get_rep_dir2(&dir)) return NULL;
    
            if (dir == 5) return NULL;
            y = py + ddy[dir];
            x = px + ddx[dir];
    
            if (!cave[y][x].m_idx)
            {
                msg_print("There is no monster.");
                return NULL;
            }
    
            py_attack(y, x, 0);
    
            if (!player_can_enter(cave[y][x].feat, 0) || is_trap(cave[y][x].feat))
                break;
    
            y += ddy[dir];
            x += ddx[dir];
    
            if (player_can_enter(cave[y][x].feat, 0) && !is_trap(cave[y][x].feat) && !cave[y][x].m_idx)
            {
                msg_print(NULL);
                (void)move_player_effect(y, x, MPE_FORGET_FLOW | MPE_HANDLE_STUFF | MPE_DONT_PICKUP);
            }
        }
        break;

    case 8:
        if (name) return "Serpent's Tongue";
        if (desc) return "Attacks a monster with more damage unless it has resistance to poison.";
        if (cast)
        {
            int y, x;

            if (!get_rep_dir2(&dir)) return NULL;
            if (dir == 5) return NULL;

            y = py + ddy[dir];
            x = px + ddx[dir];

            if (cave[y][x].m_idx)
                py_attack(y, x, HISSATSU_POISON);
            else
            {
                msg_print("There is no monster.");
                return NULL;
            }
        }
        break;

    case 9:
        if (name) return "Zammaken";
        if (desc) return "Attacks an evil unliving monster with great damage. No effect to other  monsters.";
        if (cast)
        {
            int y, x;

            if (!get_rep_dir2(&dir)) return NULL;
            if (dir == 5) return NULL;

            y = py + ddy[dir];
            x = px + ddx[dir];

            if (cave[y][x].m_idx)
                py_attack(y, x, HISSATSU_ZANMA);
            else
            {
                msg_print("There is no monster.");
                return NULL;
            }
        }
        break;

    case 10:
        if (name) return "Wind Blast";
        if (desc) return "Attacks an adjacent monster, and blow it away.";
        if (cast)
        {
            int y, x;

            if (!get_rep_dir2(&dir)) return NULL;
            if (dir == 5) return NULL;

            y = py + ddy[dir];
            x = px + ddx[dir];

            if (cave[y][x].m_idx)
                py_attack(y, x, 0);
            else
            {
                msg_print("There is no monster.");
                return NULL;
            }
            if (d_info[dungeon_type].flags1 & DF1_NO_MELEE)
            {
                return "";
            }
            if (cave[y][x].m_idx)
            {
                int i;
                int ty = y, tx = x;
                int oy = y, ox = x;
                int m_idx = cave[y][x].m_idx;
                monster_type *m_ptr = &m_list[m_idx];
                char m_name[80];
    
                monster_desc(m_name, m_ptr, 0);
    
                for (i = 0; i < 5; i++)
                {
                    y += ddy[dir];
                    x += ddx[dir];
                    if (cave_empty_bold(y, x))
                    {
                        ty = y;
                        tx = x;
                    }
                    else break;
                }
                if ((ty != oy) || (tx != ox))
                {
                    msg_format("You blow %s away!", m_name);
                    cave[oy][ox].m_idx = 0;
                    cave[ty][tx].m_idx = m_idx;
                    m_ptr->fy = ty;
                    m_ptr->fx = tx;
    
                    update_mon(m_idx, TRUE);
                    lite_spot(oy, ox);
                    lite_spot(ty, tx);
    
                    if (r_info[m_ptr->r_idx].flags7 & (RF7_LITE_MASK | RF7_DARK_MASK))
                        p_ptr->update |= (PU_MON_LITE);
                }
            }
        }
        break;

    case 11:
        if (name) return "Judge";
        if (desc) return "Identifies a weapon or armor. Or *identifies* these at level 45.";
        if (cast)
        {
            if (plev > 44)
            {
                if (!identify_fully(object_is_weapon_armour_ammo)) return NULL;
            }
            else
            {
                if (!ident_spell(object_is_weapon_armour_ammo)) return NULL;
            }
        }
        break;

    case 12:
        if (name) return "Rock Smash";
        if (desc) return "Breaks rock. Or greatly damage a monster made by rocks.";
        if (cast)
        {
            int y, x;

            if (!get_rep_dir2(&dir)) return NULL;
            if (dir == 5) return NULL;

            y = py + ddy[dir];
            x = px + ddx[dir];

            if (cave[y][x].m_idx)
                py_attack(y, x, HISSATSU_HAGAN);
    
            if (!cave_have_flag_bold(y, x, FF_HURT_ROCK)) break;

            cave_alter_feat(y, x, FF_HURT_ROCK);
            p_ptr->update |= (PU_FLOW);
        }
        break;

    case 13:
        if (name) return "Midare-Setsugekka";
        if (desc) return "Attacks a monster with increased number of attacks and more damage unless it has resistance to cold.";
        if (cast)
        {
            int y, x;

            if (!get_rep_dir2(&dir)) return NULL;
            if (dir == 5) return NULL;

            y = py + ddy[dir];
            x = px + ddx[dir];

            if (cave[y][x].m_idx)
                py_attack(y, x, HISSATSU_COLD);
            else
            {
                msg_print("There is no monster.");
                return NULL;
            }
        }
        break;

    case 14:
        if (name) return "Spot Aiming";
        if (desc) return "Attempts to kill a monster instantly. If failed cause only 1HP of damage.";
        if (cast)
        {
            int y, x;

            if (!get_rep_dir2(&dir)) return NULL;
            if (dir == 5) return NULL;

            y = py + ddy[dir];
            x = px + ddx[dir];

            if (cave[y][x].m_idx)
                py_attack(y, x, HISSATSU_KYUSHO);
            else
            {
                msg_print("There is no monster.");
                return NULL;
            }
        }
        break;

    case 15:
        if (name) return "Majingiri";
        if (desc) return "Attempts to attack with critical hit. But this attack is easy to evade for a monster.";
    
        if (cast)
        {
            int y, x;

            if (!get_rep_dir2(&dir)) return NULL;
            if (dir == 5) return NULL;

            y = py + ddy[dir];
            x = px + ddx[dir];

            if (cave[y][x].m_idx)
                py_attack(y, x, HISSATSU_MAJIN);
            else
            {
                msg_print("There is no monster.");
                return NULL;
            }
        }
        break;

    case 16:
        if (name) return "Desperate Attack";
        if (desc) return "Attacks with all of your power. But all damages you take will be doubled for one turn.";
    
        if (cast)
        {
            int y, x;

            if (!get_rep_dir2(&dir)) return NULL;
            if (dir == 5) return NULL;

            y = py + ddy[dir];
            x = px + ddx[dir];

            if (cave[y][x].m_idx)
                py_attack(y, x, HISSATSU_SUTEMI);
            else
            {
                msg_print("There is no monster.");
                return NULL;
            }
            p_ptr->sutemi = TRUE;
        }
        break;

    case 17:
        if (name) return "Lightning Eagle";
        if (desc) return "Attacks a monster with more damage unless it has resistance to electricity.";
    
        if (cast)
        {
            int y, x;

            if (!get_rep_dir2(&dir)) return NULL;
            if (dir == 5) return NULL;

            y = py + ddy[dir];
            x = px + ddx[dir];

            if (cave[y][x].m_idx)
                py_attack(y, x, HISSATSU_ELEC);
            else
            {
                msg_print("There is no monster.");
                return NULL;
            }
        }
        break;

    case 18:
        if (name) return "Rush Attack";
        if (desc) return "Steps close to a monster and attacks at a time.";
    
        if (cast)
        {
            if (!rush_attack(5, NULL)) return NULL;
        }
        break;

    case 19:
        if (name) return "Bloody Maelstrom";
        if (desc) return "Attacks all adjacent monsters with power corresponding to your cut status. Then increases your cut status. No effect to unliving monsters.";
    
        if (cast)
        {
            int y = 0, x = 0;

            cave_type       *c_ptr;
            monster_type    *m_ptr;
    
            if (p_ptr->cut < 300)
                set_cut(p_ptr->cut + 300, FALSE);
            else
                set_cut(p_ptr->cut * 2, FALSE);
    
            for (dir = 0; dir < 8; dir++)
            {
                y = py + ddy_ddd[dir];
                x = px + ddx_ddd[dir];
                c_ptr = &cave[y][x];
    
                /* Get the monster */
                m_ptr = &m_list[c_ptr->m_idx];
    
                /* Hack -- attack monsters */
                if (c_ptr->m_idx && (m_ptr->ml || cave_have_flag_bold(y, x, FF_PROJECT)))
                {
                    if (!monster_living(&r_info[m_ptr->r_idx]))
                    {
                        char m_name[80];
    
                        monster_desc(m_name, m_ptr, 0);
                        msg_format("%s is unharmed!", m_name);
                    }
                    else py_attack(y, x, HISSATSU_SEKIRYUKA);
                }
            }
        }
        break;

    case 20:
        if (name) return "Earthquake Blow";
        if (desc) return "Shakes dungeon structure, and results in random swapping of floors and walls.";
    
        if (cast)
        {
            int y,x;

            if (!get_rep_dir2(&dir)) return NULL;
            if (dir == 5) return NULL;

            y = py + ddy[dir];
            x = px + ddx[dir];

            if (cave[y][x].m_idx)
                py_attack(y, x, HISSATSU_QUAKE);
            else
                earthquake(py, px, 10);
        }
        break;

    case 21:
        if (name) return "Crack";
        if (desc) return "Fires a beam of shock wave.";
        if (cast)
        {
            int total_damage = 0, hand;
            if (!get_aim_dir(&dir)) return NULL;
            msg_print("You swing your weapon downward.");

            for (hand = 0; hand < MAX_HANDS; hand++)
            {
                int damage, basedam;
                u32b flgs[TR_FLAG_SIZE];
                object_type *o_ptr = NULL;

                if (p_ptr->weapon_info[hand].wield_how != WIELD_NONE)
                    o_ptr = equip_obj(p_ptr->weapon_info[hand].slot);

                if (!o_ptr) continue;

                basedam = (o_ptr->dd * (o_ptr->ds + 1)) * 50;
                damage = o_ptr->to_d * 100;
                
                object_flags(o_ptr, flgs);
                if (have_flag(flgs, TR_VORPAL2))
                {
                    basedam *= 5;
                    basedam /= 3;
                }
                else if (have_flag(flgs, TR_VORPAL))
                {
                    basedam *= 11;
                    basedam /= 9;
                }
                damage += basedam;
                damage *= NUM_BLOWS(hand)/100;
                total_damage += damage / 200;
            }
            fire_beam(GF_FORCE, dir, total_damage);
        }
        break;

    case 22:
        if (name) return "War Cry";
        if (desc) return "Damages all monsters in sight with sound. Aggravate nearby monsters.";
    
        if (cast)
        {
            msg_print("You roar out!");
            project_hack(GF_SOUND, randint1(plev * 3));
            aggravate_monsters(0);
        }
        break;

    case 23:
        if (name) return "Musou-Sandan";
        if (desc) return "Attacks with powerful 3 strikes.";
    
        if (cast)
        {
            int i;

            if (!get_rep_dir2(&dir)) return NULL;
            if (dir == 5) return NULL;

            for (i = 0; i < 3; i++)
            {
                int y, x;
                int ny, nx;
                int m_idx;
                cave_type *c_ptr;
                monster_type *m_ptr;
    
                y = py + ddy[dir];
                x = px + ddx[dir];
                c_ptr = &cave[y][x];
    
                if (c_ptr->m_idx)
                    py_attack(y, x, HISSATSU_3DAN);
                else
                {
                    msg_print("There is no monster.");
                    return NULL;
                }
    
                if (d_info[dungeon_type].flags1 & DF1_NO_MELEE)
                {
                    return "";
                }
    
                /* Monster is dead? */
                if (!c_ptr->m_idx) break;
    
                ny = y + ddy[dir];
                nx = x + ddx[dir];
                m_idx = c_ptr->m_idx;
                m_ptr = &m_list[m_idx];
    
                /* Monster cannot move back? */
                if (!monster_can_enter(ny, nx, &r_info[m_ptr->r_idx], 0))
                {
                    /* -more- */
                    if (i < 2) msg_print(NULL);
                    continue;
                }
    
                c_ptr->m_idx = 0;
                cave[ny][nx].m_idx = m_idx;
                m_ptr->fy = ny;
                m_ptr->fx = nx;
    
                update_mon(m_idx, TRUE);
    
                /* Redraw the old spot */
                lite_spot(y, x);
    
                /* Redraw the new spot */
                lite_spot(ny, nx);
    
                /* Player can move forward? */
                if (player_can_enter(c_ptr->feat, 0))
                {
                    /* Move the player */
                    if (!move_player_effect(y, x, MPE_FORGET_FLOW | MPE_HANDLE_STUFF | MPE_DONT_PICKUP)) break;
                }
                else
                {
                    break;
                }

                /* -more- */
                if (i < 2) msg_print(NULL);
            }
        }
        break;

    case 24:
        if (name) return "Vampire's Fang";
        if (desc) return "Attacks with vampiric strikes which absorbs HP from a monster and gives them to you. No effect to unliving monsters.";
    
        if (cast)
        {
            int y, x;

            if (!get_rep_dir2(&dir)) return NULL;
            if (dir == 5) return NULL;

            y = py + ddy[dir];
            x = px + ddx[dir];

            if (cave[y][x].m_idx)
                py_attack(y, x, HISSATSU_DRAIN);
            else
            {
                    msg_print("There is no monster.");
                return NULL;
            }
        }
        break;

    case 25:
        if (name) return "Moon Dazzling";
        if (desc) return "Attempts to stun, confuse and sleep all waking monsters.";
    
        if (cast)
        {
            msg_print("You irregularly wave your weapon...");
            project_hack(GF_ENGETSU, plev * 4);
            project_hack(GF_ENGETSU, plev * 4);
            project_hack(GF_ENGETSU, plev * 4);
        }
        break;

    case 26:
        if (name) return "Hundred Slaughter";
        if (desc) return "Performs a series of rush attacks. The series continues while killing each monster in a time and SP remains.";
    
        if (cast)
        {
            const int mana_cost_per_monster = 8;
            bool new = TRUE;
            bool mdeath;

            do
            {
                if (!rush_attack(5, &mdeath)) break;
                if (new)
                {
                    /* Reserve needed mana point */
                    p_ptr->csp -= technic_info[REALM_HISSATSU - MIN_TECHNIC][26].smana;
                    new = FALSE;
                }
                else
                    p_ptr->csp -= mana_cost_per_monster;

                if (!mdeath) break;
                command_dir = 0;

                p_ptr->redraw |= PR_MANA;
                handle_stuff();
            }
            while (p_ptr->csp > mana_cost_per_monster);

            if (new) return NULL;
    
            /* Restore reserved mana */
            p_ptr->csp += technic_info[REALM_HISSATSU - MIN_TECHNIC][26].smana;
        }
        break;

    case 27:
        if (name) return "Dragonic Flash";
        if (desc) return "Runs toward given location while attacking all monsters on the path.";
    
        if (cast)
        {
            int y, x;

            if (!tgt_pt(&x, &y, MAX_SIGHT / 2)) return NULL;

            if (!cave_player_teleportable_bold(y, x, 0L) ||
                (distance(y, x, py, px) > MAX_SIGHT / 2) ||
                !projectable(py, px, y, x))
            {
                msg_print("You cannot move to that place!");
                break;
            }
            if (p_ptr->anti_tele)
            {
                msg_print("A mysterious force prevents you from teleporting!");
    
                break;
            }
            project(0, 0, y, x, HISSATSU_ISSEN, GF_ATTACK, PROJECT_BEAM | PROJECT_KILL, -1);
            teleport_player_to(y, x, 0L);
        }
        break;

    case 28:
        if (name) return "Twin Slash";
        if (desc) return "double attacks at a time.";
    
        if (cast)
        {
            int x, y;
    
            if (!get_rep_dir(&dir, FALSE)) return NULL;

            y = py + ddy[dir];
            x = px + ddx[dir];

            if (cave[y][x].m_idx)
            {
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
                return NULL;
            }
        }
        break;

    case 29:
        if (name) return "Kofuku-Zettousei";
        if (desc) return "Performs a powerful attack which even effect nearby monsters.";
        if (cast)
        {
            int total_damage = 0, hand;
            int y, x;
    
            if (!get_rep_dir2(&dir)) return NULL;
            if (dir == 5) return NULL;

            y = py + ddy[dir];
            x = px + ddx[dir];

            if (d_info[dungeon_type].flags1 & DF1_NO_MELEE)
            {
                msg_print("Something prevent you from attacking.");
                return "";
            }
            msg_print("You swing your weapon downward.");
            for (hand = 0; hand < MAX_HANDS; hand++)
            {
                int damage, basedam;
                u32b flgs[TR_FLAG_SIZE];
                object_type *o_ptr = NULL;

                if (p_ptr->weapon_info[hand].wield_how != WIELD_NONE)
                    o_ptr = equip_obj(p_ptr->weapon_info[hand].slot);

                if (!o_ptr) continue;

                basedam = (o_ptr->dd * (o_ptr->ds + 1)) * 50;
                damage = o_ptr->to_d * 100;
                
                object_flags(o_ptr, flgs);
                if (have_flag(flgs, TR_VORPAL2))
                {
                    basedam *= 5;
                    basedam /= 3;
                }
                else if (have_flag(flgs, TR_VORPAL))
                {
                    basedam *= 11;
                    basedam /= 9;
                }
                damage += basedam;
                damage += p_ptr->weapon_info[hand].to_d * 100;
                damage *= NUM_BLOWS(hand)/100;
                total_damage += damage / 200;
            }
            project(0, (cave_have_flag_bold(y, x, FF_PROJECT) ? 5 : 0), y, x, total_damage * 3 / 2, GF_METEOR, PROJECT_KILL | PROJECT_JUMP | PROJECT_ITEM, -1);
        }
        break;

    case 30:
        if (name) return "Keiun-Kininken";
        if (desc) return "Attacks a monster with extremely powerful damage. But you also takes some damages. Hurts a undead monster greatly.";
    
        if (cast)
        {
            int y, x;

            if (!get_rep_dir2(&dir)) return NULL;
            if (dir == 5) return NULL;

            y = py + ddy[dir];
            x = px + ddx[dir];

            if (cave[y][x].m_idx)
                py_attack(y, x, HISSATSU_UNDEAD);
            else
            {
                msg_print("There is no monster.");
                return NULL;
            }
            take_hit(DAMAGE_NOESCAPE, 100 + randint1(100), "exhaustion on using Keiun-Kininken", -1);
        }
        break;

    case 31:
        if (name) return "Harakiri";
        if (desc) return "'Busido is found in death'";

        if (cast)
        {
            int i;
            if (!get_check("Do you really want to commit suicide? ")) return NULL;
                /* Special Verification for suicide */
            prt("Please verify SUICIDE by typing the '@' sign: ", 0, 0);
    
            flush();
            i = inkey();
            prt("", 0, 0);
            if (i != '@') return NULL;
            if (p_ptr->total_winner)
            {
                take_hit(DAMAGE_FORCE, 9999, "Seppuku", -1);
                p_ptr->total_winner = TRUE;
            }
            else
            {
                msg_print("Meaning of Bushi-do is found in the death.");
                take_hit(DAMAGE_FORCE, 9999, "Seppuku", -1);
            }
        }
        break;
    }

    return "";
}

/***********************************************************************
 * Samurai Postures (Kata)
 ***********************************************************************/
void samurai_posture_calc_bonuses(void)
{
    if (p_ptr->special_defense & KATA_FUUJIN)
    {
        /* see project_p for special handling ... review?
        if (!p_ptr->blind)
            p_ptr->reflect = TRUE; */
    }
    if (p_ptr->special_defense & KATA_KOUKIJIN)
    {
        p_ptr->to_a -= 50;
        p_ptr->dis_to_a -= 50;
        res_add_vuln(RES_ACID);
        res_add_vuln(RES_ELEC);
        res_add_vuln(RES_FIRE);
        res_add_vuln(RES_COLD);
    }

    if (p_ptr->special_defense & KATA_MUSOU)
    {
        p_ptr->see_inv = TRUE;
        p_ptr->free_act = TRUE;
        p_ptr->slow_digest = TRUE;
        p_ptr->regenerate = TRUE;
        p_ptr->levitation = TRUE;
        p_ptr->hold_life = TRUE;
        p_ptr->sustain_str = TRUE;
        p_ptr->sustain_int = TRUE;
        p_ptr->sustain_wis = TRUE;
        p_ptr->sustain_con = TRUE;
        p_ptr->sustain_dex = TRUE;
        p_ptr->sustain_chr = TRUE;
        p_ptr->telepathy = TRUE;
        p_ptr->lite = TRUE;
        res_add_all();
        p_ptr->reflect = TRUE;
        p_ptr->sh_fire = TRUE;
        p_ptr->sh_elec = TRUE;
        p_ptr->sh_cold = TRUE;
        p_ptr->to_a += 100;
        p_ptr->dis_to_a += 100;
    }
}

void samurai_posture_calc_stats(s16b stats[MAX_STATS])
{
    if (p_ptr->special_defense & KATA_KOUKIJIN)
    {
        int i;
        for (i = 0; i < MAX_STATS; i++)
            stats[i] += 5;
    }
}

void samurai_posture_get_flags(u32b flgs[TR_FLAG_SIZE])
{
    if (p_ptr->special_defense & KATA_FUUJIN)
        add_flag(flgs, TR_REFLECT);

    if (p_ptr->special_defense & KATA_MUSOU)
    {
        add_flag(flgs, TR_RES_FEAR);
        add_flag(flgs, TR_RES_LITE);
        add_flag(flgs, TR_RES_DARK);
        add_flag(flgs, TR_RES_BLIND);
        add_flag(flgs, TR_RES_CONF);
        add_flag(flgs, TR_RES_SOUND);
        add_flag(flgs, TR_RES_SHARDS);
        add_flag(flgs, TR_RES_NETHER);
        add_flag(flgs, TR_RES_NEXUS);
        add_flag(flgs, TR_RES_CHAOS);
        add_flag(flgs, TR_RES_DISEN);
        add_flag(flgs, TR_RES_TIME);
        add_flag(flgs, TR_REFLECT);
        add_flag(flgs, TR_HOLD_LIFE);
        add_flag(flgs, TR_FREE_ACT);
        add_flag(flgs, TR_SH_FIRE);
        add_flag(flgs, TR_SH_ELEC);
        add_flag(flgs, TR_SH_COLD);
        add_flag(flgs, TR_LEVITATION);
        add_flag(flgs, TR_LITE);
        add_flag(flgs, TR_SEE_INVIS);
        add_flag(flgs, TR_TELEPATHY);
        add_flag(flgs, TR_SLOW_DIGEST);
        add_flag(flgs, TR_REGEN);
        add_flag(flgs, TR_SUST_STR);
        add_flag(flgs, TR_SUST_INT);
        add_flag(flgs, TR_SUST_WIS);
        add_flag(flgs, TR_SUST_DEX);
        add_flag(flgs, TR_SUST_CON);
        add_flag(flgs, TR_SUST_CHR);
    }

    if (p_ptr->special_defense & KATA_KOUKIJIN)
    {
        add_flag(flgs, TR_VULN_ACID);
        add_flag(flgs, TR_VULN_ELEC);
        add_flag(flgs, TR_VULN_FIRE);
        add_flag(flgs, TR_VULN_COLD);
    }
}

static bool _choose_kata(void)
{
    char choice;
    int new_kata = 0;
    int i;
    char buf[80];

    if (p_ptr->confused)
    {
        msg_print("You are too confused.");
        return FALSE;
    }
    if (p_ptr->stun)
    {
        msg_print("You are not clear headed");
        return FALSE;
    }
    if (p_ptr->afraid)
    {
        msg_print("You are trembling with fear!");
        return FALSE;
    }

    screen_save();

    prt(" a) No Form", 2, 20);

    for (i = 0; i < MAX_KATA; i++)
    {
        if (p_ptr->lev >= kata_shurui[i].min_level)
        {
            sprintf(buf," %c) Form of %-12s  %s",I2A(i+1), kata_shurui[i].desc, kata_shurui[i].info);
            prt(buf, 3+i, 20);
        }
    }

    prt("", 1, 0);
    prt("        Choose Form: ", 1, 14);

    for(;;)
    {
        choice = inkey();

        if (choice == ESCAPE)
        {
            screen_load();
            return FALSE;
        }
        else if ((choice == 'a') || (choice == 'A'))
        {
            if (p_ptr->action == ACTION_KATA)
                set_action(ACTION_NONE);
            else
                msg_print("You are not assuming posture.");

            screen_load();
            return TRUE;
        }
        else if ((choice == 'b') || (choice == 'B'))
        {
            new_kata = 0;
            break;
        }
        else if (((choice == 'c') || (choice == 'C')) && (p_ptr->lev > 29))
        {
            new_kata = 1;
            break;
        }
        else if (((choice == 'd') || (choice == 'D')) && (p_ptr->lev > 34))
        {
            new_kata = 2;
            break;
        }
        else if (((choice == 'e') || (choice == 'E')) && (p_ptr->lev > 39))
        {
            new_kata = 3;
            break;
        }
    }
    set_action(ACTION_KATA);

    if (p_ptr->special_defense & (KATA_IAI << new_kata))
    {
        msg_print("You reassume a posture.");
    }
    else
    {
        p_ptr->special_defense &= ~(KATA_MASK);
        p_ptr->update |= (PU_BONUS);
        p_ptr->update |= (PU_MONSTERS);
        msg_format("You assume a posture of %s form.",kata_shurui[new_kata].desc);
        p_ptr->special_defense |= (KATA_IAI << new_kata);
    }
    p_ptr->redraw |= (PR_STATE);
    p_ptr->redraw |= (PR_STATUS);
    screen_load();
    return TRUE;
}

static int _max_sp(void)
{
    return MAX(p_ptr->msp*4, p_ptr->lev*5+5);
}

static void _concentrate(bool noisy)
{
    int max_csp = _max_sp();
    if (total_friends)
        return;
    if (p_ptr->special_defense & KATA_MASK)
        return;
        
    if (noisy)
        msg_print("You concentrate to charge your power.");

    p_ptr->csp += p_ptr->msp / 2;
    if (p_ptr->csp >= max_csp)
    {
        p_ptr->csp = max_csp;
        p_ptr->csp_frac = 0;
    }

    p_ptr->redraw |= (PR_MANA);
 }

void samurai_concentration_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Concentration");
        break;
    case SPELL_DESC:
        var_set_string(res, "");
        break;
    case SPELL_CAST:
    {
        var_set_bool(res, FALSE);
        if (total_friends)
        {
            msg_print("You need concentration on the pets now.");
            return;
        }
        if (p_ptr->special_defense & KATA_MASK)
        {
            msg_print("You need concentration on your form.");
            return;
        }

        _concentrate(TRUE);

        var_set_bool(res, TRUE);
        break;
    }
    default:
        default_spell(cmd, res);
        break;
    }
}

void samurai_posture_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Assume a Guard Position");
        break;
    case SPELL_DESC:
        var_set_string(res, "");
        break;
    case SPELL_CAST:
        var_set_bool(res, FALSE);
        if (!equip_find_first(object_is_melee_weapon))
        {
            msg_print("You need to wield a weapon.");
            return;
        }
        if (!_choose_kata()) return;

        p_ptr->update |= (PU_BONUS);
        var_set_bool(res, TRUE);
        break;
    default:
        default_spell(cmd, res);
        break;
    }
}

static void _calc_bonuses(void)
{
    samurai_posture_calc_bonuses();
    if (p_ptr->lev >= 30)
        res_add(RES_FEAR);
}

static void _calc_stats(s16b stats[MAX_STATS])
{
    samurai_posture_calc_stats(stats);
}

static void _get_flags(u32b flgs[TR_FLAG_SIZE])
{
    samurai_posture_get_flags(flgs);
    if (p_ptr->lev >= 30)
        add_flag(flgs, TR_RES_FEAR);
}

static int _get_powers(spell_info* spells, int max)
{
    int ct = 0;

    spell_info* spell = &spells[ct++];
    spell->level = 1;
    spell->cost = 0;
    spell->fail = 0;
    spell->fn = samurai_concentration_spell;

    spell = &spells[ct++];
    spell->level = 25;
    spell->cost = 0;
    spell->fail = 0;
    spell->fn = samurai_posture_spell;

    return ct;
}

static caster_info * _caster_info(void)
{
    static caster_info me = {0};
    static bool init = FALSE;
    if (!init)
    {
        me.magic_desc = "technique";
        me.which_stat = A_WIS;
        me.weight = 3000;
        init = TRUE;
    }
    return &me;
}

class_t *samurai_get_class(void)
{
    static class_t me = {0};
    static bool init = FALSE;

    if (!init)
    {           /* dis, dev, sav, stl, srh, fos, thn, thb */
    skills_t bs = { 25,  18,  32,   2,  16,   6,  70,  40};
    skills_t xs = { 12,   7,  10,   0,   0,   0,  23,  18};


        me.name = "Samurai";
        me.desc = "Samurai, masters of the art of the blade, are the next strongest "
                    "fighters after Warriors, and can use various special combat "
                    "techniques. Samurai are not good at most other skills, and many "
                    "magical devices may be too difficult for them to use. Wisdom "
                    "determines a Samurai's ability to use the special combat "
                    "techniques available to him.\n \n"
                    "Samurai use the art of the blade called Kendo (or Bugei). Books "
                    "of Kendo are similar to spellbooks, but Samurai don't need to "
                    "carry them around; the books are needed only when they study new "
                    "combat techniques. Samurai need a weapon wielded to use the "
                    "techniques of Kendo, and most techniques will add powerful special "
                    "properties to their blows; such as flaming, poisoning, vampiric, "
                    "etc... Their maximum spellpoints don't depend on their level but "
                    "solely on wisdom, and they can use the class power 'Concentration' "
                    "to temporarily increase SP beyond its usual maximum value. They "
                    "have one more class power - 'Assume a Posture'. They can choose "
                    "different forms of posture in different situations.";

        me.stats[A_STR] =  3;
        me.stats[A_INT] = -2;
        me.stats[A_WIS] =  1;
        me.stats[A_DEX] =  2;
        me.stats[A_CON] =  1;
        me.stats[A_CHR] =  1;
        me.base_skills = bs;
        me.extra_skills = xs;
        me.life = 111;
        me.base_hp = 12;
        me.exp = 130;
        me.pets = 40;

        me.caster_info = _caster_info;        
        me.calc_bonuses = _calc_bonuses;
        me.calc_stats = _calc_stats;
        me.get_flags = _get_flags;
        me.get_powers = _get_powers;
        me.character_dump = spellbook_character_dump;
        init = TRUE;
    }

    return &me;
}

void samurai_on_rest(void)
{
    if (p_ptr->pclass == CLASS_SAMURAI)
        _concentrate(FALSE);
}

bool samurai_can_concentrate(void)
{
    if (p_ptr->pclass != CLASS_SAMURAI) return FALSE;
    if (p_ptr->csp >= _max_sp()) return FALSE;
    return TRUE;
}
