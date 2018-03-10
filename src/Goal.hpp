#ifndef GOAL_HPP 
#define GOAL_HPP

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "StaticBox.hpp"
#include "Ball.hpp"

#define THICKNESS (0.01 * PH_WIDTH)
#define UP_LENGTH (0.1 * PH_WIDTH)

class Goal : public Object {
	float x, height;
	bool left;
	StaticBox _pillar, _up;
public:
	Goal(b2World& world, float x, bool left, float height = 0.3 * PH_HEIGHT) :
		Object(world),
		x(x), height(height), left(left),
		_pillar(world, THICKNESS, height, x, 0.75 * PH_HEIGHT - height, sf::Color::Yellow),
		_up(world, UP_LENGTH, THICKNESS, x - (left ? 0. : UP_LENGTH),
				0.75 * PH_HEIGHT - height, sf::Color::Yellow) {}
	void render(sf::RenderWindow& window) {
		_pillar.render(window);
		_up.render(window);
	}
	bool goal(Ball& ball) {
		b2Vec2 pos = ball._body->GetPosition();
		if(left)
			return pos.x > x and pos.x < x + UP_LENGTH and
				pos.y > 0.75 * PH_HEIGHT - height;
		else
			return pos.x < x and pos.x > x - UP_LENGTH and
				pos.y > 0.75 * PH_HEIGHT - height;
	}
	sf::Packet& output(sf::Packet& p) const { return p; }
	sf::Packet& input(sf::Packet& p) { return p; }
};

#endif
