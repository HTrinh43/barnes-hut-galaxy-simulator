// Body.h
#include <stdio.h>

#ifndef BODY_H
#define BODY_H

class Body {
public:
    double mass;
    double px, py;     // Position
    double vx, vy;     // Velocity
    double fx, fy;     // Force
    int ind;
    // Constructor
    Body() : mass(0), px(0), py(0), vx(0), vy(0), fx(0), fy(0), ind(0) {}
    Body(double m, double x, double y, double vx, double vy);

    // Methods
    void resetForce();
    void addForce(const Body& other, double G, double rlimit);
    void update(double dt);
    // Vec2 calculateForce(const Body& other, double G, double rlimit) const;
    void print();
};

struct Vec2 {
    double x, y;

    // Constructor
    Vec2(double x = 0, double y = 0) : x(x), y(y) {}

    // Overload the + operator for vector addition
    Vec2 operator+(const Vec2& other) const {
        return Vec2(x + other.x, y + other.y);
    }

    // Overload the - operator for vector subtraction
    Vec2 operator-(const Vec2& other) const {
        return Vec2(x - other.x, y - other.y);
    }

    // Optionally, you can also overload += and -= operators for compound assignment
    Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vec2& operator-=(const Vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
};



#endif // BODY_H
