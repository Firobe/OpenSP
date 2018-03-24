#ifndef CULBUTO_HPP
#define CULBUTO_HPP 

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "Object.hpp"
#include "Text.hpp"
#include <iostream>

using namespace std;

#define EPSILON (0.1)

class BitoMonitor : public b2ContactListener {
public:
	BitoMonitor() = default;
private:
	void BeginContact(b2Contact* contact);
	void EndContact(b2Contact* contact);
};


class Culbuto : public Object {
private:
	sf::Color _color;
	float _width;
	float _height;
	float _ballRadius = 0.5 * _width;
	float _lestDensity = 2;//250.;
	b2Fixture* bitonio = nullptr;
public:
	int colNb = 0;
    Culbuto(b2World& world, float x, float y, float w, float h, sf::Color c ) : Object(world), _color(c), _width(w), _height(h) {

		// Body definition
        b2BodyDef bDef;
        bDef.type = b2_dynamicBody;
        bDef.position = b2Vec2(x, y);

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

		// Bitonio
		b2PolygonShape bitoshape;
		b2FixtureDef bitofix;
		bitoshape.SetAsBox(
				(_width / 2.) * (0.922 + EPSILON), EPSILON * _width, b2Vec2(0, _ballRadius), 0.);
		bitofix.shape = &bitoshape;
		bitofix.isSensor = true;
		bitonio = _body->CreateFixture(&bitofix);
		bitonio->SetUserData((void*) this);


		// Weight under ball (must stay at the end)
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
    }
    void initSprite() {
        sf::Vector2f size = sf::Vector2f(_width * RATIO, (_height + _ballRadius) * RATIO);
        _sprite = new sf::RectangleShape(size);
        _sprite->setFillColor(_color);
        _sprite->setOrigin({_width * RATIO / 2.f, _height * RATIO});
    }
	void update(float elapsed) {
		/*
		float newDens = colliding() ? _lestDensity : 1.;
		float oldDens = _body->GetFixtureList()->GetDensity();
		_body->GetFixtureList()->SetDensity(oldDens + (newDens - oldDens) * 5 * elapsed);
		 oldDens = _body->GetFixtureList()->GetDensity();
		 std::cout << oldDens << std::endl;
		 */
	}
	bool colliding() const {
		return colNb > 0;
	}
};

#endif
