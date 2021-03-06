#ifndef ROBOT_H
#define ROBOT_H

#include <utility>
#include "../../box2d/include/box2d/box2d.h"

class Robot{
    private:
        b2BodyDef bodyDef;
        b2Body*   body;
        b2PolygonShape dynamicBox;
        b2FixtureDef   fixtureDef;
        b2Vec2 leftMotorPos, rightMotorPos;
    public:
        Robot();
        Robot(const b2Vec2 &position, const float &angle, const float &size, const float &density, const float &friction,
        const float &restitution, const float &restitutionThreshold, const float &linearDamping, const float &angularDamping, b2World* world);
        b2Body* get_body_ptr();
        b2PolygonShape get_shape();
        b2Vec2 get_left_motor_position();
        b2Vec2 get_right_motor_position();
};

#endif