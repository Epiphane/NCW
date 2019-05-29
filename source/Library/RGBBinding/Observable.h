//
// Observable.h
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

namespace CubeWorld
{
   
   // e.g. when the user mousedowns a scrubber, state is STARTING.
   //    As they mousemove, state is CHANGING.
   //    When they mouseup, state is ENDING.
   enum InputState {
      STARTING, CHANGING, ENDING
   };
   
   template<typename T>
   struct Message_InputChanged {
      InputState state;
      T value;
   };
   
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
   class Observable final : public DisposeBag
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
      void AddObserver(std::function<void(T)> onMessage, std::weak_ptr<DisposeBag> weakBag) {
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
      void AddObserverWithoutDisposer(std::function<void(T)> onMessage) {
         mUnbaggedObservers.push_back(onMessage);
      }
      
      void AddOwnedObservable(std::shared_ptr<void> observable) {
         mOwnedObservables.push_back(observable);
      }
      
   private:                     
      void SendMessageToObservers(T message) {
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
    *    and expose the OnChanged() method so other classes can observe
    *    your messages.
    */
   template<typename T>
   class ObservableInternal final
   {
   public:
      void SendMessage(T message) {
         observableExternal.SendMessageToObservers(message);
      };
      
      Observable<T>& OnChanged() {
         return observableExternal;
      }
      
   private:
      Observable<T> observableExternal;
   };
   
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
   //       Map<Point, bool>([](Point dragPoint) { return someElement.contains(dragPoint); };
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
   
   

} // namespace CubeWorld
