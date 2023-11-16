#ifndef GETVERSION_TEST_HPP
#define GETVERSION_TEST_HPP

#include <gtest/gtest.h>

#include "Downloader.hpp"

class getVersionTest : public ::testing::Test
{
protected:
    Downloader *downloader;
    const char *urlvirsion = "https://raw.githubusercontent.com/ronny-antoon/UpdateOTA/main/examples/version.txt";
    const char *CA_CERT =
        "-----BEGIN CERTIFICATE-----\n"
        // (Certificate data goes here)
        "MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n"
        "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
        "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"
        "QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n"
        "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
        "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n"
        "9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n"
        "CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n"
        "nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n"
        "43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n"
        "T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n"
        "gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n"
        "BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n"
        "TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n"
        "DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n"
        "hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n"
        "06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n"
        "PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n"
        "YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n"
        "CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n"
        "-----END CERTIFICATE-----\n";
    const char *WIFI_SSID = "Wokwi-GUEST";
    const char *WIFI_PASSWORD = "";
    const int WIFI_CHANNEL = 6;

    void SetUp() override
    {
        downloader = new Downloader();
    }

    void TearDown() override
    {
        delete downloader;
    }
};

TEST_F(getVersionTest, NO_URL_PROVIDED)
{
    char version[10];
    EXPECT_EQ(DOWNLOADER_ERROR::NO_URL_PROVIDED, downloader->getVersion(version, 10));
}

TEST_F(getVersionTest, INVALID_ARGUMENT)
{
    char version[10];
    EXPECT_EQ(DOWNLOADER_ERROR::INVALID_ARGUMENT, downloader->getVersion(version, 0));
    EXPECT_EQ(DOWNLOADER_ERROR::INVALID_ARGUMENT, downloader->getVersion(NULL, 0));
    EXPECT_EQ(DOWNLOADER_ERROR::INVALID_ARGUMENT, downloader->getVersion(NULL, 10));
}

TEST_F(getVersionTest, OK)
{
    char version[10];
    delay(100);
    WiFi.mode(WIFI_STA);
    delay(100);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
    delay(500);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
    }
    EXPECT_EQ(DOWNLOADER_ERROR::OK, downloader->setCA(CA_CERT));
    EXPECT_EQ(DOWNLOADER_ERROR::OK, downloader->setURLForVersion(urlvirsion));
    EXPECT_EQ(DOWNLOADER_ERROR::OK, downloader->getVersion(version, 10));
    EXPECT_STREQ("5.1.1", version);
}

TEST_F(getVersionTest, SERVER_ERROR)
{
    char version[10];
    EXPECT_EQ(DOWNLOADER_ERROR::OK, downloader->setURLForVersion("https://www.google.com/404"));
    EXPECT_EQ(DOWNLOADER_ERROR::SERVER_ERROR, downloader->getVersion(version, 10));
}

TEST_F(getVersionTest, NO_WIFI)
{
    WiFi.disconnect();
    char version[10];
    EXPECT_EQ(DOWNLOADER_ERROR::OK, downloader->setURLForVersion(urlvirsion));
    EXPECT_EQ(DOWNLOADER_ERROR::OK, downloader->setCA(CA_CERT));
    EXPECT_EQ(DOWNLOADER_ERROR::SERVER_ERROR, downloader->getVersion(version, 10));
}

#endif // GETVERSION_TEST_HPP