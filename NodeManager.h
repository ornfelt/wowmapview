#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include <vector>
#include <string>
#include <sstream>  // For std::istringstream
//#include <winsock.h>
#include <mysql.h>
#include <set>
#include <cstdint>

class Node {
public:
    int id;
    int mapId;
    double x, y, z;
    std::string links;

	std::string toString() const {
        std::ostringstream oss;
        oss << "Node ID: " << id
            << ", Map ID: " << mapId
            << ", Coordinates: (" << x << ", " << y << ", " << "z)"
            << ", Links: " << links;
        return oss.str();
    }
};

class NodeManager {
private:
    static NodeManager* instance;
    std::set<int> visitedNodes;
    NodeManager();
    ~NodeManager();

public:
    std::vector<Node> nodes;
    static NodeManager* getInstance();
    void loadNodes();
    Node getClosestNode(double x, double y, double z);
    Node getClosestNode(uint32_t mapId, double x, double y, double z);
    Node getRandomNode(uint32_t mapId);
    Node* getRandomLinkedNode(int nodeId);
    Node* getRandomLinkedNodeOld(int nodeId);
};

#endif // NODEMANAGER_H
