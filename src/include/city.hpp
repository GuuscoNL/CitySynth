#pragma once
#include <raylib.h>
#include <vector>
#include "roadsegment.hpp"
#include "settings.hpp"
#include "node.hpp"

class City {
public:
    /**
     * @brief Construct a new City object.
     *
     * @param size Size of city.
     * @param settings Pointer to the settings class.
     */
    City(float size, Settings* settings);

    /**
     * @brief Destroy the City object. Including the roads and nodes.
     *
     */
    ~City();

    /**
     * @brief Draw the plane, the roads and the nodes of the city.
     *
     */
    void Draw() const;

    /**
     * @brief Generate a population heatmap based on Simplex noise and apply it to the plane.
     * The settings for the generator are in the Settings class.
     *
     */
    void GeneratePopulationHeatmap();
    
    /**
     * @brief Generate the city based on the settings in the Settings class and the population heatmap.
     * 
     * @param segmentLimit The amount of roads to generate.
     */
    void GenerateCity(unsigned int segmentLimit);

    /**
     * @brief Delete all roads an nodes.
     * 
     */
    void ResetCity();

    /**
     * @brief Set the new size of the city. This will also update the heatmap.
     * 
     * @param size The new size
     */
    void SetSize(int size);

    /**
     * @brief Set the new Heatmap the city is based on. The city will *not* regenerate.
     *  The heatmap must be grayscale.
     * 
     * @param heatmapImage The Image of the new heatmap
     */
    void SetHeatmap(Image heatmapImage);

    /**
     * @brief Get the vector with all the pointers to RoadSegment objects that are in the city.
     * 
     * @return std::vector<RoadSegment*> All the pointers to RoadSegment objects that are in the city.
     */
    std::vector<RoadSegment*> GetRoads() const;

    /**
     * @brief Get the vector with all the pointers to Node objects that are in the city.
     * 
     * @return std::vector<Node*> All the pointers to Node objects that are in the city.
     */
    std::vector<Node*> GetNodes() const;

    /**
     * @brief Get the size of the roads vector. AKA the amount of roads in the city.
     * 
     * @return int The vector size.
     */
    int GetRoadsSize() const;

    /**
     * @brief Get the size of the nodes vector. AKA the amount of Nodes in the city.
     * 
     * @return int The vector size.
     */
    int GetNodesSize() const;

private:
    std::vector<RoadSegment*> roads;
    std::vector<Node*> nodes;
    int size;
    Texture2D populationHeatmapTex;
    Image populationHeatmapImg;
    Vector2 heatmapCenter;
    Model plane;
    Settings* settings;

    /**
     * @brief Update the plane texture based on the populationHeatmapImg
     * 
     */
    void UpdatePlaneTexture();

    /**
     * @brief Check if the RoadSegment collides with other RoadSegments, if so
     * than change it's geometry.
     * 
     * @param road The RoadSegment to test.
     * @return true The RoadSegment is not out of bounds.
     * @return false The RoadSegment is out of bounds.
     */
    bool LocalConstraints(RoadSegment* road);

    /**
     * @brief Decides if there should branch any other RoadSegments from the road.
     * 
     * @param rootRoad The road to check from.
     * @param newRoads Reference to the vector where the new roads should be added to.
     */
    void GlobalGoals(RoadSegment* rootRoad, std::vector<RoadSegment*>& newRoads);

    /**
     * @brief Delete a Node, but only if no roads are connected to it.
     * 
     * @param node The node to remove.
     */
    void DeleteNode(Node* node);
    
    /**
     * @brief Add a SideRoad to the root road.
     * 
     * @param rootRoad The road to add it to.
     * @param angle The angle the new SideRoad should be.
     * @param newRoads Reference to the vector where the new road should be added to.
     */
    void AddSideRoad(RoadSegment* rootRoad, float angle, std::vector<RoadSegment*>& newRoads);

    /**
     * @brief Calculate position based on a given starting position, angle and length.
     * 
     * @param fromPos Starting position.
     * @param angle The angle.
     * @param length The length away from starting position.
     * @return Vector2 The pos based on the angle and length.
     */
    Vector2 CalcPosWithAngle(const Vector2& fromPos, float angle, float length) const;

    /**
     * @brief Calculates set of positions with max angle and returns the position with
     * the highest population.
     * 
     * @param rootRoad The road it is connected to.
     * @param MaxAngle Max angle deviation.
     * @return Vector2 Position with highest population.
     */
    Vector2 HighwaySamples(const RoadSegment* const rootRoad, float MaxAngle) const;

    /**
     * @brief Get the Population From Heatmap.
     * 
     * @param pos The 2D pos
     * @return int the amount op population 0-255
     */
    int GetPopulationFromHeatmap(const Vector2& pos) const;

    /**
     * @brief Check if the two roads collide.
     * 
     * @param road1 
     * @param road2 
     * @param intersection reference to the a Vec2 where the intersection will be stored.
     * @return true Roads collide.
     * @return false Roads do not collide.
     */
    bool RoadsCollide(RoadSegment* road1, RoadSegment* road2, Vector2& intersection) const;

    /**
     * @brief Calculate the Cross product of two vector2Ds
     * 
     * @param v1 vector 1
     * @param v2 vector 2
     * @return float The result
     */
    float CrossProduct(const Vector2& v1, const Vector2& v2) const;

    /**
     * @brief Calculate distance of node to road, result will be the squared distance
     * 
     * @param node The node
     * @param road THe road 
     * @param closestPoint reference to the a Vec2 where the closestPoint will be stored.
     * @return float The squared distance between the node and the road. 
     */
    float DistNodeToRoad(Node* node, RoadSegment* road, Vector2& closestPoint) const;

    /**
     * @brief Add intersection to toSplitRoad and add toAddRoad.
     * 
     * @param toSplitRoad Road where the intersection will be on.
     * @param toAddRoad Road that will connect to that intersection.
     * @param intersectionPos The position of the intersection.
     * @return Node* The intersection node
     */
    Node* AddIntersection(RoadSegment* toSplitRoad, RoadSegment* toAddRoad, const Vector2& intersectionPos);
};