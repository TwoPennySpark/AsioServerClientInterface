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
            enum class owner
            {
                client,
                server
            };

            connection(owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, tsqueue<owned_message<T>>& qIn):
                       m_nOwnerType(parent), m_asioContext(asioContext), m_socket(std::move(socket)), m_qMessageIn(qIn)
            {

            }

            virtual ~connection()
            {

            }

            void ConnectToClient(uint32_t uid = 0)
            {
                if (m_nOwnerType == owner::server)
                {
                    if (m_socket.is_open())
                    {
                        m_id = uid;
                        ReadHeader();
                    }
                }
            }
            bool ConnectToServer();
            bool Disconnect();
            bool isConnected() const
            {
                return m_socket.is_open();
            }

            bool Send(const message<T>& msg);

            uint32_t GetID() const
            {
                return m_id;
            }

            // ASYNC
            void ReadHeader()
            {
                asio::async_read(m_socket, asio::buffer(&m_msgTempIn.hdr, sizeof(message_header<T>)),
                    [this](std::error_code ec, std::size_t length)
                    {
                        if (!ec)
                        {
                            if (m_msgTempIn.hdr.size > 0)
                            {
                                m_msgTempIn.body.resize(m_msgTempIn.hdr.size);
                                ReadBody();
                            }
                            else
                            {
                                AddToIncomingMessageQueue();
                            }
                        }
                        else
                        {
                            std::cout << "[" << m_id << "] Read Header Fail\n";
                            m_socket.close();
                        }
                    });
            }

            // ASYNC
            void ReadBody()
            {
                asio::async_read(m_socket, asio::buffer(m_msgTempIn.body.data(), m_msgTempIn.body.size()),
                    [this](std::error_code ec, std::size_t length)
                    {
                        if (!ec)
                        {
                            AddToIncomingMessageQueue();
                        }
                        else
                        {
                            std::cout << "[" << m_id << "] Read Body Fail\n";
                            m_socket.close();
                        }
                    });
            }

            // ASYNC
            void WriteHeader()
            {
                asio::async_write(m_socket, asio::buffer(&m_qMessageOut.front().header, sizeof(message_header<T>)),
                    [this](std::error_code ec, std::size_t length)
                    {
                        if (!ec)
                        {
                            if (m_qMessageOut.front().body.size() > 0)
                            {
                                WriteBody();
                            }
                            else
                            {
                                m_qMessageOut.pop_front();
                                if (!m_qMessageOut.empty())
                                    WriteHeader();
                            }
                        }
                        else
                        {
                            std::cout << "[" << m_id << "] Write Header Fail\n";
                            m_socket.close();
                        }
                    });
            }

            // ASYNC
            void WriteBody()
            {
                asio::async_write(m_socket, asio::buffer(m_qMessageOut.front().body.data(), m_qMessageOut.front().body.size()),
                    [this](std::error_code ec, std::size_t length)
                    {
                        if (!ec)
                        {
                                m_qMessageOut.pop_front();
                                if (!m_qMessageOut.empty())
                                    WriteHeader();
                        }
                        else
                        {
                            std::cout << "[" << m_id << "] Write Body Fail\n";
                            m_socket.close();
                        }
                    });
            }

            void AddToIncomingMessageQueue()
            {
                if (m_nOwnerType == owner::server)
                    m_qMessageIn.push_back({this->shared_from_this(), m_msgTempIn});
                else
                    m_qMessageIn.push_back({nullptr, m_msgTempIn}); // client has only 1 connection, this connection will own all of incoming msgs

                ReadHeader();
            }

        protected:
            asio::ip::tcp::socket m_socket;

            asio::io_context& m_asioContext;

            tsqueue<message<T>> m_qMessageOut;

            tsqueue<owned_message<T>>& m_qMessageIn;

            message<T> m_msgTempIn;

            owner m_nOwnerType = owner::server;

            uint32_t m_id = 0;
        };
    }
}

#endif // NET_CONNECTION_H
