#ifndef NETWORK_HPP
#define NETWORK_HPP

enum input : sf::Uint8 { p1, p2, p3, p4, connect};

struct Event {
	Event(input i, sf::Uint8 id = sf::Uint8(0)) : in(i), id(id) {}
	input in;
	sf::Uint8 id;
};

sf::Packet& operator << (sf::Packet& packet, const Event& e){
	return packet << e.id << e.in;
}

void clientRecv(std::vector<Object*>& objects, unsigned expectedPort, sf::IpAddress serverAdress) {
	sf::UdpSocket socket;
	if(socket.bind(expectedPort) != sf::Socket::Done) exit(1);
	sf::IpAddress sender;
	short unsigned port;
	while(true) {
		sf::Packet p;
		if( socket.receive(p, sender, port) != sf::Socket::Done)
			exit(1);
		if(sender == serverAdress and port == expectedPort) {
			p >> objects;
		}
	}
}


#endif
