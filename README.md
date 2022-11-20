# ASIO client-server framework
Header-only client-server framework that is built on top of Boost Asio library. Examples of usage can be seen in main.cpp or in another project: https://github.com/TwoPennySpark/MQTT-Broker.

## Build & Run:  
```
git clone https://github.com/TwoPennySpark/AsioServerClientInterface.git  
cd AsioServerClientInterface/  
make server client  
  
Server launch:  
./server <server port> <number of worker threads>  
  
Client launch:  
./client <server IP> <server port>  

```
