#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <thread>
#include <vector>
#include <mutex>

//DO NOT CHANGE ORDER
#define P1_PRESSED 0
#define P2_PRESSED 1
#define P3_PRESSED 2
#define P4_PRESSED 3
#define P1_RELEASED 4
#define P2_RELEASED 5
#define P3_RELEASED 6
#define P4_RELEASED 7
//You can change now
#define CONNECT 8
#define CHANGE_NAME 9

struct Event {
	Event() = default;
    Event(sf::Uint8 i) : in(i) {}
    sf::Uint8 in;
	std::string data;
};

sf::Packet& operator << (sf::Packet& packet, const Event& e) {
    packet << e.in << e.data;
    return packet;
}

sf::Packet& operator >> (sf::Packet& packet, Event& e) {
    packet >> e.in >> e.data;
    return packet;
}

void clientRecv(sf::TcpSocket* socket, std::vector<Object*>& objects, std::mutex& mtx,
                sf::IpAddress serverAdress, unsigned* round, int* score1, int* score2) {
    while (true) {
        sf::Packet p;

        if (socket->receive(p) != sf::Socket::Done)
            exit(1);

        mtx.lock();
		sf::Uint8 c;
		sf::Int8 u, l;
        p >> objects >> c >> u >> l;
		*round = c;
		*score1 = u;
		*score2 = l;
        mtx.unlock();
    }
}

void serverRecv(unsigned expectedPort, std::mutex& mtx,
                std::vector<sf::TcpSocket*>& clients,
                Player** p1A, Player** p1B, Player** p2A, Player** p2B, bool& canStart) {
	sf::TcpSocket* owners[] = {nullptr, nullptr, nullptr, nullptr};//P1A,P1B,P2A,P2B
	Player** players[] = {p1A, p1B, p2A, p2B};
    sf::TcpListener listener;
	std::map<sf::TcpSocket*, std::string> names;

    if (listener.listen(expectedPort) != sf::Socket::Done) exit(1);

    sf::SocketSelector selector;
    selector.add(listener);

    while (true) {
		//Bonsoir les noms
		unsigned count = 1;
		for(unsigned i = 0 ; i < 4 ; ++i)
			if(names.count(owners[i]) > 0)
				(*players[i])->setName(names[owners[i]]
						+ "-" + to_string(count++));
        if (selector.wait()) {
            mtx.lock();

            if (selector.isReady(listener)) { //Incoming connection
                sf::TcpSocket* client = new sf::TcpSocket();

                if (listener.accept(*client) == sf::Socket::Done) {
                    std::cout << "A new client connected" << std::endl;
                    clients.push_back(client);
                    selector.add(*client);
                }
                else delete client;
            }
            else { //A client sent something
                for (sf::TcpSocket* c : clients)
                    if (selector.isReady(*c)) {
                        sf::Packet p;

                        if (c->receive(p) == sf::Socket::Done) {
                            //PROCESS DATA

                            if (p1A != nullptr) {
								Event e;
								p >> e;
                                sf::Uint8 in = e.in;

								if(in < 8 and owners[in % 4] == nullptr) {
									owners[in % 4] = c;
									if(std::all_of(owners, owners + 4,
												[](sf::TcpSocket* p){return p != nullptr;}))
										canStart = true;
								}
								if(in < 8 and owners[in % 4] == c) {
									if(in < 4) (*players[in])->jump();
									else (*players[in % 4])->unjump();
								}
								if(in == CHANGE_NAME) {
									//Update local array
									names[c] = e.data;
									unsigned count = 1;
									for(unsigned i = 0 ; i < 4 ; ++i)
										if(owners[i] == c)
											(*players[i])->setName(
													e.data + " #" + to_string(count++));

								}

                            }
                        }
                    }

            }
            mtx.unlock();
        }
    }
}
#endif
