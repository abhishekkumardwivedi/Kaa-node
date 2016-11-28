#include <memory>

#include <nan.h>
#include <unistd.h>
#include <kaa/Kaa.hpp>
#include <kaa/IKaaClient.hpp>
#include <kaa/configuration/manager/IConfigurationReceiver.hpp>
#include <kaa/configuration/storage/FileConfigurationStorage.hpp>

#include <sstream>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace Nan;
using namespace kaa;

const char savedConfig[] = "saved_config.cfg";

/**
 * Native interface implementaton
 *
 * For making C++ wrapped object and factory for wrapped object.
 * Check for more details:
 * https://github.com/nodejs/nan/blob/master/doc/object_wrappers.md
 **/
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

/**
 * We are using libuv library for async I/O.
 * Here async_data structure will hold the payload to transform
 * from onConfigurationUpdated thread to main thread.
 **/
struct async_data
{
    uv_work_t request;
    std::map<std::string, std::string> dataMap;
};

Persistent<v8::Function> MyObject::constructor;

NAN_MODULE_INIT(MyObject::Init) {
    std::cout << "Init thread id: " << std::this_thread::get_id() << std::endl;
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New<v8::String>("MyObject").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    SetPrototypeMethod(tpl, "startLoop", StartLoop);

    constructor.Reset(tpl->GetFunction());
    Set(target, Nan::New("MyObject").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(MyObject::New) {
    std::cout << "New thread id: " << std::this_thread::get_id() << std::endl;
    if (!info.IsConstructCall()) {
        return Nan::ThrowError("`new` required");
    }

    MyObject* obj = new MyObject();
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
}

MyObject* MyObjectThreadLoop;
uv_thread_t mythread;
int threadnum = 1;

NAN_INLINE void noop_execute (uv_work_t* req) {}

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

std::string map_to_json(const std::map<std::string, std::string>& map) {
  ptree pt;
  for (auto& entry: map)
      pt.put (entry.first, entry.second);
  std::ostringstream buf;
  write_json (buf, pt, false);
  return buf.str();
}

/**
 * Running in Main/Node thread.
 **/
NAN_INLINE void callback_async_event (uv_work_t* req) {

    std::cout << "callback_async_event thread id: " << std::this_thread::get_id() << std::endl;
    Nan::HandleScope scope;

    async_data* data = static_cast<async_data*>(req->data);

    v8::Local<v8::Value> argv[] = { Nan::New("kaaConfig").ToLocalChecked(),
            Nan::New(map_to_json(data->dataMap)).ToLocalChecked()};

    // Emit event to JS with argv[] payload.
    Nan::MakeCallback(MyObjectThreadLoop->handle(), "emit", 2, argv);

    delete data;
}

/**
 * KAA configuration change listener
 **/
class UserConfigurationReceiver : public IConfigurationReceiver {
public:
    void displayConfiguration(const KaaRootConfiguration &configuration)
    {
        // We are in uv thread (libuv) and not in main thread. And using, heap memory
        // (common memory b/w threads), to pass data from uv thread to main thread which
        // will inturn pass data to main thread.
        async_data *asyncdata = new async_data();
	    asyncdata->request.data = (void *) asyncdata;

        if (!configuration.AddressList.is_null()) {
            auto links = configuration.AddressList.get_array();

            for (auto& e : links) {
                 std::cout << e.label << " - " << e.url << std::endl;
                 asyncdata->dataMap[e.label] = e.url;
            }
        }
	    uv_queue_work(uv_default_loop(), &asyncdata->request, noop_execute, reinterpret_cast<uv_after_work_cb>(callback_async_event));
    }

    void onConfigurationUpdated(const KaaRootConfiguration &configuration)
    {
        std::cout << "onConfigurationUpdated thread id: " << std::this_thread::get_id() << std::endl;
        displayConfiguration(configuration);
    }
};

/**
 * Do all Kaa protocol related tailoring here.
 * Basically this does Kaa server connection initialization and then connection.
 **/
void kaa_connection_threadloop(void *n) {
    auto kaaClient = Kaa::newClient();

    IConfigurationStoragePtr storage(std::make_shared<FileConfigurationStorage>(savedConfig));
    kaaClient->setConfigurationStorage(storage);

    UserConfigurationReceiver receiver;
    kaaClient->addConfigurationListener(receiver);

    kaaClient->start();
    std::cout << "========================= START =========================== " << std::endl;

    while(true) {
   		usleep(100*1000);
    }
}

NAN_METHOD(MyObject::StartLoop) {

    std::cout << "StartLoop thread id: " << std::this_thread::get_id() << std::endl;
	if(MyObjectThreadLoop) return ThrowError("can't call twice");

	MyObjectThreadLoop = Nan::ObjectWrap::Unwrap<MyObject>(info.Holder());
    uv_thread_create(&mythread, kaa_connection_threadloop, &threadnum);
}

NODE_MODULE(configuration, MyObject::Init)
