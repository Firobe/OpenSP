#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

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
	virtual void update(float) {}
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
	virtual sf::Packet& output(sf::Packet& packet) const;
	virtual sf::Packet& input(sf::Packet& packet);
	friend sf::Packet& operator << (sf::Packet& packet, const Object&);
	friend sf::Packet& operator >> (sf::Packet& packet, Object&);
	
	bool outOfBounds() {
		return _body->GetPosition().x < 0 or
			_body->GetPosition().x > PH_WIDTH;
	}
};

sf::Packet& operator << (sf::Packet& packet, const std::vector<Object*> v);
sf::Packet& operator >> (sf::Packet& packet, std::vector<Object*> v);

#endif
