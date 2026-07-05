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

#pragma once

#include <NekiraDelegate/SignalSlot/Connection.hpp>
#include <algorithm>
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <type_traits>
#include <vector>



namespace NekiraDelegate
{
// 单播信号类
template <typename RT, typename... Args>
class SingleSignal final
{
private:
    // 当前连接器
    std::shared_ptr<Connection<RT, Args...>> ConnectionPtr;

    // 读写锁
    mutable std::shared_mutex Mutex;

public:
    SingleSignal() = default;
    ~SingleSignal()
    {
        Disconnect();
    }

    SingleSignal(const SingleSignal&) = delete;
    SingleSignal& operator=(const SingleSignal&) = delete;

    SingleSignal(SingleSignal&&) noexcept = delete;
    SingleSignal& operator=(SingleSignal&&) noexcept = delete;

    // 是否有效的连接
    [[nodiscard]] bool IsValid() const
    {
        std::shared_ptr<Connection<RT, Args...>> CopyConnectionPtr;

        {
            // 先在信号层拿到稳定的连接副本，再到连接层判断有效性
            std::shared_lock<std::shared_mutex> Lock(Mutex);
            CopyConnectionPtr = ConnectionPtr;
        }

        return CopyConnectionPtr && CopyConnectionPtr->IsValid();
    }

    // 执行连接的回调
    RT Invoke(Args... args)
    {
        std::shared_ptr<Connection<RT, Args...>> CopyConnectionPtr;

        {
            std::shared_lock<std::shared_mutex> Lock(Mutex);

            if (!IsValid())
            {
                if constexpr (std::is_void_v<RT>)
                {
                    return;
                }
                else
                {
                    return RT{};
                }
            }

            CopyConnectionPtr = ConnectionPtr;
        }

        if constexpr (std::is_void_v<RT>)
        {
            CopyConnectionPtr->Invoke(args...);
        }
        else
        {
            return CopyConnectionPtr->Invoke(args...);
        }
    }

    // 断开连接
    void Disconnect()
    {
        std::shared_ptr<Connection<RT, Args...>> OldConnectionPtr;

        {
            std::unique_lock<std::shared_mutex> Lock(Mutex);
            OldConnectionPtr = std::move(ConnectionPtr);
        }

        if (OldConnectionPtr)
        {
            OldConnectionPtr->Disconnect();
        }
    }

    // 连接普通函数
    void Connect(RT (*FuncPtr)(Args...))
    {
        std::function<RT(Args...)> Func = FuncPtr;

        auto NewConnectionPtr = std::make_shared<Connection<RT, Args...>>(std::move(Func));
        std::shared_ptr<Connection<RT, Args...>> OldConnectionPtr;

        {
            std::unique_lock<std::shared_mutex> Lock(Mutex);

            // 先替换掉旧连接，避免持有信号锁时执行断开逻辑
            OldConnectionPtr = std::move(ConnectionPtr);
            ConnectionPtr = std::move(NewConnectionPtr);
        }

        if (OldConnectionPtr)
        {
            OldConnectionPtr->Disconnect();
        }
    }

    // 连接普通成员函数,要求继承 IConnectionInterface接口
    template <typename ClassType>
        requires std::is_base_of_v<IConnectionInterface, ClassType>
    void Connect(ClassType* Object, RT (ClassType::*FuncPtr)(Args...))
    {
        auto Lambda = [Object, FuncPtr](Args... args) -> RT { return (Object->*FuncPtr)(args...); };

        std::function<RT(Args...)> Func = std::move(Lambda);

        auto NewConnectionPtr = std::make_shared<Connection<RT, Args...>>(std::move(Func));
        std::shared_ptr<Connection<RT, Args...>> OldConnectionPtr;

        // 添加连接到对象的连接接口
        static_cast<IConnectionInterface*>(Object)->AddConnection(NewConnectionPtr);

        {
            // 写时使用独占锁
            std::unique_lock<std::shared_mutex> Lock(Mutex);

            // 先替换掉旧连接，避免持有信号锁时执行断开逻辑
            OldConnectionPtr = std::move(ConnectionPtr);
            ConnectionPtr = std::move(NewConnectionPtr);
        }

        if (OldConnectionPtr)
        {
            OldConnectionPtr->Disconnect();
        }
    }

    // 连接const成员函数,要求继承 IConnectionInterface接口
    template <typename ClassType>
        requires std::is_base_of_v<IConnectionInterface, ClassType>
    void Connect(const ClassType* Object, RT (ClassType::*FuncPtr)(Args...) const)
    {
        auto Lambda = [Object, FuncPtr](Args... args) -> RT { return (Object->*FuncPtr)(args...); };

        std::function<RT(Args...)> Func = std::move(Lambda);

        auto NewConnectionPtr = std::make_shared<Connection<RT, Args...>>(std::move(Func));
        std::shared_ptr<Connection<RT, Args...>> OldConnectionPtr;

        // 添加连接到对象的连接接口
        static_cast<const IConnectionInterface*>(Object)->AddConnection(NewConnectionPtr);

        {
            // 写时使用独占锁
            std::unique_lock<std::shared_mutex> Lock(Mutex);

            // 先替换掉旧连接，避免持有信号锁时执行断开逻辑
            OldConnectionPtr = std::move(ConnectionPtr);
            ConnectionPtr = std::move(NewConnectionPtr);
        }

        if (OldConnectionPtr)
        {
            OldConnectionPtr->Disconnect();
        }
    }

    // 连接函数对象、lambda表达式
    template <typename Callable>
        requires std::is_invocable_r_v<RT, Callable, Args...>
    void Connect(Callable&& CallableObj)
    {
        std::function<RT(Args...)> Func = std::forward<Callable>(CallableObj);

        auto NewConnectionPtr = std::make_shared<Connection<RT, Args...>>(std::move(Func));
        std::shared_ptr<Connection<RT, Args...>> OldConnectionPtr;

        {
            // 写时使用独占锁
            std::unique_lock<std::shared_mutex> Lock(Mutex);

            // 先替换掉旧连接，避免持有信号锁时执行断开逻辑
            OldConnectionPtr = std::move(ConnectionPtr);
            ConnectionPtr = std::move(NewConnectionPtr);
        }

        if (OldConnectionPtr)
        {
            OldConnectionPtr->Disconnect();
        }
    }
};

} // namespace NekiraDelegate



namespace NekiraDelegate
{
// 用于移除多播信号类中的特定连接
struct MultiSignalHandle final
{
    MultiSignalHandle() = default;
    ~MultiSignalHandle() = default;

    MultiSignalHandle(void* InSignalPtr, std::size_t InId) : SignalPtr(InSignalPtr), Id(InId)
    {}

    MultiSignalHandle(const MultiSignalHandle&) = default;
    MultiSignalHandle(MultiSignalHandle&&) = default;

    MultiSignalHandle& operator=(const MultiSignalHandle&) = default;
    MultiSignalHandle& operator=(MultiSignalHandle&&) = default;

    bool operator==(const MultiSignalHandle& Other) const
    {
        return SignalPtr == Other.SignalPtr && Id == Other.Id;
    }

    bool operator!=(const MultiSignalHandle& Other) const
    {
        return !(*this == Other);
    }

    void*       SignalPtr; // 指向多播信号的指针
    std::size_t Id;        // 连接的唯一标识符
};
} // namespace NekiraDelegate


namespace NekiraDelegate
{

// 多播信号类
template <typename... Args>
class MultiSignal final
{
private:
    using ConnectionType = Connection<void, Args...>;
    using ConnectionPair = std::pair<MultiSignalHandle, std::shared_ptr<ConnectionType>>;

    // 存储连接器
    std::vector<ConnectionPair> ConnectionMap;

    // 用于生成唯一的连接ID
    std::atomic<std::size_t> NextId{0};

    // 读写锁
    mutable std::shared_mutex Mutex;

public:
    MultiSignal() = default;
    ~MultiSignal()
    {
        DisconnectAll();
    }

    MultiSignal(const MultiSignal&) = delete;
    MultiSignal& operator=(const MultiSignal&) = delete;

    MultiSignal(MultiSignal&&) noexcept = delete;
    MultiSignal& operator=(MultiSignal&&) noexcept = delete;

    // 是否有效
    [[nodiscard]] bool IsValid() const
    {
        // 读时使用共享锁
        std::shared_lock<std::shared_mutex> Lock(Mutex);

        return !ConnectionMap.empty();
    }

    // 执行所有连接的回调
    void Invoke(Args... args)
    {
        std::vector<std::shared_ptr<ConnectionType>> ActiveConnections;

        {
            // 先在独占锁内清理无效连接并收集当前快照，随后再执行回调，缩短锁持有时间
            std::unique_lock<std::shared_mutex> Lock(Mutex);

            const auto It = std::remove_if(ConnectionMap.begin(), ConnectionMap.end(),
                                           [](const auto& Pair) { return !Pair.second || !Pair.second->IsValid(); });

            ConnectionMap.erase(It, ConnectionMap.end());

            ActiveConnections.reserve(ConnectionMap.size());
            for (auto& Pair : ConnectionMap)
            {
                ActiveConnections.push_back(Pair.second);
            }
        }

        for (auto& Connection : ActiveConnections)
        {
            if (Connection)
            {
                Connection->Invoke(args...);
            }
        }
    }

    // 断开特定连接
    void DisconnectSingle(const MultiSignalHandle& Handle)
    {
        std::vector<std::shared_ptr<ConnectionType>> RemovedConnections;

        std::unique_lock<std::shared_mutex> Lock(Mutex);

        const auto It = std::remove_if(ConnectionMap.begin(), ConnectionMap.end(),
                                       [&Handle, &RemovedConnections](const ConnectionPair& Pair) {
                                           if (Pair.first != Handle)
                                           {
                                               return false;
                                           }

                                           if (Pair.second)
                                           {
                                               RemovedConnections.push_back(Pair.second);
                                           }

                                           return true;
                                       });

        if (It != ConnectionMap.end())
        {
            ConnectionMap.erase(It, ConnectionMap.end());
        }

        Lock.unlock();

        for (auto& Connection : RemovedConnections)
        {
            Connection->Disconnect();
        }
    }

    // 断开所有连接
    void DisconnectAll()
    {
        std::vector<std::shared_ptr<ConnectionType>> RemovedConnections;

        std::unique_lock<std::shared_mutex> Lock(Mutex);

        for (auto& Pair : ConnectionMap)
        {
            if (Pair.second)
            {
                RemovedConnections.push_back(Pair.second);
            }
        }

        ConnectionMap.clear();

        Lock.unlock();

        for (auto& Connection : RemovedConnections)
        {
            Connection->Disconnect();
        }
    }

    // 连接普通函数
    MultiSignalHandle Connect(void (*FuncPtr)(Args...))
    {
        std::function<void(Args...)> Func = FuncPtr;

        auto NewConnection = std::make_shared<ConnectionType>(std::move(Func));

        MultiSignalHandle Handler{this, ++NextId};

        ConnectionPair Pair{Handler, std::move(NewConnection)};

        // 写时使用独占锁
        std::unique_lock<std::shared_mutex> Lock(Mutex);

        // 添加连接
        ConnectionMap.push_back(std::move(Pair));

        return Handler;
    }

    // 连接普通成员函数,要求继承 IConnectionInterface接口
    template <typename ClassType>
        requires std::is_base_of_v<IConnectionInterface, ClassType>
    MultiSignalHandle Connect(ClassType* Object, void (ClassType::*FuncPtr)(Args...))
    {
        auto Lambda = [Object, FuncPtr](Args... args) { (Object->*FuncPtr)(std::forward<Args>(args)...); };

        std::function<void(Args...)> Func = std::move(Lambda);

        auto NewConnection = std::make_shared<ConnectionType>(std::move(Func));

        // 添加连接到对象的连接接口
        static_cast<IConnectionInterface*>(Object)->AddConnection(NewConnection);

        MultiSignalHandle Handler{this, ++NextId};

        ConnectionPair Pair{Handler, std::move(NewConnection)};

        // 写时使用独占锁
        std::unique_lock<std::shared_mutex> Lock(Mutex);

        // 添加连接
        ConnectionMap.push_back(std::move(Pair));

        return Handler;
    }

    // 连接const成员函数,要求继承 IConnectionInterface接口
    template <typename ClassType>
        requires std::is_base_of_v<IConnectionInterface, ClassType>
    MultiSignalHandle Connect(const ClassType* Object, void (ClassType::*FuncPtr)(Args...) const)
    {
        auto Lambda = [Object, FuncPtr](Args... args) { (Object->*FuncPtr)(std::forward<Args>(args)...); };

        std::function<void(Args...)> Func = std::move(Lambda);

        auto NewConnection = std::make_shared<ConnectionType>(std::move(Func));

        // 添加连接到对象的连接接口
        static_cast<const IConnectionInterface*>(Object)->AddConnection(NewConnection);

        MultiSignalHandle Handler{this, ++NextId};

        ConnectionPair Pair{Handler, std::move(NewConnection)};

        // 写时使用独占锁
        std::unique_lock<std::shared_mutex> Lock(Mutex);

        // 添加连接
        ConnectionMap.push_back(std::move(Pair));

        return Handler;
    }

    // 连接函数对象、lambda表达式
    template <typename Callable>
        requires std::is_invocable_r_v<void, Callable, Args...>
    MultiSignalHandle Connect(Callable&& CallableObj)
    {
        std::function<void(Args...)> Func = std::forward<Callable>(CallableObj);

        auto NewConnection = std::make_shared<ConnectionType>(std::move(Func));

        MultiSignalHandle Handler{this, ++NextId};

        ConnectionPair Pair{Handler, std::move(NewConnection)};

        // 写时使用独占锁
        std::unique_lock<std::shared_mutex> Lock(Mutex);

        // 添加连接
        ConnectionMap.push_back(std::move(Pair));

        return Handler;
    }

};

} // namespace NekiraDelegate
