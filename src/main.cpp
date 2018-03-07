#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <iostream>
#include <thread>

#include "Ground.hpp"
#include "Ball.hpp"
#include "Player.hpp"
#include "Goal.hpp"
#include "Text.hpp"
#include "Network.hpp"

using namespace std;

bool isServer = false;

int main(int argc, char** argv) {
    srand(time(0));
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "OpenSP");
    Text::init();
    unsigned roundNb = 1;
    int score1 = 0, score2 = 0;
	sf::Clock clock;

	sf::UdpSocket socket;
	unsigned port = 2713;
	if(socket.bind(isServer ? port + 1 : port) != sf::Socket::Done) return (EXIT_FAILURE);
	sf::IpAddress serverAdress = "127.0.0.1";
	vector<Object*> objects(10);
	Player *pp1A, *pp1B, *pp2A, *pp2B;
	set<sf::IpAddress> clients;
	if(isServer) {
		thread t(serverRecv, port, std::ref(clients), pp1A, pp1B, pp2A, pp2B);
		t.detach();
	} else {
		thread t(clientRecv, std::ref(objects), port + 1, serverAdress);
		t.detach();
	}


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
        Player p3A(world, 0.4 * PH_WIDTH, sf::Color::Yellow, "Bernard");
        Player p3B(world, 0.6 * PH_WIDTH, sf::Color::Yellow, "Roger");
        objects = {&ground, &ball, &p1A, &p1B, &p2A, &p2B, &go1, &go2, &p3A, &p3B};
        bool roundActive = true;
        int lastFrames = 100;
        string endMessage;
		float accumulated = 0;
		
        while (window.isOpen() and lastFrames > 0) {
            sf::Event event;

            if (rand() % 70 == 0) {
                p3A.jump();
                p3B.jump();
            }

            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                if (event.type == sf::Event::KeyPressed) {
					input in;
                    if (event.key.code == sf::Keyboard::Q) {
                        p1A.jump();
						in = input::p1;
					}

                    if (event.key.code == sf::Keyboard::D) {
                        p1B.jump();
						in = input::p2;
					}

                    if (event.key.code == sf::Keyboard::Right) {
                        p2A.jump();
						in = input::p3;
					}

                    if (event.key.code == sf::Keyboard::Left) {
                        p2B.jump();
						in = input::p4;
					}

					if(not isServer) {
						sf::Packet p;
						p << Event(in);
						socket.send(p, serverAdress, port);
					}
                }
            }

			float elapsed = clock.restart().asSeconds();

			if(isServer) {
				accumulated += elapsed;
				if (accumulated >= 0.5) {
					sf::Packet p;
					p << objects;
					for(auto&& ip : clients)
						socket.send(p, ip, port + 1);
					accumulated = 0;
				}
			}

            if (roundActive)
                world.Step(elapsed, 8 * 10, 3 * 10);
            else
                world.Step(elapsed / 4, 8, 3);

            window.clear();

            for (auto && ob : objects) {
                ob->update();
                ob->render(window);
            }

            Text::drawText(window, "Round " + to_string(roundNb), sf::Vector2f(0, 0), 24, false);
            Text::drawText(window, "FPS : " + to_string(static_cast<int>(1./elapsed)),
					sf::Vector2f(0, SCREEN_HEIGHT - 30), 24, false);
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
