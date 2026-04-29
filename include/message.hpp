#pragma once

#include <vector>
#include <cstdint>

/*
 * struct ExampleProtocol {
 *     static constexpr std::size_t headerLength = 5;
 *
 *     enum class MessageType : uint8_t {
 *         Handshake = 0x01;
 *         Turn = 0x02;
 *         State = 0x03;
 *         Music = 0x04;
 *     };
 *
 *     static MessageType type(std::span<const uint8_t, headerLength> header) {
 *         return static_cast<MessageType>(header[0]);
 *     }
 *
 *     static uint32_t bodyLength(std::span<const uint8_t, headerLength> header) {
 *         return (header[1] << 24) | (header[2] << 16) | (header[3] << 8) | (header[4]);
 *     }
 * };
 */

template <typename TProtocol>
struct Message {
	TProtocol::MessageType type;
	uint32_t bodyLength;
	std::vector<uint8_t> body;

	std::vector<uint8_t> serialize() const {
		std::array<uint8_t, TProtocol::headerLength> header{};

		TProtocol::encodeHeader(header, type, bodyLength);

		std::vector<uint8_t> out(header.begin(), header.end());

		out.insert(out.end(), body.begin(), body.end());

		return out;
	}
};
