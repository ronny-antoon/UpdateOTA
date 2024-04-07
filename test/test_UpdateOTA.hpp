#ifndef TEST_UPDATE_OTA_HPP
#define TEST_UPDATE_OTA_HPP

#include <gtest/gtest.h>

#include "UpdateOTA.hpp"
#include "MultiPrinterLogger.hpp"

class UpdateOTATest : public ::testing::Test
{
protected:
    UpdateOTA *_updateOTA;

    const char *_uRL = "https://raw.githubusercontent.com/ronny-antoon/UpdateOTA/main/examples/firmware.bin";
    const char *_uRLVersion = "https://raw.githubusercontent.com/ronny-antoon/UpdateOTA/main/examples/version.txt";
    const char *WIFI_SSID = "Wokwi-GUEST";
    const char *WIFI_PASSWORD = "";
    const int WIFI_CHANNEL = 6;
    MultiPrinterLogger *logger = new MultiPrinterLogger();
    logger->addPrinter(Serial);
    logger->setLogLevel(MultiPrinterLoggerInterface::LogLevel::VERBOSE);
    

    void SetUp() override
    {
        // Initialize UpdateOTA
        _updateOTA = new UpdateOTA(logger);
    }

    void TearDown() override
    {
        // Clean up resources
        delete _updateOTA;
    }
};

// getVersionNumber no internet
TEST_F(UpdateOTATest, getVersionNumber_NO_INTERNET)
{
    char buffer[10];
    UpdateOTAError err = _updateOTA->getVersionNumber(_uRLVersion, buffer, sizeof(buffer));
    EXPECT_EQ(err, UpdateOTAError::NO_INTERNET);
}

// getVersionNumber page not found
TEST_F(UpdateOTATest, getVersionNumber_PAGE_NOT_FOUND)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
    int timeOut = 5;
    while (WiFi.status() != WL_CONNECTED && timeOut > 0)
    {
        delay(500);
        timeOut--;
    }

    char buffer[10];
    UpdateOTAError err = _updateOTA->getVersionNumber("https://raw.githubusercontent.com/ronny-antoon/UpdateOTA/main/examples/version1.txt", buffer, sizeof(buffer));
    EXPECT_EQ(err, UpdateOTAError::PAGE_NOT_FOUND);

    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}

// getVersionNumber success with returned version number 5.1.1
TEST_F(UpdateOTATest, getVersionNumber_SUCCESS)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
    int timeOut = 5;
    while (WiFi.status() != WL_CONNECTED && timeOut > 0)
    {
        delay(500);
        timeOut--;
    }

    char buffer[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    UpdateOTAError err = _updateOTA->getVersionNumber(_uRLVersion, buffer, sizeof(buffer));
    EXPECT_EQ(err, UpdateOTAError::SUCCESS);
    // EXPECT_STREQ(buffer, "5.1.1");

    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}

// startUpdate no internet
TEST_F(UpdateOTATest, startUpdate_NO_INTERNET)
{
    UpdateOTAError err = _updateOTA->startUpdate(_uRL, true);
    EXPECT_EQ(err, UpdateOTAError::NO_INTERNET);
}

// startUpdate page not found
TEST_F(UpdateOTATest, startUpdate_PAGE_NOT_FOUND)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
    int timeOut = 5;
    while (WiFi.status() != WL_CONNECTED && timeOut > 0)
    {
        delay(500);
        timeOut--;
    }

    UpdateOTAError err = _updateOTA->startUpdate("https://raw.githubusercontent.com/ronny-antoon/UpdateOTA/main/examples/firmware1.bin", true);
    EXPECT_EQ(err, UpdateOTAError::PAGE_NOT_FOUND);

    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}

// startUpdate success
TEST_F(UpdateOTATest, startUpdate_SUCCESS)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
    int timeOut = 5;
    while (WiFi.status() != WL_CONNECTED && timeOut > 0)
    {
        delay(500);
        timeOut--;
    }

    UpdateOTAError err = _updateOTA->startUpdate(_uRL, true);
    EXPECT_EQ(err, UpdateOTAError::SUCCESS);

    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}
#endif // TEST_UPDATE_OTA_HPP