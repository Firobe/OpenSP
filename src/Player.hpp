#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "Object.hpp"
#include "Text.hpp"
#include "Culbuto.hpp"

class Player : public Object {
private:
	float _width = 0.01 * PH_WIDTH;
	float _height = 0.15 * PH_HEIGHT;
	sf::Color _color;
	std::string _name;
	Culbuto _leftLeg, _rightLeg;
	b2RevoluteJoint* legs;
public:
    Player(b2World& world, float initX, sf::Color c, std::string n) :
		Object(world),
		_color(c),
		_name(n),
		_leftLeg(world, initX, 2*PH_HEIGHT/3, _width, _height, c),
		_rightLeg(world, initX+_width, 2*PH_HEIGHT/3, _width, _height, sf::Color::Yellow) {
		b2RevoluteJointDef revDef;
		revDef.bodyA = _leftLeg._body;
		revDef.bodyB = _rightLeg._body;
		revDef.collideConnected = false;
		revDef.localAnchorA.Set(+_width/2, -_height);
		revDef.localAnchorB.Set(-_width/2, -_height);
		revDef.enableMotor = true;
		revDef.maxMotorTorque = 5000;
		revDef.motorSpeed = 36000;

		legs = (b2RevoluteJoint*) world.CreateJoint(&revDef);

		// Initial rotation
		/*
		float angle = (rand() % 21 - 10) / 30.;
		_body->ApplyAngularImpulse(angle, true);
		*/
    }
	void render(sf::RenderWindow& window) {
		_leftLeg.render(window);
		_rightLeg.render(window);
		/*
		auto pos = _leftLeg._body->GetPosition();
		Text::drawText(window, _name,
				sf::Vector2f(pos.x * RATIO, (pos.y - _height) * RATIO),
				std::min(500., pow(abs(_body->GetLinearVelocity().y) +
							abs(_body->GetLinearVelocity().x), 2)));
							*/
	}
	void jump() {
		float angle = _leftLeg._body->GetAngle();
		float strength = 20;
		float unitX = strength * sin(angle);
		float unitY = strength * -cos(angle);
		_leftLeg._body->ApplyLinearImpulse(
				b2Vec2(unitX, unitY), _leftLeg._body->GetWorldCenter(), true);
	}
	sf::Packet& output(sf::Packet& p) const override {
		_leftLeg.output(p);
		return _rightLeg.output(p);
	}
	sf::Packet& input(sf::Packet& p) override {
		_leftLeg.input(p);
		return _rightLeg.input(p);
	}
};

#endif
