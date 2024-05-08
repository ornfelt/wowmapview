#ifndef __UNIT_H
#define __UNIT_H

#include "MotionMaster.h"
#include "Optional.h"
#include "SpellAuraDefines.h"
#include <functional>
#include <utility>
#include "Object.h"

enum UnitState
{
    UNIT_STATE_DIED            = 0x00000001,                     // player has fake death aura
    UNIT_STATE_MELEE_ATTACKING = 0x00000002,                     // player is melee attacking someone
    //UNIT_STATE_MELEE_ATTACK_BY = 0x00000004,                     // player is melee attack by someone
    UNIT_STATE_STUNNED         = 0x00000008,
    UNIT_STATE_ROAMING         = 0x00000010,
    UNIT_STATE_CHASE           = 0x00000020,
    //UNIT_STATE_SEARCHING       = 0x00000040,
    UNIT_STATE_FLEEING         = 0x00000080,
    UNIT_STATE_IN_FLIGHT       = 0x00000100,                     // player is in flight mode
    UNIT_STATE_FOLLOW          = 0x00000200,
    UNIT_STATE_ROOT            = 0x00000400,
    UNIT_STATE_CONFUSED        = 0x00000800,
    UNIT_STATE_DISTRACTED      = 0x00001000,
    UNIT_STATE_ISOLATED        = 0x00002000,                     // area auras do not affect other players
    UNIT_STATE_ATTACK_PLAYER   = 0x00004000,
    UNIT_STATE_CASTING         = 0x00008000,
    UNIT_STATE_POSSESSED       = 0x00010000,
    UNIT_STATE_CHARGING        = 0x00020000,
    UNIT_STATE_JUMPING         = 0x00040000,
    UNIT_STATE_MOVE            = 0x00100000,
    UNIT_STATE_ROTATING        = 0x00200000,
    UNIT_STATE_EVADE           = 0x00400000,
    UNIT_STATE_ROAMING_MOVE    = 0x00800000,
    UNIT_STATE_CONFUSED_MOVE   = 0x01000000,
    UNIT_STATE_FLEEING_MOVE    = 0x02000000,
    UNIT_STATE_CHASE_MOVE      = 0x04000000,
    UNIT_STATE_FOLLOW_MOVE     = 0x08000000,
    UNIT_STATE_IGNORE_PATHFINDING = 0x10000000,                 // do not use pathfinding in any MovementGenerator
    UNIT_STATE_NO_ENVIRONMENT_UPD = 0x20000000, // pussywizard

    UNIT_STATE_ALL_STATE_SUPPORTED = UNIT_STATE_DIED | UNIT_STATE_MELEE_ATTACKING | UNIT_STATE_STUNNED | UNIT_STATE_ROAMING | UNIT_STATE_CHASE
                                     | UNIT_STATE_FLEEING | UNIT_STATE_IN_FLIGHT | UNIT_STATE_FOLLOW | UNIT_STATE_ROOT | UNIT_STATE_CONFUSED
                                     | UNIT_STATE_DISTRACTED | UNIT_STATE_ISOLATED | UNIT_STATE_ATTACK_PLAYER | UNIT_STATE_CASTING
                                     | UNIT_STATE_POSSESSED | UNIT_STATE_CHARGING | UNIT_STATE_JUMPING | UNIT_STATE_MOVE | UNIT_STATE_ROTATING
                                     | UNIT_STATE_EVADE | UNIT_STATE_ROAMING_MOVE | UNIT_STATE_CONFUSED_MOVE | UNIT_STATE_FLEEING_MOVE
                                     | UNIT_STATE_CHASE_MOVE | UNIT_STATE_FOLLOW_MOVE | UNIT_STATE_IGNORE_PATHFINDING | UNIT_STATE_NO_ENVIRONMENT_UPD,

    UNIT_STATE_UNATTACKABLE         = UNIT_STATE_IN_FLIGHT,
    // for real move using movegen check and stop (except unstoppable flight)
    UNIT_STATE_MOVING               = UNIT_STATE_ROAMING_MOVE | UNIT_STATE_CONFUSED_MOVE | UNIT_STATE_FLEEING_MOVE | UNIT_STATE_CHASE_MOVE | UNIT_STATE_FOLLOW_MOVE,
    UNIT_STATE_CONTROLLED           = (UNIT_STATE_CONFUSED | UNIT_STATE_STUNNED | UNIT_STATE_FLEEING),
    UNIT_STATE_LOST_CONTROL         = (UNIT_STATE_CONTROLLED | UNIT_STATE_JUMPING | UNIT_STATE_CHARGING),
    UNIT_STATE_SIGHTLESS            = (UNIT_STATE_LOST_CONTROL | UNIT_STATE_EVADE),
    UNIT_STATE_CANNOT_AUTOATTACK    = (UNIT_STATE_LOST_CONTROL | UNIT_STATE_CASTING),
    UNIT_STATE_CANNOT_TURN          = (UNIT_STATE_LOST_CONTROL | UNIT_STATE_ROTATING | UNIT_STATE_ROOT),
    // stay by different reasons
    UNIT_STATE_NOT_MOVE             = UNIT_STATE_ROOT | UNIT_STATE_STUNNED | UNIT_STATE_DIED | UNIT_STATE_DISTRACTED,
    UNIT_STATE_IGNORE_ANTISPEEDHACK = UNIT_STATE_FLEEING | UNIT_STATE_CONFUSED | UNIT_STATE_CHARGING | UNIT_STATE_DISTRACTED | UNIT_STATE_POSSESSED,
    UNIT_STATE_ALL_STATE            = 0xffffffff                      //(UNIT_STATE_STOPPED | UNIT_STATE_MOVING | UNIT_STATE_IN_COMBAT | UNIT_STATE_IN_FLIGHT)
};

enum UnitFlags : uint32
{
    UNIT_FLAG_NONE                          = 0x00000000,
    UNIT_FLAG_SERVER_CONTROLLED             = 0x00000001,           // set only when unit movement is controlled by server - by SPLINE/MONSTER_MOVE packets, together with UNIT_FLAG_STUNNED; only set to units controlled by client; client function CGUnit_C::IsClientControlled returns false when set for owner
    UNIT_FLAG_NON_ATTACKABLE                = 0x00000002,           // not attackable
    UNIT_FLAG_DISABLE_MOVE                  = 0x00000004,
    UNIT_FLAG_PLAYER_CONTROLLED             = 0x00000008,           // controlled by player, use _IMMUNE_TO_PC instead of _IMMUNE_TO_NPC
    UNIT_FLAG_RENAME                        = 0x00000010,
    UNIT_FLAG_PREPARATION                   = 0x00000020,           // don't take reagents for spells with SPELL_ATTR5_NO_REAGENT_COST_WITH_AURA
    UNIT_FLAG_UNK_6                         = 0x00000040,
    UNIT_FLAG_NOT_ATTACKABLE_1              = 0x00000080,           // ?? (UNIT_FLAG_PLAYER_CONTROLLED | UNIT_FLAG_NOT_ATTACKABLE_1) is NON_PVP_ATTACKABLE
    UNIT_FLAG_IMMUNE_TO_PC                  = 0x00000100,           // disables combat/assistance with PlayerCharacters (PC) - see Unit::_IsValidAttackTarget, Unit::_IsValidAssistTarget
    UNIT_FLAG_IMMUNE_TO_NPC                 = 0x00000200,           // disables combat/assistance with NonPlayerCharacters (NPC) - see Unit::_IsValidAttackTarget, Unit::_IsValidAssistTarget
    UNIT_FLAG_LOOTING                       = 0x00000400,           // loot animation
    UNIT_FLAG_PET_IN_COMBAT                 = 0x00000800,           // in combat?, 2.0.8
    UNIT_FLAG_PVP                           = 0x00001000,           // changed in 3.0.3
    UNIT_FLAG_SILENCED                      = 0x00002000,           // silenced, 2.1.1
    UNIT_FLAG_CANNOT_SWIM                   = 0x00004000,           // 2.0.8
    UNIT_FLAG_SWIMMING                      = 0x00008000,           // shows swim animation in water
    UNIT_FLAG_NON_ATTACKABLE_2              = 0x00010000,           // removes attackable icon, if on yourself, cannot assist self but can cast TARGET_SELF spells - added by SPELL_AURA_MOD_UNATTACKABLE
    UNIT_FLAG_PACIFIED                      = 0x00020000,           // 3.0.3 ok
    UNIT_FLAG_STUNNED                       = 0x00040000,           // 3.0.3 ok
    UNIT_FLAG_IN_COMBAT                     = 0x00080000,
    UNIT_FLAG_TAXI_FLIGHT                   = 0x00100000,           // disable casting at client side spell not allowed by taxi flight (mounted?), probably used with 0x4 flag
    UNIT_FLAG_DISARMED                      = 0x00200000,           // 3.0.3, disable melee spells casting..., "Required melee weapon" added to melee spells tooltip.
    UNIT_FLAG_CONFUSED                      = 0x00400000,
    UNIT_FLAG_FLEEING                       = 0x00800000,
    UNIT_FLAG_POSSESSED                     = 0x01000000,           // under direct client control by a player (possess or vehicle)
    UNIT_FLAG_NOT_SELECTABLE                = 0x02000000,
    UNIT_FLAG_SKINNABLE                     = 0x04000000,
    UNIT_FLAG_MOUNT                         = 0x08000000,
    UNIT_FLAG_UNK_28                        = 0x10000000,
    UNIT_FLAG_PREVENT_EMOTES_FROM_CHAT_TEXT = 0x20000000,           // Prevent automatically playing emotes from parsing chat text, for example "lol" in /say, ending message with ? or !, or using /yell
    UNIT_FLAG_SHEATHE                       = 0x40000000,
    UNIT_FLAG_IMMUNE                        = 0x80000000,           // Immune to damage
};

enum UnitFlags2 : uint32
{
    UNIT_FLAG2_NONE                         = 0x00000000,
    UNIT_FLAG2_FEIGN_DEATH                  = 0x00000001,
    UNIT_FLAG2_HIDE_BODY                    = 0x00000002,   // Hide unit model (show only player equip)
    UNIT_FLAG2_IGNORE_REPUTATION            = 0x00000004,
    UNIT_FLAG2_COMPREHEND_LANG              = 0x00000008,
    UNIT_FLAG2_MIRROR_IMAGE                 = 0x00000010,
    UNIT_FLAG2_DO_NOT_FADE_IN               = 0x00000020,   // Unit model instantly appears when summoned (does not fade in)
    UNIT_FLAG2_FORCE_MOVEMENT               = 0x00000040,
    UNIT_FLAG2_DISARM_OFFHAND               = 0x00000080,
    UNIT_FLAG2_DISABLE_PRED_STATS           = 0x00000100,   // Player has disabled predicted stats (Used by raid frames)
    UNIT_FLAG2_DISARM_RANGED                = 0x00000400,   // this does not disable ranged weapon display (maybe additional flag needed?)
    UNIT_FLAG2_REGENERATE_POWER             = 0x00000800,
    UNIT_FLAG2_RESTRICT_PARTY_INTERACTION   = 0x00001000,   // Restrict interaction to party or raid
    UNIT_FLAG2_PREVENT_SPELL_CLICK          = 0x00002000,   // Prevent spellclick
    UNIT_FLAG2_ALLOW_ENEMY_INTERACT         = 0x00004000,
    UNIT_FLAG2_CANNOT_TURN                  = 0x00008000,
    UNIT_FLAG2_UNK2                         = 0x00010000,
    UNIT_FLAG2_PLAY_DEATH_ANIM              = 0x00020000,   // Plays special death animation upon death
    UNIT_FLAG2_ALLOW_CHEAT_SPELLS           = 0x00040000,   // Allows casting spells with AttributesEx7 & SPELL_ATTR7_DEBUG_SPELL
    UNIT_FLAG2_UNUSED_6                     = 0x01000000,
};

class Unit : public WorldObject
{
public:
    //~Unit() override;
    //Unit();

    //Unit(bool isWorldObject = false) : WorldObject(isWorldObject) {}
    Unit() : WorldObject() {}
    //Unit() : WorldObject() {}
    virtual ~Unit() override {}

    //UnitFlags GetUnitFlags() const { return UnitFlags(GetUInt32Value(UNIT_FIELD_FLAGS)); }
    //bool HasUnitFlag(UnitFlags flags) const { return HasFlag(UNIT_FIELD_FLAGS, flags); }
    //void SetUnitFlag(UnitFlags flags) { SetFlag(UNIT_FIELD_FLAGS, flags); }
    //void RemoveUnitFlag(UnitFlags flags) { RemoveFlag(UNIT_FIELD_FLAGS, flags); }
    //void ReplaceAllUnitFlags(UnitFlags flags) { SetUInt32Value(UNIT_FIELD_FLAGS, flags); }
    //UnitFlags2 GetUnitFlags2() const { return UnitFlags2(GetUInt32Value(UNIT_FIELD_FLAGS_2)); }
    //bool HasUnitFlag2(UnitFlags2 flags) const { return HasFlag(UNIT_FIELD_FLAGS_2, flags); }
    //void SetUnitFlag2(UnitFlags2 flags) { SetFlag(UNIT_FIELD_FLAGS_2, flags); }
    //void RemoveUnitFlag2(UnitFlags2 flags) { RemoveFlag(UNIT_FIELD_FLAGS_2, flags); }
    //void ReplaceAllUnitFlags2(UnitFlags2 flags) { SetUInt32Value(UNIT_FIELD_FLAGS_2, flags); }

	bool IsInWater() const
	{
		//return (GetLiquidData().Status & MAP_LIQUID_STATUS_SWIMMING) != 0;
		return false;
	}

	bool IsUnderWater() const
	{
		//return GetLiquidData().Status == LIQUID_MAP_UNDER_WATER;
		return false;
	}

	bool CanFly() const
	{
		return false;
	}

	bool IsFlying() const
	{
		return false;
	}

	bool CanSwim() const
	{
		// Mirror client behavior, if this method returns false then client will not use swimming animation and for players will apply gravity as if there was no water
		//if (HasUnitFlag(UNIT_FLAG_CANNOT_SWIM))
		//	return false;
		//if (HasUnitFlag(UNIT_FLAG_POSSESSED) || HasUnitFlag(UNIT_FLAG_PLAYER_CONTROLLED)) // is player
		//	return true;
		//if (HasUnitFlag2(UNIT_FLAG2_UNUSED_6))
		//	return false;
		//if (HasUnitFlag(UNIT_FLAG_PET_IN_COMBAT))
		//	return true;
		//return HasUnitFlag(UNIT_FLAG_RENAME | UNIT_FLAG_SWIMMING);
        return true;
	}

    //[[nodiscard]] bool IsHovering() const { return m_movementInfo.HasMovementFlag(MOVEMENTFLAG_HOVER); }
    //[[nodiscard]] float GetHoverHeight() const { return IsHovering() ? GetFloatValue(UNIT_FIELD_HOVERHEIGHT) : 0.0f; }
    [[nodiscard]] float GetHoverHeight() const { return 0.0f; }

    void AddUnitState(uint32 f) { m_state |= f; }
    [[nodiscard]] bool HasUnitState(const uint32 f) const { return (m_state & f); }
    void ClearUnitState(uint32 f) { m_state &= ~f; }
    [[nodiscard]] uint32 GetUnitState() const { return m_state; }

//protected:
//    explicit Unit (bool isWorldObject);

private:
    uint32 m_state;                                     // Even derived shouldn't modify

};

#endif
