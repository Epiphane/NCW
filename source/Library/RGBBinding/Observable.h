//
// Observable.h
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include <map>
#include <vector>
#include <typeinfo> // TODO-EF: deletarino
#include <string>

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
      mDisposalCallbacks.insert(std::pair(callbackOwner, newCallback));
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
   Observable() {}
   
   // This constructor lets you start with a default message that new subscribers will receive
   explicit Observable(T defaultMessage) {
      mMostRecentMessage = defaultMessage;
      mbHasEmittedOnce = true;
   }
   
   virtual ~Observable() {
      // elliot: wait this doesn't make any sense :(((
      //          mBaggedObservers contains all the people subscribed TO ME.
      //          why would I be looking at people subscribed to me and remove them from
      //          myself...
      //
      //       uhhhh
//      for (const auto& [weakBag, _] : mBaggedObservers) {
//         auto strongBag = weakBag.lock();
//         if (strongBag) {
//            strongBag->RemoveOnDispose(this);  
//         }
//      }
      mBaggedObservers.clear();
//      RemoveOnDispose(this);
//      mDisposalCallbacks.clear();
      // harley davidson
   }

   //
   // Add an observer callback that will be called for every message sent by this observer.
   //    With this method you specify a DisposeBag. When the DisposeBag dies, the link
   //    between this observer and the callback will be cleaned up.
   //
   virtual void AddObserver(std::function<void(T)> onMessage, std::weak_ptr<DisposeBag> weakBag) {
      auto strongBag = weakBag.lock();
      mBaggedObservers.insert(std::pair(weakBag, onMessage));
      
      if (mbHasEmittedOnce) {
         onMessage(mMostRecentMessage);
      }
      
      strongBag->AddOnDispose([&]() {
//         if (mBaggedObservers.count(weakBag) == 0) {
////            assert(false && "What's all this then???");
//            // meh
//         }
//         else {
//            mBaggedObservers.erase(weakBag);
//         }
      }, this);
   }
   
   //
   // By using this function, you're promising that whatever is subscribing will outlive this
   //    Observable. That is, without specifying an owner, the only way the observer will
   //    be cleaned up is when this Observable dies.
   //
   virtual void AddObserverWithoutDisposer(std::function<void(T)> onMessage) {
      mUnbaggedObservers.push_back(onMessage);
      
      if (mbHasEmittedOnce) {
         onMessage(mMostRecentMessage);
      }
   }
   
   void AddOwnedObservable(std::shared_ptr<void> observable) {
      mOwnedObservables.push_back(observable);
   }
                
   virtual void SendMessage(T message) {
      mbHasEmittedOnce = true;
      mMostRecentMessage = message;
      
      // Make a copy of mBaggedListeners in case a listener is deleted by this event
      //    being emitted.
      auto observersCopy = mBaggedObservers;
      
      for (const auto& [weakBag, onMessageCallback] : observersCopy) {
         // If a listener was deleted while iterating, don't send it a message
         if (!weakBag.expired()) {
            onMessageCallback(message);
         }
      };
      
      auto unbaggedObserversCopy = mUnbaggedObservers;
      for (const auto& onMessageCallback : unbaggedObserversCopy) {
         onMessageCallback(message);
      }
   }
   
   bool HasEmittedOnce() const { return mbHasEmittedOnce; }
   
   T GetMostRecentMessage() const {
      assert(mbHasEmittedOnce && "Asking for the most recent message before there is one!!");
      return mMostRecentMessage; 
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
   
   // When a new Observer is attached, we send it the most recent message
   T mMostRecentMessage;
   
   // Tracks whether this Observable has ever emitted a message 
   bool mbHasEmittedOnce = false;
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

//template<typename Last>
//class Observable_CombineLatest : public Observable<std::tuple<Last>>
//{
//public:
//   Observable_CombineLatest(Observable<T> cool, T... args)
//   {
//      cool >>
//         OnMessage([&](T message) {
//            
//         });
//   }
   
//protected:
//   
//};
   
//template<typename First, typename Second, typename ...Rest>
//class Observable_CombineLatest : public Observable<std::tuple<First, Second, Rest...>>
//{
//   
//};

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
      if (!this->mbHasEmittedOnce || this->mMostRecentMessage != message) {
         Observable<T>::SendMessage(message);
      }
   }
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
