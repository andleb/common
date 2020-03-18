/** \file patterns.h
 * \author Andrej Leban
 * \date 3/2020
 *
 * Design pattern helper classes.
 */


#ifndef PATTERNS_H
#define PATTERNS_H

#include <map>
#include <memory>
#include <iostream>
#include <string>

#include "functional.h"

namespace cm
{


//! @name Design Pattern helpers.
//!@{

//! \brief A base class for the implementation of the Singleton pattern via public inheritance.
//! Note: don't forget to friend this class as well if the derived's constructors are not public.
template <typename T>
class Singleton
{
public:
    Singleton(const Singleton &) = delete;
    Singleton(Singleton &&) = delete;
    Singleton & operator=(const Singleton &) = delete;
    Singleton & operator=(Singleton &&) = delete;

    //! \brief The Singleton instance is accessed through this.
    static T & instance();

protected:
    Singleton() = default;
    ~Singleton() = default;
};


// Initially developed as part of the Derivatives project.
//! \brief A Generic factory class for a hierarchy of classes with \p Base as the top-level base class.
template <class Base>
class Factory : public Singleton<Factory<Base>>
{
public:
    using Ret = std::unique_ptr<Base>;

    template <typename... Args>
    using FactoryMethod = Ret (*)(Args...);

    //! \brief Registers a factory method \p p_factory for a class under \p p_name.
    //! \param p_name
    //! \param p_factory
    template <typename... Args>
    void registerFactoryMethod(std::string p_name, FactoryMethod<Args...> p_factory);

    //! \brief Creates a pointer to the \p Base class via the method stored under \p p_name.
    //! \param p_name
    //! \param p_args: constructor parameters.
    //! \return A std::unique_ptr to the \p Base class of the class requested,
    //!  or a nullptr if \p p_name not registered.
    template <typename... Args>
    Ret create(const std::string & p_name, Args... p_args);

protected:
    friend class Singleton<Factory<Base>>;
    Factory() = default;

private:
    std::map<std::string, AnyCallable<Ret>> m_registeredFactories{};
};

//! \brief An adapter that makes any class usable with the generic Factory.
//! At the moment, specialized for use with a single constructor.
// TODO: can we make this even more generic, i.e. multiple constructors?
template <class T, class Base, typename... Args>
class factoryRegisterer
{
public:

    //! \brief Automatically registers the class with PayoffFactory under \p p_name.
    //! \param p_name
    factoryRegisterer(std::string p_name);

    static std::unique_ptr<Base> create(Args... p_args);
};

//!@}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Singleton


template <typename T>
T & Singleton<T>::instance()
{
    static T instance;
    return instance;
}


// Factory


template <class Base>
template <typename... Args>
void Factory<Base>::registerFactoryMethod(std::string p_name, FactoryMethod<Args...> p_factory)
{
    //    m_registeredFactories[p_name] = cm::AnyCallable<Ret>(std::forward<FactoryMethod<Args...>>(p_factory));
    m_registeredFactories[std::move(p_name)] = cm::AnyCallable<Ret>(std::move(p_factory));
}

template <class Base>
template <typename... Args>
typename Factory<Base>::Ret Factory<Base>::create(const std::string & p_name, Args... p_args)
{
    try
    {
        return m_registeredFactories.at(p_name)(std::forward<Args>(p_args)...);
    }
    catch (const std::out_of_range &)
    {
        std::cerr << "cm::Factory::create: No function registered as " << p_name << "\n";
        return nullptr;
    }
}


// factoryRegisterer


template <class T, class Base, typename... Args>
factoryRegisterer<T, Base, Args ...>::factoryRegisterer(std::string p_name)
{
    // this should fail @compile-time if no matching constructor
    Factory<Base>::instance().registerFactoryMethod(std::move(p_name),
        &factoryRegisterer<T, Base, Args...>::create);
}

template <class T, class Base, typename... Args>
std::unique_ptr<Base> factoryRegisterer<T, Base, Args...>::create(Args... p_args)
{
    return std::make_unique<T>(std::forward<Args>(p_args)...);
}

} // namespace cm
#endif // PATTERNS_H
