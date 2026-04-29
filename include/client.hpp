#pragma once

#include <asio.hpp>
#include <deque>
#include <array>
#include <cstdint>
#include "message.hpp"
#include <iostream>

template <typename TProtocol>
class Client {
public:
	explicit Client(asio::io_context& io, const asio::ip::tcp::resolver::results_type& endpoints) : _io(io), _socket(io) {
		connect(endpoints);
	}

	void write(const Message<TProtocol>& msg) {
		asio::post(_io, [this, msg]() {
			bool writing = !_queue.empty();
			_queue.push_back(msg);
			if (!writing) doWrite();
		});
	}

	void close() {
		asio::post(_io, [this]() {
			_socket.close();
		});
	}
protected:
	virtual void onMessage(const Message<TProtocol>& msg) = 0;
private:
	asio::io_context& _io;
	asio::ip::tcp::socket _socket;
	Message<TProtocol> _read;
	std::deque<Message<TProtocol>> _queue;
	std::array<uint8_t, TProtocol::headerLength> _header;

	void connect(const asio::ip::tcp::resolver::results_type& endpoints) {
		asio::async_connect(_socket, endpoints, [this](std::error_code errorCode, asio::ip::tcp::endpoint) {
			if (!errorCode) readHeader();
		});
	}

	void readHeader() {
		asio::async_read(_socket, asio::buffer(_header), [this](std::error_code errorCode, std::size_t) {
			if(!errorCode) {
				_read.type = TProtocol::type(_header);
				_read.bodyLength = TProtocol::bodyLength(_header);
				readBody();
			} else _socket.close();
		});
	}

	void readBody() {
		_read.body.resize(_read.bodyLength);
		asio::async_read(_socket, asio::buffer(_read.body), [this](std::error_code errorCode, std::size_t) {
			if (!errorCode) {
				onMessage(_read);
				readHeader();
			} else _socket.close();
		});
	}

	void doWrite() {
		auto buffer = std::make_shared<std::vector<uint8_t>>(_queue.front().serialize());
		asio::async_write(_socket, asio::buffer(*buffer), [this](std::error_code errorCode, std::size_t) {
			if (!errorCode) {
				_queue.pop_front();
				if (!_queue.empty()) doWrite();
			} else _socket.close();
		});
	}
};
