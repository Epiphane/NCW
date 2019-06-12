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
//    mButton.OnClick() >> OnMessage([]() { printf("Tap received!"); };
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
//    ObservableFromVector({1, 2, 3}) >>
//       Map<int, double>([](int inputInt) { return inputInt / 2.0; });
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

   return newObservable->MessageProducer();
}

//
// Filter an Observable with a function, returning a new Observable that only
//    lets through messages that pass a conditional
//
// Example usage:
//    ObservableFromVector({1, 2, 3}) >>
//       Filter<int>([](int someInt) { return someInt % 2 == 0; )
//
template<typename T>
struct Filter {
   explicit Filter(std::function<bool(T)> filterer)
         : filterer(filterer)
   {}

   std::function<bool(T)> filterer;
};

template<typename T>
Observable<T>& operator>>(Observable<T>& inObservable, const Filter<T>& filter)
{
   std::shared_ptr<ObservableInternal<T>> newObservable = std::make_shared<ObservableInternal<T>>();
   inObservable.AddOwnedObservable(newObservable);

   inObservable.AddObserverWithoutDisposer([=](T message) {
      if (filter.filterer(message)) {
         newObservable->SendMessage(message);
      }
   });

   return newObservable->MessageProducer();
}

//
// Combine two Observables' messages together. Both Observables should be the
//    same type.
//
// Example usage:
//    Merge(mButtonA.OnClick(), mButtonB.OnClick()) >>
//       OnMessage(handleEitherButtonClicked)
//
// TODO-EF: Someday, make this use variadic args so it can merge any # of Observables.
//
template<typename T>
Observable<T>& Merge(Observable<T>& firstObs, Observable<T>& secondObs) 
{
   std::shared_ptr<ObservableInternal<T>> newObservable = std::make_shared<ObservableInternal<T>>();
   firstObs.AddOwnedObservable(newObservable);
   secondObs.AddOwnedObservable(newObservable);
   
   firstObs.AddObserverWithoutDisposer([=](T message) {
      newObservable->SendMessage(message);
   });
   
   secondObs.AddObserverWithoutDisposer([=](T message) {
      newObservable->SendMessage(message);
   });
   
   return newObservable->MessageProducer();
}
   
//
// Only get messages when the Observable value changes (includes the first message sent)
//
// Example usage:
//    mToggleButton.OnToggleStateChanged() >> 
//       RemoveDuplicates();
//
struct RemoveDuplicates {};
   
template<typename T>
Observable<T>& operator>>(Observable<T>& inObservable, RemoveDuplicates distincter)
{
   std::unique_ptr<Observable_RemoveDuplicates<T>> newRemoveDuplicatesObservable = std::make_unique<Observable_RemoveDuplicates<T>>();
   std::shared_ptr<ObservableInternal<T>> newObservable = std::make_shared<ObservableInternal<T>>(std::move(newRemoveDuplicatesObservable));
   
   inObservable.AddOwnedObservable(newObservable);
   
   inObservable.AddObserverWithoutDisposer([=](T message) {
      newObservable->SendMessage(message);
   });
   
   return newObservable->MessageProducer();
}
 
//
// New listeners will receive a given message first, then continue with the rest
//    of the messages as normal.
//
// Example usage:
//    mButton.OnClick() >>
//       StartWith(someClick);
//
template<typename T>
struct StartWith {
   explicit StartWith(T message)
   : startingMessage(message) 
   {}
   
   T startingMessage;
};
   
template<typename T>
Observable<T>& operator>>(Observable<T>& inObservable, StartWith<T> starter)
{
   std::unique_ptr<Observable_StartWith<T>> newStartWithObservable = std::make_unique<Observable_StartWith<T>>();
   newStartWithObservable->SetStarterMessage(starter.startingMessage);
   
   std::shared_ptr<ObservableInternal<T>> newObservable = std::make_shared<ObservableInternal<T>>(std::move(newStartWithObservable));
   
   inObservable.AddOwnedObservable(newObservable);
   inObservable.AddObserverWithoutDisposer([=](T message) {
      newObservable->SendMessage(message);
   });
   
   return newObservable->MessageProducer();
}

//
// Send an Observable's messages into a container that has a push_back function
//
// Example usage:
//    std::vector<Point> points;
//    mButton.OnClick() >>
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
      OnMessage<T>([=](T message) {
         containerOwner.container.push_back(message);
      }, containerOwner.owner);

   return inObservable;
}

} // namespace Observables

} // namespace CubeWorld
