#include "Object.hpp"

sf::Packet& operator << (sf::Packet& packet, const Object& o) {
	return packet << o._body->GetPosition().x
			<< o._body->GetPosition().y
			<< o._body->GetAngle()
			<< o._body->GetLinearVelocity().x
			<< o._body->GetLinearVelocity().y
			<< o._body->GetAngularVelocity();
}

sf::Packet& operator >> (sf::Packet& packet, Object& o) {
	float x, y, a, vx, vy, va;
	packet >> x >> y >> a >> vx >> vy >> va;
	o._body->SetTransform(b2Vec2(x,y), a);
	o._body->SetLinearVelocity(b2Vec2(vx,vy));
	o._body->SetAngularVelocity(va);
	return packet;
}

sf::Packet& operator << (sf::Packet& packet, const std::vector<Object*> v) {
	for(Object* o : v) packet << *o;
	return packet;
}

sf::Packet& operator >> (sf::Packet& packet, std::vector<Object*> v) {
	for(unsigned i = 0 ; i < v.size() ; ++i)
		packet >> *(v[i]);
	return packet;
}
