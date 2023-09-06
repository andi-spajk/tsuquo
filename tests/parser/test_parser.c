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
	const char test_10[] = "(abc)";
	const char test_11[] = "(abc)|(def)";
	const char test_12[] = "ab(c|d)ef";
	const char test_13[] = "xy*z";
	const char test_14[] = "(abc(def))";
	const char test_15[] = "(a)*";
	const char test_16[] = "(abc)+";
	const char test_17[] = "(wow)(two)";
	const char test_18[] = "(group)(group)+";
	const char test_19[] = "(ab|cd)";
	const char test_20[] = "gray|grey";
	const char test_21[] = "gr(a|e)y";
	const char test_22[] = "t(a(c|l)|i(c|n))k";
	const char test_23[] = "w(a(t(ch)))this";
	const char test_24[] = "w(a(t(ch)))this|orthis";
	const char test_25[] = "@#$+%&!*";
	const char test_26[] = "a\\(b\\|c\\)";
	const char test_27[] = "\\(a\\(b\\|c\\)\\)";
	const char test_28[] = "\\(abc\\)";
	const char test_29[] = "\\(abc\\)\\|\\(def\\)";
	const char test_30[] = "\\(\\)\\[\\|\\*\\?\\+\\]";
	const char test_31[] = "(hi+)";
	const char test_32[] = "(a|(b|(c)))";
	const char test_33[] = "[a]";
	const char test_34[] = "[ab]";
	const char test_35[] = "[a-b]";
	const char test_36[] = "[a-c]";
	const char test_37[] = "[a-f]";
	const char test_38[] = "[a-z]";
	const char test_39[] = "[-abc\t]";
	const char test_40[] = "[\\[\\]\\(\\)]";
	const char test_41[] = "[\\[-`]";
	const char test_42[] = "[x-z]*";
	const char test_43[] = "test_([0-9]|[A-F])+.dot";
	const char test_44[] = "[A-Fa-f0-9]?";
	const char test_45[] = "[ab-e]lincoln";
	const char test_46[] = "[2-901]";
	const char test_47[] = "[X-\\[]";
	const char test_48[] = "(0|(1(01*(00)*0)*1)*)*";

	// for test_24
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
	TEST_PARSE_HELPER(cc, test_10, nfa, "dots/test_10.dot");
	TEST_PARSE_HELPER(cc, test_11, nfa, "dots/test_11.dot");
	TEST_PARSE_HELPER(cc, test_12, nfa, "dots/test_12.dot");
	TEST_PARSE_HELPER(cc, test_13, nfa, "dots/test_13.dot");
	TEST_PARSE_HELPER(cc, test_14, nfa, "dots/test_14.dot");
	TEST_PARSE_HELPER(cc, test_15, nfa, "dots/test_15.dot");
	TEST_PARSE_HELPER(cc, test_16, nfa, "dots/test_16.dot");
	TEST_PARSE_HELPER(cc, test_17, nfa, "dots/test_17.dot");
	TEST_PARSE_HELPER(cc, test_18, nfa, "dots/test_18.dot");
	TEST_PARSE_HELPER(cc, test_19, nfa, "dots/test_19.dot");
	TEST_PARSE_HELPER(cc, test_20, nfa, "dots/test_20.dot");
	TEST_PARSE_HELPER(cc, test_21, nfa, "dots/test_21.dot");
	TEST_PARSE_HELPER(cc, test_22, nfa, "dots/test_22.dot");
	TEST_PARSE_HELPER(cc, test_23, nfa, "dots/test_23.dot");

	read_line(cc, test_24, sizeof(test_24)-1);
	nfa = parse(cc);
	TEST_ASSERT_NOT_NULL(nfa);
	gen_nfa_graphviz(nfa, "dots/test_24.dot");
	TEST_ASSERT_EQUAL_UINT64(nfa->alphabet64_127, check64_127);
	destroy_nfa_and_states(nfa);

	TEST_PARSE_HELPER(cc, test_25, nfa, "dots/test_25.dot");
	TEST_PARSE_HELPER(cc, test_26, nfa, "dots/test_26.dot");
	TEST_PARSE_HELPER(cc, test_27, nfa, "dots/test_27.dot");
	TEST_PARSE_HELPER(cc, test_28, nfa, "dots/test_28.dot");
	TEST_PARSE_HELPER(cc, test_29, nfa, "dots/test_29.dot");
	TEST_PARSE_HELPER(cc, test_30, nfa, "dots/test_30.dot");
	TEST_PARSE_HELPER(cc, test_31, nfa, "dots/test_31.dot");
	TEST_PARSE_HELPER(cc, test_32, nfa, "dots/test_32.dot");
	TEST_PARSE_HELPER(cc, test_33, nfa, "dots/test_33.dot");
	TEST_PARSE_HELPER(cc, test_34, nfa, "dots/test_34.dot");
	TEST_PARSE_HELPER(cc, test_35, nfa, "dots/test_35.dot");
	TEST_PARSE_HELPER(cc, test_36, nfa, "dots/test_36.dot");
	TEST_PARSE_HELPER(cc, test_37, nfa, "dots/test_37.dot");
	TEST_PARSE_HELPER(cc, test_38, nfa, "dots/test_38.dot");
	TEST_PARSE_HELPER(cc, test_39, nfa, "dots/test_39.dot");
	TEST_PARSE_HELPER(cc, test_40, nfa, "dots/test_40.dot");
	TEST_PARSE_HELPER(cc, test_41, nfa, "dots/test_41.dot");
	TEST_PARSE_HELPER(cc, test_42, nfa, "dots/test_42.dot");
	TEST_PARSE_HELPER(cc, test_43, nfa, "dots/test_43.dot");
	TEST_PARSE_HELPER(cc, test_44, nfa, "dots/test_44.dot");
	TEST_PARSE_HELPER(cc, test_45, nfa, "dots/test_45.dot");
	TEST_PARSE_HELPER(cc, test_46, nfa, "dots/test_46.dot");
	TEST_PARSE_HELPER(cc, test_47, nfa, "dots/test_47.dot");
	TEST_PARSE_HELPER(cc, test_48, nfa, "dots/test_48.dot");

	destroy_cmpctrl(cc);
}

#define TEST_ERROR_HELPER(cc, str, nfa) { \
	printf("----------------------------------------" \
	       "----------------------------------------\n"); \
	read_line((cc), (str), sizeof((str))-1); \
	(nfa) = parse((cc)); \
	TEST_ASSERT_NULL((nfa)); \
}

void test_errors_and_recovery(void)
{
	CmpCtrl *cc = init_cmpctrl();

	read_file(cc, "../escapes.txt");
	NFA *nfa = parse(cc);
	TEST_ASSERT_NULL(nfa);

	TEST_ERROR_HELPER(cc, "a)",             nfa);
	TEST_ERROR_HELPER(cc, "a+*",            nfa);
	TEST_ERROR_HELPER(cc, "()",             nfa);
	TEST_ERROR_HELPER(cc, "(",              nfa);
	TEST_ERROR_HELPER(cc, "(?",             nfa);
	TEST_ERROR_HELPER(cc, "(abc",           nfa);
	TEST_ERROR_HELPER(cc, "q|",             nfa);
	TEST_ERROR_HELPER(cc, "(ab|",           nfa);
	TEST_ERROR_HELPER(cc, "($$$|)",         nfa);
	TEST_ERROR_HELPER(cc, "(xyz|+)",        nfa);
	TEST_ERROR_HELPER(cc, "(abc\\)",        nfa);
	TEST_ERROR_HELPER(cc, "(123(987",       nfa);
	TEST_ERROR_HELPER(cc, "(123(onoes)",    nfa);
	TEST_ERROR_HELPER(cc, "(hi(bye|",       nfa);
	TEST_ERROR_HELPER(cc, "(a(z|)",         nfa);
	TEST_ERROR_HELPER(cc, "(woah|(nelly)",  nfa);
	TEST_ERROR_HELPER(cc, "two||wtf",       nfa);
	TEST_ERROR_HELPER(cc, "(ab|)",          nfa);
	TEST_ERROR_HELPER(cc, "(@#$|%&!|)",     nfa);
	TEST_ERROR_HELPER(cc, "[",              nfa);
	TEST_ERROR_HELPER(cc, "[]",             nfa);
	TEST_ERROR_HELPER(cc, "[[]",            nfa);
	TEST_ERROR_HELPER(cc, "[a-c",           nfa);
	TEST_ERROR_HELPER(cc, "[q-[]",          nfa);
	TEST_ERROR_HELPER(cc, "[()]",           nfa);
	TEST_ERROR_HELPER(cc, "[a-c*",          nfa);
	TEST_ERROR_HELPER(cc, "[a-c\\*",        nfa);
	TEST_ERROR_HELPER(cc, "[q-\\[]",        nfa);

	destroy_cmpctrl(cc);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_parse);
	RUN_TEST(test_errors_and_recovery);

	return UNITY_END();
}