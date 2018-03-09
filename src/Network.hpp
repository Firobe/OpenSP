#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <thread>
#include <vector>
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

sf::Packet& operator << (sf::Packet& packet, const Event& e) {
    packet << e.id << e.in;
    return packet;
}

void clientRecv(sf::TcpSocket* socket, std::vector<Object*>& objects, std::mutex& mtx,
                sf::IpAddress serverAdress) {
    while (true) {
        sf::Packet p;

        if (socket->receive(p) != sf::Socket::Done)
            exit(1);

        std::cout << "grumeau" << std::endl;
        mtx.lock();
        p >> objects;
        mtx.unlock();
    }
}

void serverRecv(unsigned expectedPort, std::mutex& mtx,
                std::vector<sf::TcpSocket*> clients,
                Player** p1A, Player** p1B, Player** p2A, Player** p2B) {
    sf::TcpListener listener;

    if (listener.listen(expectedPort) != sf::Socket::Done) exit(1);

    sf::SocketSelector selector;
    selector.add(listener);

    while (true) {
        if (selector.wait()) {
            if (selector.isReady(listener)) { //Incoming connection
                sf::TcpSocket* client = new sf::TcpSocket();

                if (listener.accept(*client) == sf::Socket::Done) {
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
                                sf::Uint8 id;
                                sf::Uint8 in;
                                p >> id >> in;
                                mtx.lock();

                                if (in == P1)
                                    (*p1A)->jump();
                                else if (in == P2)
                                    (*p1B)->jump();
                                else if (in == P3)
                                    (*p2A)->jump();
                                else if (in == P4)
                                    (*p2B)->jump();
                                else std::cout << "grou" << std::endl;

                                mtx.unlock();
                            }
                        }
                    }
            }
        }
    }
}
#endif
