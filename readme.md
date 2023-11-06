# UpdateOTA Library

[![Latest](https://img.shields.io/github/v/tag/ronny-antoon/UpdateOTA?color=red&label=last+release)](https://github.com/ronny-antoon/UpdateOTA/releases)
[![Latest](https://badges.registry.platformio.org/packages/ronny-antoon/library/UpdateOTA.svg)](https://registry.platformio.org/libraries/ronny-antoon/UpdateOTA)
[![UnitTests](https://github.com/ronny-antoon/UpdateOTA/actions/workflows/build-and-test-embeded.yaml/badge.svg)](https://github.com/ronny-antoon/UpdateOTA/actions/workflows/build-and-test-embeded.yaml)

The UpdateOTA library is a C++ library for ESP32 devices that provides a convenient way to download and perform updates on firmware and SPIFFS partitions. It allows you to easily update your ESP32 device's firmware and SPIFFS data from remote servers over HTTP(S) protocol.

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Dependencies](#dependencies)
- [Installation](#installation)
- [Usage](#usage)
- [License](#license)
- [Contributions](#contributions)
- [Platformio Registry](#platformio-registry)

## Introduction

This library is designed to be flexible and extensible, providing you with the ability to easily integrate it into your ESP32 projects. It consists of three main classes:
1. `Downloader`: This class is responsible for downloading files from a URL using the HTTP(S) protocol. It provides methods for setting the URL, configuring a Certificate Authority certificate, and performing the actual download.
2. `Updater`: The Updater class is used to perform updates on the ESP32 device. It provides methods for updating both firmware and SPIFFS partitions. You can set callback functions to be invoked at various stages of the update process, such as start, progress, end, and error.
3.`UpdateOTA`: The UpdateOTA class serves as the main interface for updating firmware and SPIFFS partitions. It combines the functionality of the Downloader and Updater classes to provide a high-level API for updating your ESP32 device. You can use this class to update firmware, SPIFFS data, and check for newer versions of files on remote servers.

## Features

- Easy-to-use interface for downloading and updating files on ESP32 devices.
- Support for firmware and SPIFFS updates.
- Ability to set a Certificate Authority certificate for secure connections.
- Callback functions to track the progress and status of the update process.
- Check for newer versions of files on remote servers.

## Dependencies

The UpdateOTA Library has the following dependencies:
- MultiPrinterLogger @ 3.1.1
- googletest @ 1.12.1
- HTTPClient @ 2.0.0
- WiFi @ 2.0.0
- WiFiClientSecure @ 2.0.0

## Installation

**Method 1**:
To use the **UpdateOTA** library in your PlatformIO project, follow these steps:

1. Open "platformio.ini", a project configuration file located in the root of PlatformIO project.

2. Add the following line to the `lib_deps` option of `[env:]` section:
`ronny-antoon/UpdateOTA@^1.0.0`

3. Build a project, PlatformIO will automatically install dependencies.

**Method 2**:
To use the **UpdateOTA** library in your Arduino project, follow these steps:

1. Download the latest release from the [GitHub repository](https://github.com/ronny-antoon/UpdateOTA).

2. In the Arduino IDE, click "Sketch" -> "Include Library" -> "Add .ZIP Library" and select the downloaded `.zip` file.

3. Make sure to link your project with the necessary ESP-IDF libraries for NVS support.

## Usage

- Here's a basic example of how to use the UpdateOTA library to perform a firmware update:

```cpp
// Dont forget to connect to wifi internet
#include "UpdateOTA.hpp"

// Create Downloader and Updater objects
Downloader downloader;
Updater updater;

// Create UpdateOTA object with Downloader and Updater
UpdateOTA updateOTA(&downloader, &updater);

void setup() {
  Serial.begin(115200);
  
  // Set the Certificate Authority certificate (optional)
  const char *caCertificate = "-----BEGIN CERTIFICATE-----\n...";
  updateOTA.setCACertificate(caCertificate);
}

void loop() {
  // Check for a newer version of firmware
  const char *firmwareURL = "https://example.com/firmware.bin";
  const char *versionURL = "https://example.com/version.txt";
  const char *currentVersion = "1.0.0";
  
  UpdateOTA_ERROR result = updateOTA.updateFirmware(firmwareURL, versionURL, currentVersion);
  
  if (result == UPDATE_OTA_ERROR::OK) {
    Serial.println("Firmware update completed successfully!");
  } else if (result == UPDATE_OTA_ERROR::NO_NEW_VERSION) {
    Serial.println("No new firmware version available.");
  } else {
    Serial.println("Firmware update failed.");
  }
}
```

## License

This library is provided under the MIT License. You are free to use, modify, and distribute it as needed.

## Contributions

If you'd like to contribute to the Database Library, please follow these guidelines:
1. Fork the repository.
2. Make your changes and document them.
3. Submit a pull request.

## Author

This library was created by Ronny Antoon. You can contact me at [ronny.antoon@gmail.com] for any questions or feedback.

## Platformio Registry

https://registry.platformio.org/libraries/ronny-antoon/UpdateOTA
