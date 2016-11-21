#include <memory>
#include <nan.h>
#include <unistd.h>
#include <v8.h>
#include <kaa/Kaa.hpp>
#include <kaa/logging/Log.hpp>
#include <kaa/logging/LoggingUtils.hpp>
#include <kaa/notification/INotificationTopicListListener.hpp>
#include <kaa/common/exception/UnavailableTopicException.hpp>

#define Sleep(x) usleep((x)*1000)

#define DEBUG_KAA_CLIENT false

using namespace kaa;
using namespace Nan;

void emit_event(const KaaNotification& notification);

static void showTopicList(const Topics& topics)
{
    if (topics.empty()) {
        std::cout << "Topic list is empty" << std::endl;
    } else {
        for (const auto& topic : topics) {
            std::cout << (boost::format("Topic: id '%1%', name '%2%', type '%3%'")
                % topic.id % topic.name % LoggingUtils::TopicSubscriptionTypeToString(topic.subscriptionType)) << std::endl;
        }
    }
}

static std::list<std::int64_t> extractOptionalTopicIds(const Topics& topics) {
    std::list<std::int64_t> topicIds;
    for (const auto& topic : topics) {
        if (topic.subscriptionType == SubscriptionType::OPTIONAL_SUBSCRIPTION) {
            topicIds.push_back(topic.id);
        }
    }
    return topicIds;
}


// The listener which receives notifications on topics.
class BasicNotificationListener : public INotificationListener {
public:
    virtual void onNotification(const std::int64_t topicId, const KaaNotification& notification)
    {
        std::cout << (boost::format("Notification for topic id '%1%' received") % topicId) << std::endl;
        std::cout << (boost::format("Notification body: '%1%'")
            % (notification.message.is_null() ? "" : notification.message.get_string())) << std::endl;
        emit_event(notification);
    }
};

// A listener that tracks the notification topic list updates
// and subscribes the Kaa client to every new topic available.
class BasicNotificationTopicListListener : public INotificationTopicListListener {
    public:
        BasicNotificationTopicListListener(IKaaClient& kaaClient) : kaaClient_(kaaClient) {}

        virtual void onListUpdated(const Topics& topics)
        {
            if(DEBUG_KAA_CLIENT) {
                std::cout << ("Topic list was updated") << std::endl;
                showTopicList(topics);
            }

            try {
                auto optionalTopicIds = extractOptionalTopicIds(topics);
                if(DEBUG_KAA_CLIENT) {
                    for (const auto& id : optionalTopicIds) {
                        std::cout << (boost::format("Subscribing to optional topic '%1%'") % id) << std::endl;
                    }
                }

                kaaClient_.subscribeToTopics(optionalTopicIds);
            } catch (UnavailableTopicException& e) {
                std::cout << (boost::format("Topic is unavailable, can't subscribe: %1%") % e.what()) << std::endl;
            }
        }

    private:
        IKaaClient& kaaClient_;
};

class MyObject : public ObjectWrap {
 public:
  static NAN_MODULE_INIT(Init);

 private:
  MyObject() {}
  ~MyObject() {}

  static NAN_METHOD(New);
  static NAN_METHOD(StartLoop);
  static Persistent<v8::Function> constructor;
};

struct async_event_data 
{
    uv_work_t request;
    std::string *event_name;
    std::string *someArg;
};

Persistent<v8::Function> MyObject::constructor;

NAN_MODULE_INIT(MyObject::Init) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New<v8::String>("MyObject").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    SetPrototypeMethod(tpl, "startLoop", StartLoop);

    constructor.Reset(tpl->GetFunction());
    Set(target, Nan::New("MyObject").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(MyObject::New) {
    if (!info.IsConstructCall())
        return Nan::ThrowError("`new` required");
  
    MyObject* obj = new MyObject();
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
}

MyObject* myObjectThreadLoop;
uv_thread_t mythread;
int threadnum = 1;

struct SendMessage
{
    // required
    uv_work_t request;                  // libuv

    // optional : data goes here.
    // data that doesn't go back to javascript can be any typedef
    // data that goes back to javascript needs to be a supported type
    int         seconds;
    std::string *greeting;
};

NAN_INLINE void noop_execute (uv_work_t* req) {}

NAN_INLINE void callback_async_event (uv_work_t* req) {
    Nan::HandleScope scope;

    SendMessage *sendMessage = static_cast<SendMessage *>(req->data);

    // set up return arguments
//    Handle<Value> argv[] =
//        {
//            Handle<Value>(String::New(baton->greeting))
//        };

    // execute the callback
//    SendMessage->callback->Call(Context::GetCurrent()->Global(),2,argv);

    // dispose the callback object from the baton
//    baton->callback.Dispose();

    // delete the baton object
//    delete baton;

     
    // here we are executing in the node thread, so we can call nan and v8 functions
    // we could retrieve some data that was passed around
//    std::cout << "================================" <<std::endl; 
//    std::cout << *(sendMessage->greeting) <<std::endl; 
//    std::cout << "--------------------------------" <<std::endl; 
    // arguments to sendo to js function
    v8::Local<v8::Value> emit_argv[] = 
            { Nan::New("someEvent").ToLocalChecked(), Nan::New(*(sendMessage->greeting)).ToLocalChecked()};
  
    // calling emit function on our object
    MakeCallback(myObjectThreadLoop->handle(), "emit", 2, emit_argv);

  delete sendMessage;
}

void emit_event(const KaaNotification& notification) {
    // populate it to send some data from C to JS Land,
    async_event_data *asyncdata = new async_event_data();
    asyncdata->request.data = (void *) asyncdata;
   
    // Send data to worker thread
    SendMessage* sendMessage = new SendMessage;
    sendMessage->request.data = sendMessage;
//     v8::Local<v8::String> t = Nan::New<v8::String>(notification.message.get_string());
//     v8::Handle<v8::Value> t = Nan::New<v8::String>(s).ToLocalChecked();
//     v8::Handle< v8::Value > t = v8::String::New(s);
//     Handle<Value> something_else = String::New(s);
    std::basic_string<char> str(notification.message.get_string());
//    v8::String::Utf8Value x(str);
    sendMessage->greeting = new std::string(str);
//    strncpy(sendMessage->greeting, str, sizeof(sendMessage->greeting));

    //Function to be called on node thread
    uv_queue_work(uv_default_loop(), &sendMessage->request, noop_execute,
            reinterpret_cast<uv_after_work_cb>(callback_async_event));
}

void connectKaaServer(void *n) {
    auto kaaClient = Kaa::newClient();

    std::unique_ptr<INotificationTopicListListener> topicListListener(
              new BasicNotificationTopicListListener(*kaaClient));
    kaaClient->addTopicListListener(*topicListListener);

    std::unique_ptr<INotificationListener> commonNotificationListener(
              new BasicNotificationListener);
    kaaClient->addNotificationListener(*commonNotificationListener);
    kaaClient->start();

    auto availableTopics = kaaClient->getTopics();
    showTopicList(availableTopics);

    do {
      std::cin.get();
    std::cout << "Holding kaa connection to receive notification!!" << std::endl;
    } while(true);
//    kaaClient->removeTopicListListener(*topicListListener);
//    kaaClient->stop();
    std::cout << "Stopped kaa notification listener client!!" << std::endl;

}


NAN_METHOD(MyObject::StartLoop) {
    if(myObjectThreadLoop)
        return ThrowError("cant call twice"); 
	
    myObjectThreadLoop = Nan::ObjectWrap::Unwrap<MyObject>(info.Holder());
	
    //start kaa notification listener in seperate thread.
    uv_thread_create(&mythread, connectKaaServer, &threadnum);
}

NODE_MODULE(makecallback, MyObject::Init)
