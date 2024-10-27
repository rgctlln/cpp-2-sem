import sys
import time
import os

from suite.tester import *
from suite.config import *

# Usage.
if len(sys.argv) < 2:
	print("Usage: python tests.py <program name> [<value>]*.")
	exit(EXIT_FAILURE)

# Name of the program being tested.
program_name = sys.argv[1]

if not os.path.exists(program_name):
	print("File %s not found." % (program_name))
	exit(EXIT_FAILURE)

# Tests.

def test_single_print():
	tester = CmdTester("Float (with single precision) printing number.", program_name)
	tester.add_easy(input = ["f", "0", "0xB9CD542"], expected = "0x1.39aa84p-104", name = "float (single): print (toward zero)",         categories = ["0", "single_print"])
	tester.add_easy(input = ["f", "1", "0x3"      ], expected = "0x1.800000p-148", name = "float (single): print (toward nearest even)", categories = ["1", "single_print"])
	return tester

def test_single_mul():
	tester = CmdTester("Float (with single precision) multiplying numbers.", program_name)
	tester.add_easy(input = ["f", "0", "0x414587dd", "*", "0x42ebf110"], expected = "0x1.6c1b72p+10", name = "float (single): multiply (toward zero)",         categories = ["0", "single_multiply"])
	tester.add_easy(input = ["f", "1", "0x414587dd", "*", "0x42ebf110"], expected = "0x1.6c1b72p+10", name = "float (single): multiply (toward nearest even)", categories = ["1", "single_multiply"])
	tester.add_easy(input = ["f", "2", "0x414587dd", "*", "0x42ebf110"], expected = "0x1.6c1b74p+10", name = "float (single): multiply (toward pos infinity)", categories = ["2", "single_multiply"])
	tester.add_easy(input = ["f", "3", "0x414587dd", "*", "0x42ebf110"], expected = "0x1.6c1b72p+10", name = "float (single): multiply (toward neg infinity)", categories = ["3", "single_multiply"])
	return tester

def test_single_sum():
	tester = CmdTester("Float (with single precision) sum of numbers.", program_name)
	tester.add_easy(input = ["f", "0", "0x414587dd", "+", "0x42ebf110"], expected = "0x1.04a20ap+7", name = "float (single): sum (toward zero)",         categories = ["0", "single_sum"])
	tester.add_easy(input = ["f", "1", "0x414587dd", "+", "0x42ebf110"], expected = "0x1.04a20cp+7", name = "float (single): sum (toward nearest even)", categories = ["1", "single_sum"])
	tester.add_easy(input = ["f", "2", "0x414587dd", "+", "0x42ebf110"], expected = "0x1.04a20cp+7", name = "float (single): sum (toward pos infinity)", categories = ["2", "single_sum"])
	tester.add_easy(input = ["f", "3", "0x414587dd", "+", "0x42ebf110"], expected = "0x1.04a20ap+7", name = "float (single): sum (toward neg infinity)", categories = ["3", "single_sum"])
	return tester

def test_half_mul():
	tester = CmdTester("Float (with half precision) multiplying numbers.", program_name)
	tester.add_easy(input = ["h", "0", "0x4145", "*", "0x42eb"], expected = "0x1.238p+3", name = "float (half): multiply (toward zero)",         categories = ["0", "half_multiply"])
	tester.add_easy(input = ["h", "1", "0x4145", "*", "0x42eb"], expected = "0x1.23cp+3", name = "float (half): multiply (toward nearest even)", categories = ["1", "half_multiply"])
	tester.add_easy(input = ["h", "2", "0x4145", "*", "0x42eb"], expected = "0x1.23cp+3", name = "float (half): multiply (toward pos infinity",  categories = ["2", "half_multiply"])
	tester.add_easy(input = ["h", "3", "0x4145", "*", "0x42eb"], expected = "0x1.238p+3", name = "float (half): multiply (toward neg infinity)", categories = ["3", "half_multiply"])
	return tester

def test_special_cases():
	tester = CmdTester("Float (half and single precisions) special cases.", program_name)
	tester.add_easy(input = ["h", "0", "0x8000", "+", "0x0"            ], expected = "0x0.000p+0",    name = "float (half): -0 + 0 (toward zero)",         categories = ["0", "special"])
	tester.add_easy(input = ["h", "0", "0x0"                           ], expected = "0x0.000p+0",    name = "float (half): 0 (toward zero)",              categories = ["0", "special"])
	tester.add_easy(input = ["f", "0", "0x0"                           ], expected = "0x0.000000p+0", name = "float (single): 0 (toward zero)",            categories = ["0", "special"])
	tester.add_easy(input = ["f", "0", "0x7f800000"                    ], expected = "inf",           name = "float (single): infinity (toward zero)",     categories = ["0", "special"])
	tester.add_easy(input = ["f", "0", "0xff800000"                    ], expected = "-inf",          name = "float (single): -infinity (toward zero)",    categories = ["0", "special"])
	tester.add_easy(input = ["f", "0", "0x7fc00000"                    ], expected = "nan",           name = "float (single): not-a-number (toward zero)", categories = ["0", "special"])
	tester.add_easy(input = ["f", "0", "0x1", "/", "0x0"               ], expected = "inf",           name = "float (single): 1 / 0 (toward zero)",        categories = ["0", "special"])
	tester.add_easy(input = ["f", "0", "0xff800000", "/", "0x7f800000" ], expected = "nan",           name = "float (single): not-a-number (toward zero)", categories = ["0", "special"])
	return tester

ERROR_ARGUMENTS_INVALID = 4

def test_failing_cases():
	tester = CmdTester("Some failing cases", program_name)
	tester.add_fail([                            ], expected_exitcode = ERROR_ARGUMENTS_INVALID, name = "no arguments")
	tester.add_fail(["x", "0",  "0x0"            ], expected_exitcode = ERROR_ARGUMENTS_INVALID, name = "incorrect format")
	tester.add_fail(["f", "13", "0x0"            ], expected_exitcode = ERROR_ARGUMENTS_INVALID, name = "incorrect rounding")
	tester.add_fail(["f", "0",  "0x0", "_", "0x0"], expected_exitcode = ERROR_ARGUMENTS_INVALID, name = "invalid operation")
	return tester

# Flags.

## all testers
all_testers = [
    test_single_print(),
    test_single_mul(),
    test_single_sum(),
    test_half_mul(),
    test_special_cases(),
    test_failing_cases()
]
## selected testers
testers = []
## selected tests
selected = []
if len(sys.argv) >= 3:
	### extract testers
	selected = sys.argv[2:]
	for tester in all_testers:
		new_tester = tester.extract_only(selected)
		if len(new_tester.tests) != 0:
			testers.append(new_tester)
else:
	testers = all_testers

# Task name.
print("L1 - Floating")

# Global all-in-one tester.
mega = MegaTester()
[mega.add_tester(tester) for tester in testers]

# Start.
start = time.time_ns() // 1000000
result = mega.run()
end = time.time_ns() // 1000000

# Output results.
print("=" * 30)
result.print_table()
print("=" * 30)
result.print_groups()
print("=" * 30)
print("%d/%d tests passed in %d ms" % (result.passed, result.total, end - start))

# Exit.
if result.is_passed(): exit(EXIT_SUCCESS)
else: exit(EXIT_FAILURE)
