#ifndef __BASE85ED__
#define __BASE85ED__

#include <cstdint>
#include <vector>

/**
 * @namespace base85
 * @brief Utilities for Base85 <en/de>coding of bytes
 */
namespace base85 {

/**
 * @brief Encodes bytes into a Base85.
 *
 * @param[in] bytes A vector containing the raw bytes.
 * @return std::vector<uint8_t> A vector of Base85-encoded bytes.
 *
 * @see decode()
 */
std::vector<uint8_t>
encode(std::vector<uint8_t> const &bytes); // TODO: add @throws if needed

std::vector<uint8_t> encodeBytes(uint8_t &byte1, uint8_t &byte2, uint8_t &byte3,
                                 uint8_t &byte4);

/**
 * @brief Decodes Base85 to bytes.
 *
 * @param[in] b85str A vector containing Base85-encoded data.
 * @return std::vector<uint8_t> The decoded bytes.
 *
 * @see encode()
 */
std::vector<uint8_t>
decode(std::vector<uint8_t> const &b85str); // TODO: add @throws if needed
std::vector<uint8_t> decodeChars(uint8_t &char1, uint8_t &char2, uint8_t &char3,
                                 uint8_t &char4, uint8_t &char5);

} // namespace base85

#endif // __BASE85ED__
