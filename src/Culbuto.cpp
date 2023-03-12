#include "Culbuto.hpp"

void BitoMonitor::BeginContact(b2Contact* contact) {
	void* ua = (void*) contact->GetFixtureA()->GetUserData().pointer;
	void* ub = (void*) contact->GetFixtureB()->GetUserData().pointer;
	if(ua != nullptr) ++((Culbuto*)ua)->colNb;
	if(ub != nullptr) ++((Culbuto*)ub)->colNb;
}

void BitoMonitor::EndContact(b2Contact* contact) {
	void* ua = (void*) contact->GetFixtureA()->GetUserData().pointer;
	void* ub = (void*) contact->GetFixtureB()->GetUserData().pointer;
	if(ua != nullptr) --((Culbuto*)ua)->colNb;
	if(ub != nullptr) --((Culbuto*)ub)->colNb;
}
