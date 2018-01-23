//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef PrivateImplementation_hpp
#define PrivateImplementation_hpp

#include <memory>
#include <new>

#include "Utilities.hpp"

// Allows the private implementation of an object allocated within a size
// given as a template parameter. This allows the storage of the object to
// reside on the stack or within another struct/class.
template <int SizeInBytes>
class PrivateImplementationLocal
{
public:
  using Destructor = void(*)(byte*);

  PrivateImplementationLocal() : mDestructor(GenericDoNothing) {}

  ~PrivateImplementationLocal()
  {
    // Destruct our data if it's already been constructed.
    Release();
  }

  void Release()
  {
    mDestructor(mMemory);
    mDestructor = GenericDoNothing;
  }

  template <typename tType, typename... tArguments>
  tType* ConstructAndGet(tArguments &&...aArguments)
  {
    static_assert(sizeof(tType) < SizeInBytes,
                  "Constructed Type must be smaller than our size.");

    // Destruct any undestructed object.
    mDestructor(mMemory);

    // Capture the destructor of the new type.
    mDestructor = GenericDestruct<tType>;

    // Create a T in our local memory by forwarding any provided arguments.
    new (mMemory) tType(std::forward<tArguments &&>(aArguments)...);
      
    return Get<tType>();
  }

  template <typename tType>
  tType* Get()
  {
    return std::launder<tType>(reinterpret_cast<tType*>(mMemory));
  }


private:

  byte mMemory[SizeInBytes];
  Destructor mDestructor;
};

// Allows the private implementation of an object allocated on the heap.
class PrivateImplementationDynamic
{
public:
  using Destructor = void(*)(byte*);
  using Storage = std::unique_ptr<byte[], Destructor>;

  PrivateImplementationDynamic()
    : mMemory(nullptr, GenericDoNothing)
  {
  }

  PrivateImplementationDynamic(PrivateImplementationDynamic &&aRight)
    : mMemory(std::move(aRight.mMemory))
  {

  }

  ~PrivateImplementationDynamic()
  {
  }

  void Release()
  {
    mMemory.release();
    mMemory = Storage(nullptr, GenericDoNothing);
  }

  template <typename tType, typename... tArguments>
  tType* ConstructAndGet(tArguments &&...aArguments)
  {
    // Destruct our data if it's already been constructed.
    mMemory.release();

    // Create a new T by forwarding any provided constructor arguments and
    // store the destructor.
    tType *tPtr = new tType(std::forward<tArguments &&>(aArguments)...);

    mMemory = Storage(reinterpret_cast<byte*>(tPtr), GenericDestructByte<tType>);

    return reinterpret_cast<tType*>(mMemory.get());
  }

  template <typename tType, typename... tArguments>
  tType* ConstructAndGetIfNotAlready(tArguments &&...aArguments)
  {
    if (mMemory.get_deleter() != GenericDestructByte<tType>)
    {
      return ConstructAndGet<tType>(std::forward<tArguments &&>(aArguments)...);
    }

    return Get<tType>();
  }

  template <typename tType>
  tType* Get()
  {
    return reinterpret_cast<tType*>(mMemory.get());
  }

private:
  Storage mMemory;
};


namespace detail
{
  template <typename tReturn, typename Arg = tReturn>
  struct GetReturnType {};

  template <typename tReturn, typename tObject, typename ...tArguments>
  struct GetReturnType<tReturn(tObject::*)(tArguments...)>
  {
    using ReturnType = tReturn;
    using tObjectType = tObject;
  };

  template <typename tIteratorType>
  struct iterator
  {
    iterator(tIteratorType &aIterator)
      : mPair(aIterator, 0)
    {

    }

    iterator& operator++()
    {
      ++mPair.first;
      ++mPair.second;

      return *this;
    }

    auto& operator*()
    {
      return mPair;
    }

    template <typename tOtherIterator>
    bool operator!=(const iterator<tOtherIterator> aRight)
    {
      return mPair.first != aRight.mPair.first;
    }

  private:
    std::pair<tIteratorType, size_t> mPair;
  };

  template <typename tIteratorBegin, typename tIteratorEnd = tIteratorBegin>
  struct range
  {
  public:
    range(tIteratorBegin aBegin, tIteratorEnd aEnd)
      : mBegin(aBegin)
      , mEnd(aEnd)
    {

    }

    bool IsRange() { return mBegin != mEnd; }

    iterator<tIteratorBegin> begin() { return mBegin; }
    iterator<tIteratorEnd> end() { return mEnd; }

  private:
    iterator<tIteratorBegin> mBegin;
    iterator<tIteratorEnd> mEnd;
  };
}

template <typename tType>
auto enumerate(tType &aContainer)
{
  detail::range<decltype(aContainer.begin()), decltype(aContainer.end())> toReturn{ aContainer.begin(), aContainer.end() };

  return toReturn;
}

#endif
