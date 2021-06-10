
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <cmath>
#define _USE_MATH_DEFINES

using namespace std;

//Auxiliary objects
#include "field/field.hpp"
#include "server/server.hpp"
#include "robot/robot.hpp"

//Box2D
#include "../box2d/include/box2d/box2d.h"


void main(){

    //Scale factors for box2d objects to fit between 0.1 and 10 m
    float scale_factor = 5;

    //Create box2d world
    b2Vec2 gravity(0.0f, 0.0f);
    b2World world(gravity);

    //Field declaration
    Field field = Field(1.5f * scale_factor, 1.3f * scale_factor, 40.f/130.f, 10.f/150.f, 
                    7.f/130.f, 70.f/130.f, 15.f/150.f, 40.f/130.f, 20.f/130.f, 5.f/150.f);
    
    //Create chain of points that represent the field's borders
    const int field_points = 16;
    assert(field_points == field.box2D_borders.size());
    b2Vec2 vs[field_points];
    for(int i = 0; i < field.box2D_borders.size(); ++i){
        vs[i].Set(field.box2D_borders[i].first, field.box2D_borders[i].second);
    }
    b2ChainShape chain;
    chain.CreateLoop(vs, field_points);

    b2BodyDef groundBodyDef;
    groundBodyDef.type = b2_staticBody;
    groundBodyDef.position.Set(0.0f, 0.0f);

    b2Body* borders = world.CreateBody(&groundBodyDef);

    b2FixtureDef groundFixtureDef;
    groundFixtureDef.shape = &chain;
    //groundFixtureDef.density = 10.0f;
    groundFixtureDef.friction = 0.0f;
    groundFixtureDef.restitution = 1.0f;
    borders->CreateFixture(&groundFixtureDef);

    /*
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0.0f, -10.0f);
    
    b2Body* groundBody = world.CreateBody(&groundBodyDef);
    b2PolygonShape groundBox;
    groundBox.SetAsBox(50.0f, 10.0f);

    groundBody->CreateFixture(&groundBox, 0.0f);
    */

    //Ball Shape
    b2CircleShape circle;
    //circle.m_p.Set(0.0f, 0.0f);
    circle.m_radius = 0.02135f * scale_factor;

    //Ball dynamic body
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;

    bodyDef.position.Set(0.0f, 0.0f); // the body's origin position.
    bodyDef.angle = 0.0f; // the body's angle in radians.
    bodyDef.fixedRotation = true;
    bodyDef.linearDamping = 0.1f;
    //bodyDef.angularDamping = 0.01f;

    b2Body* body = world.CreateBody(&bodyDef);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;
    fixtureDef.density = 0.25f;
    fixtureDef.friction = 0.0f;
    fixtureDef.restitution = 0.7f;
    fixtureDef.restitutionThreshold = 0.1f;  // Fixes magnetic behavior with slow velocity collisions
    body->CreateFixture(&fixtureDef);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //------------------------------------------------ Robots ---------------------------------------------------------------//
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float side_length           = 0.075f * scale_factor;
    float density               = 5.0f;
    float friction              = 0.1f;
    float restitution           = 0.0f;
    float restitutionThreshold  = 0.2f;
    float linearDamping         = 20.0f;
    float angularDamping        = 20.0f;

    int number_of_robots = 6;

    vector <b2Vec2> position(number_of_robots);
    vector <float>  angle(number_of_robots);
    vector <Robot>  robots(number_of_robots);
    vector <pair<float, float>> forces(number_of_robots);

    position[0].Set(-3.5f, 2.7f);
    angle[0] = 0.f;
    position[1].Set(0.8f, 0.5f);
    angle[1] = 0.f;
    position[2].Set(-0.8f, -0.5f);
    angle[2] = 0.f;
    position[3].Set(0.8f, -0.5f);
    angle[3] = 0.f;
    position[4].Set(1.5f, 0.5f);
    angle[4] = 0.f;
    position[5].Set(-1.5f, 0.5f);
    angle[5] = 0.f;

    for(int i = 0; i < robots.size(); ++i){
        robots[i] = Robot(position[i], angle[i], side_length, density, friction, restitution, 
        restitutionThreshold, linearDamping, angularDamping, &world);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //------------------------------------------------ Server ---------------------------------------------------------------//
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float timeStep = 1.0f / 60.0f;
    /*
    int32 velocityIterations = 6;
    int32 positionIterations = 2;
    */
    int32 velocityIterations = 12;
    int32 positionIterations = 10;

    //Vector for ki
    vector <float> sum_w_l(number_of_robots);
    vector <float> sum_w_r(number_of_robots);
    float max_acceleration = 3.0f; //3 m/s^2
    float robots_mass = robots[0].get_body_ptr()->GetMass();
    float max_motor_torque = robots_mass * max_acceleration / 2.0f * 20.0f;

    printf("robot mass: %.2f\n", robots_mass);
    printf("max motor torque: %.2f\n", max_motor_torque);

    char msg[DEFAULT_BUFLEN];
    char aux[DEFAULT_BUFLEN];
    Server server = Server();
    if(server.setup_server()){
        printf("Server setup failed...");
        return;
    }
    // Receive until the peer shuts down the connection
    do {
        
        server.read_message();
        if (server.get_send_result() > 0) {
            memset(aux, 0, DEFAULT_BUFLEN);
            //printf("Bytes received: %d\n", server.get_send_result());

            char *data = server.get_message_data();
            //printf("data received: %s\n", data);
            if(data[0] == 's'){
               // printf("Sending world state...\n");

                b2Vec2 position = body->GetPosition();
                float angle = body->GetAngle();

                msg[0] = '\0';
                //Ball position
                sprintf_s(aux,"%.2f %.2f ", position.x, position.y);
                strcat_s(msg, aux);

                //Robots positions and angles
                for(int i = 0; i < robots.size(); ++i){
                    b2Body* robotBody = robots[i].get_body_ptr();
                    b2Vec2 position   = robotBody->GetPosition();
                    float angle       = robotBody->GetAngle();
                    sprintf_s(aux,"%.2f %.2f %.2f ", position.x, position.y, angle);
                    strcat_s(msg, aux);
                }
                strcat_s(msg, "\n");
                //printf(msg);
                server.send_message(msg);
                //printf("Bytes sent: %d\n", server.get_send_result());

                b2Body* robotBody;
                float robotAngle, motorAngle;

                //controller variables
                float wheel_radius = 0.1f;
                float length_between_wheels = 0.2f;

                //Apply forces
                for (int i = 0; i < robots.size(); ++i){
                    robotBody  = robots[i].get_body_ptr();
                    robotAngle = robotBody->GetAngle();
                    motorAngle = robotAngle + (float)M_PI / 2.f;

                    //Controller 
                    b2Vec2 linear_velocity = robotBody->GetLinearVelocity();
                    float angular_velocity = robotBody->GetAngularVelocity();

                    //Vector projection
                    b2Vec2 robotOrientation = b2Vec2(cos(robotAngle), sin(robotAngle));
                    float scalar = (robotOrientation.x * linear_velocity.x + robotOrientation.y * linear_velocity.y) / robotOrientation.LengthSquared();
                    b2Vec2 longitudinal_velocity = scalar * robotOrientation;
                    float long_vel_magn = longitudinal_velocity.Length();

                    float w_l  = (2.0f * long_vel_magn - angular_velocity * length_between_wheels) / (2.0f * wheel_radius); 
                    float w_r  = (2.0f * long_vel_magn + angular_velocity * length_between_wheels) / (2.0f * wheel_radius); 

                    //Desired velocities
                    float w_l_d  = (2.0f * forces[i].first - forces[i].second * length_between_wheels) / (2.0f * wheel_radius); 
                    float w_r_d  = (2.0f * forces[i].first + forces[i].second * length_between_wheels) / (2.0f * wheel_radius);

                    float diff_w_l = w_l_d - w_l;
                    float diff_w_r = w_r_d - w_r;


                    sum_w_l[i] += diff_w_l;
                    sum_w_r[i] += diff_w_r;

                    sum_w_l[i] = min(max(sum_w_l[i], -1000.0f), 1000.0f);
                    sum_w_r[i] = min(max(sum_w_r[i], -1000.0f), 1000.0f);

                    //float Kp = 1.2f;
                    //float Ki = 0.01f;
                    float Kp = 2.5f;
                    float Ki = 0.5f;
                    float L = Kp * diff_w_l + Ki * sum_w_l[i];
                    float R = Kp * diff_w_r + Ki * sum_w_r[i];
                    //Limit max torque
                    L = min(max(L, -max_motor_torque), max_motor_torque);
                    R = min(max(R, -max_motor_torque), max_motor_torque);
                    if(i == 0){
                        printf("[diff] wl: %.2f wr: %.2f\n", diff_w_l, diff_w_r);
                        printf("[diff] L: %.2f R: %.2f\n", L, R);
                        printf("linear velocity: %.2f %.2f\n", linear_velocity.x, linear_velocity.y);
                        printf("angular velocity: %.2f\n", angular_velocity);
                        printf("angle: %.2f\n", robotAngle);
                        printf("orientation: %.2f %.2f\n", robotOrientation.x, robotOrientation.y);
                        printf("longitudinal velocity: %.2f %.2f\n", longitudinal_velocity.x, longitudinal_velocity.y);
                        printf("long vel magnitud: %.4f\n", long_vel_magn);
                    }

                    //printf("[current] wl: %.2f wr: %.2f\n", w_l, w_r);
                    //printf("[desired] wl: %.2f wr: %.2f\n", w_l_d, w_r_d);
                    
                    //printf("angle: %.2f\n", robotAngle);
                    //printf("orientation: %.2f %.2f\n", robotOrientation.x, robotOrientation.y);
                    //printf("longitudinal velocity: %.2f %.2f\n", longitudinal_velocity.x, longitudinal_velocity.y);


                    b2Vec2 leftMotorPos  = robotBody->GetWorldPoint(robots[i].get_left_motor_position());  //Get left  motor's position in world coord
                    b2Vec2 rightMotorPos = robotBody->GetWorldPoint(robots[i].get_right_motor_position()); //Get right motor's position in world coord

                    b2Vec2 leftMotorForce (L * cos(robotAngle), L * sin(robotAngle));
                    b2Vec2 rightMotorForce(R * cos(robotAngle), R * sin(robotAngle));

                    robotBody->ApplyForce(leftMotorForce,  leftMotorPos,  true);
                    robotBody->ApplyForce(rightMotorForce, rightMotorPos, true);
                }

                //Simulate 1 step
                world.Step(timeStep, velocityIterations, positionIterations);

                //Debug
                /*
                for(int i = 0; i < robots.size(); ++i){
                    b2Vec2 position = robots[i].get_position();
                    float angle     = robots[i].get_angle();
                    printf("\trobot %d:\n", i);
                    printf("\t\tposition: %.4f %.4f\n", position.x, position.y);
                    printf("\t\tangle: %.4f\n", angle);
                }
                */


            } else if(data[0] == 'f'){
                printf("Sending field description...\n");
                msg[0] = '\0';
                for(int i = 0; i < field.box2D_borders.size(); ++i){
                    sprintf_s(aux,"%.2f %.2f ", field.box2D_borders[i].first, field.box2D_borders[i].second);
                    strcat_s(msg, aux);
                } 
                for(int i = 0; i < field.main_area.size(); ++i){
                    sprintf_s(aux,"%.2f %.2f ", field.main_area[i].first, field.main_area[i].second);
                    strcat_s(msg, aux);
                } 
                for(int i = 0; i < field.small_area_left.size(); ++i){
                    sprintf_s(aux,"%.2f %.2f ", field.small_area_left[i].first, field.small_area_left[i].second);
                    strcat_s(msg, aux);
                } 
                for(int i = 0; i < field.small_area_right.size(); ++i){
                    sprintf_s(aux,"%.2f %.2f ", field.small_area_right[i].first, field.small_area_right[i].second);
                    strcat_s(msg, aux);
                } 
                for(int i = 0; i < field.mid_line.size(); ++i){
                    sprintf_s(aux,"%.2f %.2f ", field.mid_line[i].first, field.mid_line[i].second);
                    strcat_s(msg, aux);
                } 
                //Mid circle
                sprintf_s(aux,"%.2f %.2f %.2f ", get<0>(field.mid_circle), get<1>(field.mid_circle), get<2>(field.mid_circle));
                strcat_s(msg, aux);
                //Small circle left
                sprintf_s(aux,"%.2f %.2f %.2f ", get<0>(field.small_circle_left), get<1>(field.small_circle_left), get<2>(field.small_circle_left));
                strcat_s(msg, aux);
                //Small circle right
                sprintf_s(aux,"%.2f %.2f %.2f ", get<0>(field.small_circle_right), get<1>(field.small_circle_right), get<2>(field.small_circle_right));
                strcat_s(msg, aux);

                strcat_s(msg, "\n");
                printf(msg); 
                server.send_message(msg);
                printf("Bytes sent: %d\n", server.get_send_result());
            } else if(data[0] == 'b'){
                printf("Sending ball description...\n");
                sprintf_s(msg,"%.4f\n", circle.m_radius);
                printf(msg);
                server.send_message(msg);
                printf("Bytes sent: %d\n", server.get_send_result());
            } else if(data[0] == 'r'){
                printf("Sending robot definition...\n");
                sprintf_s(msg,"%.4f %zd\n", side_length, robots.size());
                printf(msg);
                server.send_message(msg);
                printf("Bytes sent: %d\n", server.get_send_result());


            } else if (data[0] == 'a') {
                printf("\n");
                printf("Recieving forces definition...\n");
                memcpy(aux, &data[2], strlen(data)-2);
                float force_m1;
                float force_m2;
                int idx;
                printf("aux: %s\n", aux);
                sscanf_s(aux, "%f %f %d", &force_m1, &force_m2, &idx);
                //printf("\nForce 1: %.2f Force 2: %.2f idx: %d\n\n", force_m1, force_m2, idx);
                forces[idx] = {force_m1, force_m2};
               // msg[0] = 'K';
               // msg[1] = '\n';
               // msg[2] = '\0';
               // printf(msg);
               // server.send_message(msg);
               // printf("Bytes sent: %d\n", server.get_send_result());
            }
        }
        else if (server.get_send_result() == 0)
            printf("Connection closing...\n");
        else  {
            server.close_socket_with_error();
            return;
        }
    } while (server.get_send_result() > 0);

    if(server.shutdown_server()){
        printf("Server shutdown failed...");
    }
    return;
}