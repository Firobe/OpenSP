#include "Culbuto.hpp"

void BitoMonitor::BeginContact(b2Contact* contact) {
	void* ua = contact->GetFixtureA()->GetUserData();
	void* ub = contact->GetFixtureA()->GetUserData();
	if(ua != nullptr) ++((Culbuto*)ua)->colNb;
	if(ub != nullptr) ++((Culbuto*)ub)->colNb;
}

void BitoMonitor::EndContact(b2Contact* contact) {
	void* ua = contact->GetFixtureA()->GetUserData();
	void* ub = contact->GetFixtureA()->GetUserData();
	if(ua != nullptr) --((Culbuto*)ua)->colNb;
	if(ub != nullptr) --((Culbuto*)ub)->colNb;
}
