# 🌟UpdateOTA Library🌟

[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/donate/?hosted_button_id=BACPRJTAU4G4E)
[![Latest](https://img.shields.io/github/v/tag/ronny-antoon/UpdateOTA?color=red&label=last+release)](https://github.com/ronny-antoon/UpdateOTA/releases)
[![Latest](https://badges.registry.platformio.org/packages/ronny-antoon/library/UpdateOTA.svg)](https://registry.platformio.org/libraries/ronny-antoon/UpdateOTA)
[![UnitTests](https://github.com/ronny-antoon/UpdateOTA/actions/workflows/build-and-test-embeded.yaml/badge.svg)](https://github.com/ronny-antoon/UpdateOTA/actions/workflows/build-and-test-embeded.yaml)

The UpdateOTA Library facilitates Over-The-Air (OTA) firmware updates for ESP8266-based projects using the Arduino framework. It provides a straightforward interface for handling OTA updates, including the ability to start updates, retrieve version numbers, and convert update errors to human-readable strings.

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Installation](#installation)
- [Dependencies](#dependencies)
- [Usage](#usage)
- [API](#API)
- [Example](#example)
- [License](#license)
- [Contributions](#contributions)
- [Platformio Registry](#platformio-registry)

## Introduction

The UpdateOTA Library streamlines the implementation of Over-The-Air firmware updates for ESP8266-based projects. It introduces an abstract class, *UpdateOTAInterface*, defining methods for starting updates, retrieving version numbers, and handling update errors. The library is designed to work seamlessly with ESP8266, Arduino, and other related libraries.

## Features

- Abstracts OTA update functionality into a clear and consistent interface.
- Handles various update errors through the *UpdateOTAError* enumeration.
- Supports firmware and version retrieval from specified URLs.
- Utilizes secure communication through *WiFiClientSecure* and HTTP requests.
- Provides customizable LED control during the update process.

## Dependencies

The UpdateOTA Library depends on the following libraries:
- MultiPrinterLogger @ 4.1.0
- RelayModule @ 4.1.0
- HTTPClient
- WiFi
- WiFiClientSecure
- esp_ota_ops.h
- esp_partition.h

Ensure that these dependencies are correctly configured in your project.

## Installation

**Method 1**:
To integrate the *UpdateOTA* library into your PlatformIO project, follow these steps:

1. Open "platformio.ini," located in the root of your PlatformIO project.

2. Add the following line to the `lib_deps` option under the `[env:]` section:
```cpp
ronny-antoon/UpdateOTA@^7.0.0
```

3. Build your project, and PlatformIO will automatically handle library installation.

**Method 2**:
To use the **UpdateOTA** library in your Arduino project, follow these steps:

1. Download the latest release from the [GitHub repository](https://github.com/ronny-antoon/UpdateOTA).

2. In the Arduino IDE, click "Sketch" -> "Include Library" -> "Add .ZIP Library" and select the downloaded `.zip` file.

3. Make sure to link your project with the necessary ESP-IDF libraries for NVS support.

## Usage

Include the *UpdateOTA.hpp* header file in your project to use the UpdateOTA library. The *UpdateOTA* class provides methods to initiate firmware updates, retrieve version numbers, and handle update errors. For detailed usage information, refer to the library's source code.

## API

The *UpdateOTA* Library exposes an abstract class, *UpdateOTAInterface*, with methods defining the OTA update interface. Refer to the header files in the source code for comprehensive documentation and usage examples.

## Example

Explore example sketches in the "examples" directory of the library repository to understand the implementation of OTA updates using UpdateOTA.

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
