#include "../../unity/unity.h"
#include "common.h"
#include "control.h"
#include "nfa.h"
#include "parser.h"

#define TEST_PARSE_HELPER(cc, str, nfa, outf_name) { \
	read_line((cc), (str), sizeof((str))-1); \
	(nfa) = parse((cc)); \
	TEST_ASSERT_NOT_NULL((nfa)); \
	gen_nfa_graphviz((nfa), (outf_name)); \
	destroy_nfa_and_states((nfa)); \
}

#define TEST_ERROR_HELPER(cc, str, nfa) { \
	printf("----------------------------------------" \
	       "----------------------------------------\n"); \
	read_line((cc), (str), sizeof((str))-1); \
	(nfa) = parse((cc)); \
	TEST_ASSERT_NULL((nfa)); \
}

#define SET_BIT(u64, i) (u64) |= (1ULL << (i))

void setUp(void) {}
void tearDown(void) {}

void test_parse(void)
{
	CmpCtrl *cc = init_cmpctrl();
	NFA *nfa;

	const char test_0[] = "a";
	const char test_1[] = "a|b";
	const char test_2[] = "abc";
	const char test_3[] = "abc|def";
	const char test_4[] = "a*";
	const char test_5[] = "abc+";
	const char test_6[] = "(a)";
	const char test_7[] = "(a|b)";
	const char test_8[] = "a(b|c)";
	const char test_9[] = "(a(b|c))";
	const char test_A[] = "(abc)";
	const char test_B[] = "(abc)|(def)";
	const char test_C[] = "ab(c|d)ef";
	const char test_D[] = "xy*z";
	const char test_E[] = "(abc(def))";
	const char test_F[] = "(a)*";
	const char test_10[] = "(abc)+";
	const char test_11[] = "(wow)(two)";
	const char test_12[] = "(group)(group)+";
	const char test_13[] = "(ab|cd)";
	const char test_14[] = "gray|grey";
	const char test_15[] = "gr(a|e)y";
	const char test_16[] = "t(a(c|l)|i(c|n))k";
	const char test_17[] = "w(a(t(ch)))this";
	const char test_18[] = "w(a(t(ch)))this|orthis";
	const char test_19[] = "@#$+%&!*";
	const char test_1A[] = "a\\(b\\|c\\)";
	const char test_1B[] = "\\(a\\(b\\|c\\)\\)";
	const char test_1C[] = "\\(abc\\)";
	const char test_1D[] = "\\(abc\\)\\|\\(def\\)";
	const char test_1E[] = "\\(\\)\\[\\|\\*\\?\\+\\]";
	const char test_1F[] = "(hi+)";
	const char test_20[] = "(a|(b|(c)))";

	// for test_18
	U64 check64_127 = 0;
	SET_BIT(check64_127, 'w'-64);
	SET_BIT(check64_127, 'a'-64);
	SET_BIT(check64_127, 't'-64);
	SET_BIT(check64_127, 'c'-64);
	SET_BIT(check64_127, 'h'-64);
	SET_BIT(check64_127, 'i'-64);
	SET_BIT(check64_127, 's'-64);
	SET_BIT(check64_127, 'o'-64);
	SET_BIT(check64_127, 'r'-64);

	read_file(cc, "../example.txt");
	nfa = parse(cc);
	TEST_ASSERT_NOT_NULL(nfa);
	TEST_ASSERT_TRUE(cc->pos >= cc->buffer_len);
	gen_nfa_graphviz(nfa, "dots/a_b_or_c_closure.dot");
	destroy_nfa_and_states(nfa);

	read_file(cc, "../example2.txt");
	nfa = parse(cc);
	TEST_ASSERT_NOT_NULL(nfa);
	gen_nfa_graphviz(nfa, "dots/verylong.dot");
	destroy_nfa_and_states(nfa);

	TEST_PARSE_HELPER(cc, test_0, nfa, "dots/test_0.dot");
	TEST_PARSE_HELPER(cc, test_1, nfa, "dots/test_1.dot");
	TEST_PARSE_HELPER(cc, test_2, nfa, "dots/test_2.dot");
	TEST_PARSE_HELPER(cc, test_3, nfa, "dots/test_3.dot");
	TEST_PARSE_HELPER(cc, test_4, nfa, "dots/test_4.dot");
	TEST_PARSE_HELPER(cc, test_5, nfa, "dots/test_5.dot");
	TEST_PARSE_HELPER(cc, test_6, nfa, "dots/test_6.dot");
	TEST_PARSE_HELPER(cc, test_7, nfa, "dots/test_7.dot");
	TEST_PARSE_HELPER(cc, test_8, nfa, "dots/test_8.dot");
	TEST_PARSE_HELPER(cc, test_9, nfa, "dots/test_9.dot");
	TEST_PARSE_HELPER(cc, test_A, nfa, "dots/test_A.dot");
	TEST_PARSE_HELPER(cc, test_B, nfa, "dots/test_B.dot");
	TEST_PARSE_HELPER(cc, test_C, nfa, "dots/test_C.dot");
	TEST_PARSE_HELPER(cc, test_D, nfa, "dots/test_D.dot");
	TEST_PARSE_HELPER(cc, test_E, nfa, "dots/test_E.dot");
	TEST_PARSE_HELPER(cc, test_F, nfa, "dots/test_F.dot");
	TEST_PARSE_HELPER(cc, test_10, nfa, "dots/test_10.dot");
	TEST_PARSE_HELPER(cc, test_11, nfa, "dots/test_11.dot");
	TEST_PARSE_HELPER(cc, test_12, nfa, "dots/test_12.dot");
	TEST_PARSE_HELPER(cc, test_13, nfa, "dots/test_13.dot");
	TEST_PARSE_HELPER(cc, test_14, nfa, "dots/test_14.dot");
	TEST_PARSE_HELPER(cc, test_15, nfa, "dots/test_15.dot");
	TEST_PARSE_HELPER(cc, test_16, nfa, "dots/test_16.dot");
	TEST_PARSE_HELPER(cc, test_17, nfa, "dots/test_17.dot");

	read_line(cc, test_18, sizeof(test_18)-1);
	nfa = parse(cc);
	TEST_ASSERT_NOT_NULL(nfa);
	gen_nfa_graphviz(nfa, "dots/test_18.dot");
	TEST_ASSERT_EQUAL_UINT64(nfa->alphabet64_127, check64_127);
	destroy_nfa_and_states(nfa);

	TEST_PARSE_HELPER(cc, test_19, nfa, "dots/test_19.dot");
	TEST_PARSE_HELPER(cc, test_1A, nfa, "dots/test_1A.dot");
	TEST_PARSE_HELPER(cc, test_1B, nfa, "dots/test_1B.dot");
	TEST_PARSE_HELPER(cc, test_1C, nfa, "dots/test_1C.dot");
	TEST_PARSE_HELPER(cc, test_1D, nfa, "dots/test_1D.dot");
	TEST_PARSE_HELPER(cc, test_1E, nfa, "dots/test_1E.dot");
	TEST_PARSE_HELPER(cc, test_1F, nfa, "dots/test_1F.dot");
	TEST_PARSE_HELPER(cc, test_20, nfa, "dots/test_20.dot");

	destroy_cmpctrl(cc);
}

void test_errors_and_recovery(void)
{
	CmpCtrl *cc = init_cmpctrl();

	read_file(cc, "../escapes.txt");
	NFA *nfa = parse(cc);
	TEST_ASSERT_NULL(nfa);

	const char error_0[] = "a)";
	const char error_1[] = "a+*";
	const char error_2[] = "()";
	const char error_3[] = "(";
	const char error_4[] = "(?";
	const char error_5[] = "(abc";
	const char error_6[] = "q|";
	const char error_7[] = "(ab|";
	const char error_8[] = "($$$|)";
	const char error_9[] = "(xyz|+)";
	const char error_A[] = "(abc\\)";
	const char error_B[] = "(123(987";
	const char error_C[] = "(123(onoes)";
	const char error_D[] = "(hi(bye|";
	const char error_E[] = "(a(z|)";
	const char error_F[] = "(woah|(nelly)";
	const char error_10[] = "two||wtf";
	const char error_11[] = "(ab|)";
	const char error_12[] = "(@#$|%&!|)";

	TEST_ERROR_HELPER(cc, error_0, nfa);
	TEST_ERROR_HELPER(cc, error_1, nfa);
	TEST_ERROR_HELPER(cc, error_2, nfa);
	TEST_ERROR_HELPER(cc, error_3, nfa);
	TEST_ERROR_HELPER(cc, error_4, nfa);
	TEST_ERROR_HELPER(cc, error_5, nfa);
	TEST_ERROR_HELPER(cc, error_6, nfa);
	TEST_ERROR_HELPER(cc, error_7, nfa);
	TEST_ERROR_HELPER(cc, error_8, nfa);
	TEST_ERROR_HELPER(cc, error_9, nfa);
	TEST_ERROR_HELPER(cc, error_A, nfa);
	TEST_ERROR_HELPER(cc, error_B, nfa);
	TEST_ERROR_HELPER(cc, error_C, nfa);
	TEST_ERROR_HELPER(cc, error_D, nfa);
	TEST_ERROR_HELPER(cc, error_E, nfa);
	TEST_ERROR_HELPER(cc, error_F, nfa);
	TEST_ERROR_HELPER(cc, error_10, nfa);
	TEST_ERROR_HELPER(cc, error_11, nfa);
	TEST_ERROR_HELPER(cc, error_12, nfa);

	destroy_cmpctrl(cc);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_parse);
	RUN_TEST(test_errors_and_recovery);

	return UNITY_END();
}