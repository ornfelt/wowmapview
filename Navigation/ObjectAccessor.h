#ifndef ACORE_OBJECTACCESSOR_H
#define ACORE_OBJECTACCESSOR_H

#include "Define.h"
#include "GridDefines.h"
#include "Object.h"
#include "UpdateData.h"
#include <mutex>
#include <set>
#include <shared_mutex>
#include <unordered_map>

class Creature;
class Corpse;
class Unit;
class GameObject;
class DynamicObject;
class WorldObject;
class Vehicle;
class Map;
class WorldRunnable;
class Transport;
class StaticTransport;
class MotionTransport;

template <class T>
class HashMapHolder
{
    //Non instanceable only static
    HashMapHolder() = default;

public:

    typedef std::unordered_map<ObjectGuid, T*> MapType;

    static void Insert(T* o);

    static void Remove(T* o);

    static T* Find(ObjectGuid guid);

    static MapType& GetContainer();

    static std::shared_mutex* GetLock();
};

namespace ObjectAccessor
{
    // these functions return objects only if in map of specified object
    WorldObject* GetWorldObject(WorldObject const&, ObjectGuid const guid);
    Object* GetObjectByTypeMask(WorldObject const&, ObjectGuid const guid, uint32 typemask);
    Corpse* GetCorpse(WorldObject const& u, ObjectGuid const guid);
    GameObject* GetGameObject(WorldObject const& u, ObjectGuid const guid);
    Transport* GetTransport(WorldObject const& u, ObjectGuid const guid);
    DynamicObject* GetDynamicObject(WorldObject const& u, ObjectGuid const guid);
    Unit* GetUnit(WorldObject const&, ObjectGuid const guid);
    Creature* GetCreature(WorldObject const& u, ObjectGuid const guid);
    Pet* GetPet(WorldObject const&, ObjectGuid const guid);
    Player* GetPlayer(Map const*, ObjectGuid const guid);
    Player* GetPlayer(WorldObject const&, ObjectGuid const guid);
    Creature* GetCreatureOrPetOrVehicle(WorldObject const&, ObjectGuid const);

    // these functions return objects if found in whole world
    // ACCESS LIKE THAT IS NOT THREAD SAFE
    Player* FindPlayer(ObjectGuid const guid);
    Player* FindConnectedPlayer(ObjectGuid const guid);
    Player* FindPlayerByName(std::string const& name, bool checkInWorld = true);
    Creature* GetSpawnedCreatureByDBGUID(uint32 mapId, uint64 guid);
    GameObject* GetSpawnedGameObjectByDBGUID(uint32 mapId, uint64 guid);

    // when using this, you must use the hashmapholder's lock
    HashMapHolder<Player>::MapType const& GetPlayers();

    template<class T>
    void AddObject(T* object)
    {
        HashMapHolder<T>::Insert(object);
    }

    template<class T>
    void RemoveObject(T* object)
    {
        HashMapHolder<T>::Remove(object);
    }

    void SaveAllPlayers();

    template<>
    void AddObject(Player* player);

    template<>
    void RemoveObject(Player* player);

    void UpdatePlayerNameMapReference(std::string oldname, Player* player);
}

#endif
