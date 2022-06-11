#include "NetCommon/include/net_common.h"
#include "NetCommon/include/net_tsqueue.h"

using namespace boost;

std::vector<char> vBuffer(1024*1);

void grab_some_data(asio::ip::tcp::socket& socket)
{
    socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()),
                           [&](std::error_code ec, std::size_t length) {
        if (!ec)
        {
            std::cout << "\nLEN:" << length << "\n\n";
            for (char c: vBuffer)
                std::cout << c;
            std::cout << std::endl;
            std::memset(vBuffer.data(), 0, vBuffer.size());
        }
        grab_some_data(socket);
    });
}

int main()
{
    system::error_code ec;

    asio::io_context context;

    std::string host = "videosundry.com";

    asio::io_context::work idleWork(context);

    std::thread thrContext = std::thread([&](){ std::cout << "CONTEXT THREAD:" << std::this_thread::get_id() << std::endl; context.run();});

    asio::ip::tcp::socket socket(context);

    asio::ip::tcp::resolver resolver(context);
    auto endpoints = resolver.resolve(host, std::to_string(80));
    for (auto i = endpoints.begin(); i != endpoints.end(); i++)
    {
        socket.connect(*i);
        if (!ec)
        {
            std::cout << "connected\n";
            break;
        }

    }

    if (socket.is_open())
    {
        grab_some_data(socket);

        std::string req = "GET /music-videos/rick-roll-lyrics/ HTTP/1.1\r\n"
                          "Host:www." + host + "\r\n"
                          "Connection: close\r\n\r\n";

        socket.write_some(asio::buffer(req.data(), req.size()), ec);
        if (ec)
            std::cout << ec.message();

//        sleep(3);
    }

//    context.stop();
    if (thrContext.joinable())
        thrContext.join();

    std::cout << "END" << std::endl;
    return 0;
}
