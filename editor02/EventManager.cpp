#include "EventManager.h"
using namespace std;

map< std::string, vector<ListenerEntity*>> EventManager::eventRegistry;

void EventManager::RegisterEvent(std::string e, IEventListener* listener, EventMethodCallBack cb)
{
	map<std::string, vector<ListenerEntity*>>::iterator iter;
	iter = eventRegistry.find(e);

	ListenerEntity* ent = new ListenerEntity();
	ent->eventName = e;
	ent->listener = listener;
	ent->cb = cb;

	if (iter == eventRegistry.end())
	{
		eventRegistry.insert(make_pair(e, vector<ListenerEntity*>()));
		eventRegistry.find(e)->second.push_back(ent);
	}
	else
	{
		eventRegistry.find(e)->second.push_back(ent);
	}
}

void EventManager::SendOutEvent(std::string e, Event* data)
{
	map<string, vector<ListenerEntity*>>::iterator iter2;
	iter2 = eventRegistry.find(e);

	if (iter2 != eventRegistry.end())
	{
		std::vector<ListenerEntity*> vecs = iter2->second;
		for (size_t i = 0; i < iter2->second.size(); i++)
		{
			vecs[i]->cb(data);
		}
	}
}

void EventManager::SendOutEvent(std::string e, IEventListener* to, Event* data)
{
	map<string, vector<ListenerEntity*>>::iterator iter;
	iter = eventRegistry.find(e);

	if (iter != eventRegistry.end())
	{
		std::vector<ListenerEntity*> vecs = iter->second;
		for (size_t i = 0; i < iter->second.size(); i++)
		{
			if ((iter)->second[i]->listener->GetID() == to->GetID())
			{
				vecs[i]->cb(data);
			}
		}
	}
}

void EventManager::UnregisterFromEvent(std::string e, IEventListener* listener)
{
	map<string, vector<ListenerEntity*>>::iterator iter;
	iter = eventRegistry.find(e);

	if (iter != eventRegistry.end())
	{
		vector<ListenerEntity*>::iterator veciter = iter->second.begin();
		for (veciter; veciter != iter->second.end(); veciter++)
		{
			if ((*veciter)->listener->GetID() == listener->GetID())
			{
				(iter)->second.erase(veciter);
				break;
			}
		}
	}
}
