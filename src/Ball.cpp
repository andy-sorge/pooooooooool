#include "../include/Ball.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <iostream>
#include <memory>
#include <string>

Ball::Ball(Vector position, Vector velocity, int8_t ball_number) :
    sf::Sprite(resolveTexture(ball_number))
{
    number = ball_number;  // assign FIRST

    // std::string fname("graphics/balls/");
    // fname += std::to_string(number);  // now safe
    // fname += ".png";

    // this->tex_.loadFromFile(fname);
    // this->setTexture(this->tex_);  // keeps tex_ alive (it's a member)

    pos = position;
    vel = velocity;
    radius = BALL_RADIUS;
    mass = 1;

    if      (number == 0)                 { type = BallType::Cue;    }
    else if (number >= 1 && number <= 7)  { type = BallType::Solid;  }  // also fixed: was >= 0
    else if (number == 8)                 { type = BallType::Eight;  }
    else if (number >= 9 && number <= 15) { type = BallType::Stripe; }

    this->setCenter(pos.sf());
}

void Ball::hit(Ball& rhs) {
    // so we get the normal from the centers of our balls
    // then
    // Vector dmomentum = vel * mass - rhs.vel * rhs.mass; // delta momentum
    // Vector normal = (pos - rhs.pos).normalized();
    // Vector force = normal * dmomentum.dot(normal);
    // rhs.vel += force / rhs.mass;
    // this->vel -= force / mass;
    // I think I fundamentally misunderstand collision physics

    // Formula: |rhs.vel| = (2*|this->vel| * (this->vel.norm() dot (rhs.pos-this->pos))) / (m2/m1 + 1)
    if (this->vel.magnitude() == 0.0 && rhs.vel.magnitude() == 0.0) return;
    
    // goofy bodge fix
    this->pos -= (this->pos - rhs.pos).normalized() * ((rhs.pos - this->pos).magnitude() - this->radius - rhs.radius);

    Vector V0 = this->vel - rhs.vel; // transforming rhs to be stationary
    Vector dpos = rhs.pos - this->pos; // change in position
    Vector VB = dpos.normalized() *
                (2 * V0.magnitude() * (V0.normalized().dot(dpos.normalized())))
                / (rhs.mass / this->mass + 1);
    Vector VA = VB * -(this->mass / rhs.mass);

    // std::cout << "VA: " << VA.x << ' ' << VA.y << std::endl;
    // std::cout << "VB: " << VB.x << ' ' << VB.y << std::endl;
    // std::cout << "V1: " << this->vel.x << ' ' << this->vel.y << std::endl;
    // std::cout << "V2: " << rhs.vel.x << ' ' << rhs.vel.y << std::endl;
    rhs.vel = VB + rhs.vel;
    this->vel = VA + this->vel;
}
Vector Ball::friction() const {
    return vel.normalized() * mass * 200;
}

void Ball::tick_physics(double dt) { // dt in seconds
    Vector dpos = {0,0};
    if (vel.magnitude() > (friction() * dt).magnitude()) {
        dpos = vel * dt - friction() * (dt*dt/2);
        vel -= friction() * dt;
        // vel += Vector{0.0, 10000.0} * dt; // gravity mode :>
    }
    else vel = {0,0};
    this->pos += dpos;

    // if (vel.magnitude() < 0.01) std::cout << dpos.x << ' ' << dpos.y << std::endl;
    // std:: cout << "vel " << vel.x << ' ' << vel.y << std::endl;
    // std:: cout << "dt " << dt << std::endl;

    setCenter(pos.sf());
}

Ball::~Ball() = default;

void Ball::setCenter(sf::Vector2f pos) {
    this->setPosition({pos.x - this->getLocalBounds().size.x / 2, pos.y - this->getLocalBounds().size.y / 2});
}

void create_arranged_balls(std::vector<Ball>& balls) {
    // the *spec* is to have the eight ball in a particular position, and for
    // the back corners to each be one of solids and one of stripes
    // then the rest of the balls random
    // obv they're hardcoded rn but we can change that
    balls.emplace_back(Vector{800,491.0}, Vector{0.0,0.0}, 1);

    balls.emplace_back(Vector{800 + BALL_RADIUS*0.866*2,491 - BALL_RADIUS}, Vector{0.0,0.0}, 2);
    balls.emplace_back(Vector{800 + BALL_RADIUS*0.866*2,491 + BALL_RADIUS}, Vector{0.0,0.0}, 11);

    balls.emplace_back(Vector{800 + BALL_RADIUS*0.866*4,491 - BALL_RADIUS*2}, Vector{0.0,0.0}, 4);
    balls.emplace_back(Vector{800 + BALL_RADIUS*0.866*4,491}, Vector{0.0,0.0}, 8);
    balls.emplace_back(Vector{800 + BALL_RADIUS*0.866*4,491 + BALL_RADIUS*2}, Vector{0.0,0.0}, 6);

    balls.emplace_back(Vector{800 + BALL_RADIUS*0.866*6,491 - BALL_RADIUS*3}, Vector{0.0,0.0}, 7);
    balls.emplace_back(Vector{800 + BALL_RADIUS*0.866*6,491 - BALL_RADIUS*1}, Vector{0.0,0.0}, 5);
    balls.emplace_back(Vector{800 + BALL_RADIUS*0.866*6,491 + BALL_RADIUS*1}, Vector{0.0,0.0}, 9);
    balls.emplace_back(Vector{800 + BALL_RADIUS*0.866*6,491 + BALL_RADIUS*3}, Vector{0.0,0.0}, 10);

    balls.emplace_back(Vector{800 + BALL_RADIUS*0.866*8,491 + BALL_RADIUS*4}, Vector{0.0,0.0}, 3);
    balls.emplace_back(Vector{800 + BALL_RADIUS*0.866*8,491 - BALL_RADIUS*2}, Vector{0.0,0.0}, 12);
    balls.emplace_back(Vector{800 + BALL_RADIUS*0.866*8,491}, Vector{0.0,0.0}, 13);
    balls.emplace_back(Vector{800 + BALL_RADIUS*0.866*8,491 + BALL_RADIUS*2}, Vector{0.0,0.0}, 14);
    balls.emplace_back(Vector{800 + BALL_RADIUS*0.866*8,491 - BALL_RADIUS*4}, Vector{0.0,0.0}, 15);
}
