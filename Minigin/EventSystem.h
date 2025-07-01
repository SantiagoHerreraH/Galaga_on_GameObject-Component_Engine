#pragma once

#include <vector>
#include <functional>

namespace dae {

    using EventListenerKey = int;

    template <typename... Args>
    class Event {
    public:
        using EventListener = std::function<void(Args...)>;

        size_t Size() const { return m_EventListeners.size(); }

        EventListenerKey Subscribe(const EventListener& eventListener) {
            ++m_LastKey;
            m_EventListeners.push_back(std::move(eventListener));
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
            for (const auto& eventListener : m_EventListeners) {
                eventListener(args...);
            }
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

