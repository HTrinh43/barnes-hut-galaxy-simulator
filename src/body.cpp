// body.cpp
#include <iostream>
#include "body.h"
#include <cmath>

Body::Body(double m, double x, double y, double vx, double vy)
    : mass(m), px(x), py(y), vx(vx), vy(vy), fx(0), fy(0) {}


void Body::resetForce() {
    fx = 0;
    fy = 0;
}

void Body::addForce(const Body& other, double G, double rlimit) {
    if (this->mass == -1 || other.mass == -1) return; // Ignore lost bodies
    double dx = other.px - px;
    double dy = other.py - py;
    double distance = sqrt(dx*dx + dy*dy);
    // if (distance == 0) return; // Ignore overlap

    double distanceCubed = (distance < rlimit) ? (distance*rlimit*rlimit) : (distance*distance*distance);

    double force = (G * mass * other.mass) / distanceCubed;
    fx += force * dx;
    fy += force * dy;
}

// Vec2 Body::calculateForce(const Body& other, double G, double rlimit) {
//     if (this->mass == -1 || other.mass == -1) return; // Ignore lost bodies
//     double dx = other.px - px;
//     double dy = other.py - py;
//     double distance = sqrt(dx*dx + dy*dy);
//     if (distance == 0) return; // Ignore overlap

//     double distanceCubed = (distance < rlimit) ? (rlimit*rlimit*rlimit) : (distance*distance*distance);

//     double force = (G * mass * other.mass) / distanceCubed;
//     fx += force * dx;
//     fy += force * dy;
//     return {fx, fy};
// }

void Body::update(double dt) {
    double ax = fx / mass;
    double ay = fy / mass;

    px += vx*dt + 0.5*ax*dt*dt;
    py += vy*dt + 0.5*ay*dt*dt;
    vx += ax*dt;
    vy += ay*dt;
}

void Body::print(){
        std::cout << ind << " " << px << " " << py << " " << mass << " " << vx << " " << vy << "\n";
    }