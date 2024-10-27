import subprocess
import hashlib
import os
import time
import difflib

# Constants.

EXIT_SUCCESS = 0
EXIT_FAILURE = -1
DEFAULT_TIMEOUT = 1

if os.name == "nt":
	SYS_EXEC = ".\\"
else:
	SYS_EXEC = "./"

# Classes.

## Results.

class ExpectedResult:
	def __init__(self, stdout = None, stderr = None, is_success = True, is_sha256 = False):
		self.stdout = stdout
		self.stderr = stderr
		self.is_success = is_success
		self.is_sha256 = is_sha256

	def compare(self, other):
		expected_stdout = self.stdout
		expected_success = self.is_success
		expected_sha256 = self.is_sha256
		actual_stdout = other.stdout
		actual_success = other.is_success
		actual_stderr = other.stderr

		if not expected_success and actual_success:
			raise ValueError("Program returns ERROR_CODE = 0.")

		if expected_success and not actual_success:
			raise ValueError("Program should not fail, returns ERROR_CODE != 0.")

		if not expected_success:
			if actual_stderr == None or actual_stderr == "":
				print("====> WARNING: Standard error output is empty.")
			if actual_stdout != None and actual_stdout != "":
				raise ValueError("On error program should not writing anything to standard output.")
			return

		if expected_sha256:
			actual_stdout = hashlib.sha256(actual_stdout.encode("utf-8")).hexdigest()
			if actual_stdout != expected_stdout:
				raise ValueError("SHA-256 from expected output (%s) not equals to actual (%s)." % (expected_stdout, actual_stdout))
			else:
				return

		if actual_stdout != expected_stdout:
			diff = difflib.unified_diff(expected_stdout.splitlines(keepends=True),
							actual_stdout.splitlines(keepends=True),
							fromfile = 'expected_stdout',
							tofile = 'actual_stdout')
			ndiff = '\n'.join(diff)
			error_str = "Expected:\n\"\"\"\n%s\"\"\"\nbut actual is:\n\"\"\"\n%s\"\"\"\nDifference (https://docs.python.org/3/library/difflib.html#difflib.unified_diff):\n%s\n(max 40 lines)\n" % ('\n'.join(expected_stdout.splitlines()[0:40]), '\n'.join(actual_stdout.splitlines()[0:40]), '\n'.join(ndiff.splitlines()[0:40]))
			raise ValueError(error_str)

class TestResult(ExpectedResult):
	def __init__(self, stdout, stderr, return_code):
		ExpectedResult.__init__(self, stdout = stdout, stderr = stderr, is_success = (return_code == EXIT_SUCCESS))

## Tests.

class IOTest:
	def __init__(self, input, expected_result, timeout = DEFAULT_TIMEOUT):
		self.input = (' '.join(input) + "\n").encode("ascii")
		self.expected_result = expected_result
		self.timeout = timeout

	def run(self, filename):
		program = subprocess.Popen([SYS_EXEC + filename], stdout = subprocess.PIPE, stdin = subprocess.PIPE, stderr = subprocess.PIPE, universal_newlines = True)
		try:
			output, error = program.communicate(self.input.decode('utf-8'), timeout = self.timeout)
			result = TestResult(output, error, program.returncode)
			self.expected_result.compare(result)
		except ValueError as err:
			raise err
		except subprocess.TimeoutExpired:
			raise ValueError("Timeout [%d s]" % (self.timeout))

## Testers.

class IOTester:
	def __init__(self, groupname, filename):
		self.groupname = groupname
		self.filename = filename
		self.tests = []

	def run(self):
		passed = 0
		print("=> Test suite: %s tests." % self.groupname)
		for i, test in enumerate(self.tests):
			try:
				print("==> Running test %d" % (i + 1))
				start = time.time_ns() // 1000000
				test.run(self.filename)
				end = time.time_ns() // 1000000
				print("===> SUCCESS in %d ms" % (end - start))
				passed += 1
			except ValueError as err:
				end = time.time_ns() // 1000000
				print("===> FAILED in %d ms" % (end - start))
				print("====> ERROR:", err)
			except Exception as err:
				print("===> FAILED WITH UNKNOWN ERROR")
				print(err)
		return passed

	def add_fail(self, input, timeout = DEFAULT_TIMEOUT):
		self.add_easy(input, None, timeout)

	def add_easy(self, input, expected, timeout = DEFAULT_TIMEOUT):
		expected = ExpectedResult(expected, is_success = expected != None)
		self.tests.append(IOTest(input, expected, timeout))

	def add_hard(self, input, expected, timeout = DEFAULT_TIMEOUT):
		expected = ExpectedResult(expected, is_sha256 = True)
		self.tests.append(IOTest(input, expected, timeout))

	def total(self):
		return len(self.tests)
