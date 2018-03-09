#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <set>
#include <thread>
#include <mutex>

#define P1 0
#define P2 1
#define P3 2
#define P4 3
#define CONNECT 5

struct Event {
	Event(sf::Uint8 i, sf::Uint8 id) : in(i), id(id) {}
	sf::Uint8 in;
	sf::Uint8 id;
};

sf::Packet& operator << (sf::Packet& packet, const Event& e){
	packet << e.id << e.in;
	return packet;
}

void clientRecv(sf::UdpSocket& socket, std::vector<Object*>& objects, std::mutex& mtx, sf::IpAddress serverAdress) {
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

struct Client {
	sf::IpAddress ip;
	short unsigned port;
	bool operator<(const Client& c) const {
		return ip.toInteger() < c.ip.toInteger();
	}
};

void serverRecv(unsigned expectedPort, std::mutex& mtx, std::set<Client>& clients,
		Player** p1A, Player** p1B, Player** p2A, Player** p2B) {
	sf::UdpSocket socket;
	if(socket.bind(expectedPort) != sf::Socket::Done) exit(1);
	Client client;
	while(true) {
		sf::Packet p;
		if( socket.receive(p, client.ip, client.port) != sf::Socket::Done)
			exit(1);
		std::cout << "Ouille" << std::endl;
		if(p1A != nullptr) {
			sf::Uint8 id;
			sf::Uint8 in;
			p >> id >> in;
			mtx.lock();
			clients.insert(client);
			if(in == P1){
				(*p1A)->jump();
			}
			else if(in == P2){
				(*p1B)->jump();
			}
			else if(in == P3){
				(*p2A)->jump();
			}
			else if(in == P4){
				(*p2B)->jump();
			}
			else std::cout << "grou" << std::endl;
			/*switch(in) {
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
			}*/
			mtx.unlock();
		}
	}
}


#endif
