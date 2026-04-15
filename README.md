# AVR Programming
This repository contains a comprehensive collection of firmware developed for ATmega32U4 microcontroller, with integrated automated testing infrastructure and continuous integration/continuous delivery (CI/CD) pipeline.

## Project Structure and Navigation
The repository is organized into six core modules. Each module folder contains the problem statements and multiple sub-tasks.

### How to Navigate
* Requirement Specifications: Look for the .pdf file inside each module folder (e.g., Peripherals.pdf). This contains the problem statements and technical constraints for that specific unit.
* Task Implementation: Each problem is solved in a dedicated task folder (e.g., task_2-4, task_2-5).
* Source Code: The firmware implementation is located within the src folder of each task. Path Example: 2_Peripherals/task_2-4/src/main.c
* Shared hardware definitions used across all modules are maintained in the lib/ses directory.

---

## Automated Test Infrastructure

This repository includes a comprehensive automated testing system for embedded C drivers with CI/CD integration.

### Testing Framework
- **Framework:** Unity Test Framework (embedded C unit testing)
- **Coverage Tool:** LCOV/gcov (code coverage analysis and reporting)
- **Test Location:** `test/` directory
  - `test/unit/` - Unit test cases
  - `test/unity/` - Unity framework source
  - `test/support/` - Mock hardware and test utilities
  - `test/support/avr/` - Mock AVR headers for cross-platform testing

### Test Files
- **test_ses_led.c** - Comprehensive tests for LED driver

### CI/CD Pipeline
**GitHub Actions Workflow:** `.github/workflows/tests.yml`

Automatically triggered on commits to `lib/ses/` or `test/`:
1. Compiles tests on Linux environment
2. Executes all unit tests
3. Analyzes code coverage
4. Generates HTML coverage reports
5. Publishes results to GitHub Actions artifacts
6. Posts results to Pull Request comments

### Viewing Coverage Reports
1. Go to GitHub → Actions
2. Select latest workflow run
3. Download `coverage-report-html` artifact
4. Extract and open `index.html` in browser

### Adding New Driver Tests
To add tests for a new driver (e.g., button driver):
1. Create `test/unit/test_ses_button.c`
2. Use the test framework provided in the test directory
3. Update workflow compilation step if needed
4. Push to trigger automatic testing

### Technologies Used
- **C** - Driver code and unit tests
- **GitHub Actions** - CI/CD orchestration
- **LCOV/gcov** - Code coverage analysis
- **Unity Framework** - Unit testing
- **Mock Hardware** - Cross-platform testing
- **Python** - Automation scripts (scripts/generate_tests_copilot.py)
