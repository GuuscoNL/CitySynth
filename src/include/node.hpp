#pragma once
#include "raylib.h"
#include <vector>
#include "settings.hpp"

class RoadSegment;

class Node {
public:
    /**
     * @brief Construct a new Node object.
     * 
     * @param pos2D Position of the node.
     * @param settings Pointer the Settings class.
     */
    Node(const Vector2& pos2D, Settings* settings);

    /**
     * @brief Draw Node.
     * 
     */
    void Draw() const;

    /**
     * @brief Get the position of the Node.
     * 
     * @return Vector2 The Position
     */
    Vector2 GetPos() const;

    /**
     * @brief Get the amount of connected Roads to this Node.
     * 
     * @return int Amount of connected roads.
     */
    int GetAmountConnectedRoads() const;

    /**
     * @brief Add road to this Node.
     * 
     * @param road Road to add
     */
    void AddRoad(RoadSegment* const road);

    /**
     * @brief Remove road from this Node.
     * 
     * @param road Road to remove.
     */
    void RemoveRoad(const RoadSegment* const road);

    /**
     * @brief Get the Connected Roads
     * 
     * @return std::vector<RoadSegment*> vector with pointers to connected RoadSegment objects.
     */
    std::vector<RoadSegment*> GetConnectedRoads() const;

    /**
     * @brief Get the Id of this Node.
     * 
     * @return int The Id
     */
    int GetId() const;

    /**
     * @brief Reset nextId to 0
     * 
     */
    void ResetId();

    Color color;

private:
    int id;
    static int nextId;
    std::vector<RoadSegment*> connectedRoads;
    Vector2 pos2D;
    Vector3 pos3D;
    Settings* settings;
    Model model;
};