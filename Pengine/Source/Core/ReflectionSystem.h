#pragma once

#include "Core.h"

#include <rttr/registration>

#include <string>
#include <map>
#include <functional>
#include <vector>

namespace ReflectedProps
{

#define COM ,

#define REFLECTED_PRIMITIVE_PROPERTY(type) \
    static std::string get_##type() \
    { \
        using namespace std; \
        using namespace glm; \
        return typeid(type).name(); \
    } \
\
    static bool is_##type(const std::string _type) \
    { \
        return _type == get_##type(); \
    }

#define EXPLICIT_REFLECTED_PRIMITIVE_PROPERTY(_name, type) \
    static std::string get_##_name() \
    { \
        return #type; \
    } \
\
    static bool is_##_name(const std::string _type) \
    { \
        return _type == get_##_name(); \
    }

#define EXPLICIT_REFLECTED_VECTOR_PROPERTY(_name, type) \
    static std::string get_##_name() \
    { \
         return typeid(type).name(); \
    } \
\
    static bool is_##_name(const std::string _type) \
    { \
        return _type == get_##_name(); \
    }

REFLECTED_PRIMITIVE_PROPERTY(int)
REFLECTED_PRIMITIVE_PROPERTY(int64_t)
REFLECTED_PRIMITIVE_PROPERTY(uint32_t)
REFLECTED_PRIMITIVE_PROPERTY(uint64_t)
REFLECTED_PRIMITIVE_PROPERTY(float)
REFLECTED_PRIMITIVE_PROPERTY(double)
REFLECTED_PRIMITIVE_PROPERTY(bool)
REFLECTED_PRIMITIVE_PROPERTY(vec2)
REFLECTED_PRIMITIVE_PROPERTY(vec3)
REFLECTED_PRIMITIVE_PROPERTY(vec4)
REFLECTED_PRIMITIVE_PROPERTY(ivec2)
REFLECTED_PRIMITIVE_PROPERTY(ivec3)
REFLECTED_PRIMITIVE_PROPERTY(ivec4)
REFLECTED_PRIMITIVE_PROPERTY(dvec2)
REFLECTED_PRIMITIVE_PROPERTY(dvec3)
REFLECTED_PRIMITIVE_PROPERTY(dvec4)

EXPLICIT_REFLECTED_VECTOR_PROPERTY(vectorint, std::vector<int>)
EXPLICIT_REFLECTED_VECTOR_PROPERTY(vectorint64_t, std::vector<__int64>)
EXPLICIT_REFLECTED_VECTOR_PROPERTY(vectoruint32_t, std::vector<unsigned int>)
EXPLICIT_REFLECTED_VECTOR_PROPERTY(vectoruint64_t, std::vector<unsigned __int64>)
EXPLICIT_REFLECTED_VECTOR_PROPERTY(vectorfloat, std::vector<float>)
EXPLICIT_REFLECTED_VECTOR_PROPERTY(vectordouble, std::vector<double>)
EXPLICIT_REFLECTED_VECTOR_PROPERTY(vectorbool, std::vector<bool>)
EXPLICIT_REFLECTED_VECTOR_PROPERTY(vectorvec2, std::vector<glm::vec2>)
EXPLICIT_REFLECTED_VECTOR_PROPERTY(vectorvec3, std::vector<glm::vec3>)
EXPLICIT_REFLECTED_VECTOR_PROPERTY(vectorvec4, std::vector<glm::vec4>)
EXPLICIT_REFLECTED_VECTOR_PROPERTY(vectorivec2, std::vector<glm::ivec2>)
EXPLICIT_REFLECTED_VECTOR_PROPERTY(vectorivec3, std::vector<glm::ivec3>)
EXPLICIT_REFLECTED_VECTOR_PROPERTY(vectorivec4, std::vector<glm::ivec4>)
EXPLICIT_REFLECTED_VECTOR_PROPERTY(vectordvec2, std::vector<glm::dvec2>)
EXPLICIT_REFLECTED_VECTOR_PROPERTY(vectordvec3, std::vector<glm::dvec3>)
EXPLICIT_REFLECTED_VECTOR_PROPERTY(vectordvec4, std::vector<glm::dvec4>)
EXPLICIT_REFLECTED_VECTOR_PROPERTY(vectorstring, std::vector<std::string>)

EXPLICIT_REFLECTED_PRIMITIVE_PROPERTY(string, std::string)
EXPLICIT_REFLECTED_PRIMITIVE_PROPERTY(asset, asset)
}

namespace Pengine
{

    // Only used for components for now!!!
    class PENGINE_API ReflectionSystem
    {
    private:

        ReflectionSystem() = default;
        ReflectionSystem(const ReflectionSystem&) = delete;
        ReflectionSystem& operator=(const ReflectionSystem&) { return *this; }
        ~ReflectionSystem()
        {
            for (auto& registeredClass : m_RegisteredClasses)
            {
                delete registeredClass.second.m_Class;
            }

            m_RegisteredClasses.clear();
        };
    public:

        class ReflectionWrapper
        {
        public:
            ReflectionWrapper(std::function<void()> callback)
            {
                callback();
            }
        };

        struct RegisteredClass
        {
            void* m_Class = nullptr;
            std::function<void(void*)> m_AddComponentCallBack;
            std::vector<std::function<void(void*, void*)>> m_CopyPropertyCallBacks;

            RegisteredClass(void* registeredClass,
                std::function<void(void*)> AddCallback)
                : m_Class(registeredClass)
                , m_AddComponentCallBack(AddCallback)
            {}
        };

        std::map<std::string, RegisteredClass> m_RegisteredClasses;

        static ReflectionSystem& GetInstance() { static ReflectionSystem reflectionSystem; return reflectionSystem; }
    };

#define RTTR_REGISTRATION_USER_DEFINED(type)                            \
    static void rttr_auto_register_reflection_function_##type();            \
    namespace                                                               \
    {                                                                       \
        struct rttr__auto__register__##type                                 \
        {                                                                   \
            rttr__auto__register__##type()                                  \
            {                                                               \
                rttr_auto_register_reflection_function_##type();            \
            }                                                               \
        };                                                                  \
    }                                                                       \
    static const rttr__auto__register__##type RTTR_CAT(RTTR_CAT(auto_register__, __LINE__), type); \
    static void rttr_auto_register_reflection_function_##type()

#define REGISTER_CLASS(type) RTTR_REGISTRATION_USER_DEFINED(type) \
    { \
        Pengine::ReflectionSystem::GetInstance().m_RegisteredClasses.insert( \
            std::make_pair(std::string(typeid(type).name()).substr(6), \
            Pengine::ReflectionSystem::RegisteredClass( new rttr::registration::class_<type>(std::string(typeid(type).name()).substr(6).c_str()), \
            [](void* componentManager) { ReflectionAddComponent<type>(componentManager); }) \
        )); \
    }

#define REGISTER_ASSET(type) RTTR_REGISTRATION_USER_DEFINED(type) \
    { \
        Pengine::ReflectionSystem::GetInstance().m_RegisteredClasses.insert( \
            std::make_pair(std::string(typeid(type).name()).substr(6), \
            Pengine::ReflectionSystem::RegisteredClass( new rttr::registration::class_<type>(std::string(typeid(type).name()).substr(6).c_str()), \
            [](void*) {}) \
        )); \
    }

#define PROPERTY(baseClass, _type, _name, value) _type _name = value; \
    private: \
    Pengine::ReflectionSystem::ReflectionWrapper RW##_name = Pengine::ReflectionSystem::ReflectionWrapper([=] \
    { \
        auto classIter = Pengine::ReflectionSystem::GetInstance().m_RegisteredClasses.find(std::string(typeid(baseClass).name()).substr(6)); \
        if (classIter != Pengine::ReflectionSystem::GetInstance().m_RegisteredClasses.end()) \
        { \
            if (rttr::type::get_by_name(classIter->first.c_str()).get_property(#_name).is_valid()) return; \
            classIter->second.m_CopyPropertyCallBacks.push_back([](void* componentA, void* componentB) { static_cast<baseClass*>(componentA)->_name = static_cast<baseClass*>(componentB)->_name; }); \
            rttr::registration::class_<baseClass>* registeredClass = static_cast<rttr::registration::class_<baseClass>*>(classIter->second.m_Class); \
            registeredClass->property(#_name, &baseClass::_name); \
        } \
    });

    #define COPY_PROPERTIES(baseClass, component) \
    auto classIter = Pengine::ReflectionSystem::GetInstance().m_RegisteredClasses.find(#baseClass); \
    if (classIter != Pengine::ReflectionSystem::GetInstance().m_RegisteredClasses.end()) \
    { \
        for (size_t i = 0; i < classIter->second.m_CopyPropertyCallBacks.size(); i++) \
        { \
            classIter->second.m_CopyPropertyCallBacks[i]((void*)this, (void*)&component); \
        } \
    }

}