#include <gtest/gtest.h>

#include <cstdint>
#include <stdexcept>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#include "base85ed.h"

const std::vector<std::pair<const char *, const char *>> short_cases =
{
    {"", ""},
    {"F#", "1"},
    {"F){", "12"},
    {"F)}j", "123"},
    {"F)}kW", "1234"}
};

static std::vector<uint8_t> cstr2v(const char *s)
{
    return std::vector<uint8_t>(
               s,
               s + std::string(s).size());
}

static std::vector<uint8_t> v(std::initializer_list<uint8_t> bytes)
{
    return std::vector<uint8_t>(bytes);
}

// Тесты encode
TEST(Base85ShortsEncode, TrivialShortEncodes)
{
    for (const auto &p : short_cases)
    {
        EXPECT_EQ(base85::encode(cstr2v(p.second)), cstr2v(p.first));
    }
}

// Тесты decode
TEST(Base85ShortsDecode, TrivialShortDecodes)
{
    for (const auto &p : short_cases)
    {
        EXPECT_EQ(base85::decode(cstr2v(p.first)), cstr2v(p.second));
    }
}

// Encode
TEST(Base85Encode, EmptyInput)
{
    EXPECT_EQ(base85::encode(v({})), v({}));
}

TEST(Base85Encode, SingleZeroByte)
{
    EXPECT_EQ(base85::encode(v({0})), cstr2v("00"));
}

TEST(Base85Encode, ThreeAsciiBytes)
{
    EXPECT_EQ(base85::encode(v({'a', 'b', 'c'})), cstr2v("VPaz"));
}

TEST(Base85Encode, FullBlockAsciiBytes)
{
    EXPECT_EQ(base85::encode(v({'a', 'b', 'c', 'd'})), cstr2v("VPa!s"));
}

TEST(Base85Encode, BinaryBytes)
{
    EXPECT_EQ(base85::encode(v({0, 255, 1, 254})), cstr2v("0RI90"));
}

// Decode

TEST(Base85Decode, EmptyInput)
{
    EXPECT_EQ(base85::decode(v({})), v({}));
}

TEST(Base85Decode, SingleZeroByte)
{
    EXPECT_EQ(base85::decode(cstr2v("00")), v({0}));
}

TEST(Base85Decode, ThreeAsciiBytes)
{
    EXPECT_EQ(base85::decode(cstr2v("VPaz")), v({'a', 'b', 'c'}));
}

TEST(Base85Decode, FullBlockAsciiBytes)
{
    EXPECT_EQ(base85::decode(cstr2v("VPa!s")), v({'a', 'b', 'c', 'd'}));
}

TEST(Base85Decode, BinaryBytes)
{
    EXPECT_EQ(base85::decode(cstr2v("0RI90")), v({0, 255, 1, 254}));
}
