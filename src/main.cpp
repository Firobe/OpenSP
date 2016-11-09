#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <iostream>

using namespace std;

#define WIDTH 12080
#define HEIGHT 720
#define SCALE 30.

#define BOX_SIZE 32
#define STATIC_SIZE 40

void createGround(b2World& world, float x, float y){
	b2BodyDef bDef;
	bDef.position = b2Vec2(x/SCALE, y/SCALE);
	bDef.type = b2_staticBody;
	b2Body* body = world.CreateBody(&bDef);

	b2PolygonShape shape;
	shape.SetAsBox(STATIC_SIZE/2./SCALE, STATIC_SIZE/2./SCALE);

	b2FixtureDef fixDef;
	fixDef.density = 0.;
	fixDef.shape = &shape;

	body->CreateFixture(&fixDef);
}

void createBox(b2World& world, int x, int y){
	b2BodyDef bDef;
	bDef.position = b2Vec2(x/SCALE, y/SCALE);
	bDef.type = b2_dynamicBody;
	b2Body* body = world.CreateBody(&bDef);

	b2PolygonShape shape;
	shape.SetAsBox(BOX_SIZE/2./SCALE, BOX_SIZE/2./SCALE);

	b2FixtureDef fixDef;
	fixDef.density = 0.1;
	fixDef.friction = 0.7;
	fixDef.restitution = 1.1;
	fixDef.shape = &shape;

	body->CreateFixture(&fixDef);
}

void render(b2World& world, sf::RenderWindow& window){
	for(b2Body* bIt = world.GetBodyList();
			bIt != 0;
			bIt = bIt->GetNext()){
		sf::Vector2f size = (bIt->GetType() == b2_dynamicBody) ?
			sf::Vector2f(BOX_SIZE, BOX_SIZE) : sf::Vector2f(STATIC_SIZE, STATIC_SIZE);
		sf::RectangleShape sprite(size);
		sprite.setFillColor(sf::Color::Blue);
		sprite.setOrigin(size / 2.f);
		sprite.setPosition(
				SCALE * bIt->GetPosition().x,
				SCALE * bIt->GetPosition().y);
		sprite.setRotation(bIt->GetAngle() * 180. / b2_pi);
		window.draw(sprite);
	}
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(1280, 720), "OpenSP");
	window.setVerticalSyncEnabled(true);

	b2Vec2 gravity(0., 9.81);
	b2World world(gravity);
	createGround(world, 0., 500.);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
				createBox(world, sf::Mouse::getPosition(window).x,
						sf::Mouse::getPosition(window).y);
			if(sf::Mouse::isButtonPressed(sf::Mouse::Right))
				createGround(world, sf::Mouse::getPosition(window).x,
						sf::Mouse::getPosition(window).y);
		}

		world.Step(1/60., 8, 3);
		window.clear();
		render(world, window);
		window.display();
	}

	return 0;
}
