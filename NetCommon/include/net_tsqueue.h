#ifndef NET_TSQUEUE_H
#define NET_TSQUEUE_H

#include "net_common.h"
#include <deque>

namespace tps
{
    namespace net
    {
        template <typename T>
        class tsqueue
        {
        public:
            tsqueue() = default;
            tsqueue(const tsqueue<T>&) = delete;
            ~tsqueue() {clear();}

            bool empty()
            {
                const std::lock_guard<std::mutex> lock(muxQueue);
                return deqQueue.empty();
            }

            size_t count()
            {
                const std::lock_guard<std::mutex> lock(muxQueue);
                return deqQueue.size();
            }

            void clear()
            {
                const std::lock_guard<std::mutex> lock(muxQueue);
                deqQueue.clear();
            }

            const T& front()
            {
                const std::lock_guard<std::mutex> lock(muxQueue);
                return deqQueue.front();
            }

            const T& back()
            {
                const std::lock_guard<std::mutex> lock(muxQueue);
                return deqQueue.back();
            }

            void push_front(const T& item)
            {
                const std::lock_guard<std::mutex> lock(muxQueue);
                deqQueue.emplace_front(item);
            }

            void push_back(const T& item)
            {
                const std::lock_guard<std::mutex> lock(muxQueue);
                deqQueue.emplace_back(item);
            }

            T pop_front()
            {
                const std::lock_guard<std::mutex> lock(muxQueue);
                auto t = std::move(deqQueue.front());
                deqQueue.pop_front();
                return t;
            }

            T pop_back()
            {
                const std::lock_guard<std::mutex> lock(muxQueue);
                auto t = std::move(deqQueue.back());
                deqQueue.pop_back();
                return t;
            }

        private:
            std::deque<T> deqQueue;
            std::mutex muxQueue;
        };

    }
}

#endif // NET_TSQUEUE_H
