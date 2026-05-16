#include <array>
#include <bitset>
#include <cstdint>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#include "base85ed.h"

// TODO: remove this
// static std::vector<uint8_t> run_command_io(const std::string &command,
//                                            const std::vector<uint8_t> &in) {
//   int inpipe[2];  // parent -> child
//   int outpipe[2]; // child -> parent

//   if (pipe(inpipe) == -1)
//     throw std::runtime_error(strerror(errno));
//   if (pipe(outpipe) == -1) {
//     close(inpipe[0]);
//     close(inpipe[1]);
//     throw std::runtime_error(strerror(errno));
//   }

//   pid_t pid = fork();
//   if (pid == -1) {
//     close(inpipe[0]);
//     close(inpipe[1]);
//     close(outpipe[0]);
//     close(outpipe[1]);
//     throw std::runtime_error(strerror(errno));
//   }

//   if (pid == 0) {
//     // child
//     dup2(inpipe[0], STDIN_FILENO);
//     dup2(outpipe[1], STDOUT_FILENO);
//     close(inpipe[0]);
//     close(inpipe[1]);
//     close(outpipe[0]);
//     close(outpipe[1]);
//     execl("/bin/sh", "sh", "-c", command.c_str(), (char *)nullptr);
//     _exit(127);
//   }

//   // parent
//   close(inpipe[0]);
//   close(outpipe[1]);

//   // write input
//   const uint8_t *wp = in.data();
//   ssize_t remaining = static_cast<ssize_t>(in.size());
//   while (remaining > 0) {
//     ssize_t n = write(inpipe[1], wp, remaining);
//     if (n == -1) {
//       if (errno == EINTR)
//         continue;
//       close(inpipe[1]);
//       close(outpipe[0]);
//       waitpid(pid, nullptr, 0);
//       throw std::runtime_error(strerror(errno));
//     }
//     remaining -= n;
//     wp += n;
//   }
//   close(inpipe[1]); // signal EOF

//   // read all stdout
//   std::vector<uint8_t> out;
//   uint8_t buf[4096];
//   while (true) {
//     ssize_t n = read(outpipe[0], buf, sizeof(buf));
//     if (n > 0)
//       out.insert(out.end(), buf, buf + n);
//     else if (n == 0)
//       break;
//     else {
//       if (errno == EINTR)
//         continue;
//       close(outpipe[0]);
//       waitpid(pid, nullptr, 0);
//       throw std::runtime_error(strerror(errno));
//     }
//   }
//   close(outpipe[0]);

//   int status = 0;
//   if (waitpid(pid, &status, 0) == -1)
//     throw std::runtime_error(strerror(errno));
//   if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
//     throw std::runtime_error("child exited with non-zero status");

//   return out;
// }

static std::string encode_alphabet =
    R"(0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!#$%&()*+-;<=>?@^_`{|}~)";

// TODO: implement this in C++
std::vector<uint8_t> base85::encodeBytes(uint8_t &byte1, uint8_t &byte2,
                                         uint8_t &byte3, uint8_t &byte4) {

  uint32_t value = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | (byte4);

  uint8_t rem5 = value % 85;
  value /= 85;
  uint8_t rem4 = value % 85;
  value /= 85;
  uint8_t rem3 = value % 85;
  value /= 85;
  uint8_t rem2 = value % 85;
  value /= 85;
  uint8_t rem1 = value % 85;
  return {rem5, rem4, rem3, rem2, rem1};
}

std::vector<uint8_t> base85::encode(std::vector<uint8_t> const &bytes) {

  std::vector<uint8_t> encoded_bytes;
  std::size_t remainder = bytes.size() % 4;
  std::size_t div = bytes.size() / 4;

  if (remainder == 0 && div == 0) {
    return encoded_bytes;
  }

  if (div != 0) {
    for (uint8_t byte_index = 0; byte_index < bytes.size() - 3;
         byte_index += 4) {
      uint8_t byte1 = bytes[byte_index];
      uint8_t byte2 = bytes[byte_index + 1];
      uint8_t byte3 = bytes[byte_index + 2];
      uint8_t byte4 = bytes[byte_index + 3];

      auto bytesToInsert = encodeBytes(byte1, byte2, byte3, byte4);

      for (auto i = bytesToInsert.rbegin(); i != bytesToInsert.rend(); i++) {
        encoded_bytes.push_back(encode_alphabet[*i]);
      }
    }
  }
  if (remainder == 1) {
    uint8_t byte1 = *(bytes.end() - 1);
    uint8_t byte2 = 0;
    uint8_t byte3 = 0;
    uint8_t byte4 = 0;
    auto bytesToInsert = encodeBytes(byte1, byte2, byte3, byte4);

    for (auto i = bytesToInsert.rbegin(); i != bytesToInsert.rend() - 3; i++) {
      encoded_bytes.push_back(encode_alphabet[*i]);
    }
  }

  if (remainder == 2) {
    uint8_t byte1 = *(bytes.end() - 2);
    uint8_t byte2 = *(bytes.end() - 1);
    uint8_t byte3 = 0;
    uint8_t byte4 = 0;

    auto bytesToInsert = encodeBytes(byte1, byte2, byte3, byte4);

    for (auto i = bytesToInsert.rbegin(); i != bytesToInsert.rend() - 2; i++) {
      encoded_bytes.push_back(encode_alphabet[*i]);
    }
  }

  if (remainder == 3) {
    uint8_t byte1 = *(bytes.end() - 3);
    uint8_t byte2 = *(bytes.end() - 2);
    uint8_t byte3 = *(bytes.end() - 1);
    uint8_t byte4 = 0;

    auto bytesToInsert = encodeBytes(byte1, byte2, byte3, byte4);

    for (auto i = bytesToInsert.rbegin(); i != bytesToInsert.rend() - 1; i++) {
      encoded_bytes.push_back(encode_alphabet[*i]);
    }
  }

  return encoded_bytes;
}

std::vector<uint8_t> base85::decodeChars(uint8_t &char1, uint8_t &char2,
                                         uint8_t &char3, uint8_t &char4,
                                         uint8_t &char5) {
  uint32_t number = char1 * 85 * 85 * 85 * 85 + char2 * 85 * 85 * 85 +
                    char3 * 85 * 85 + char4 * 85 + char5;

  uint8_t byte1 = (number >> 24) & 0xFF;
  uint8_t byte2 = (number >> 16) & 0xFF;
  uint8_t byte3 = (number >> 8) & 0xFF;
  uint8_t byte4 = number & 0xFF;

  return {byte1, byte2, byte3, byte4};
}

// TODO: implement this in C++
std::vector<uint8_t> base85::decode(std::vector<uint8_t> const &b85str) {
  std::size_t div = b85str.size() / 5;
  std::size_t remainder = b85str.size() % 5;

  std::array<uint8_t, 256> decode_alphabet;

  for (std::size_t i = 0; i < 85; i++) {
    decode_alphabet[encode_alphabet[i]] = i;
  }

  std::vector<uint8_t> decodedBytes;

  if (div != 0) {
    for (std::size_t vectorIndex = 0; vectorIndex + 4 < b85str.size();
         vectorIndex += 5) {
      uint8_t char1 = decode_alphabet[b85str[vectorIndex]];
      uint8_t char2 = decode_alphabet[b85str[vectorIndex + 1]];
      uint8_t char3 = decode_alphabet[b85str[vectorIndex + 2]];
      uint8_t char4 = decode_alphabet[b85str[vectorIndex + 3]];
      uint8_t char5 = decode_alphabet[b85str[vectorIndex + 4]];

      std::vector<uint8_t> bytesToAdd =
          decodeChars(char1, char2, char3, char4, char5);
      decodedBytes.insert(decodedBytes.end(), bytesToAdd.begin(),
                          bytesToAdd.end());
    }
  }
  if (remainder == 4) {
    uint8_t char1 = decode_alphabet[*(b85str.end() - 4)];
    uint8_t char2 = decode_alphabet[*(b85str.end() - 3)];
    uint8_t char3 = decode_alphabet[*(b85str.end() - 2)];
    uint8_t char4 = decode_alphabet[*(b85str.end() - 1)];
    uint8_t char5 = decode_alphabet['~']; // pass last element of alphabet

    std::vector<uint8_t> bytesToAdd =
        decodeChars(char1, char2, char3, char4, char5);

    decodedBytes.insert(decodedBytes.end(), bytesToAdd.begin(),
                        bytesToAdd.end() - 1);
  }
  if (remainder == 3) {
    uint8_t char1 = decode_alphabet[*(b85str.end() - 3)];
    uint8_t char2 = decode_alphabet[*(b85str.end() - 2)];
    uint8_t char3 = decode_alphabet[*(b85str.end() - 1)];
    uint8_t char4 = decode_alphabet['~'];
    uint8_t char5 = decode_alphabet['~'];

    std::vector<uint8_t> bytesToAdd =
        decodeChars(char1, char2, char3, char4, char5);
    decodedBytes.insert(decodedBytes.end(), bytesToAdd.begin(),
                        bytesToAdd.end() - 2);
  }

  if (remainder == 2) {
    uint8_t char1 = decode_alphabet[*(b85str.end() - 2)];
    uint8_t char2 = decode_alphabet[*(b85str.end() - 1)];
    uint8_t char3 = decode_alphabet['~'];
    uint8_t char4 = decode_alphabet['~'];
    uint8_t char5 = decode_alphabet['~'];

    std::vector<uint8_t> bytesToAdd =
        decodeChars(char1, char2, char3, char4, char5);
    decodedBytes.insert(decodedBytes.end(), bytesToAdd.begin(),
                        bytesToAdd.end() - 3);
  }
  if (remainder == 1) {
    std::cerr << "Invalid input";
  }
  return decodedBytes;
}
