
#include <Arduino.h>
#include <gtest/gtest.h>
#include "loggme.hpp"
#include "test_UpdateOTA.hpp"

void setup()
{
    // Initialize Serial
    Serial.begin(115200);

    logger12 = new MultiPrinterLogger();
    logger12->addPrinter(&Serial);
    logger12->setLogLevel(MultiPrinterLoggerInterface::VERBOSE);

    ::testing::InitGoogleTest();
}

void loop()
{
    if (RUN_ALL_TESTS())
        ;

    delay(1000);

    Serial.println("-----------------------------------Finished all tests!-----------------------------------");

    delay(10000);
}
