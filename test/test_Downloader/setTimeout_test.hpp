#ifndef SETTIMEOUT_TEST_HPP
#define SETTIMEOUT_TEST_HPP

#include <gtest/gtest.h>

#include "Downloader.hpp"

class setTimeoutTest : public ::testing::Test
{
protected:
    uint32_t _startFreeHeap;
    Downloader *downloader;
    void SetUp() override
    {
        delay(10);
        _startFreeHeap = ESP.getFreeHeap();
        downloader = new Downloader();
    }

    void TearDown() override
    {
        delete downloader;
        delay(10);
        if (ESP.getFreeHeap() != _startFreeHeap)
            FAIL() << "Memory leak of " << _startFreeHeap - ESP.getFreeHeap() << " bytes"; // Fail the test if there is a memory leak
    }
};

TEST_F(setTimeoutTest, OK)
{
    EXPECT_EQ(DOWNLOADER_ERROR::OK, downloader->setTimeout(10000));
}

TEST_F(setTimeoutTest, INVALID_ARGUMENT)
{
    EXPECT_EQ(DOWNLOADER_ERROR::INVALID_ARGUMENT, downloader->setTimeout(0));
    EXPECT_EQ(DOWNLOADER_ERROR::INVALID_ARGUMENT, downloader->setTimeout(-1));
    EXPECT_EQ(DOWNLOADER_ERROR::INVALID_ARGUMENT, downloader->setTimeout(20));
    EXPECT_EQ(DOWNLOADER_ERROR::INVALID_ARGUMENT, downloader->setTimeout(60001));
}

#endif // SETTIMEOUT_TEST_HPP