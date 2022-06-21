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
        tps::net::message<CustomMsgTypes> msg;
        msg.hdr.id = CustomMsgTypes::ServerAccept;

        client->Send(msg);

        return true;
    }

    virtual void OnClientDisconnect(std::shared_ptr<tps::net::connection<CustomMsgTypes>> client)
    {
        std::cout << "Removing client [" << client->GetID() << "]\n";
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
            case CustomMsgTypes::MessageAll:
            {
                std::cout << "[" << client->GetID() << "]" << "Message All\n";
                tps::net::message<CustomMsgTypes> msg;
                msg.hdr.id = CustomMsgTypes::ServerMessage;
                msg << client->GetID();
                MessageAllClients(msg, client);
                break;
            }
            default:
                std::cout << "[-]Unknown type of msg: " << uint32_t(msg.hdr.id) << "\n";
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

    void MessageAll()
    {
        tps::net::message<CustomMsgTypes> msg;
        msg.hdr.id = CustomMsgTypes::MessageAll;

        Send(msg);
    }
};

#define CLIENT

int main()
{
#ifdef CLIENT
    CustomClient Client;
    Client.Connect("127.0.0.1", 5000);

    tps::net::tsqueue<int>input;
    std::thread IOThread = std::thread([&input]()
    {
        while (1)
        {
            std::string in;
            std::getline(std::cin, in);
            try {
                input.push_back(std::stoi(in));
            } catch (...) {
                // in case stoi fails
            }
        }
    });

    while (1)
    {
        if (Client.IsConnected())
        {
            if (!input.empty())
            {
                switch (input.pop_front())
                {
                    case 1: Client.PingServer(); break;
                    case 2: Client.MessageAll(); break;
                    case 3: return 0;
                }
            }

            if (!Client.Incoming().empty())
            {
                tps::net::message<CustomMsgTypes> msg = Client.Incoming().pop_front().msg;
                switch (msg.hdr.id)
                {
                    case CustomMsgTypes::ServerAccept:
                        std::cout << "Server accepted connection\n";
                        break;
                    case CustomMsgTypes::ServerPing:
                    {
                        std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
                        std::chrono::system_clock::time_point timeThen;
                        msg >> timeThen;
                        auto timeDiff = std::chrono::duration<double>(timeNow - timeThen);
                        std::cout << "Ping:" << timeDiff.count() << "\n";
                        break;
                    }
                    case CustomMsgTypes::ServerMessage:
                    {
                        uint32_t clientID = 0;
                        msg >> clientID;
                        std::cout << "Broadcast from:" << clientID << "\n";
                        break;
                    }
                default:
                    std::cout << "[-]Unknown type of msg: " << uint32_t(msg.hdr.id) << "\n";
                    break;
                }
            }
        }
    }

#else
    CustomServer Server(5000);
    Server.Start();

    while(1)
    {
        Server.Update();
    }
#endif

    return 0;
}
