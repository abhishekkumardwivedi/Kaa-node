# KAA - NodeJS and Node-RED module

This project provides NodeJS and Node-RED interface to KAA server. Project modules are made by using NAN (v8)
addon interface with C++ native code.
All modules are KAA client, the end node to KAA server, and should not be confused with part of KAA server.
This client application is tested in Ubuntu-16.04 OS.

# Demo video
[![Creating Node-RED KAA node](https://i.ytimg.com/vi/mV8ofe8rQ7I/2.jpg?time=1480625110782)](https://www.youtube.com/embed/mV8ofe8rQ7I "KAA - Node-RED | NodeJS - Click to Watch!")

## Getting Started

These instructions will help you to setup build environemnt and application execution environment.

### Prerequisites

KAA server should be up

### Installing
These are the needed softwares to build Node-RED module and get it running

* Install NodeJS and check the version. nodejs v4.2.6 was used for compilation.
```
sudo apt-get install nodejs
nodejs -v
```
* Install npm and check version. npm version 3.10.9 was used for compilation. Very old version would lead to silent failure in overall project execution. Make sure to use atleast this version to avoid it.
```
sudo apt-get install npm
npm -v
```
* Install cmake-js and check the version. cmake-js version 3.3.1 was used for project compilation.
We will be using cmake-js for compiling addon and Kaa SDK library (usual way of compiling Kaa SDK is executing provided 
shell script which inturn uses cmake).
```
npm install -g cmake-js
cmake-js --version
```
* If you are using only NodeJS then above setups are anough to go with. But if you are using Node-RED then install it and check the version. This application is tested with Node-RED version v0.15.2.
```
sudo npm install -g --unsafe-perm node-red node-red-admin
sudo ufw allow 1880
node-red --help
```

As we will be using C++ Kaa SDK, we need following build tool chain:

* Install g++, CMake, Boost, SQLite3 for compiling Kaa C++ SDK library
```
sudo apt-get install g++ cmake libboost1.61-all-dev libsqlite3-0 libsqlite3-dev

```
* Install AvroC++ library
```
wget http://archive.apache.org/dist/avro/avro-1.8.1/cpp/avro-cpp-1.8.1.tar.gz
tar -zxvf avro-cpp-1.8.1.tar.gz
cd avro-cpp-1.8.1/
cmake .
sudo make install
```
* Install Botan 1.11 library
```
wget https://github.com/randombit/botan/archive/1.11.28.tar.gz
tar -zxf 1.11.28.tar.gz
cd botan-1.11.28/
./configure.py
sudo make install
sudo ln -s /usr/local/include/botan-1.11/botan /usr/local/include/botan
```
### Compiling and running app

There are separate folders for KAA interface applications. Go to the folder which ever you are interested in.
Download corresponding C++ SDK library from KAA server to {APP_DIRECTORY}/lib/kaa/ and extract.
```
cd lib/kaa/
tar -zxvf kaa-cpp-ep-sdk-*.tar.gz
cd -
```
Now you are all set to compile and run application.
* For each application compile KAA SDK
* Install node libraries
* Compile application

Now, go into application root folder e.g. kaa-notification and install nodeJS packages needed:
```
npm install
```
Let's compile SDK lib. This compilation step is needed only one time. And needed to be compiled again only when SDK lib is changed/updated.
```
cd lib/kaa/
cmake-js build
cd -
```
This is the time to compile addon library which will be referred from JS land. Outcome of this compilation will be .node
library. This step is needed everytime when there are changes in native application or SDK library.
```
cmake-js build
```
After success of above compilation your application is ready to test. We can start with NodeJS test first.
```
nodejs test.js
```
Now we can move this application to .node-red/nodes/ and start node-red to test Node-RED app
```
node-red
```
After running Node-RED browserapp, we will be able to see Kaa modules in Node-RED browser pannel. You can go ahead with usual way of Node-RED test flow test from here.

### Output
Output of kaa-configuration will be something like below:
```
msg : Object
{ "payload": "{\"Kaa GitHub repository\":\"https:\\/\\/github.com\\/kaaproject\\/kaa\",\"Kaa configuration design reference\":\"http:\\/\\/docs.kaaproject.org\\/display\\/KAA\\/Configuration\",\"Kaa docs\":\"http:\\/\\/docs.kaaproject.org\\/display\\/KAA\\/Kaa+IoT+Platform+Home\",\"Kaa website\":\"http:\\/\\/www.kaaproject.org\"}\n", "_msgid": "b0441197.4fbbf" }
```

### What is not mentioned here
All the setup guide described with consideration that you already have prior experience with relevant projects. If you are new to any of these, it is very important to go through and understand at first.

## Contribute

If you are supporting more features to KAA or you find some changes in reference then raise issue and we will folloup with
pull request if your change would look helpful to others. Or you can apply the changes which will help us.

## Authors

* **Abhishek Dwivedi** - *independent technical consultant* - [OERDev](https://github.com/abhishekkumardwivedi)
* *contact to hire [abhishekk@oerdev.com](abhishekk@oerdev.com) or sponser for helping in your project R&D.*

See also the list of [contributors](https://github.com/abhishekkumardwivedi/Kaa-node/contributors) who participated
in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* Thanking to nodeJS addon sample which helped to dig into some really challenging issues with.
* Would thank KAA for providing sample application which did ease life in reaching here.
