#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <cmath>
#include "Object.hpp"
#include "Text.hpp"
#include "Culbuto.hpp"

#define DEGTORAD (M_PI / 180.)

class Player : public Object {
private:
	float _width = 0.03 * PH_WIDTH;
	float _height = 0.14 * PH_HEIGHT;
	float _legWidth = 0.01 * PH_WIDTH;
	float _legHeight = 0.07 * PH_HEIGHT;
	sf::Color _color;
	std::string _name;
	Culbuto _leftLeg, _rightLeg;
	b2RevoluteJoint* rightJoint;
	b2RevoluteJoint* leftJoint;
	bool rightLooking;
public:
    Player(b2World& world, float initX, sf::Color c, std::string n, bool rightLooking) :
		Object(world),
		_color(c),
		_name(n),
		_leftLeg(world, initX, 2*PH_HEIGHT/3, _legWidth, _legHeight, sf::Color::White),
		_rightLeg(world, initX+_width, 2*PH_HEIGHT/3, _legWidth, _legHeight, sf::Color::Yellow) {

		// Main body
        b2BodyDef bDef;
        bDef.type = b2_dynamicBody;
        bDef.position = b2Vec2(initX, 2*PH_HEIGHT/3 - _height);
        _body = _world.CreateBody(&bDef);
        b2PolygonShape shape;
        shape.SetAsBox(_width / 2, _height / 2, b2Vec2(0, -_height / 2.), 0.);
        b2FixtureDef fixDef;
		fixDef.restitution = 0.1;
		fixDef.density = 0.01;
		fixDef.friction = 1.;
        fixDef.shape = &shape;
        _body->CreateFixture(&fixDef);

		//Attach right leg
		b2RevoluteJointDef revDef1;
		revDef1.bodyA = _body;
		revDef1.bodyB = _rightLeg._body;
		revDef1.collideConnected = false;
		revDef1.localAnchorA.Set(+_width/2, 0);
		revDef1.localAnchorB.Set(_legWidth/2, -_legHeight);
		revDef1.enableLimit = true;
		revDef1.upperAngle = 0;
		revDef1.lowerAngle = rightLooking ? (-90 * DEGTORAD) : 0;
		revDef1.enableMotor = rightLooking;
		revDef1.maxMotorTorque = 0;
		revDef1.motorSpeed = -3600;

		//Attach left
		b2RevoluteJointDef revDef2;
		revDef2.bodyA = _body;
		revDef2.bodyB = _leftLeg._body;
		revDef2.collideConnected = false;
		revDef2.localAnchorA.Set(-_width/2, 0);
		revDef2.localAnchorB.Set(-_legWidth/2, -_legHeight);
		revDef2.enableLimit = true;
		revDef2.lowerAngle = 0;
		revDef2.upperAngle = rightLooking ? 0 : (90 * DEGTORAD);
		revDef2.enableMotor = not rightLooking;
		revDef2.maxMotorTorque = 0;
		revDef2.motorSpeed = 3600;

		rightJoint = (b2RevoluteJoint*) world.CreateJoint(&revDef1);
		leftJoint = (b2RevoluteJoint*) world.CreateJoint(&revDef2);

		// Initial rotation
		/*
		float angle = (rand() % 21 - 10) / 30.;
		_body->ApplyAngularImpulse(angle, true);
		*/
		initSprite();
    }

    void initSprite() {
        sf::Vector2f size = sf::Vector2f(_width * RATIO, _height * RATIO);
        _sprite = new sf::RectangleShape(size);
        _sprite->setFillColor(_color);
        _sprite->setOrigin({_width * RATIO / 2.f, _height * RATIO});
    }
	void render(sf::RenderWindow& window) {
		Object::render(window);
		_leftLeg.render(window);
		_rightLeg.render(window);
		auto pos = _body->GetPosition();
		Text::drawText(window, _name,
				sf::Vector2f(pos.x * RATIO, (pos.y - _height) * RATIO),
				std::min(500., pow(abs(_body->GetLinearVelocity().y) +
							abs(_body->GetLinearVelocity().x), 2)));
	}
	void jump() {
		float angle = _body->GetAngle();
		float strength = 20;
		float unitX = strength * sin(angle);
		float unitY = strength * -cos(angle);
		//_body->ApplyLinearImpulse(
		//		b2Vec2(unitX, unitY), _body->GetWorldCenter(), true);
		if(rightLooking)
			rightJoint->SetMaxMotorTorque(rightJoint->GetMaxMotorTorque() - 0.5);	
		else
			leftJoint->SetMaxMotorTorque(leftJoint->GetMaxMotorTorque() + 0.5);	
	}
	sf::Packet& output(sf::Packet& p) const override {
		Object::output(p);
		_leftLeg.output(p);
		return _rightLeg.output(p);
	}
	sf::Packet& input(sf::Packet& p) override {
		Object::input(p);
		_leftLeg.input(p);
		return _rightLeg.input(p);
	}
};

#endif
