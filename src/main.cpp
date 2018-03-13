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
	if(argc < 2) return EXIT_FAILURE;
	unsigned port = 2713;
	if(argv[1] == string("server"))
		isServer = true;
	if(argc > 2) port = stoi(argv[2]);
    srand(time(0));
	sf::RenderWindow* window;
	if(not isServer) {
		window = new sf::RenderWindow(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "OpenSP");
		window->setKeyRepeatEnabled(false);
	}
    Text::init();
    unsigned roundNb = 1;
    int score1 = 0, score2 = 0;
	sf::Clock clock;

	sf::TcpSocket socket;
	sf::IpAddress serverAdress = argv[1];
	if(not isServer)
		if(socket.connect(serverAdress, port) != sf::Socket::Done)
			exit(1);
	vector<Object*> objects(8);
	Player *pp1A, *pp1B, *pp2A, *pp2B;
	vector<sf::TcpSocket*> clients;
	mutex mtx;
	if(isServer) {
		thread t(serverRecv, port, std::ref(mtx), std::ref(clients), &pp1A, &pp1B, &pp2A, &pp2B);
		t.detach();
	} else {
		thread t(clientRecv, &socket,
				std::ref(objects), std::ref(mtx), serverAdress);
		t.detach();
	}


    while (score1 < 5 and score2 < 5) {
        b2Vec2 gravity(0., 9.81);
        b2World world(gravity);
		BitoMonitor bebouzi;
		world.SetContactListener(&bebouzi);
        Ground ground(world);
        Goal go1(world, 0.01 * PH_WIDTH, true);
        Goal go2(world, 0.98 * PH_WIDTH, false);
        Ball ball(world);
        Player p1A(world, 0.2 * PH_WIDTH, sf::Color::Blue, "Hugo", true);
        Player p1B(world, 0.3 * PH_WIDTH, sf::Color::Blue, "Dorian", true);
        Player p2A(world, 0.8 * PH_WIDTH, sf::Color::Red, "Maxime", false);
        Player p2B(world, 0.7 * PH_WIDTH, sf::Color::Red, "Virgile", false);
        //Player p3A(world, 0.4 * PH_WIDTH, sf::Color::Yellow, "Bernard", true);
        //Player p3B(world, 0.6 * PH_WIDTH, sf::Color::Yellow, "Roger", false);
        objects = {&ground, &ball, &p1A, &p1B, &p2A, &p2B, &go1, &go2};
		pp1A = &p1A; pp1B = &p1B; pp2A = &p2A; pp2B = &p2B;
        bool roundActive = true;
        int lastFrames = 100;
        string endMessage;
		float accumulated = 0;
		
        while ((isServer or window->isOpen()) and lastFrames > 0) {
            sf::Event event;

            if (isServer and rand() % 7000 == 0) {
				mtx.lock();
                //p3A.jump();
                //p3B.jump();
				mtx.unlock();
            }

            while (not isServer and window->pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window->close();

				sf::Uint8 in = CONNECT;
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Q) {
						mtx.lock();
                        p1A.jump();
						mtx.unlock();
						in = P1_PRESSED;
					}

                    if (event.key.code == sf::Keyboard::D) {
						mtx.lock();
                        p1B.jump();
						mtx.unlock();
						in = P2_PRESSED;
					}

                    if (event.key.code == sf::Keyboard::Right) {
						mtx.lock();
                        p2A.jump();
						mtx.unlock();
						in = P3_PRESSED;
					}

                    if (event.key.code == sf::Keyboard::Left) {
						mtx.lock();
                        p2B.jump();
						mtx.unlock();
						in = P4_PRESSED;
					}
                }
                if (event.type == sf::Event::KeyReleased) {
                    if (event.key.code == sf::Keyboard::Q) {
						mtx.lock();
                        p1A.unjump();
						mtx.unlock();
						in = P1_RELEASED;
					}

                    if (event.key.code == sf::Keyboard::D) {
						mtx.lock();
                        p1B.unjump();
						mtx.unlock();
						in = P2_RELEASED;
					}

                    if (event.key.code == sf::Keyboard::Right) {
						mtx.lock();
                        p2A.unjump();
						mtx.unlock();
						in = P3_RELEASED;
					}

                    if (event.key.code == sf::Keyboard::Left) {
						mtx.lock();
                        p2B.unjump();
						mtx.unlock();
						in = P4_RELEASED;
					}
                }
				if(in != CONNECT) {
					sf::Packet p;
					Event e(in, 42);
					p << e;
					cout << e.in << endl;
					socket.send(p);
				}
            }

			float elapsed = clock.restart().asSeconds();

			if(isServer) {
				accumulated += elapsed;
				if (accumulated >= 1./128.) {
					sf::Packet p;
					p << objects;
					int toDelete = 0;
					mtx.lock();
					for(sf::TcpSocket* c : clients) {
						if(c->send(p) == sf::Socket::Disconnected) {
							delete c;
							c = *(clients.end() - 1 - toDelete);
							++toDelete;
						}
					}
					clients.resize(clients.size() - toDelete);
					mtx.unlock();
					accumulated = 0;
				}
			}

			if(not isServer or clients.size() >= 0) {
				mtx.lock();
				if (roundActive)
					world.Step(elapsed, 8 * 10, 3 * 10);
				else
					world.Step(elapsed / 4, 8, 3);

				if(not isServer) window->clear();

				for (auto && ob : objects) {
					ob->update();
					if(not isServer) ob->render(*window);
				}
				mtx.unlock();
			}

            if (!roundActive) {
                lastFrames--;
                if(not isServer)
					Text::drawText(*window, endMessage, sf::Vector2f(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4), 40);
            }

			if(not isServer) {
				Text::drawText(*window, "Round " + to_string(roundNb), sf::Vector2f(0, 0), 24, false);
				Text::drawText(*window, "FPS : " + to_string(static_cast<int>(1./elapsed)),
						sf::Vector2f(0, SCREEN_HEIGHT - 30), 24, false);
				Text::drawText(*window, to_string(score1) + " - " + to_string(score2),
							   sf::Vector2f(SCREEN_WIDTH / 2, 0.05 * SCREEN_HEIGHT), 45);

				window->display();
			}

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
    }

    return 0;
}
