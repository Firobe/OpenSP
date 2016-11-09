#ifndef GROUND_HPP 
#define GROUND_HPP

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "StaticBox.hpp"

class Ground : public StaticBox {
public:
    Ground(b2World& world) :
		StaticBox(world, PH_WIDTH, 0.25 * PH_HEIGHT, 0., 0.75 * PH_HEIGHT, sf::Color::Magenta) {}
};

#endif
