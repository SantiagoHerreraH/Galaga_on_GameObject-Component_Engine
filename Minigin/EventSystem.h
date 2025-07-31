#pragma once

#include <vector>
#include <functional>
#include <memory>
#include <tuple>
#include "Singleton.h"
#include "ActivatableFlatMap.h"

namespace dae {

    class IEventAction {
    public:
        virtual ~IEventAction() = default;
        virtual void Trigger() = 0;
    };

    template <typename... Args>
    class EventAction final : public IEventAction {

        using EventListener = std::function<void(Args...)>;
    public:
        EventAction(const std::vector<EventListener>& eventListeners, const std::tuple<Args...>& args) :
            m_EventListeners(eventListeners),
            m_Args(args){ }

            virtual void Trigger() override {

            for (size_t i = 0; i < m_EventListeners.size(); i++)
            {
                std::apply(m_EventListeners[i], m_Args);
            }

        }

    private:
        std::vector<EventListener> m_EventListeners;
        std::tuple<Args...> m_Args;
    };

    class EventSystem : public Singleton<EventSystem> {

    public:
        void Update() {
            for (size_t i = 0; i < m_EventActions.size(); i++)
            {
                m_EventActions[i]->Trigger();
            }

            m_EventActions.clear();
        }
        void AddEventAction(std::unique_ptr<IEventAction> eventAction) {
            m_EventActions.push_back(std::move(eventAction));
        }

        //Maybe add a remove function, but first test

    private:

        std::vector<std::unique_ptr<IEventAction>> m_EventActions;

    };


    using EventListenerKey = int;

    template <typename... Args>
    class Event {
    public:
        using EventListener = std::function<void(Args...)>;

        size_t Size() const { return m_EventListeners.size(); }

        EventListenerKey Subscribe(const EventListener& eventListener) {
            ++m_LastKey;
            m_EventListeners.push_back(eventListener);
            m_EventListenerKeys.push_back(m_LastKey);

            return m_LastKey;
        }

        bool Unsubscribe(EventListenerKey eventListenerKey) {

            for (size_t i = 0; i < m_EventListenerKeys.size(); i++)
            {
                if (m_EventListenerKeys[i] == eventListenerKey)
                {
                    m_EventListenerKeys.erase(m_EventListenerKeys.begin() + i);
                    m_EventListeners.erase(m_EventListeners.begin() + i);
                    return true;
                }
            }

            return false;
        }

        void UnsubscribeAll() {
            m_EventListenerKeys.clear();
            m_EventListeners.clear();
        }


        void Invoke(Args... args) {

            for (size_t i = 0; i < m_EventListeners.size(); i++)
            {
                m_EventListeners[i](args...);
            }
        }

        void QueueInvoke(Args... args) {
            EventSystem::GetInstance().AddEventAction(
                    std::make_unique<EventAction<Args...>>
                    (m_EventListeners , std::make_tuple(args...)));
        }

        EventListenerKey operator+=(const EventListener& eventListener) {
            return Subscribe(eventListener);
        }


        bool operator-=(EventListenerKey eventListenerKey) {
            return Unsubscribe(eventListenerKey);
        }

    private:

        std::vector<EventListenerKey> m_EventListenerKeys;
        std::vector<EventListener> m_EventListeners;
        EventListenerKey m_LastKey = 0;
    };


}

