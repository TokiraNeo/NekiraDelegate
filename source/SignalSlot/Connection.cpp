/**
 * MIT License
 *
 * Copyright (c) 2025 TokiraNeo (https://github.com/TokiraNeo)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <Connection.hpp>
#include <memory>
#include <mutex>

namespace NekiraDelegate
{

IConnectionInterface::~IConnectionInterface()
{
    // 在析构时清理所有连接
    DisconnectAll();
}

// 添加连接
void IConnectionInterface::AddConnection(std::shared_ptr<ConnectionBase> InConnection) const
{
    std::lock_guard<std::mutex> Lock(Mutex);

    if (InConnection && InConnection->IsValid())
    {
        Connections.push_back(InConnection);
    }
}

// 断开所有连接
void IConnectionInterface::DisconnectAll() const
{
    std::vector<std::weak_ptr<ConnectionBase>> TempConnections;

    // 写时使用独占锁。为了避免在删除时有新的连接添加进来，这里拷贝一份副本
    {
        std::lock_guard<std::mutex> Lock(Mutex);

        TempConnections = std::move(Connections);
    }

    // 断开所有连接
    for (const auto& WeakConnection : TempConnections)
    {
        if (auto Ptr = WeakConnection.lock())
        {
            Ptr->Disconnect();
        }
    }
}

} // namespace NekiraDelegate
