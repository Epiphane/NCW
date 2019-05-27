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
   
   /*
    Example usage I wanna see:
    
    SomeUIElement::SomeUIElement() {
      myTap.OnTap().Subscribe([&](Message_TapGesture tap) {
         // do something with tap
         this->SetBackgroundColor(red);
      }, mDisposer);
    
      // When either the gesture recognizer dies (obv not gonna happen here but whatever)
      //   or the SomeUIElement dies, we want no remnant of the subscription to stick around. 
    
    */
   
//   template<typename T>
//   class Observable; // Forward declare
   
   /**
    * A DisposeBag holds onto some callbacks, and when it dies, it calls all the
    *    callbacks. The idea is that you can toss some "disposables" into the bag,
    *    and they will all get correctly dealloced when the bag deallocs.
    *
    * Also, if the object dies before the bag, it is removed from the bag.
    *
    * See DisposeBag pattern here: http://adamborek.com/memory-managment-rxswift/
    */
   class DisposeBag {
   public:
      void AddOnDispose(std::function<void(void)> newCallback, void* callbackOwner) 
      {
         mDisposalCallbacks.emplace(callbackOwner, newCallback);
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
   class ObservableInternal;
   
   // IDK if this is a good idea
   class BaseObservable {
   };
   
   template<typename T>
   class Observable : public DisposeBag, public BaseObservable
   {
   public:
      friend class ObservableInternal<T>;
      
      ~Observable() {
         for (const auto& [weakBag, _] : mListeners) {
            auto strongBag = weakBag.lock();
            if (strongBag) {
               strongBag->RemoveOnDispose(this);  
            }
         }
      }
      
      // notes: You can specify weakOwner as nullptr; in that case, the only time this subscription
      //          will be disposed is if the Observable itself dies. Useful if you can guarentee
      //          the subscriber will outlive the Observable.
      void Subscribe(std::function<void(T)> onMessage, std::weak_ptr<DisposeBag> weakOwner) {
         auto strongOwner = weakOwner.lock();
         mListeners.emplace(weakOwner, onMessage);
         
         // When the owner is deinitted, it will also erase the callbacks that it owns.
         if (strongOwner != nullptr) {
            strongOwner->AddOnDispose([&]() {
               mListeners.erase(weakOwner);
            }, this);
         }
      }
      
   public:
      template<typename U>
      Observable<U>& Map(std::function<U(T)> mapper) {
         std::shared_ptr<ObservableInternal<U>> result = std::make_shared<ObservableInternal<U>>();
         
         mOwnedObservables.push_back(result);
         
         Subscribe([&](T message) {
            U newMessage = mapper(message);
            result->Emit(newMessage);
         }, std::shared_ptr<DisposeBag>(nullptr));
         
         return *result.get();
      }
      
      Observable Filter(std::function<bool(T)> filterer) {
         
         return this;
      }
      
      void AddOwnedObservable(std::shared_ptr<void> observable) {
         mOwnedObservables.push_back(observable);
      }
      
   private:                     
      void Emit(T message) {
         // Make a copy of mListeners in case a listener is deleted by this event
         //    being emitted.
//         auto listenersCopy = mListeners;
         
         for (const auto& [weakBag, subscriptionCallback] : mListeners) {
            // If a listener was deleted, don't send it a message
//            if (!weakBag.expired()) {
               subscriptionCallback(message);
//            }
         };
      }
      
      // std::map needs to know how to compare weak_ptrs, we use the built-in owner_less function.
      typedef std::owner_less<std::weak_ptr<DisposeBag>> weak_less;
      
      // Key is a weak_ptr to a dispose bag, value is a message-receiving callback that is
      //    owned by that bag.
      std::multimap<std::weak_ptr<DisposeBag>, std::function<void(T)>, weak_less> mListeners;
      
      // When you call Observable.Map<U>, it creates a new observable of type U.
      //    The new baby observables' lifecycles are managed here.
      std::vector<std::shared_ptr<void>> mOwnedObservables;
   };
   
   template<typename T>
   class ObservableInternal
   {
   public:
      void SendMessage(T message) {
         mp.Emit(message);
      };
      
      Observable<T>& OnChanged() {
         return mp;
      }
      
   private:
      Observable<T> mp;
   };
   
   template<typename T, typename U>
   struct Map {
      explicit Map(std::function<U(T)> mapper)
      : mMapper(mapper) 
      {}
      
      std::function<U(T)> mMapper;
   };
   
   template<typename T, typename U>
   Observable<U>& operator<<(Observable<T>& inObservable, const Map<T, U>& mapper) 
   {
      std::shared_ptr<ObservableInternal<U>> newObservable = std::make_shared<ObservableInternal<U>>();
      inObservable.AddOwnedObservable(newObservable);
      
      inObservable.Subscribe([=](T message) {
         U newMessage = mapper.mMapper(message);
         newObservable->SendMessage(newMessage);
      }, std::shared_ptr<DisposeBag>(nullptr));
      
      return newObservable->OnChanged();
   }
   
   

} // namespace CubeWorld
