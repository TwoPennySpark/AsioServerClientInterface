#include "NetCommon/include/net_common.h"
#include "NetCommon/include/net_server.h"

enum class CustomMsgTypes: uint32_t
{
    ServerAccept,
    ServerDeny,
    ServerPing,
    MessageAll,
    ServerMessage
};

class CustomServer: public tps::net::server_interface<CustomMsgTypes>
{
public:
    CustomServer(uint16_t nPort): tps::net::server_interface<CustomMsgTypes>(nPort) {}

protected:
    virtual bool OnClientConnect(std::shared_ptr<tps::net::connection<CustomMsgTypes>> client)
    {
        return true;
    }

    virtual void OnClientDisconnect(std::shared_ptr<tps::net::connection<CustomMsgTypes>> client)
    {

    }

    virtual void OnMessage(std::shared_ptr<tps::net::connection<CustomMsgTypes>> client,
                           tps::net::message<CustomMsgTypes>& msg)
    {

    }

};

int main()
{
    CustomServer Server(5000);
    Server.Start();

    while(1)
    {
        Server.Update();
        usleep(100*1000);
    }

    return 0;
}
