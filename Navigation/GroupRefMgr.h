#ifndef _GROUPREFMANAGER
#define _GROUPREFMANAGER

#include "RefMgr.h"

class Group;
class Player;
class GroupReference;

class GroupRefMgr : public RefMgr<Group, Player>
{
public:
    GroupReference* getFirst() { return ((GroupReference*)RefMgr<Group, Player>::getFirst()); }
    GroupReference const* getFirst() const { return ((GroupReference const*)RefMgr<Group, Player>::getFirst()); }
};

//npcbot
class Creature;
class GroupBotReference;

class GroupBotRefManager : public RefMgr<Group, Creature>
{
    public:
        GroupBotReference* getFirst() { return ((GroupBotReference*)RefMgr<Group, Creature>::getFirst()); }
        GroupBotReference const* getFirst() const { return ((GroupBotReference const*)RefMgr<Group, Creature>::getFirst()); }
};
//end npcbot
#endif
