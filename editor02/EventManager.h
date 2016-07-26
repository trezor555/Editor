#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H
#include "IEventListener.h"
#include <string>
#include <map>
#include <vector>
#include <utility>
#include <functional>

struct Event
{
	std::string evtType;
};

typedef std::function<void(Event*)> EventMethodCallBack;

struct MouseEventData : public Event
{
	int XPos;
	int YPos;
};

struct EntityData : public Event
{
	void* data;
};

struct ListenerEntity
{
	ListenerEntity(){}

	ListenerEntity(IEventListener* listener, std::string eventName, EventMethodCallBack methodToCall)
	{
		this->listener = listener;
		this->eventName = eventName;
		this->cb = methodToCall;
	}

	IEventListener* listener;
	std::string eventName;
	EventMethodCallBack cb;
};

class EventManager
{
public:
	static void RegisterEvent(std::string e, IEventListener* listener, EventMethodCallBack cb);
	static void SendOutEvent(std::string e, Event* data = nullptr);
	static void SendOutEvent(std::string e, IEventListener* to, Event* data = nullptr);
	static void UnregisterFromEvent(std::string e, IEventListener* listener);
private:
	static std::map< std::string, std::vector<ListenerEntity*>> eventRegistry;
};

#endif