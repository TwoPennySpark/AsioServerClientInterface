#ifndef NET_MESSAGE_H
#define NET_MESSAGE_H

#include "net_common.h"

namespace tps
{
    namespace net
    {
        template <typename T>
        struct message_header
        {
            T id{};
            uint32_t size = 0; // size of the message(body)
        };

        template <typename T>
        struct message
        {
            message_header<T> hdr{};
            std::vector<uint8_t> body;

            size_t size() const
            {
                return body.size();
            }

            friend std::ostream& operator<< (std::ostream& os, const message<T>& m)
            {
                std::cout << "ID:" << int(m.hdr.id) << " SIZE:" << m.body.size();
                return os;
            }

            template <typename DataType>
            friend message<T>& operator<<(message<T>& msg, const DataType& data)
            {
                static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed");

                size_t end = msg.body.size();

                msg.body.resize(msg.body.size()+sizeof(data));

                std::memcpy(msg.body.data()+end, &data, sizeof(data));

                msg.hdr.size += sizeof(data);

                return msg;
            }

            template <typename DataType>
            friend message<T>& operator>>(message<T>& msg, DataType& data)
            {
                static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be poped");

                size_t i = msg.body.size()-sizeof(data);

                std::memcpy(&data, msg.body.data()+i, sizeof(data));

                msg.body.resize(i);

                msg.hdr.size -= sizeof(data);

                return msg;
            }
        };

        template <typename T>
        class connection;

        template <typename T>
        struct owned_message
        {
            std::shared_ptr<connection<T>> owner = nullptr;
            message<T> msg;

            friend std::ostream& operator<<(std::ostream& os, const owned_message<T>& msg)
            {
                os << msg.msg;
                return os;
            }
        };
    }
}

#endif // NET_MESSAGE_H
