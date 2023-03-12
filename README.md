# AWS IoT ExpressLink - Arduino library

This project is a library to interface with AWS IoT ExpressLink modules with Arduino microcontrollers. It uses the AT commands and provides a C++ wrapper for parsing and integrating into your Arduino sketches using setup+loop functions.

It currently follows and implements the AWS IoT ExpressLink [Technical Specification v1.1](https://docs.aws.amazon.com/iot-expresslink/latest/programmersguide/elpg-history.html). You can find the full programmer's guide and [further documentation here](https://docs.aws.amazon.com/iot-expresslink/index.html).

## Related libraries and resources

If you are using the Arduino framework, you can use the [awslabs/aws-iot-expresslink-library-arduino](https://github.com/awslabs/aws-iot-expresslink-library-arduino) library for C++.

For additional resources, tutorials, workshops, and videos, please see the [aws/iot-expresslink](https://github.com/aws/iot-expresslink) repository.

## Installation using PlatformIO

You can install this library using PlatformIO:

`pio pkg install https://github.com/awslabs/aws-iot-expresslink-library-arduino.git`

See [this PlatformIO CLI documentation page](https://docs.platformio.org/en/latest/core/userguide/pkg/cmd_install.html#cmd-pkg-install) for more install options.

See [this PlatformIO documentation page](https://docs.platformio.org/en/latest/librarymanager/dependencies.html#installing-dependencies) for more details on dependency management in PlatformIO projects.

This library is currently not available in the PlatformIO Library Registry - please install it instead via the direct GitHub URL as shown above.

## Installation using Arduino CLI

You can install this library using the Arduino CLI:

```
arduino-cli config init
arduino-cli config set library.enable_unsafe_install true
arduino-cli lib install --git-url https://github.com/awslabs/aws-iot-expresslink-library-arduino.git
```

See the [Arduino CLI documentation](https://arduino.github.io/arduino-cli/latest/commands/arduino-cli_lib_install/) for more options.

## Installation using Arduino IDE

You can install this library using the Arduino IDE: with your sketch opened in the Arduino IDE, go the the menu bar item *Sketch*, then *Include Library*, then *Add .ZIP Library...*.

This library is currently not available in the Arduino Library Index - please install it instead via the CLI or the ZIP file.

## Usage

See the [examples folder](/examples).

See the auto-generated [API documentation](https://awslabs.github.io/aws-iot-expresslink-library-arduino/functions.html). This documentation can also be generated locally using [doxygen](https://www.doxygen.nl/).

## Security

See [CONTRIBUTING](CONTRIBUTING.md#security-issue-notifications) for more information.

## License

This library is licensed under the MIT-0 License. See the LICENSE file.
