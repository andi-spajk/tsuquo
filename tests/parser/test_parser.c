#include "../../unity/unity.h"
#include "common.h"
#include "control.h"
#include "parser.h"

void setUp(void) {}
void tearDown(void) {}

void test_parse(void)
{
	CmpCtrl *cc = init_cmpctrl();
	read_file(cc, "../example.txt");
	NFA *nfa = parse(cc);
	TEST_ASSERT_NOT_NULL(nfa);
	gen_nfa_graphviz(nfa, "a_b_or_c_closure.dot");
	destroy_nfa_and_states(nfa);
	destroy_cmpctrl(cc);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_parse);

	return UNITY_END();
}