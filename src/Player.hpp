#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <cmath>
#include "Object.hpp"
#include "Text.hpp"
#include "Culbuto.hpp"

#define TORQUE 130
#define JUMP_STRENGTH 7.
#define DEGTORAD (M_PI / 180.)
#define SPEED (600 * DEGTORAD)
#define CONTINUOUS 12.5

#define MIN_NSIZE 40.
#define MAX_NSIZE 110.

class Player : public Object {
private:
    float _width = 0.03 * PH_WIDTH;
    float _height = 0.10* PH_HEIGHT;
    float _legWidth = _width / 2.2;
    float _legHeight = 0.075 * PH_HEIGHT;
    sf::Color _color;
    std::string _name;
    Culbuto _leftLeg, _rightLeg;
    b2RevoluteJoint* rightJoint;
    b2RevoluteJoint* leftJoint;
    bool rightLooking, jumping;
public:
    Player(b2World& world, float initX, sf::Color c, std::string n, bool rightLooking) :
        Object(world),
        _color(c),
        _name(n),
        _leftLeg(world, initX, 2 * PH_HEIGHT / 3, _legWidth, _legHeight, sf::Color::White),
        _rightLeg(world, initX + _width, 2 * PH_HEIGHT / 3, _legWidth, _legHeight,
                  sf::Color::Yellow),
        rightLooking(rightLooking) {
        // Main body
        b2BodyDef bDef;
        bDef.type = b2_dynamicBody;
        bDef.position = b2Vec2(initX, 2 * PH_HEIGHT / 3 - _height);
        _body = _world.CreateBody(&bDef);
        b2PolygonShape shape;
        shape.SetAsBox(_width / 2, _height / 2, b2Vec2(0, -_height / 2.), 0.);
        b2FixtureDef fixDef;
        fixDef.restitution = 0.1;
        fixDef.density = 1.;
        fixDef.friction = 1.;
        fixDef.shape = &shape;
        _body->CreateFixture(&fixDef);
        //Attach right leg
        b2RevoluteJointDef revDef1;
        revDef1.bodyA = _body;
        revDef1.bodyB = _rightLeg._body;
        revDef1.collideConnected = false;
        revDef1.localAnchorA.Set(+_width / 2, 0);
        revDef1.localAnchorB.Set(_legWidth / 2, -_legHeight);
        revDef1.enableLimit = true;
        revDef1.upperAngle = 0;
        revDef1.lowerAngle = rightLooking ? (-90 * DEGTORAD) : 0;
        revDef1.enableMotor = rightLooking;
        revDef1.maxMotorTorque = TORQUE;
        revDef1.motorSpeed = SPEED;
        //Attach left
        b2RevoluteJointDef revDef2;
        revDef2.bodyA = _body;
        revDef2.bodyB = _leftLeg._body;
        revDef2.collideConnected = false;
        revDef2.localAnchorA.Set(-_width / 2, 0);
        revDef2.localAnchorB.Set(-_legWidth / 2, -_legHeight);
        revDef2.enableLimit = true;
        revDef2.lowerAngle = 0;
        revDef2.upperAngle = rightLooking ? 0 : (90 * DEGTORAD);
        revDef2.enableMotor = not rightLooking;
        revDef2.maxMotorTorque = TORQUE;
        revDef2.motorSpeed = -SPEED;
        rightJoint = (b2RevoluteJoint*) world.CreateJoint(&revDef1);
        leftJoint = (b2RevoluteJoint*) world.CreateJoint(&revDef2);

        // Initial rotation
        float angle = (rand() % 21 - 10) / 30.;
        _body->ApplyAngularImpulse(angle, true);
        
        initSprite();
    }

    void initSprite() {
        sf::Vector2f size = sf::Vector2f(_width * RATIO, _height * RATIO);
        _sprite = new sf::RectangleShape(size);
        _sprite->setFillColor(_color);
        _sprite->setOrigin({_width* RATIO / 2.f, _height * RATIO});
    }

    void render(sf::RenderWindow& window) {
        Object::render(window);
        _leftLeg.render(window);
        _rightLeg.render(window);
        auto pos = _body->GetPosition();

        Text::drawText(window, _name,
                       sf::Vector2f(pos.x * RATIO, (pos.y - _height) * RATIO),
					   std::max(MIN_NSIZE, 
						   std::min(MAX_NSIZE, pow(abs(_body->GetLinearVelocity().y) +
											  abs(_body->GetLinearVelocity().x), 2))));
    }

    void update(float step) {
        if (jumping) {
            if (rightLooking)
				rightJoint->SetMotorSpeed(
					SPEED * (rightJoint->GetLowerLimit() - rightJoint->GetJointAngle()));
            else
                leftJoint->SetMotorSpeed(
                    SPEED * (leftJoint->GetUpperLimit() - leftJoint->GetJointAngle()));
        }
        else {
            if (rightLooking)
                rightJoint->SetMotorSpeed(
                    SPEED * (rightJoint->GetUpperLimit() - rightJoint->GetJointAngle()));
            else
                leftJoint->SetMotorSpeed(
                    SPEED * (leftJoint->GetLowerLimit() - leftJoint->GetJointAngle()));
        }
		if(canJump() and not jumping){
			_body->ApplyForce(b2Vec2(0, -CONTINUOUS), _body->GetWorldCenter() + b2Vec2(0, -_height/2), true);
		
			_leftLeg._body->ApplyForce(b2Vec2(0, CONTINUOUS/2), _leftLeg._body->GetWorldCenter() + b2Vec2(0, _legHeight/2), true);
			_rightLeg._body->ApplyForce(b2Vec2(0, CONTINUOUS/2), _rightLeg._body->GetWorldCenter() + b2Vec2(0, _legHeight/2), true);
		}
		_leftLeg.update(step);
		_rightLeg.update(step);
    }
	bool canJump() const {
		return _leftLeg.colliding() or _rightLeg.colliding();
	}
    void jump() {
		jumping = true;
		if(canJump()) {
			float angle = _body->GetAngle();
			float strength = JUMP_STRENGTH;
			float unitX = strength * sin(angle);
			float unitY = strength * -cos(angle);
			_body->ApplyLinearImpulse(b2Vec2(unitX, unitY), _body->GetWorldCenter(), true);
		}
    }
    void unjump() {
        jumping = false;
    }
    sf::Packet& output(sf::Packet& p) const override {
		p << _name;
        Object::output(p);
        _leftLeg.output(p);
        return _rightLeg.output(p);
    }
    sf::Packet& input(sf::Packet& p) override {
		p >> _name;
        Object::input(p);
        _leftLeg.input(p);
        return _rightLeg.input(p);
    }
	void setName(std::string newp) {
		_name = newp;
	}
};

#endif
