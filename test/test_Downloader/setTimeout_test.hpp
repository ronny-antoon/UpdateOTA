#ifndef SETTIMEOUT_TEST_HPP
#define SETTIMEOUT_TEST_HPP

#include <gtest/gtest.h>

#include "Downloader.hpp"

class setTimeoutTest : public ::testing::Test
{
protected:
    Downloader *downloader;
    void SetUp() override
    {
        downloader = new Downloader();
    }

    void TearDown() override
    {
        delete downloader;
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