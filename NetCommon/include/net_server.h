#ifndef NET_SERVER_H
#define NET_SERVER_H

#include "net_common.h"
#include "net_connection.h"
#include "net_message.h"
#include "net_tsqueue.h"
#include "net_client.h"

namespace TPS
{
    namespace net
    {
        template <typename T>
        class server_interface
        {
        public:
            server_interface(uint16_t port)
            {

            }

            virtual ~server_interface()
            {

            }

            bool Start()
            {

            }

            void Stop()
            {

            }

            void WaitForClientConnection()
            {

            }


        };
    }
}

#endif // NET_SERVER_H
