#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <set>
#include <thread>
#include <mutex>


enum input : sf::Uint8 { p1, p2, p3, p4, connect};

struct Event {
	Event(input i, sf::Uint8 id = sf::Uint8(0)) : in(i), id(id) {}
	input in;
	sf::Uint8 id;
};

sf::Packet& operator << (sf::Packet& packet, const Event& e){
	return packet << e.id << e.in;
}

void clientRecv(std::vector<Object*>& objects, std::mutex& mtx, unsigned expectedPort, sf::IpAddress serverAdress) {
	sf::UdpSocket socket;
	if(socket.bind(expectedPort) != sf::Socket::Done) exit(1);
	sf::IpAddress sender;
	short unsigned port;
	while(true) {
		sf::Packet p;
		if( socket.receive(p, sender, port) != sf::Socket::Done)
			exit(1);
		if(sender == serverAdress) {
			mtx.lock();
			p >> objects;
			mtx.unlock();
		}
	}
}

void serverRecv(unsigned expectedPort, std::mutex& mtx, std::set<sf::IpAddress>& clients,
		Player** p1A, Player** p1B, Player** p2A, Player** p2B) {
	sf::UdpSocket socket;
	if(socket.bind(expectedPort) != sf::Socket::Done) exit(1);
	sf::IpAddress sender;
	short unsigned port;
	while(true) {
		sf::Packet p;
		if( socket.receive(p, sender, port) != sf::Socket::Done)
			exit(1);
		if(p1A != nullptr) {
			sf::Uint8 id;
			sf::Uint8 in;
			p >> id >> in;
			mtx.lock();
			clients.insert(sender);
			switch(in) {
				case p1:
					(*p1A)->jump();
					break;
				case p2:
					(*p1B)->jump();
					break;
				case p3:
					(*p2A)->jump();
					break;
				case p4:
					(*p2B)->jump();
					break;
			}
			mtx.unlock();
		}
	}
}


#endif
