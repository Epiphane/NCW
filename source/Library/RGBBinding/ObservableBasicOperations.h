//
// ObservableBasicOperations.h
//
// This file created by the ELLIOT FISKE gang
//

#pragma once


namespace CubeWorld
{

namespace Observables
{
 
//
// Simple callback when a message is sent.
//
// Example usage:
//    mButton.OnTap() >> OnMessage([]() { printf("Nice tap"); };
//
template<typename T>
struct OnMessage {
   explicit OnMessage(std::function<void(T)> callback, std::weak_ptr<DisposeBag> owner)
         : callback(callback)
         , owner(owner)
   {}

   std::function<void(T)> callback;
   std::shared_ptr<DisposeBag> owner;
};

template<typename T>
Observable<T>& operator>>(Observable<T>& inObservable, const OnMessage<T>& onMessage)
{
   inObservable.AddObserver(onMessage.callback, onMessage.owner);
   return inObservable;
}

//
// Transform an Observable with a function, returning a new Observable.
//
// Example usage:
//    mButton.OnPan() >>
//       Map<Point, bool>([](Point dragPoint) { return someElement.contains(dragPoint); });
//
template<typename T, typename U>
struct Map {
   explicit Map(std::function<U(T)> mapper)
         : mMapper(mapper)
   {}

   std::function<U(T)> mMapper;
};

template<typename T, typename U>
Observable<U>& operator>>(Observable<T>& inObservable, const Map<T, U>& mapper)
{
   std::shared_ptr<ObservableInternal<U>> newObservable = std::make_shared<ObservableInternal<U>>();
   inObservable.AddOwnedObservable(newObservable);

   inObservable.AddObserverWithoutDisposer([=](T message) {
      U newMessage = mapper.mMapper(message);
      newObservable->SendMessage(newMessage);
   });

   return newObservable->OnChanged();
}
   
//
// Send an Observable's messages into a container that has a push_back function
//
// Example usage:
//    std::vector<Point> points;
//    mButton.OnTap() >>
//       ToContainer(points};
//
template<typename ContainerType>
struct ToContainer {
   explicit ToContainer(ContainerType& newContainer, std::shared_ptr<DisposeBag> newOwner)
      : container(newContainer)
      , owner(newOwner)
   {}
   
   ContainerType& container;
   std::shared_ptr<DisposeBag> owner;
};

template<typename T, typename ContainerType>
Observable<T>& operator>>(Observable<T>& inObservable, const ToContainer<ContainerType>& containerOwner)
{
   inObservable >> 
      OnMessage<T>([&](T message) {
         containerOwner.container.push_back(message);
      }, containerOwner.owner);
   
   return inObservable;
}

} // namespace Observables
   
} // namespace CubeWorld
