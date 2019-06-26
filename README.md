# socklynx

A thin wrapper for calling cross platform IO APIs from managed code. This project was made specifically with games in mind, and is part of a technology suite provided by [Kitty.City Studios](https://kitty.city)

## Getting Started

This library is currently under development, in alpha phase. It works, but it's not feature frozen, and while the API is mainly solidified some small changes are yet to come.

socklynx is not licensed middleware for any gaming consoles, but is easily portable to PS4, Xbox One, and Switch. Please keep in mind that console platforms may require you to modify the library by adding a small amount of platform-specific code from their respective development kits. You will also have to generate a project using tools provided to registered developers of those platforms.

For Windows, Linux, OSX, and iOS a CMake build system is provided. Currently evaluating the most optimal solution for Android. Your platform should be auto-detected, but please open an issue if you encounter build errors.

### Prerequisites

You will need CMake for desktop and mobile platforms, and a developer kit for any console platforms. socklynx itself uses only C and OS networking APIs.

## Authors

* **Chris Burns** - [Kitty.City Studios](https://kitty.city)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

A heartfelt thank you to AWS Labs for their excellent work on aws-c-common, which is the inspiration for the cross platform support. Thanks to them, this library could probably run on an IoT enabled toaster, if such a thing exists.
