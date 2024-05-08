#ifndef AZEROTHCORE_CREATURE_H
#define AZEROTHCORE_CREATURE_H

#include "Cell.h"
#include "Common.h"
#include "Unit.h"
#include "UpdateMask.h"
#include <list>

class Creature : public Unit, public GridObject<Creature>//, public MovableMapObject
{
public:
	//~Creature() override;
	~Creature();

	void SetPosition(float x, float y, float z, float o);
	void SetPosition(const Position& pos) { SetPosition(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation()); }

	bool CanSwim() const
	{
		//if (Unit::CanSwim() || (!Unit::CanSwim() && !CanFly()))
		//	return true;

		//if (IsPet())
		//	return true;

		//return false;
		return true;
	}

	//[[nodiscard]] bool CanFly()  const override { return GetMovementTemplate().IsFlightAllowed() || IsFlying(); }
	[[nodiscard]] bool CanFly()  const { return false; }

	bool CanEnterWater() const
	{
		//if (CanSwim())
		//	return true;

		//return GetMovementTemplate().IsSwimAllowed();
		return true;
	}

    //Creature* ToCreature() { if (GetTypeId() == TYPEID_UNIT) return reinterpret_cast<Creature*>(this); else return nullptr; }
    //[[nodiscard]] Creature const* ToCreature() const { if (GetTypeId() == TYPEID_UNIT) return (Creature const*)((Creature*)this); else return nullptr; }
    Creature* ToCreature() { return reinterpret_cast<Creature*>(this); }
    [[nodiscard]] Creature const* ToCreature() const { return (Creature const*)((Creature*)this); }

protected:
    bool m_moveInLineOfSightDisabled;
    bool m_moveInLineOfSightStrictlyDisabled;

};

#endif
