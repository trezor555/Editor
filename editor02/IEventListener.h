#ifndef IEVENTLISTENER_H
#define IEVENTLISTENER_H

class IEventListener
{
private:
	static int m_EventListenerGlobalID;
	int m_EventListenerID;
public:
	IEventListener();
	int GetID();

};

#endif