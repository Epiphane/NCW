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
   
   class DisposeBag; // Forward declare
   
//   class Disposable {
//   public:
//      void Dispose();
//   };
   
   class DisposeBag {
   public:
      //      void AddDisposableObject(MessageProducer* fart) {
      //         
      //      }
      void AddOnDispose(std::function<void(void)>);
      
   private:
      //      std::vector<
   };
   
   template<typename T>
   class MessageReceiver;
   
   template<typename T>
   class MessageProducer
   {
   public:
      friend class MessageReceiver<T>;
      
      ~MessageProducer() {
         
      }
      
      // See DisposeBag pattern here: http://adamborek.com/memory-managment-rxswift/
      void Subscribe(std::function<void(T)> onMessage, std::weak_ptr<DisposeBag> weakOwner) {
         // The problem: 
         //    We sort of want each Subscription to have "2 parents," so to speak.
         //    That is, if the owner dies, the Subscription should die, and if
         //    the MessageProducer dies, the Subscription should also die.
         //
         // How about:
         //    map<Disposer, MessageCallback> callbacks as a member here; that way callbacks
         //    will simply die with MessageProducer. Then, we have owner->AddThingToDispose(me)
         //    which makes the Disposer call RemoveMe(me) on this MessageProducr. Nice!
         //    Need to watch out for what happens when we remove a Subscription on myself through an
         //    Emit call?
         //
         auto strongOwner = weakOwner.lock();
         mListeners.insert(strongOwner, onMessage);
         
         // When the owner is deinitted, it will also erase the callbacks that it owns.
         strongOwner->AddOnDispose([&]() {
            mListeners.erase(weakOwner);
         });
      }
      
      template<typename U>
      std::unique_ptr<MessageProducer> Map(std::function<U(T)> mapper) {
         std::unique_ptr<MessageProducer<U>> newProducer = std::make_unique<MessageProducer<U>>();
         this->Subscribe([&](T message) {
            U mappedMessage = mapper(message);
            newProducer->Emit(mappedMessage);
         }, this);
         return std::move(newProducer);
      }
      
      MessageProducer Filter(std::function<bool(T)> filterer) {
         
         return this;
      }
      
   private:
      template<typename U>
      MessageProducer CreateMessageProducerWithMapper(std::function<U(T)> mapper) {
         
      }
                                
      void Emit(T message) {
         // Make a copy of mListeners in case a listener is deleted by this event
         //    being emitted.
         auto listenersCopy = mListeners;
         
         for (const auto& [disposeBag, subscriptionCallback] : mListeners) {
            // If a listener was deleted, don't send it a message
            if (disposeBag) {
               subscriptionCallback(message);
            }
         };
      }
      
      std::map<std::weak_ptr<DisposeBag>, std::function<void(T)>> mListeners;
   };
   
   template<typename T>
   class MessageReceiver
   {
   public:
      void SendMessage(T message) {
         mp.Emit(message);
      };
      
   private:
      MessageProducer<T> mp;
   };
   
   template<typename IN_T, typename OUT_T>
   class MessagePipe
   {
      MessageReceiver<IN_T> mInput;
      MessageProducer<OUT_T> mOutput;
   };
   

// My idea:
//  These "Observable" values live in the model or the VC.
//  The view can have a ref to them.
//  When one side changes the Observable value, the other
//    side gets a callback that the value has been updated.
//  (also you can have more than 2 parties involved in this
//    Observable)
//  Could these be something complicated like a UIConstraint??
//    I'm not sure! Let's find out!
template<typename T>
class Observable
{
public:
   typedef std::function<void(T)> ValueChangedCallback;

   T GetData() const { return internalData; };
   void SetData(T t) {
      internalData = t;

      for (auto callback : mCallbacks) {
         callback(t);
      }
   };

   void AddCallback(ValueChangedCallback newCallback)
   {
      mCallbacks.push_back(newCallback);
   };

private:
   T internalData;
   std::vector<ValueChangedCallback> mCallbacks;
};

} // namespace CubeWorld
