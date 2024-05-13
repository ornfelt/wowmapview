#include "NodeManager.h"
#include <iostream> // For std::cerr and other I/O operations
#include <cstdlib>
#include <cmath>    // For sqrt and pow
#include <algorithm>// For std::min_element and std::find_if

#include <random>   // For std::random_device, std::mt19937, and std::uniform_int_distribution
#include <stdexcept>// For std::runtime_error

NodeManager* NodeManager::instance = nullptr;

NodeManager::NodeManager() {
    mysql_library_init(0, nullptr, nullptr);
}

NodeManager::~NodeManager() {
    mysql_library_end();
}

NodeManager* NodeManager::getInstance() {
    if (instance == nullptr) {
        instance = new NodeManager();
    }
    return instance;
}

MYSQL* connectDatabase() {
    MYSQL* conn = mysql_init(nullptr);
    if (!mysql_real_connect(conn, "localhost", "trinity", "trinity", "world", 3306, nullptr, 0)) {
        std::cerr << "Connection error: " << mysql_error(conn) << std::endl;
        return nullptr;
    }
    return conn;
}

void NodeManager::loadNodes() {
    MYSQL* conn = connectDatabase();
    if (!conn) return;

    const char* query = "SELECT id, mapid, x, y, z, links FROM creature_template_npcbot_wander_nodes;";
    if (mysql_query(conn, query)) {
        std::cerr << "MySQL query error: " << mysql_error(conn) << std::endl;
        mysql_close(conn);
        return;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result)) != nullptr) {
        Node node;
        node.id = atoi(row[0]);
        node.mapId = atoi(row[1]);
        node.x = atof(row[2]);
        node.y = atof(row[3]);
        node.z = atof(row[4]);
        node.links = row[5] ? row[5] : "";
        nodes.push_back(node);
    }

    mysql_free_result(result);
    mysql_close(conn);
}

Node NodeManager::getClosestNode(double x, double y, double z) {
    if (nodes.empty()) {
        throw std::runtime_error("Nodes are not loaded yet.");
    }
    return *std::min_element(nodes.begin(), nodes.end(), [x, y, z](const Node& a, const Node& b) {
        double distA = sqrt(pow(a.x - x, 2) + pow(a.y - y, 2) + pow(a.z - z, 2));
        double distB = sqrt(pow(b.x - x, 2) + pow(b.y - y, 2) + pow(b.z - z, 2));
        return distA < distB;
    });
}

Node NodeManager::getClosestNode(uint32_t mapId, double x, double y, double z) {
    std::vector<Node> filteredNodes;
    std::copy_if(nodes.begin(), nodes.end(), std::back_inserter(filteredNodes), [mapId](const Node& node) {
        return node.mapId == mapId;
    });

    if (filteredNodes.empty()) {
        throw std::runtime_error("No nodes loaded for the specified mapId or nodes are not loaded yet.");
    }

    return *std::min_element(filteredNodes.begin(), filteredNodes.end(), [x, y, z](const Node& a, const Node& b) {
        double distA = sqrt(pow(a.x - x, 2) + pow(a.y - y, 2) + pow(a.z - z, 2));
        double distB = sqrt(pow(b.x - x, 2) + pow(b.y - y, 2) + pow(b.z - z, 2));
        return distA < distB;
    });
}

Node* NodeManager::getRandomLinkedNode(int nodeId) {
    auto it = std::find_if(nodes.begin(), nodes.end(), [nodeId](const Node& node) {
        return node.id == nodeId;
    });

    if (it == nodes.end() || it->links.empty()) {
        std::cerr << "Node not found or no links available" << std::endl;
        return nullptr;
    }

    std::vector<std::string> links;
    std::istringstream iss(it->links);
    std::string link;
    while (std::getline(iss, link, ' ')) {
        links.push_back(link);
    }

    if (links.empty()) {
        std::cerr << "No valid links found" << std::endl;
        return nullptr;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, links.size() - 1);
    int randomIndex = dis(gen);
    int linkedId = std::stoi(links[randomIndex]);

    return &(*std::find_if(nodes.begin(), nodes.end(), [linkedId](const Node& node) {
        return node.id == linkedId;
    }));
}
