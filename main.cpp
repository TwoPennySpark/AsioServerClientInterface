#include "NetCommon/include/net_common.h"
#include "NetCommon/include/net_server.h"
#include "NetCommon/include/net_client.h"

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
        switch (msg.hdr.id)
        {
            case CustomMsgTypes::ServerPing:
                std::cout << "[" << client->GetID() << "]" << "Server Ping\n";
                client->Send(msg);
                break;
        }
    }

};

class CustomClient: public tps::net::client_interface<CustomMsgTypes>
{
public:
    void PingServer()
    {
        tps::net::message<CustomMsgTypes> msg;
        msg.hdr.id = CustomMsgTypes::ServerPing;
        auto time = std::chrono::system_clock::now();

        msg << time;

        Send(msg);
    }
};

#define CLIENT

int main()
{
#ifdef CLIENT
    CustomClient Client;
    Client.Connect("127.0.0.1", 5000);
sleep(1);
    while (1)
    {
        if (Client.IsConnected())
        {
            std::string input;
            std::getline(std::cin, input);
            if (input == std::to_string(1))
                Client.PingServer();
            else if (input == std::to_string(3))
                break;

            if (!Client.Incoming().empty())
            {
                std::cout << "New msg\n";
                tps::net::message<CustomMsgTypes> msg = Client.Incoming().pop_front().msg;
                switch (msg.hdr.id)
                {
                    case CustomMsgTypes::ServerPing:
                        std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
                        std::chrono::system_clock::time_point timeThen;
                        msg >> timeThen;
                        auto timeDiff = std::chrono::duration<double>(timeNow - timeThen);
                        std::cout << "Ping:" << timeDiff.count() << "\n";

                        break;
                }
            }
        }
        usleep(100*1000);
    }

#else
    CustomServer Server(5000);
    Server.Start();

    while(1)
    {
        Server.Update();
        usleep(100*1000);
    }
#endif

    return 0;
}
