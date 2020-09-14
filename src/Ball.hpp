#ifndef BALL_HPP
#define BALL_HPP

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include "Object.hpp"

class Ball : public Object {
private:
	float _radius;
public:
    Ball(b2World& world, float radius = 0.02 * PH_HEIGHT) : Object(world), _radius(radius) {
        b2BodyDef bDef;
        bDef.type = b2_dynamicBody;
        bDef.position = b2Vec2(0.5 * PH_WIDTH, 0.25 * PH_HEIGHT);
        _body = _world.CreateBody(&bDef);
        b2CircleShape shape;
        shape.m_p.Set(0, 0);
		shape.m_radius = radius;
        b2FixtureDef fixDef;
		fixDef.restitution = 0.5;
		fixDef.density = 0.1;
        fixDef.shape = &shape;
        _body->CreateFixture(&fixDef);
		initSprite();
    }
    void initSprite() {
		float nr = _radius * RATIO;
		_sprite = new sf::CircleShape(nr);
        _sprite->setFillColor(sf::Color::Green);
        _sprite->setOrigin({nr, nr});
    }
};

#endif
