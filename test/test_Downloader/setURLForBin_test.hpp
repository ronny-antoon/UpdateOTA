#ifndef SETURLFORBIN_TEST_HPP
#define SETURLFORBIN_TEST_HPP

#include <gtest/gtest.h>

#include "Downloader.hpp"

class setURLForBinTest : public ::testing::Test
{
protected:
    Downloader *downloader;
    const char *urlFirmware = "https://raw.githubusercontent.com/ronny-antoon/UpdateOTA_M/main/examples/updateFirmware/firmware.bin";
    void SetUp() override
    {
        downloader = new Downloader();
    }

    void TearDown() override
    {
        delete downloader;
    }
};

TEST_F(setURLForBinTest, OK)
{
    EXPECT_EQ(DOWNLOADER_ERROR::OK, downloader->setURLForBin(urlFirmware));
}

TEST_F(setURLForBinTest, INVALID_ARGUMENT)
{
    EXPECT_EQ(DOWNLOADER_ERROR::INVALID_ARGUMENT, downloader->setURLForBin(nullptr));
    EXPECT_EQ(DOWNLOADER_ERROR::INVALID_ARGUMENT, downloader->setURLForBin(""));
}

#endif // SETURLFORBIN_TEST_HPP