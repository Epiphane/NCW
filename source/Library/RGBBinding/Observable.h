//
// Observable.h
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include <map>
#include <vector>

namespace CubeWorld
{
   
namespace Observables
{
   
/**
 * A DisposeBag holds onto some callbacks, and when it dies, it calls all the
 *    callbacks. The idea is that you can toss some "disposables" into the bag,
 *    and they will all get correctly dealloced when the bag deallocs.
 *
 * Also, if the object dies before the bag, it should be removed from the bag
 *    with RemoveOnDispose().
 *
 * See DisposeBag pattern here: http://adamborek.com/memory-managment-rxswift/
 */
class DisposeBag {
public:
   void AddOnDispose(std::function<void(void)> newCallback, void* callbackOwner) 
   {
      mDisposalCallbacks.insert(std::make_pair(callbackOwner, newCallback));
   }
   
   void RemoveOnDispose(void* callbackOwner) 
   {
      mDisposalCallbacks.erase(callbackOwner);
   }
   
   ~DisposeBag() 
   {
      for (const auto& [_, callback] : mDisposalCallbacks) {
         callback();
      }
   }
   
private:
   // Key is the address of the object we're calling the callback for,
   //    value is all the disposal callbacks for that object.
   //
   // We use this to ensure we never call a callback to a dead object.
   std::multimap<void*, std::function<void(void)>> mDisposalCallbacks;
};

/**
 * An Observable is something that spits out messages. You can set a
 *    callback to react to these messages, and set up mapping functions
 *    to turn these messages into different messages.
 */
template<typename T>
class Observable : public DisposeBag
{
public:
   virtual ~Observable() {
      for (const auto& weakBag : mBaggedObservers) {
         auto strongBag = weakBag.first.lock();
         if (strongBag) {
            strongBag->RemoveOnDispose(this);  
         }
      }
   }
   
   //
   // Add an observer callback that will be called for every message sent by this observer.
   //    With this method you specify a DisposeBag. When the DisposeBag dies, the link
   //    between this observer and the callback will be cleaned up.
   //
   virtual void AddObserver(std::function<void(T)> onMessage, std::weak_ptr<DisposeBag> weakBag) {
      auto strongBag = weakBag.lock();
      mBaggedObservers.insert(std::make_pair(weakBag, onMessage));
      
      strongBag->AddOnDispose([&]() {
         mBaggedObservers.erase(weakBag);
      }, this);
   }
   
   //
   // By using this function, you're promising that whatever is subscribing will outlive this
   //    Observable. That is, without specifying an owner, the only way the observer will
   //    be cleaned up is when this Observable dies.
   //
   virtual void AddObserverWithoutDisposer(std::function<void(T)> onMessage) {
      mUnbaggedObservers.push_back(onMessage);
   }
   
   void AddOwnedObservable(std::shared_ptr<void> observable) {
      mOwnedObservables.push_back(observable);
   }
                
   virtual void SendMessage(T message) {
      // Make a copy of mBaggedListeners in case a listener is deleted by this event
      //    being emitted.
      auto observersCopy = mBaggedObservers;
      
      for (const auto& observer : observersCopy) {
         // If a listener was deleted while iterating, don't send it a message
         if (!observer.first.expired()) {
            observer.second(message);
         }
      };
      
      auto unbaggedObserversCopy = mUnbaggedObservers;
      for (const auto& onMessageCallback : unbaggedObserversCopy) {
         onMessageCallback(message);
      }
   }
   
protected:        
   // std::map needs to know how to compare weak_ptrs, we use the built-in owner_less function.
   typedef std::owner_less<std::weak_ptr<DisposeBag>> weak_less;
   
   // Key is a weak_ptr to a dispose bag, value is a message-receiving callback that is
   //    owned by that bag.
   std::multimap<std::weak_ptr<DisposeBag>, std::function<void(T)>, weak_less> mBaggedObservers;
   
   // Observer callbacks that do not have a corresponding bag.
   std::vector<std::function<void(T)>> mUnbaggedObservers;
   
   // When you call Observable.Map<U> or any other operator, it creates a new observable.
   //    The new baby observable's lifecycle is managed here.
   std::vector<std::shared_ptr<void>> mOwnedObservables;
};

/**
 * Pipe the messages from one Observable into another. The 'sink' will forward
 *    all the messages it receives.
 */
template<typename T>
Observable<T>& operator>>(Observable<T>& producer, Observable<T>& sink) 
{
   // TODO-EF: Lifecycle problem here. Will need to work out later.
   producer.AddObserverWithoutDisposer([&](T message) {
      sink.SendMessage(message);
   });
   
   return sink;
}
   
/**
 * Subclass that takes the last 2 messages it received and combines them
 *    into a new message.
 *
 * If it hasn't received both a T and a U message yet, it won't send any
 *    messages.
 *
 * NOTE: You probably shouldn't make a instance of this class yourself. Use
 *          ObservableBasicOperations::CombineLatest(...)
 */
template<typename T, typename U>
class Observable_CombineLatest : public Observable<std::tuple<T, U>>
{
public:
   void SendType1Message(T message) {
      mMostRecentTMessage = message;
      mbHasReceivedTMessage = true;
      
      if (!mbHasReceivedUMessage) {
         return;
      }
      
      this->SendMessage(std::make_tuple(message, mMostRecentUMessage));
   }
   
   void SendType2Message(U message) {
      mMostRecentUMessage = message;
      mbHasReceivedUMessage = true;
      
      if (!mbHasReceivedTMessage) {
         return;
      }
      
      this->SendMessage(std::make_tuple(mMostRecentTMessage, message));
   }
   
protected:
   bool mbHasReceivedTMessage = false;
   T mMostRecentTMessage;
   
   bool mbHasReceivedUMessage = false;
   U mMostRecentUMessage;
};

/**
 * Subclass that can remember the last message it sent, as well as if it has
 *    ever sent a message before.
 *
 * Will only emit messages DIFFERENT from the most recent message. Will also emit
 *    if this is the first message it's asked to emit. 
 *
 * NOTE: You probably shouldn't make a instance of this class yourself. Use
 *          ObservableBasicOperations::RemoveDuplicates()
 */
template<typename T>
class Observable_RemoveDuplicates : public Observable<T>
{
public:
   virtual void SendMessage(T message) override {
      if (!mDidSendFirstMessage) {
         mDidSendFirstMessage = true;
         
         Observable<T>::SendMessage(message);
         mMostRecentMessage = message;
      } else if (mMostRecentMessage != message) {
         Observable<T>::SendMessage(message);
         mMostRecentMessage = message;
      }
   }
   
private:
   T mMostRecentMessage;
   bool mDidSendFirstMessage = false;
};

/**
 * Subclass that can store a message that all new listeners should immediately
 *    receive when they start listening. 
 *
 * NOTE: You probably shouldn't make a instance of this class yourself. Use
 *          ObservableBasicOperations::StartWith()
 */
template<typename T>
class Observable_StartWith : public Observable<T>
{
public:
   virtual void AddObserver(std::function<void(T)> onMessage, std::weak_ptr<DisposeBag> weakBag) override {
      Observable<T>::AddObserver(onMessage, weakBag);
      this->SendMessage(mStarterMessage);
   }
   
   virtual void AddObserverWithoutDisposer(std::function<void(T)> onMessage) override {
      Observable<T>::AddObserverWithoutDisposer(onMessage);
      this->SendMessage(mStarterMessage);
   }
   
   void SetStarterMessage(T starterMessage) {
      mStarterMessage = starterMessage;
   }
   
private:
   T mStarterMessage;
};
   
} // namespace Observables
   
} // namespace CubeWorld
