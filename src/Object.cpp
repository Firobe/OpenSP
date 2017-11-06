#include "Object.hpp"

sf::Packet& operator << (sf::Packet& packet, const Object& o) {
	return o.output(packet);
}

sf::Packet& Object::output(sf::Packet& packet) const {
	return packet << _body->GetPosition().x
			<< _body->GetPosition().y
			<< _body->GetAngle()
			<< _body->GetLinearVelocity().x
			<< _body->GetLinearVelocity().y
			<< _body->GetAngularVelocity();
}

sf::Packet& operator >> (sf::Packet& packet, Object& o) {
	return o.input(packet);
}

sf::Packet& Object::input(sf::Packet& packet) {
	float x, y, a, vx, vy, va;
	packet >> x >> y >> a >> vx >> vy >> va;
	_body->SetTransform(b2Vec2(x,y), a);
	_body->SetLinearVelocity(b2Vec2(vx,vy));
	_body->SetAngularVelocity(va);
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
