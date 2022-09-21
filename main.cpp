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
    virtual bool on_client_connect(std::shared_ptr<tps::net::connection<CustomMsgTypes>> client) override
    {
        tps::net::message<CustomMsgTypes> msg;
        msg.hdr.id = CustomMsgTypes::ServerAccept;

        client->send(msg, this);

        return true;
    }

    virtual void on_client_disconnect(std::shared_ptr<tps::net::connection<CustomMsgTypes>> client) override
    {
        std::cout << "Removing client [" << client->get_ID() << "]\n";
    }

    virtual void on_message(std::shared_ptr<tps::net::connection<CustomMsgTypes>> client,
                           tps::net::message<CustomMsgTypes>& msg) override
    {
        switch (msg.hdr.id)
        {
            case CustomMsgTypes::ServerPing:
                std::cout << "[" << client->get_ID() << "]" << "Server Ping\n";
                client->send(msg, this);
                break;
            case CustomMsgTypes::MessageAll:
            {
                std::cout << "[" << client->get_ID() << "]" << "Message All\n";
                tps::net::message<CustomMsgTypes> msg;
                msg.hdr.id = CustomMsgTypes::ServerMessage;
                msg << client->get_ID();
                message_all_clients(msg, client);
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
    void ping_server()
    {
        tps::net::message<CustomMsgTypes> msg;
        msg.hdr.id = CustomMsgTypes::ServerPing;
        auto time = std::chrono::system_clock::now();

        msg << time;

        send(msg);
    }

    void message_all()
    {
        tps::net::message<CustomMsgTypes> msg;
        msg.hdr.id = CustomMsgTypes::MessageAll;

        send(msg);
    }
};

//#define CLIENT

int main()
{
#ifdef CLIENT
    CustomClient client;
    client.connect("127.0.0.1", 5000);

    std::cout << "Enter 1 to send ping msg to the server\n"
                 "Enter 2 to send broadcast msg to all client connected to the server\n"
                 "Enter 3 to exit\n";

    // get input from user in separate thread
    tps::net::tsqueue<int>input;
    std::thread InputThread = std::thread([&input]()
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
        if (client.is_connected())
        {
            if (!input.empty())
            {
                switch (input.pop_front())
                {
                    case 1: client.ping_server(); break;
                    case 2: client.message_all(); break;
                    case 3: return 0;
                }
            }

            // if there is a new message in the receiving queue
            if (!client.incoming().empty())
            {
                tps::net::message<CustomMsgTypes> msg = client.incoming().pop_front().msg;
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
    CustomServer server(5000);
    server.start();

    while(1)
    {
        server.update();
    }
#endif

    return 0;
}
