#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "Object.hpp"
#include "Text.hpp"

class Player : public Object {
private:
	float _width = 0.01 * PH_WIDTH;
	float _height = 0.15 * PH_HEIGHT;
	float _ballRadius = 2 * _width;
	float _lestDensity = 300.;
	sf::Color _color;
	std::string _name;
public:
    Player(b2World& world, float initX, sf::Color c, std::string n) : Object(world), _color(c), _name(n) {
		// Body definition
        b2BodyDef bDef;
        bDef.type = b2_dynamicBody;
        bDef.position = b2Vec2(initX - _width / 2, 0.3 * PH_HEIGHT - _height / 2);

		// Main body
        _body = _world.CreateBody(&bDef);
        b2PolygonShape shape;
        shape.SetAsBox(_width / 2, _height / 2, b2Vec2(0, -_height / 2.), 0.);
        b2FixtureDef fixDef;
		fixDef.restitution = 0.1;
		fixDef.density = 2;
		fixDef.friction = 1.;
        fixDef.shape = &shape;
        _body->CreateFixture(&fixDef);

		// Ball under body
        b2CircleShape ball;
        ball.m_p.Set(0, 0);
		ball.m_radius = _ballRadius;
        b2FixtureDef fixDef2;
		fixDef2.restitution = 0.1;
		fixDef2.density = 1.;
		fixDef2.friction = 0.1;
		fixDef2.filter.maskBits = 0x0002;
        fixDef2.shape = &ball;
        _body->CreateFixture(&fixDef2);

		// Weight under ball
        b2CircleShape lest;
        lest.m_p.Set(0, 0.75 * _ballRadius);
		lest.m_radius = 0.25 * _ballRadius;
        b2FixtureDef fixDef3;
		fixDef3.restitution = 0.1;
		fixDef3.density = _lestDensity;
		fixDef3.friction = 1.;
        fixDef3.shape = &lest;
        _body->CreateFixture(&fixDef3);

		initSprite();

		// Initial rotation
		float angle = (rand() % 21 - 10) / 30.;
		_body->ApplyAngularImpulse(angle, true);
    }
    void initSprite() {
        sf::Vector2f size = sf::Vector2f(_width * RATIO, (_height + _ballRadius) * RATIO);
        _sprite = new sf::RectangleShape(size);
        _sprite->setFillColor(_color);
        _sprite->setOrigin({_width * RATIO / 2.f, _height * RATIO});
    }
	void render(sf::RenderWindow& window) {
		Object::render(window);
		auto pos = _body->GetPosition();
		Text::drawText(window, _name,
				sf::Vector2f(pos.x * RATIO, (pos.y - _height) * RATIO));
	}
	void update() {
		//_body->ApplyTorque(- _standupFactor * pow(_body->GetAngle(), _standupExp), false);
	}
	void jump() {
		float angle = _body->GetAngle();
		float strength = 20;
		float unitX = strength * sin(angle);
		float unitY = strength * -cos(angle);
		_body->ApplyLinearImpulse(b2Vec2(unitX, unitY), _body->GetWorldCenter(), true);
	}
};

#endif
