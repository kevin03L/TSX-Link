# Contributing to TSX-Link

First off, thank you for considering contributing to TSX-Link! It's people like you that make TSX-Link such a great tool.

## Code of Conduct

This project and everyone participating in it is governed by our [Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code.

## How Can I Contribute?

### Reporting Bugs

Before creating bug reports, please check the issue list as you might find out that you don't need to create one. When you are creating a bug report, please include as many details as possible:

* **Use a clear and descriptive title**
* **Describe the exact steps which reproduce the problem**
* **Provide specific examples to demonstrate the steps**
* **Describe the behavior you observed after following the steps**
* **Explain which behavior you expected to see instead and why**
* **Include screenshots and animated GIFs if possible**
* **Include your environment details** (OS, ESP-IDF version, PlatformIO version, etc.)

### Suggesting Enhancements

Enhancement suggestions are tracked as GitHub issues. When creating an enhancement suggestion, please include:

* **Use a clear and descriptive title**
* **Provide a step-by-step description of the suggested enhancement**
* **Provide specific examples to demonstrate the steps**
* **Describe the current behavior and expected behavior**
* **Explain why this enhancement would be useful**

### Pull Requests

* Fill in the required template
* Follow the C++ style guide (see below)
* Include appropriate test cases
* Update documentation as needed
* End all files with a newline
* Avoid platform-dependent code

## C++ Style Guide

### Naming Conventions

* **Classes and structs**: PascalCase (e.g., `UnitTelwayDriver`)
* **Functions and methods**: camelCase (e.g., `readMemory()`)
* **Constants**: UPPER_SNAKE_CASE (e.g., `MAX_BUFFER_SIZE`)
* **Member variables**: snake_case with underscore prefix (e.g., `_buffer_size`)
* **Local variables**: snake_case (e.g., `temp_value`)

### Code Style

* Use 4 spaces for indentation (not tabs)
* Maximum line length: 100 characters
* Opening braces on the same line (K&R style)
* Always use braces for if/while/for blocks
* Use `const` and `constexpr` where appropriate
* Use nullptr instead of NULL
* Use auto sparingly and only where type is obvious

### Documentation

* Document all public functions with Doxygen comments
* Include parameter descriptions and return value documentation
* Include usage examples for complex functions
* Update README.md if adding new features

### Commits

* Use clear and descriptive commit messages
* Use the present tense ("Add feature" not "Added feature")
* Use the imperative mood ("Move cursor to..." not "Moves cursor to...")
* Limit the first line to 72 characters or less
* Reference issues and pull requests liberally after the first line

## Additional Notes

### Issue and Pull Request Labels

* `bug` - Something isn't working
* `enhancement` - New feature or request
* `documentation` - Improvements or additions to documentation
* `hardware` - Related to PCB design
* `firmware` - Related to embedded code
* `web-ui` - Related to web interface
* `help-wanted` - Extra attention is needed
* `good first issue` - Good for newcomers

## Project Setup

1. Fork the repository
2. Clone your fork: `git clone https://github.com/your-username/TSX-Link.git`
3. Add upstream remote: `git remote add upstream https://github.com/kevin03L/TSX-Link.git`
4. Create a feature branch: `git checkout -b feature/your-feature-name`
5. Make your changes
6. Push to your fork: `git push origin feature/your-feature-name`
7. Create a Pull Request

## Questions?

Feel free to open an issue with the label `question` or reach out to the maintainers.

Thank you for contributing! 🚀
