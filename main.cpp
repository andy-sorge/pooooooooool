#include <iostream>
#include <string>
#include <cstdint>
#include <vector>

#include "message.hpp"
#include "server.hpp"
#include "client.hpp"

#define ADDRESS "127.0.0.1"
#define PORT 58008
#define PORT_STRING "58008"

// example of a protocol, has only one message type
struct TestProtocol {
	static constexpr std::size_t headerLength = 5;

	enum class MessageType : uint8_t {
		Test  = 0x01
	};

	static MessageType type(std::span<const uint8_t, headerLength> header) {
		return static_cast<MessageType>(header[0]);
	}

	static uint32_t bodyLength(std::span<const uint8_t, headerLength> header) {
		return (header[1] << 24) | (header[2] << 16) | (header[3] << 8) | header[4];
	}

	static void encodeHeader(std::array<uint8_t, 5>& header, MessageType type, uint32_t bodyLength) {
		header[0] = static_cast<uint8_t>(type);
		header[1] = bodyLength >> 24;
		header[2] = bodyLength >> 16;
		header[3] = bodyLength >> 8;
		header[4] = bodyLength;
	}
};

// test message, implementing basics
struct TestMessage : Message<TestProtocol> {
	static TestMessage text(const std::string& string) {
		TestMessage msg;
		msg.type = TestProtocol::MessageType::Test;
		msg.body = std::vector<uint8_t>(string.begin(), string.end());
		msg.bodyLength = static_cast<uint32_t>(msg.body.size());
		return msg;
	}
};

// connection, handles messages per connection
class TestConnection : public Connection<TestProtocol> {
public:
	using Connection<TestProtocol>::Connection;
protected:
	void onMessage(const Message<TestProtocol>& msg) override {
		Connection<TestProtocol>::room().deliver(msg);
	}
};

class TestClient : public Client<TestProtocol> {
public:
	using Client<TestProtocol>::Client;
protected:
	void onMessage(const Message<TestProtocol>& msg) override {
		std::cout << TestClient::bodyAsString(msg) << std::endl;
	}

	static std::string bodyAsString(const Message<TestProtocol>& msg) {
		return std::string(msg.body.begin(), msg.body.end());
	}
};

int main() {
	asio::io_context io;

	Server<TestConnection, TestProtocol> server(io, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), PORT));

	asio::ip::tcp::resolver resolver(io);
	auto endpoints = resolver.resolve(ADDRESS, PORT_STRING);

	TestClient a(io, endpoints), b(io, endpoints);

	std::thread thread([&io]() {
		io.run();
	});

	std::string line;
	while (std::getline(std::cin, line)) {
		if (line == "quit") break;
		a.write(TestMessage::text(line));
	}

	a.close();
	b.close();

	io.stop();
	thread.join();

	return 0;
}
