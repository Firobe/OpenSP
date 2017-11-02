#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <iostream>

#include "Ground.hpp"
#include "Ball.hpp"
#include "Player.hpp"
#include "Goal.hpp"
#include "Text.hpp"

using namespace std;

int main(int argc, char** argv) {
    srand(time(0));
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "OpenSP");
    window.setVerticalSyncEnabled(true);
    Text::init();
    unsigned roundNb = 1;

	int score1 = 0, score2 = 0;
    while (score1 < 5 and score2 < 5) {
        b2Vec2 gravity(0., 9.81);
        b2World world(gravity);
        Ground ground(world);
        Goal go1(world, 0.01 * PH_WIDTH, true);
        Goal go2(world, 0.98 * PH_WIDTH, false);
        Ball ball(world);
        Player p1A(world, 0.2 * PH_WIDTH, sf::Color::Blue, "Hugo");
        Player p1B(world, 0.3 * PH_WIDTH, sf::Color::Blue, "Dorian");
        Player p2A(world, 0.8 * PH_WIDTH, sf::Color::Red, "Maxime");
        Player p2B(world, 0.7 * PH_WIDTH, sf::Color::Red, "Virgile");
        vector<Object*> objects {&ground, &ball, &p1A, &p1B, &p2A, &p2B, &go1, &go2};
        bool roundActive = true;
        int lastFrames = 100;
        string endMessage;

        while (window.isOpen() and lastFrames > 0) {
            sf::Event event;

            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
				if (event.type == sf::Event::KeyPressed) {
					if(event.key.code == sf::Keyboard::Q)
						p1A.jump();
					if(event.key.code == sf::Keyboard::D)
						p1B.jump();
					if(event.key.code == sf::Keyboard::Right)
						p2A.jump();
					if(event.key.code == sf::Keyboard::Left)
						p2B.jump();
				}
            }

            if (roundActive)
                world.Step(1 / 60., 8*10, 3*10);
            else
                world.Step(1 / 200., 8, 3);

            window.clear();

            for (auto&& ob : objects) {
                ob->update();
                ob->render(window);
            }

            Text::drawText(window, "Round " + to_string(roundNb), sf::Vector2f(0, 0), 24, false);
            Text::drawText(window, to_string(score1) + " - " + to_string(score2),
                           sf::Vector2f(SCREEN_WIDTH / 2, 0.05 * SCREEN_HEIGHT), 45);

            if (!roundActive) {
                lastFrames--;
                Text::drawText(window, endMessage, sf::Vector2f(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4), 40);
            }

            window.display();

            if (roundActive) {
                if (ball.outOfBounds()) {
                    endMessage = "Out of bounds";
                    roundActive = false;
                }

                if (go1.goal(ball)) {
                    endMessage = "Goal team red";
                    ++score2;
                    roundActive = false;
                }

                if (go2.goal(ball)) {
                    endMessage = "Goal team blue";
                    ++score1;
                    roundActive = false;
                }
            }
        }

        ++roundNb;

        if (!window.isOpen()) break;
    }

    return 0;
}
