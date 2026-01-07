import subprocess
import sys


def run_tests():
    print("🚀 Starting Watchdog Tests...")

    # 1. Run the compiled binary
    process = subprocess.Popen(
        ["./test"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )
    stdout, stderr = process.communicate()
    combined_output = stdout + stderr

    # 2. Define what we EXPECT to see based on your example functions
    expected_markers = {
        "Leak Detection": "[LEAK]",
        "Double Free Detection": "Double free error.",
        "Overflow Detection": "Out of bounds access.",
        "Invalid Free Detection": "Attempt to free unallocated/untracked memory.",
    }

    passed = True
    print("-" * 40)

    for feature, marker in expected_markers.items():
        if marker in combined_output:
            print(f"✅ {feature:25} : FOUND")
        else:
            print(f"❌ {feature:25} : NOT FOUND")
            passed = False

    print("-" * 40)
    return passed


if __name__ == "__main__":
    if run_tests():
        print("🎉 ALL TESTS PASSED")
        sys.exit(0)
    else:
        print("🚨 TESTS FAILED")
        sys.exit(1)
