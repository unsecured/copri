// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

#define test_assert(message, test) do { if (!(test)) return message; } while (0)

#define test_evaluate(result) do { if (result != 0) { tests_failed++; printf("FAIL: %s\n", result); } else {tests_passed++; printf("PASS\n"); } } while (0)

#define test_end() do { printf("%d tests out of %d passed\n", tests_passed, tests_passed+tests_failed);	if (tests_failed > 0) {	printf("%d tests failed!!!\n", tests_failed); } return tests_failed; } while (0)

extern int tests_passed;
extern int tests_failed;
