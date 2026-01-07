import subprocess

tests = [
    ("leak", "[LEAK]"),
    ("double_free", "[ERROR] Double free error."),
    ("overflow", "[ERROR] Out of bounds access."),
]


def run_test(mode, expected_msg):
    print(f"Running test: {mode}...", end=" ")
    # Compile and run the target
    process = subprocess.Popen(
        ["./target", mode], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True
    )
    stdout, stderr = process.communicate()

    # Check if the expected watchdog message is in stdout or stderr
    if expected_msg in stdout or expected_msg in stderr:
        print("✅ PASSED")
        return True
    else:
        print("❌ FAILED")
        return False


if __name__ == "__main__":
    # Ensure target is compiled
    subprocess.run(["gcc", "tests/target.c", "watchdog.c", "-o", "target", "-lpthread"])

    results = [run_test(m, msg) for m, msg in tests]
    if all(results):
        exit(0)
    exit(1)
