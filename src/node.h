// Node.h

#ifndef NODE_H
#define NODE_H

#include "body.h"  // Include the Body class
#include <cmath>
class Node {
private:
    Node *children[4];  // Pointers to child nodes (NE, NW, SE, SW)

public:
    double xCenter;     // X coordinate of the center of the region
    double yCenter;     // Y coordinate of the center of the region
    double mass;        // Total mass of the node
    double massCenterX; // X coordinate of the center of mass
    double massCenterY; // Y coordinate of the center of mass
    double width;       // Width of the region
    Body *body;         // Pointer to a body if the node contains exactly one body

    // Constructor
    Node(double x, double y, double w);

    // Destructor to deallocate child nodes
    ~Node() ;

    Node *getChild(int index) const;

    // Setter for children
    void setChild(int index, Node* child);

    void insertBody(Body* b);

    bool isWithinBoundary(Body* b) const;

    Vec2 calculateForceForBody(Body* targetBody, double theta, double G, double rlimit) const;

    static Vec2 calculateGravitationalForce(const Body* a, const Body* b, double G, double rlimit);
};

#endif // NODE_H
