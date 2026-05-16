#include <bitset>
#include <cstdint>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <valarray>
#include <vector>

#include "base85ed.h"

// TODO: remove this
static std::vector<uint8_t> run_command_io(const std::string &command,
                                           const std::vector<uint8_t> &in) {
  int inpipe[2];  // parent -> child
  int outpipe[2]; // child -> parent

  if (pipe(inpipe) == -1)
    throw std::runtime_error(strerror(errno));
  if (pipe(outpipe) == -1) {
    close(inpipe[0]);
    close(inpipe[1]);
    throw std::runtime_error(strerror(errno));
  }

  pid_t pid = fork();
  if (pid == -1) {
    close(inpipe[0]);
    close(inpipe[1]);
    close(outpipe[0]);
    close(outpipe[1]);
    throw std::runtime_error(strerror(errno));
  }

  if (pid == 0) {
    // child
    dup2(inpipe[0], STDIN_FILENO);
    dup2(outpipe[1], STDOUT_FILENO);
    close(inpipe[0]);
    close(inpipe[1]);
    close(outpipe[0]);
    close(outpipe[1]);
    execl("/bin/sh", "sh", "-c", command.c_str(), (char *)nullptr);
    _exit(127);
  }

  // parent
  close(inpipe[0]);
  close(outpipe[1]);

  // write input
  const uint8_t *wp = in.data();
  ssize_t remaining = static_cast<ssize_t>(in.size());
  while (remaining > 0) {
    ssize_t n = write(inpipe[1], wp, remaining);
    if (n == -1) {
      if (errno == EINTR)
        continue;
      close(inpipe[1]);
      close(outpipe[0]);
      waitpid(pid, nullptr, 0);
      throw std::runtime_error(strerror(errno));
    }
    remaining -= n;
    wp += n;
  }
  close(inpipe[1]); // signal EOF

  // read all stdout
  std::vector<uint8_t> out;
  uint8_t buf[4096];
  while (true) {
    ssize_t n = read(outpipe[0], buf, sizeof(buf));
    if (n > 0)
      out.insert(out.end(), buf, buf + n);
    else if (n == 0)
      break;
    else {
      if (errno == EINTR)
        continue;
      close(outpipe[0]);
      waitpid(pid, nullptr, 0);
      throw std::runtime_error(strerror(errno));
    }
  }
  close(outpipe[0]);

  int status = 0;
  if (waitpid(pid, &status, 0) == -1)
    throw std::runtime_error(strerror(errno));
  if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
    throw std::runtime_error("child exited with non-zero status");

  return out;
}

static std::string alphabet85 =
    R"(0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!#$%&()*+-;<=>?@^_`{|}~)";

// TODO: implement this in C++
std::vector<uint8_t> base85::executeBytes(uint8_t &byte1, uint8_t &byte2,
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

      auto bytesToInsert = executeBytes(byte1, byte2, byte3, byte4);

      for (auto i = bytesToInsert.rbegin(); i != bytesToInsert.rend(); i++) {
        encoded_bytes.push_back(alphabet85[*i]);
      }
    }
  }
  if (remainder == 1) {
    uint8_t byte1 = *(bytes.end() - 1);
    uint8_t byte2 = 0;
    uint8_t byte3 = 0;
    uint8_t byte4 = 0;
    auto bytesToInsert = executeBytes(byte1, byte2, byte3, byte4);

    for (auto i = bytesToInsert.rbegin(); i != bytesToInsert.rend() - 3; i++) {
      encoded_bytes.push_back(alphabet85[*i]);
    }
  }

  if (remainder == 2) {
    uint8_t byte1 = *(bytes.end() - 2);
    uint8_t byte2 = *(bytes.end() - 1);
    uint8_t byte3 = 0;
    uint8_t byte4 = 0;

    auto bytesToInsert = executeBytes(byte1, byte2, byte3, byte4);

    for (auto i = bytesToInsert.rbegin(); i != bytesToInsert.rend() - 2; i++) {
      encoded_bytes.push_back(alphabet85[*i]);
    }
  }

  if (remainder == 3) {
    uint8_t byte1 = *(bytes.end() - 3);
    uint8_t byte2 = *(bytes.end() - 2);
    uint8_t byte3 = *(bytes.end() - 1);
    uint8_t byte4 = 0;

    auto bytesToInsert = executeBytes(byte1, byte2, byte3, byte4);

    for (auto i = bytesToInsert.rbegin(); i != bytesToInsert.rend() - 1; i++) {
      encoded_bytes.push_back(alphabet85[*i]);
    }
  }

  return encoded_bytes;
}
// TODO: implement this in C++
std::vector<uint8_t> base85::decode(std::vector<uint8_t> const &b85str) {

  

 
}
