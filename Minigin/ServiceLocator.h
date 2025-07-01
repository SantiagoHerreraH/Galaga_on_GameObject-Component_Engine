#pragma once
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <memory>
#include "Singleton.h"

namespace dae {

    template<typename T>
    concept Abstract = std::is_abstract_v<T>;

    class ServiceLocator : public Singleton<ServiceLocator> {

    public:
        template<Abstract ServiceType>
        ServiceType* SetService(std::unique_ptr<ServiceType> service);

        template<Abstract ServiceType>
        ServiceType* GetService();

    private:

        std::unordered_map<std::type_index, void*> m_Type_to_Any;
    };

    template<Abstract ServiceType>
    inline ServiceType* ServiceLocator::SetService(std::unique_ptr<ServiceType> service) {
        std::type_index index{ typeid(ServiceType) };

        if (m_Type_to_Any.contains(index)) {
            delete static_cast<std::unique_ptr<ServiceType>*>(m_Type_to_Any[index]);
        }

        m_Type_to_Any[index] = new std::unique_ptr<ServiceType>(std::move(service));

        return m_Type_to_Any[index] ? static_cast<std::unique_ptr<ServiceType>*>(m_Type_to_Any[index])->get() : nullptr;
    }

    template<Abstract ServiceType>
    inline ServiceType* ServiceLocator::GetService() {
        std::type_index index{ typeid(ServiceType) };

        if (m_Type_to_Any.contains(index)) {
            return static_cast<std::unique_ptr<ServiceType>*>(m_Type_to_Any[index])->get();
        }

        return nullptr;
    }
}
