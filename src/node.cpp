// Node.cpp

#include "node.h"

// Constructor
Node::Node(double x, double y, double w)
    : xCenter(x), yCenter(y), mass(0.0), massCenterX(0.0), massCenterY(0.0), width(w), body(nullptr) {
    for (int i = 0; i < 4; ++i) {
        children[i] = nullptr;
    }
}

// Destructor
Node::~Node() {
    for (int i = 0; i < 4; ++i) {
        delete children[i];
    }
}

// Getter for children
Node* Node::getChild(int index) const {
    if (index >= 0 && index < 4) {
        return children[index];
    }
    return nullptr;
}

// Setter for children
void Node::setChild(int index, Node* child) {
    if (index >= 0 && index < 4) {
        children[index] = child;
    }
}

bool Node::isWithinBoundary(Body* b) const {
    return b->px >= xCenter - width/2 && b->px <= xCenter + width/2 &&
           b->py >= yCenter - width/2 && b->py <= yCenter + width/2;
}


void Node::insertBody(Body* b) {
    // Check if the body is within the boundary of the node
    if (!isWithinBoundary(b)) {
        return; 
    }
    if (body == nullptr) {
        body = b;
        mass = b->mass;
        massCenterX = b->px;
        massCenterY = b->py;
    } else {
        if (children[0] == nullptr) {
            // Create children
            double halfWidth = width / 2.0;
            children[0] = new Node(xCenter + halfWidth, yCenter + halfWidth, halfWidth);
            children[1] = new Node(xCenter - halfWidth, yCenter + halfWidth, halfWidth);
            children[2] = new Node(xCenter + halfWidth, yCenter - halfWidth, halfWidth);
            children[3] = new Node(xCenter - halfWidth, yCenter - halfWidth, halfWidth);
            // Insert bodies
            children[0]->insertBody(body);
            children[1]->insertBody(body);
            children[2]->insertBody(body);
            children[3]->insertBody(body);
            body = nullptr;
        }
        // Insert body into children
        children[0]->insertBody(b);
        children[1]->insertBody(b);
        children[2]->insertBody(b);
        children[3]->insertBody(b);
        // Update mass and center of mass
        double totalMass = mass + b->mass;
        massCenterX = (massCenterX * mass + b->px * b->mass) / totalMass;
        massCenterY = (massCenterY * mass + b->py * b->mass) / totalMass;
        mass = totalMass;
    }
}

Vec2 Node::calculateForceForBody(Body* targetBody, double theta, double G, double rlimit) const {
    Vec2 force = {0, 0};

    // If this is a leaf node with a body that's not the target
    if (body != nullptr && body != targetBody) {
        force = calculateGravitationalForce(targetBody, this->body,  G, rlimit);
    }
    // If this is an internal node
    else if (body == nullptr) {
        double distance = sqrt(pow(targetBody->px - massCenterX, 2) + pow(targetBody->py - massCenterY, 2));
        double sOverD = width / distance;

        // Check if the node is sufficiently far away to approximate
        if (sOverD < theta) {
            // Create a temporary body to represent the mass of this node
            Body tempNodeMass(mass, massCenterX, massCenterY,0,0);
            force = calculateGravitationalForce(targetBody, &tempNodeMass, G, rlimit);
        } else {
            // Recursively calculate force for each child
            for (auto& child : children) {
                if (child != nullptr) {
                    force += child->calculateForceForBody(targetBody, theta, G, rlimit);
                }
            }
        }
    }

    return force;
}

Vec2 Node::calculateGravitationalForce(const Body* a, const Body* b, double G, double rlimit) {
    if (a->mass == -1 || b->mass == -1) return Vec2(0,0); // Ignore lost bodies
    double dx = a->px - b->px;
    double dy = a->py - b->py;
    double distance = sqrt(dx*dx + dy*dy);
    if (distance == 0) return Vec2(0,0); // Ignore overlap

    double distanceCubed = (distance < rlimit) ? (distance*rlimit*rlimit) : (distance*distance*distance);

    double force = (G *  a->mass *  b->mass) / distanceCubed;
    return Vec2(force * dx, force * dy);
}
