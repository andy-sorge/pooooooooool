#pragma once

#include "asio.hpp"
#include <memory>
#include <deque>
#include <set>
#include <array>
#include <cstdint>

#include "message.hpp"

template <typename TProtocol> class Room;

template <typename TProtocol>
class Connection : public std::enable_shared_from_this<Connection<TProtocol>> {
public:
	explicit Connection(asio::ip::tcp::socket socket, Room<TProtocol>& room) : _socket(std::move(socket)), _room(room) {}

	virtual ~Connection() = default;

	virtual void start() {
		_room.join(std::move(this->shared_from_this()));
		readHeader();
	}

	virtual void deliver(const Message<TProtocol>& msg) {
		bool writing  = !_queue.empty();
		_queue.push_back(msg);
		if (!writing) write();
	}
protected:
	virtual void onMessage(const Message<TProtocol>& msg) = 0;

	Room<TProtocol>& room() const {
	    return _room;
	}
private:
	asio::ip::tcp::socket _socket;
	Room<TProtocol>& _room;
	Message<TProtocol> _read;
	std::deque<Message<TProtocol>> _queue;

	std::array<uint8_t, TProtocol::headerLength> _header{};

	void readHeader() {
		auto self(this->shared_from_this());
		asio::async_read(_socket, asio::buffer(_header), [this, self](std::error_code errorCode, std::size_t) {
			if (!errorCode) {
				_read.type = TProtocol::type(_header);
				_read.bodyLength = TProtocol::bodyLength(_header);
				readBody();
			} else _room.leave(this->shared_from_this());
		});
	}

	void readBody() {
		auto self(this->shared_from_this());
		_read.body.resize(_read.bodyLength);
		asio::async_read(_socket, asio::buffer(_read.body), [this, self](std::error_code errorCode, std::size_t) {
			if (!errorCode) {
				onMessage(_read);
				readHeader();
			} else _room.leave(this->shared_from_this());
		});
	}

	void write() {
		auto self(this->shared_from_this());
		asio::async_write(_socket, asio::buffer(_queue.front().serialize()), [this, self](std::error_code errorCode, std::size_t) {
			if (!errorCode) {
				_queue.pop_front();
				if (!_queue.empty()) write();
			} else _room.leave(this->shared_from_this());
		});
	}
};

template <typename TProtocol>
class Room {
public:
	void join(std::shared_ptr<Connection<TProtocol>> connection) {
		_connections.insert(connection);
	}

	void leave(std::shared_ptr<Connection<TProtocol>> connection) {
		_connections.erase(connection);
	};

	void deliver(const Message<TProtocol>& msg) {
		for (auto& connection : _connections) connection->deliver(msg);
	};
protected:
	std::set<std::shared_ptr<Connection<TProtocol>>> _connections;
};

template <typename TConnection, typename TProtocol>
class Server {
public:
	explicit Server(asio::io_context& io, const asio::ip::tcp::endpoint& endpoint) : _acceptor(io, endpoint) {
		doAccept();
	}
protected:
	asio::ip::tcp::acceptor _acceptor;
	Room<TProtocol> _room;

	virtual void doAccept() {
		_acceptor.async_accept([this](std::error_code errorCode, asio::ip::tcp::socket socket) {
			if (!errorCode) std::make_shared<TConnection>(std::move(socket), _room)->start();

			doAccept();
		});
	}
};
