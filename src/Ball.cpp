#include "../include/Ball.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <iostream>
#include <memory>
#include <string>

#include "../include/Audio.hpp"
Ball::Ball(Vector position, Vector velocity, int8_t ball_number, float scale) :
    sf::Sprite(resolveTexture(ball_number))
{
    this->setOrigin({ 96, 96 }); // center the balls position
    number = ball_number;  // assign FIRST

    // std::string fname("graphics/balls/");
    // fname += std::to_string(number);  // now safe
    // fname += ".png";

    // this->tex_.loadFromFile(fname);
    // this->setTexture(this->tex_);  // keeps tex_ alive (it's a member)

    this->setScale({ scale, scale });
    pos = position;
    vel = velocity;
    radius = BALL_RADIUS;
    mass = 1;

    if      (number == 0)                 { type = BallType::Cue;    }
    else if (number >= 1 && number <= 7)  { type = BallType::Solid;  }  // also fixed: was >= 0
    else if (number == 8)                 { type = BallType::Eight;  }
    else if (number >= 9 && number <= 15) { type = BallType::Stripe; }

    this->setPosition(pos.sf());
}

void Ball::hit(Ball& rhs, double dt) {
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

    // if (this->vel.magnitude() > 0) this->vel += this->friction() * dt; // give me back the original velocity
    // if (rhs.vel.magnitude() > 0) rhs.vel += rhs.friction() * dt; // give me back the original velocity

    // goofy bodge fix
    // this->pos -= (this->pos - rhs.pos).normalized() * ((rhs.pos - this->pos).magnitude() - this->radius - rhs.radius);

    // Finding T values of intersection
    unit_t dvelx = this->vel.x - rhs.vel.x;
    unit_t dvely = this->vel.y - rhs.vel.y;

    unit_t dposx = this->pos.x - rhs.pos.x;
    unit_t dposy = this->pos.y - rhs.pos.y;

    unit_t a = dvelx*dvelx + dvely*dvely;
    unit_t b = 2*dposx*dvelx + 2*dposy*dvely;
    unit_t c = -(this->radius+rhs.radius)*(this->radius+rhs.radius) + dposx*dposx + dposy*dposy;

    unit_t discriminant = b * b - 4 * a * c;
    unit_t root1 = 0;
    unit_t root2 = 0;
    if (discriminant > 0) {
        root1 = (-b + std::sqrt(discriminant)) / (2 * a);
        root2 = (-b - std::sqrt(discriminant)) / (2 * a);
    }
    else {
        std::cout << "wtf (inside Ball::hit)" << std::endl;
    }

    unit_t time_of_impact = std::min(root1, root2);
    // this & the rest of this part does not account for friction because I'm not that cracked at algebra
    this->pos += this->vel * time_of_impact;
    rhs.pos += rhs.vel * time_of_impact;
    // /Finding T values of intersection


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

    // TODO: I think doing it like this means I don't even need dt
    this->tick_physics(-time_of_impact);
    rhs.tick_physics(-time_of_impact);

    // division split so it's more readable
    float volume = (rhs.mass*VB.dot(VB) + this->mass*VA.dot(VA)) / 50.0 / 1000.0;
    // float volume = VB.magnitude() + VA.magnitude() / 6000.0;
    if (volume > 100.0) volume = 100.0;
    playBallHit(volume);
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

    setPosition(pos.sf());
}

Ball::~Ball() = default;

// void Ball::setCenter(sf::Vector2f pos) {
//     this->setPosition({pos.x - this->getLocalBounds().size.x / 2, pos.y - this->getLocalBounds().size.y / 2});
// }

void create_arranged_balls(std::vector<Ball>& balls, float scale) {
    // the *spec* is to have the eight ball in a particular position, and for
    // the back corners to each be one of solids and one of stripes
    // then the rest of the balls random
    // obv they're hardcoded rn but we can change that
    double space = 1.05;
    balls.emplace_back(Vector{500,335.0}, Vector{0.0,0.0}, 1, scale);

    balls.emplace_back(Vector{500 + BALL_RADIUS*0.866*2*space,335 - BALL_RADIUS*space}, Vector{0.0,0.0}, 2, scale);
    balls.emplace_back(Vector{500 + BALL_RADIUS*0.866*2*space,335 + BALL_RADIUS*space}, Vector{0.0,0.0}, 11, scale);

    balls.emplace_back(Vector{500 + BALL_RADIUS*0.866*4*space,335 - BALL_RADIUS*2*space}, Vector{0.0,0.0}, 4, scale);
    balls.emplace_back(Vector{500 + BALL_RADIUS*0.866*4*space,335}, Vector{0.0,0.0}, 8, scale);
    balls.emplace_back(Vector{500 + BALL_RADIUS*0.866*4*space,335 + BALL_RADIUS*2*space}, Vector{0.0,0.0}, 6, scale);

    balls.emplace_back(Vector{500 + BALL_RADIUS*0.866*6*space,335 - BALL_RADIUS*3*space}, Vector{0.0,0.0}, 7, scale);
    balls.emplace_back(Vector{500 + BALL_RADIUS*0.866*6*space,335 - BALL_RADIUS*1*space}, Vector{0.0,0.0}, 5, scale);
    balls.emplace_back(Vector{500 + BALL_RADIUS*0.866*6*space,335 + BALL_RADIUS*1*space}, Vector{0.0,0.0}, 9, scale);
    balls.emplace_back(Vector{500 + BALL_RADIUS*0.866*6*space,335 + BALL_RADIUS*3*space}, Vector{0.0,0.0}, 10, scale);

    balls.emplace_back(Vector{500 + BALL_RADIUS*0.866*8*space,335 + BALL_RADIUS*4*space}, Vector{0.0,0.0}, 3, scale);
    balls.emplace_back(Vector{500 + BALL_RADIUS*0.866*8*space,335 - BALL_RADIUS*2*space}, Vector{0.0,0.0}, 12, scale);
    balls.emplace_back(Vector{500 + BALL_RADIUS*0.866*8*space,335}, Vector{0.0,0.0}, 13, scale);
    balls.emplace_back(Vector{500 + BALL_RADIUS*0.866*8*space,335 + BALL_RADIUS*2*space}, Vector{0.0,0.0}, 14, scale);
    balls.emplace_back(Vector{500 + BALL_RADIUS*0.866*8*space,335 - BALL_RADIUS*4*space}, Vector{0.0,0.0}, 15, scale);
}
