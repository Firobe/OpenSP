#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>

#define SCREEN_WIDTH 1240
#define SCREEN_HEIGHT 900

#define WRATIO ((float) SCREEN_WIDTH / (float) SCREEN_HEIGHT)
#define RATIO ((float) SCREEN_HEIGHT / (float) PH_HEIGHT)

#define PH_HEIGHT 10
#define PH_WIDTH (WRATIO * PH_HEIGHT)

class Object {
protected:
		b2World& _world;
		sf::Shape* _sprite = nullptr;
	Object(b2World& w) : _world(w) { }
	virtual void initSprite() {}
public:
	b2Body* _body = nullptr;
	virtual void update() {}
	virtual void render(sf::RenderWindow& window) {
		_sprite->setPosition(
			_body->GetPosition().x * RATIO,
			_body->GetPosition().y * RATIO);
		_sprite->setRotation(_body->GetAngle() * 180. / b2_pi);
		window.draw(*_sprite);
	}
	virtual ~Object() {
		delete _sprite;
	}
	
};

#endif
