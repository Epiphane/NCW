//
// Observable.h
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include <map>
#include <vector>

// Convenience macro to declare an Observable.
//    !!! MUST BE USED IN THE 'PRIVATE' SECTION OF CLASSES !!!
//
// Example:
//
// DECLARE_OBSERVABLE(bool, Active, OnActiveChanged)   expands into ->
// 
//    Observables::ObservableInternal<bool> mActive;
// public:
//    Observable::Observable<bool>& OnActiveChanged() 
//    {
//       return mActive.MessageProducer();
//    }
// private:
//    ...Class members continue here
//
#define DECLARE_OBSERVABLE(TYPE, NAME, GETTER_NAME)  \
   Observables::ObservableInternal<TYPE> NAME;\
public:\
   Observables::Observable<TYPE>& GETTER_NAME() {\
      return NAME.MessageProducer();\
   }\
private:


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
   
   template<typename T>
   class ObservableInternal; // Forward declare
   
   /**
    * An Observable is something that spits out messages. You can set a
    *    callback to react to these messages, and set up mapping functions
    *    to turn these messages into different messages.
    *
    * Generally you would declare an ObservableInternal as a private/protected
    *    class field, and expose the corresponding Observable through a public
    *    method.
    */
   template<typename T>
   class Observable : public DisposeBag
   {
   public:
      friend class ObservableInternal<T>;
      
      ~Observable() {
         for (const auto& [weakBag, _] : mBaggedObservers) {
            auto strongBag = weakBag.lock();
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
         mBaggedObservers.insert(std::pair(weakBag, onMessage));
         
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
      
   protected:                     
      virtual void SendMessageToObservers(T message) {
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
    * ObservableInternal is the class that lets you actually emit messages.
    *    Generally you would have an ObservableInternal as a private class member
    *    and expose the MessageProducer() method so other classes can observe
    *    your messages.
    */
   template<typename T>
   class ObservableInternal
   {
   public:
      ObservableInternal()
         : observableExternal(std::make_unique<Observable<T>>())
      {}
      
      ObservableInternal(std::unique_ptr<Observable<T>> providedObservable)
         : observableExternal(std::move(providedObservable))
      {}
      
      virtual void SendMessage(T message) {
         observableExternal->SendMessageToObservers(message);
      };
      
      Observable<T>& MessageProducer() {
         return *(observableExternal.get());
      }
      
   private:
      std::unique_ptr<Observable<T>> observableExternal;
   };
   
   /**
    * Subclass that can remember the last message it sent, as well as if it has
    *    ever sent a message before.
    *
    * Will only emit messages DIFFERENT from the most recent message. Will also emit
    *    if this is the first message it's asked to emit. 
    *
    * NOTE: You probably shouldn't make a instance of this class yourself. Use
    *          ObservableBasicOperations::Distinct()
    */
   template<typename T>
   class Observable_Distinct : public Observable<T>
   {
   public:
      virtual void SendMessageToObservers(T message) override {
         if (!mDidSendFirstMessage) {
            mDidSendFirstMessage = true;
            
            Observable<T>::SendMessageToObservers(message);
            mMostRecentMessage = message;
         } else if (mMostRecentMessage != message) {
            Observable<T>::SendMessageToObservers(message);
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
         this->SendMessageToObservers(mStarterMessage);
      }
      
      virtual void AddObserverWithoutDisposer(std::function<void(T)> onMessage) override {
         Observable<T>::AddObserverWithoutDisposer(onMessage);
         this->SendMessageToObservers(mStarterMessage);
      }
      
      void SetStarterMessage(T starterMessage) {
         mStarterMessage = starterMessage;
      }
      
   private:
      T mStarterMessage;
   };
   
} // namespace Observables
   
} // namespace CubeWorld
