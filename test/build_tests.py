#!/usr/bin/env python3
"""
build_tests.py - Compile and run tests for AVR MCU drivers
"""

import subprocess
import sys
import os
import glob

def run_command(cmd, description):
    """Run a shell command and return success status"""
    print(f"\n{'='*60}")
    print(f"{description}")
    print(f"{'='*60}")
    print(f"Running: {' '.join(cmd)}\n")
    
    try:
        result = subprocess.run(cmd, check=True)
        print(f"✓ {description} - SUCCESS\n")
        return True
    except subprocess.CalledProcessError as e:
        print(f"✗ {description} - FAILED")
        print(f"Error: {e}\n")
        return False
    except Exception as e:
        print(f"✗ Error running command: {e}\n")
        return False

def main():
    """Main build script"""
    print("\n" + "="*60)
    print("AVR MCU Test Build System")
    print("="*60)
    
    # Get the test directory
    test_dir = os.path.dirname(os.path.abspath(__file__))
    os.chdir(test_dir)
    
    print(f"Working directory: {test_dir}\n")
    
    # Compiler and flags
    cc = "gcc"
    cflags = [
        "-I../lib/ses",
        "-I./support", 
        "-I./unity",
        "-Wall",
        "-Wextra",
        "-g",
        "-std=c99"
    ]
    ldflags = ["--coverage"]
    
    # Source files
    sources = [
        "unit/test_ses_led.c",
        "../lib/ses/ses_led.c",
        "support/mock_avr_io.c",
        "unity/unity.c"
    ]
    
    output = "test_led"
    if sys.platform == "win32":
        output += ".exe"
    
    # Build command
    build_cmd = [cc] + cflags + ["-o", output] + sources + ldflags
    
    # Compile
    if not run_command(build_cmd, "Compiling tests"):
        sys.exit(1)
    
    # Run tests
    run_cmd = [f"./{output}"] if sys.platform != "win32" else [output]
    if not run_command(run_cmd, "Running tests"):
        sys.exit(1)
    
    print("\n" + "="*60)
    print("✓ ALL STEPS COMPLETED SUCCESSFULLY")
    print("="*60 + "\n")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
