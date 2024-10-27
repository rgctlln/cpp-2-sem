import tester
import sys
import time

if len(sys.argv) != 2:
	print("Usage: python tests.py <program name>.")
	exit(tester.EXIT_FAILURE)

program_name = sys.argv[1]

def tests_easy():
	tr = tester.IOTester("easy", program_name)
	tr.add_easy(["6", "10", "-1"],
"""+----+---------+
| n  | n!      |
+----+---------+
| 6  | 720     |
| 7  | 5040    |
| 8  | 40320   |
| 9  | 362880  |
| 10 | 3628800 |
+----+---------+
""")
	tr.add_easy(["6", "10", "0"],
"""+----+---------+
|  n |    n!   |
+----+---------+
|  6 |   720   |
|  7 |   5040  |
|  8 |  40320  |
|  9 |  362880 |
| 10 | 3628800 |
+----+---------+
""")
	tr.add_easy(["6", "10", "1"],
"""+----+---------+
|  n |      n! |
+----+---------+
|  6 |     720 |
|  7 |    5040 |
|  8 |   40320 |
|  9 |  362880 |
| 10 | 3628800 |
+----+---------+
""")
	tr.add_fail(["-1", "2", "0"])
	tr.add_fail(["1", "-2", "0"])
	tr.add_fail(["-1", "-2", "0"])
	return tr

def tests_hard():
	tr = tester.IOTester("hard", program_name)
	tr.add_hard(["1", "11", "0"], "ef979820396b982271fd21e0edf6a37278f58f1e067bafaa36f4bd54997196f0")
	tr.add_hard(["1", "12", "-1"], "33bc3d7b96f4918126c9e599d2a9fe9cf6fcc111eccdee30ff366eeb477b4fbd")
	tr.add_hard(["1", "12", "1"], "749567c348747710173d3d9b4742d3da3a2379147db65fff81758ea5b1149cd8")
	tr.add_hard(["1", "12", "0"], "d3c1ff8294fa5f1b23cf2eaed2c6b72ed66218f2ec83af75e2329b3d6a9ab115")
	tr.add_hard(["1", "1", "-1"], "807ba42f034d3abaada3c9ed8d3ba793dbe19be4a45e87d04c32ce168c18655a")
	tr.add_hard(["1", "1", "0"], "dbabe0a765eb5cdd35c2041aef39bcc2a7ebf87dd32be23b575f75c4e97b7bbf")
	tr.add_hard(["1", "1", "1"], "dbabe0a765eb5cdd35c2041aef39bcc2a7ebf87dd32be23b575f75c4e97b7bbf")
	tr.add_hard(["1", "2", "-1"], "cd2f6ea9760acabf05d301d5d0ac7cff2731e25c46d4d7ca68cd5efe2d2cd7de")
	tr.add_hard(["1", "2", "0"], "9c47cbd99a5bb8eda330a55bbe2d48d2fe7285ea528b030f0adf7edbedc13e1a")
	tr.add_hard(["1", "2", "1"], "9c47cbd99a5bb8eda330a55bbe2d48d2fe7285ea528b030f0adf7edbedc13e1a")
	tr.add_hard(["0", "0", "0"], "8f7e2141599e5444121402ea3b26b66dc76258b7ad01a5bc20d5e6814809ddf2")
	tr.add_hard(["6", "30", "-1"], "5a1902f946f023acd50151b8bca00637befb173f7036c1673f601563f6bf296a")
	tr.add_hard(["6", "30", "1"], "3a7172bd9b8acc6d40e781c873437606cbca9b44b5ea9480abb96819bf2d20b1")
	tr.add_hard(["65530", "30", "0"], "2cc43d58b393d326e4d861f067c1c3c1d8d173f6483d3cf571a453916bf110f7")
	tr.add_hard(["65530", "30", "-1"], "92f1ad6b3fe7c9def9975ef6de0da337c163f2d66a1174bbdd527d8be4f7615a")
	tr.add_hard(["65530", "1", "-1"], "ccf95b341610bd2ea19b2317c4e96aac88b99034f5b745c52faa2cfde9547281")
	tr.add_hard(["65535", "0", "-1"], "34f651fb6211568de940268fa2523c8abd40088c2dc1ddd7f82102132f08050b")
	tr.add_hard(["65535", "65534", "0"], "7f265a6ed0f21386a6b2fe5d305500c1a9868887e0e2d3bb227d799cc44b7bd5", timeout = 180)
	return tr

testers = [
	tests_easy(),
	tests_hard()
]

print("Homework 1 - Factorial")
start = time.time_ns() // 1000000
passed = 0
for tr in testers:
	passed += tr.run()
end = time.time_ns() // 1000000
total = sum([tr.total() for tr in testers])
print("=" * 30)
print("%d/%d tests passed in %d ms" % (passed, total, end - start))

if total == passed: exit(tester.EXIT_SUCCESS)
else: exit(tester.EXIT_FAILURE)
