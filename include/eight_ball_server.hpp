#include "server.hpp"

class EightBallConnection : public Connection {
public:
	explicit EightBallConnection(tcp::socket socket) : Connection(std::move(socket)) {}

	override void start() {
	
	}
};	

class EightBallServer : public Server<EightBallConnection> {
public:
};
	explicit EightBallServer()V
