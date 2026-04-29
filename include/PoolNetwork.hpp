#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <functional>
#include <span>
#include <utility>
#include <vector>
#include <asio.hpp>

#include "client.hpp"
#include "message.hpp"
#include "server.hpp"

constexpr uint16_t kPoolPort = 58008;
constexpr const char* kPoolPortString = "58008";

struct PoolProtocol {
    static constexpr std::size_t headerLength = 5;

    enum class MessageType : uint8_t {
        State = 0x01
    };

    static MessageType type(std::span<const uint8_t, headerLength> header) {
        return static_cast<MessageType>(header[0]);
    }

    static uint32_t bodyLength(std::span<const uint8_t, headerLength> header) {
        return (static_cast<uint32_t>(header[1]) << 24) |
               (static_cast<uint32_t>(header[2]) << 16) |
               (static_cast<uint32_t>(header[3]) << 8) |
               static_cast<uint32_t>(header[4]);
    }

    static void encodeHeader(std::array<uint8_t, headerLength>& header, MessageType type, uint32_t bodyLength) {
        header[0] = static_cast<uint8_t>(type);
        header[1] = static_cast<uint8_t>(bodyLength >> 24);
        header[2] = static_cast<uint8_t>(bodyLength >> 16);
        header[3] = static_cast<uint8_t>(bodyLength >> 8);
        header[4] = static_cast<uint8_t>(bodyLength);
    }
};

struct PoolBallState {
    int8_t number;
    double x;
    double y;
    double vx;
    double vy;
};

inline void pushBytes(std::vector<uint8_t>& out, const void* data, std::size_t size) {
    const auto* bytes = static_cast<const uint8_t*>(data);
    out.insert(out.end(), bytes, bytes + size);
}

template <typename T>
inline void pushPod(std::vector<uint8_t>& out, const T& value) {
    pushBytes(out, &value, sizeof(T));
}

template <typename T>
inline bool readPod(const std::vector<uint8_t>& data, std::size_t& offset, T& out) {
    if (offset + sizeof(T) > data.size()) return false;
    std::memcpy(&out, data.data() + offset, sizeof(T));
    offset += sizeof(T);
    return true;
}

inline Message<PoolProtocol> makeStateMessage(const std::vector<PoolBallState>& balls) {
    Message<PoolProtocol> msg;
    msg.type = PoolProtocol::MessageType::State;

    std::vector<uint8_t> body;
    body.reserve(sizeof(uint32_t) + balls.size() * (sizeof(int8_t) + sizeof(double) * 4));

    uint32_t count = static_cast<uint32_t>(balls.size());
    pushPod(body, count);

    for (const auto& b : balls) {
        pushPod(body, b.number);
        pushPod(body, b.x);
        pushPod(body, b.y);
        pushPod(body, b.vx);
        pushPod(body, b.vy);
    }

    msg.body = std::move(body);
    msg.bodyLength = static_cast<uint32_t>(msg.body.size());
    return msg;
}


inline bool decodeStateMessage(const Message<PoolProtocol>& msg, std::vector<PoolBallState>& outBalls) {
    if (msg.type != PoolProtocol::MessageType::State) return false;

    std::size_t offset = 0;
    uint32_t count = 0;
    if (!readPod(msg.body, offset, count)) return false;

    const std::size_t perBallSize = sizeof(int8_t) + sizeof(double) * 4;
    if (msg.body.size() < offset + (static_cast<std::size_t>(count) * perBallSize)) return false;

    outBalls.clear();
    outBalls.reserve(count);

    for (uint32_t i = 0; i < count; ++i) {
        PoolBallState b{};
        if (!readPod(msg.body, offset, b.number)) return false;
        if (!readPod(msg.body, offset, b.x)) return false;
        if (!readPod(msg.body, offset, b.y)) return false;
        if (!readPod(msg.body, offset, b.vx)) return false;
        if (!readPod(msg.body, offset, b.vy)) return false;
        outBalls.push_back(b);
    }

    return true;
}

class PoolConnection : public Connection<PoolProtocol> {
public:
    using Connection::Connection;
protected:
    void onMessage(const Message<PoolProtocol>&) override {}
};

class PoolServer : public Server<PoolConnection, PoolProtocol> {
public:
    using OnConnect = std::function<void()>;

    PoolServer(asio::io_context& io, const asio::ip::tcp::endpoint& endpoint, OnConnect onConnect = {})
        : Server<PoolConnection, PoolProtocol>(io, endpoint), io_(io), onConnect_(std::move(onConnect)) {}

    void broadcast(const Message<PoolProtocol>& msg) {
        asio::post(io_, [this, msg]() {
            this->_room.deliver(msg);
        });
    }
protected:
    void doAccept() override {
        this->_acceptor.async_accept([this](std::error_code errorCode, asio::ip::tcp::socket socket) {
            if (!errorCode) {
                if (onConnect_) onConnect_();
                std::make_shared<PoolConnection>(std::move(socket), this->_room)->start();
            }
            doAccept();
        });
    }
private:
    asio::io_context& io_;
    OnConnect onConnect_;
};

class PoolClient : public Client<PoolProtocol> {
public:
    using StateHandler = std::function<void(const std::vector<PoolBallState>&)>;

    PoolClient(asio::io_context& io, const asio::ip::tcp::resolver::results_type& endpoints, StateHandler handler)
        : Client<PoolProtocol>(io, endpoints), handler_(std::move(handler)) {}

protected:
    void onMessage(const Message<PoolProtocol>& msg) override {
        std::vector<PoolBallState> state;
        if (decodeStateMessage(msg, state)) handler_(state);
    }
private:
    StateHandler handler_;
};
