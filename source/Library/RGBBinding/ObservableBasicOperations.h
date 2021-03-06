//
// ObservableBasicOperations.h
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include "Observable.h"

#undef SendMessage

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
   std::shared_ptr<Observable<U>> newObservable = std::make_shared<Observable<U>>();
   inObservable.AddOwnedObservable(newObservable);

   inObservable.AddObserver([=](T message) {
      U newMessage = mapper.mMapper(message);
      newObservable->SendMessage(newMessage);
   }, newObservable);

   return *newObservable;
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
   std::shared_ptr<Observable<T>> newObservable = std::make_shared<Observable<T>>();
   inObservable.AddOwnedObservable(newObservable);

   inObservable.AddObserver([=](T message) {
      if (filter.filterer(message)) {
         newObservable->SendMessage(message);
      }
   }, newObservable);

   return *newObservable;
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
   std::shared_ptr<Observable<T>> newObservable = std::make_shared<Observable<T>>();
   firstObs.AddOwnedObservable(newObservable);
   secondObs.AddOwnedObservable(newObservable);
   
   firstObs.AddObserver([=](T message) {
      newObservable->SendMessage(message);
   }, newObservable);
   
   secondObs.AddObserver([=](T message) {
      newObservable->SendMessage(message);
   }, newObservable);
   
   return *newObservable;
}
   
/**
 * Operator that takes the latest messages it received and combines them
 *    into a new message.
 *
 * If any one of the source Observables has not emitted a message yet, this
 *    will not emit any messages.
 */
template <typename Last>
Observable<std::tuple<Last>>& CombineLatest(Observable<Last>& l) {
   std::shared_ptr<Observable<std::tuple<Last>>> result = std::make_shared<Observable<std::tuple<Last>>>();
   
   l.AddOwnedObservable(result);
   l.AddObserver([=](Last message) {
      result->SendMessage(std::make_tuple(message));
   }, result);
   
   return *result;
}

// SUPER WACKY IMPLEMENTATION :O
// C++ doesn't really let you loop through variadic template args. So, to support any # of arguments, we
//    recursively apply this template until there's only 1 argument left, which is applied to the "Last"
//    template above. For instance, when there are exactly 2 args left, ...Rest has a length of 0, and
//    both "First" and "Second" are handled by the "Last" specialization above.
//
// This (might??) end up non-performant, and the way I've seen other libraries handle this is just manually
//    specifying CombineLatest<1arg>, CombineLatest<1arg, 2arg>, ... etc. up to like 8 args.
template <typename First, typename Second, typename ...Rest>
Observable<std::tuple<First, Second, Rest...>>& CombineLatest(Observable<First>& f, Observable<Second>& s, Observable<Rest>&... r) {
   typedef Observable<std::tuple<First, Second, Rest...>> CombinedObservable;
   std::shared_ptr<CombinedObservable> result = std::make_shared<CombinedObservable>();
   std::weak_ptr<CombinedObservable> weakResult = result;
   
   Observable<std::tuple<Second, Rest...>>& restResult = CombineLatest(s, r...);
   Observable<std::tuple<First>>& singleResult = CombineLatest(f);
   
   restResult.AddOwnedObservable(result);
   singleResult.AddOwnedObservable(result);
   
   restResult.AddObserver([&, result](std::tuple<Second, Rest...> message) {
      if (singleResult.HasEmittedOnce()) {
//         if (!weakResult.expired()) {
//            weakResult.lock()->SendMessage(std::tuple_cat(singleResult.GetMostRecentMessage(), message));
//         }
         result->SendMessage(std::tuple_cat(singleResult.GetMostRecentMessage(), message));
      }
   }, result);
   
   singleResult.AddObserver([&, result](std::tuple<First> message) {
      if (restResult.HasEmittedOnce()) {
//         if (!weakResult.expired()) {
//            weakResult.lock()->SendMessage(std::tuple_cat(message, restResult.GetMostRecentMessage()));
//         }
         result->SendMessage(std::tuple_cat(message, restResult.GetMostRecentMessage()));
      }
   }, result);
   
   return *result;
}
   
//
// Only get messages when the Observable value changes (always includes the first message sent)
//
// Example usage:
//    mToggleButton.OnToggleStateChanged() >> 
//       RemoveDuplicates();
//
struct RemoveDuplicates {};
   
template<typename T>
Observable<T>& operator>>(Observable<T>& inObservable, RemoveDuplicates /*distincter*/)
{
   std::shared_ptr<Observable_RemoveDuplicates<T>> newObservable = std::make_shared<Observable_RemoveDuplicates<T>>();
   
   inObservable.AddOwnedObservable(newObservable);
   
   inObservable.AddObserver([=](T message) {
      newObservable->SendMessage(message);
   }, newObservable);
   
   return *newObservable;
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
   std::shared_ptr<Observable_StartWith<T>> newObservable = std::make_shared<Observable_StartWith<T>>();
   newObservable->SetStarterMessage(starter.startingMessage);
   
   inObservable.AddOwnedObservable(newObservable);
   
   inObservable.AddObserver([=](T message) {
      newObservable->SendMessage(message);
   }, newObservable);
   
   return *newObservable;
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
