#include "IEventListener.h"

int IEventListener::m_EventListenerGlobalID = 0;

IEventListener::IEventListener()
{
	m_EventListenerID = m_EventListenerGlobalID;
	m_EventListenerGlobalID++;
}

int IEventListener::GetID()
{
	return m_EventListenerID;
}
