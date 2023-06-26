#pragma once
#include <raylib.h>
#include <vector>
#include <iostream>
#include "settings.hpp"
#include "Node.hpp"

class RoadSegment {
public:
    enum RoadType { HIGHWAY, SIDEROAD };

    /**
     * @brief Construct a new Road Segment object.
     * 
     * @param delay The delay.
     * @param settings Pointer to Settings.
     * @param from The Node where this RoadSegment starts.
     * @param to The Node where this RoadSegment ends.
     */
    RoadSegment(int delay, Settings* settings, Node* from, Node* to);

    /**
     * @brief Destroy the Road Segment object and remove this road from the `from` and `to` nodes.
     * 
     */
    virtual ~RoadSegment();

    /**
     * @brief Draw this road.
     * 
     */
    void Draw() const;

    /**
     * @brief Get the Delay of this road.
     * 
     * @return unsigned int The delay.
     */
    unsigned int GetDelay() const;

    /**
     * @brief Get the `from` Node of this road.
     * 
     * @return Node* The `from` Node.
     */
    Node* GetFrom() const;

    /**
     * @brief Get the `to` Node of this road.
     * 
     * @return Node* The `to` Node.
     */
    Node* GetTo() const;

    /**
     * @brief Get the position of the `to` Node of this road.
     * 
     * @return Vector2 The position of the `to` Node.
     */
    Vector2 GetToPos() const;

    /**
     * @brief Get the position of the `from` Node of this road.
     * 
     * @return Vector2 The position of the `from` Node.
     */
    Vector2 GetFromPos() const;

    /**
     * @brief Get the Angle of this road.
     * 
     * @return float The angle in degrees.
     */
    float GetAngle() const;

    /**
     * @brief Get the Type of this road
     * 
     * @return RoadSegment::RoadType 
     */
    RoadSegment::RoadType GetType() const;

    /**
     * @brief Set the Delay of this road
     * 
     * @param delay The number to set the delay to.
     */
    void SetDelay(int delay);

    /**
     * @brief Set the `from` Node and remove this road from last `from` Node 
     * and add it to the new `from` Node.
     * 
     * @param Node The new `from` Node.
     */
    void SetFrom(Node* node);

    /**
     * @brief Set the `to` Node and remove this road from last `to` Node 
     * and add it to the new `to` Node.
     * 
     * @param Node The new `to` Node.
     */
    void SetTo(Node* node);

    /**
     * @brief Set the Color of this road
     * 
     * @param color The new Color
     */
    void SetColor(Color color);

    /**
     * @brief Print the positions of the `from` Node and the `to` Node. 
     * 
     * @param os iostream
     * @param road RoadSegment
     * @return std::ostream& 
     */
    friend std::ostream& operator<<(std::ostream& os, const RoadSegment& road);

protected:
    Model model;
    unsigned int delay;
    Settings* settings;
    Color color;
    RoadSegment::RoadType type;

    Vector3 pos;
    Node* from;
    Node* to;
    float angle;
    float length;
    float modelLength;
    float height;

    /**
     * @brief Calculate the position and angle based on the `from` Node and the `to` Node.
     * 
     */
    void CalculatePosAndAngle();
};