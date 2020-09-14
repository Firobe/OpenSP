#ifndef STATBOX_HPP 
#define STATBOX_HPP

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include "Object.hpp"

class StaticBox : public Object {
	float _width;
	float _height;
	float _orX, _orY;
	sf::Color _color;
public:
    StaticBox(b2World& world, float w, float h, float x, float y, sf::Color c = sf::Color::Green) :
			Object(world), _width(w), _height(h), _orX(x), _orY(y), _color(c) {
        b2BodyDef bDef;
        bDef.position = b2Vec2(_orX + _width / 2., _orY +  _height / 2);
        bDef.type = b2_staticBody;
        _body = _world.CreateBody(&bDef);
        b2PolygonShape shape;
        shape.SetAsBox(_width / 2, _height / 2);
        b2FixtureDef fixDef;
        fixDef.density = 0.;
		fixDef.filter.categoryBits = 0x0002;
		fixDef.restitution = 0.1;
		fixDef.friction = 0.8;
        fixDef.shape = &shape;
        _body->CreateFixture(&fixDef);
		initSprite();
    }
    void initSprite() {
        sf::Vector2f size = sf::Vector2f(_width * RATIO, _height * RATIO);
        _sprite = new sf::RectangleShape(size);
        _sprite->setFillColor(_color);
        _sprite->setOrigin(size / 2.f);
    }

	sf::Packet& output(sf::Packet& p) const { return p; }
	sf::Packet& input(sf::Packet& p) { return p; }
};

#endif
