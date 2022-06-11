#ifndef NET_CONNECTION_H
#define NET_CONNECTION_H

#include "net_common.h"
#include "net_message.h"
#include "net_tsqueue.h"

namespace tps
{
    namespace net
    {
        using namespace boost;

        template <typename T>
        class connection: public std::enable_shared_from_this<connection<T>>
        {
        public:
            connection()
            {

            }

            virtual ~connection()
            {

            }

            bool ConnectToServer();
            bool Disconnect();
            bool isConnected() const;

            bool Send(const message<T>& msg);

        protected:
            asio::ip::tcp::socket m_socket;

            asio::io_context& m_asioContext;

            tsqueue<message<T>> m_qMessageOut;

            tsqueue<owned_message<T>>& m_qMessageIn;
        };
    }
}

#endif // NET_CONNECTION_H
