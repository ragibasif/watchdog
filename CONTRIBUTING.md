# Contributing to Watchdog

Thank you for your interest in contributing to Watchdog! This document provides guidelines and instructions for contributing to the project.

## Code of Conduct

Please be respectful and constructive in all interactions with other contributors and maintainers.

## Getting Started

### Prerequisites

- POSIX-compliant system (Linux or macOS)
- C compiler (gcc, clang, etc.)
- Git
- Basic familiarity with C memory management

### Setting Up Your Development Environment

1. Fork the repository on GitHub
2. Clone your fork locally:
   ```bash
   git clone https://github.com/YOUR_USERNAME/watchdog.git
   cd watchdog
   ```
3. Create a new branch for your changes:
   ```bash
   git checkout -b feature/your-feature-name
   ```

## How to Contribute

### Reporting Bugs

If you find a bug, please open an issue on GitHub with the following information:

- A clear, descriptive title
- A detailed description of the issue
- Steps to reproduce the bug
- Expected behavior vs. actual behavior
- Your system information (OS, compiler, version)
- Any relevant code snippets or error messages

### Suggesting Enhancements

Enhancement suggestions are welcome! Please open an issue with:

- A clear title describing the enhancement
- A detailed description of the proposed improvement
- Use cases or examples that demonstrate the value
- Any alternatives you've considered

### Submitting Code Changes

#### Before You Start

- Check existing issues and pull requests to avoid duplicate work
- Discuss significant changes by opening an issue first
- Ensure your changes align with the project's scope and goals

#### Making Changes

1. Write clean, well-commented C code following the existing style
2. Keep functions focused and modular
3. Add comments for non-obvious logic
4. Test your changes thoroughly

#### Adding Examples

If your contribution adds new functionality, please include an example in the `examples/` folder:

- Follow the naming convention: `your_feature_example.c`
- Include a clear description as a comment at the top
- Demonstrate both correct and potentially problematic usage patterns
- Test your example with Watchdog enabled

#### Testing

- Test your changes with `WATCHDOG_ENABLE` defined
- Test with different initialization options (verbose logging on/off, color output on/off, file logging on/off)
- Test edge cases and error conditions
- Verify thread safety if applicable
- Test on multiple systems if possible (Linux, macOS)

#### Documentation

- Update relevant comments in the code
- If you modify public APIs, update the README with usage examples
- Add examples demonstrating new features in the `examples/` folder
- Include comments explaining the purpose and behavior of new functions

### Submitting a Pull Request

1. Ensure your code follows the existing style and conventions
2. Update the README if necessary
3. Add an example if introducing new functionality
4. Write a clear commit message:

   ```
   [Type] Brief description

   Detailed explanation of changes if needed
   ```

   Examples: `[Feature] Add Windows support`, `[Fix] Correct buffer overflow detection`

5. Push to your fork and open a pull request
6. Provide a clear description of what your PR addresses
7. Reference any related issues using `#issue_number`

## Code Style Guidelines

- Use descriptive variable and function names
- Keep lines reasonably readable (aim for < 100 characters where practical)
- Prefix internal functions with `w_` (following Watchdog convention)
- Add comments for complex logic
- Use consistent indentation (matching existing code)
- Free allocated memory and set pointers to NULL when appropriate

## Areas for Contribution

### High Priority

- **Windows Support**: Extend platform compatibility beyond POSIX systems
- **Performance Optimization**: Reduce overhead of memory tracking
- **Thread Safety Testing**: Comprehensive tests for multithreaded scenarios

### Medium Priority

- **Additional Examples**: More comprehensive usage examples
- **Documentation**: Expanded API documentation and tutorials
- **Testing Framework**: Automated test suite

### Lower Priority

- **Feature Requests**: See open issues for ideas
- **Bug Fixes**: Check the issues tab for known problems

## Testing and Quality Assurance

- Test with both `verbose_log` enabled and disabled
- Verify `log_to_file` output is correct
- Test with color output both enabled and disabled
- Ensure memory leak detection works across various allocation patterns
- Test buffer overflow and double-free detection scenarios
- Verify thread safety in multithreaded applications

## Commit Message Guidelines

Write clear, descriptive commit messages:

- Use the imperative mood ("Add feature" not "Added feature")
- Keep the first line concise (< 50 characters)
- Reference issues when relevant (`Fixes #123`)
- Separate the subject from body with a blank line

## Questions?

- Open an issue for questions or discussion
- Check existing issues for similar questions
- Contact the maintainer at <ragibasif@tuta.io>

## License

By contributing to Watchdog, you agree that your contributions will be licensed under the MIT License, the same license as the project.

Thank you for helping make Watchdog better!
