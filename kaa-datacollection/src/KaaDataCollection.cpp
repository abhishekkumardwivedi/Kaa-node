/**
 *  Copyright 2014-2016 CyberVision, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */


#include <memory>
#include <cstdint>
#include <string>
#include <list>
#include <nan.h>

#include <kaa/Kaa.hpp>
#include <kaa/log/strategies/RecordCountLogUploadStrategy.hpp>

using namespace kaa;

/*
 * A demo application that shows how to use the Kaa logging API.
 */
void messageUpdater( std::string data)
{
    std::cout << data << std::endl;

    /*
     * Initialize the Kaa endpoint.
     */
    auto kaaClient =  Kaa::newClient();

    /*
     * Set a custom strategy for uploading logs.
     */
    kaaClient->setLogUploadStrategy(std::make_shared<RecordCountLogUploadStrategy>(1, kaaClient->getKaaClientContext()));

    /*
     * Run the Kaa endpoint.
     */
    kaaClient->start();

    std::list<std::pair<RecordFuture, std::size_t>> futurePairs;

    // Send LOGS_TO_SEND_COUNT logs in a loop.
    std::size_t logNumber = 0;
        KaaUserLogRecord logRecord;
//        logRecord.level = kaa_log::Level::KAA_INFO;
//        logRecord.tag = "TAG";
//        logRecord.message = data;
//        logRecord.timeStamp = TimeUtils::getCurrentTimeInMs();
        logRecord.timestamp = TimeUtils::getCurrentTimeInMs();

        futurePairs.push_back(std::make_pair(std::move(kaaClient->addLogRecord(logRecord)), logRecord.timestamp));
        std::cout << "Sent " << logNumber << "th record" << std::endl;

    for (auto& pair : futurePairs) {
        try {
            RecordInfo recordInfo = pair.first.get();
            BucketInfo bucketInfo = recordInfo.getBucketInfo();

            std::size_t timeSpent = (recordInfo.getRecordAddedTimestampMs() - pair.second)
                                        + recordInfo.getRecordDeliveryTimeMs();

            std::cout << "Received log record delivery info. Bucket Id [" <<  bucketInfo.getBucketId() << "]. "
                      << "Record delivery time [" << timeSpent << " ms]." << std::endl;
        } catch (std::exception& e) {
            std::cout << "Exception was caught while waiting for callback future" << e.what();
        }
    }

    /*
     * Stop the Kaa endpoint.
     */
    kaaClient->stop();

    std::cout << "Data collection demo stopped" << std::endl;
}


void SendData(const Nan::FunctionCallbackInfo<v8::Value>& info) {

  if (info.Length() != 1) {
    Nan::ThrowTypeError("Wrong number of arguments");
    return;
  }

//  if (!info[0]->IsNumber() || !info[1]->IsNumber()) {
//    Nan::ThrowTypeError("Wrong arguments");
//    return;
//  }

  v8::Local<v8::String> arg0 = info[0]->ToString();
//  double arg1 = info[1]->NumberValue();
/////  v8::Local<v8::String> num = Nan::New(arg0);
  v8::String::Utf8Value param(arg0);
  std::string d = std::string(*param);
  messageUpdater(d);
  info.GetReturnValue().Set(arg0);
}

void Init(v8::Local<v8::Object> exports) {
  exports->Set(Nan::New("sendData").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(SendData)->GetFunction());
}

NODE_MODULE(addon, Init)

