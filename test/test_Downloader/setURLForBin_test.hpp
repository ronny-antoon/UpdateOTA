#ifndef SETURLFORBIN_TEST_HPP
#define SETURLFORBIN_TEST_HPP

#include <gtest/gtest.h>

#include "Downloader.hpp"

class setURLForBinTest : public ::testing::Test
{
protected:
    uint32_t _startFreeHeap;
    Downloader *downloader;
    const char *urlFirmware = "https://raw.githubusercontent.com/ronny-antoon/UpdateOTA/main/examples/firmware.bin";
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