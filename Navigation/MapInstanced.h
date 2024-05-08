#ifndef ACORE_MAP_INSTANCED_H
#define ACORE_MAP_INSTANCED_H

#include "Map.h"

class MapInstanced : public Map
{
    friend class MapMgr;
public:
    using InstancedMaps = std::unordered_map<uint32, Map*>;

    MapInstanced(uint32 id);
    ~MapInstanced() override {}

    // functions overwrite Map versions
    //void RelocationNotify();
    void UnloadAll() override;
    EnterState CannotEnter(Player* player, bool loginCheck = false) override;

    Map* CreateInstanceForPlayer(const uint32 mapId, Player* player);
    Map* FindInstanceMap(uint32 instanceId) const
    {
        InstancedMaps::const_iterator i = m_InstancedMaps.find(instanceId);
        return(i == m_InstancedMaps.end() ? nullptr : i->second);
    }
    bool DestroyInstance(InstancedMaps::iterator& itr);

    InstancedMaps& GetInstancedMaps() { return m_InstancedMaps; }

private:
    InstancedMaps m_InstancedMaps;
};
#endif
