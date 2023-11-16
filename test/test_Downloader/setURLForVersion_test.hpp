#ifndef SETURLFORVERSION_TEST_HPP
#define SETURLFORVERSION_TEST_HPP

#include <gtest/gtest.h>

#include "Downloader.hpp"

class setURLForVersionTest : public ::testing::Test
{
protected:
    Downloader *downloader;
    const char *urlvirsion = "https://raw.githubusercontent.com/ronny-antoon/UpdateOTA/main/examples/version.txt";
    void SetUp() override
    {
        downloader = new Downloader();
    }

    void TearDown() override
    {
        delete downloader;
    }
};

TEST_F(setURLForVersionTest, OK)
{
    EXPECT_EQ(DOWNLOADER_ERROR::OK, downloader->setURLForVersion(urlvirsion));
}

TEST_F(setURLForVersionTest, INVALID_ARGUMENT)
{
    EXPECT_EQ(DOWNLOADER_ERROR::INVALID_ARGUMENT, downloader->setURLForVersion(nullptr));
    EXPECT_EQ(DOWNLOADER_ERROR::INVALID_ARGUMENT, downloader->setURLForVersion(""));
}

#endif // SETURLFORVERSION_TEST_HPP