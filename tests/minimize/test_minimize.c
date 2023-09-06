#include "../../unity/unity.h"
#include "control.h"
#include "dfa.h"
#include "minimize.h"
#include "nfa.h"
#include "parser.h"

void setUp(void) {}
void tearDown(void) {}

void test_inits(void)
{
	MinimalDFAState *min_state = init_minimal_dfastate();
	TEST_ASSERT_NOT_NULL(min_state);
	TEST_ASSERT_EQUAL_INT(0, min_state->index);
	TEST_ASSERT_FALSE(min_state->is_accept);
	TEST_ASSERT_NULL(min_state->constituent_dfa_indices);
	destroy_minimal_dfastate(min_state);

	CmpCtrl *cc = init_cmpctrl();
	read_line(cc, "abc|[bx]*", 9);
	NFA *nfa = parse(cc);
	index_states(nfa);
	DFA *dfa = convert_nfa_to_dfa(nfa);
	MinimalDFA *min_dfa = init_minimal_dfa(dfa);
	TEST_ASSERT_NOT_NULL(min_dfa);
	TEST_ASSERT_NULL(min_dfa->start);
	TEST_ASSERT_TRUE(set_is_empty(min_dfa->accepts));
	TEST_ASSERT_NULL(min_dfa->delta);

	// check table of indistinguishables
	int expected0[] = {0, 1, 1, 0, 1};
	int expected1[] =    {0, 0, 1, 0};
	int expected2[] =       {1, 0, 1};
	int expected3[] =          {0, 1};
	int expected4[] =             {0};

	TEST_ASSERT_EQUAL_INT_ARRAY(expected0, &(min_dfa->merge[0][1]), 5);
	TEST_ASSERT_EQUAL_INT_ARRAY(expected1, &(min_dfa->merge[1][2]), 4);
	TEST_ASSERT_EQUAL_INT_ARRAY(expected2, &(min_dfa->merge[2][3]), 3);
	TEST_ASSERT_EQUAL_INT_ARRAY(expected3, &(min_dfa->merge[3][4]), 2);
	TEST_ASSERT_EQUAL_INT_ARRAY(expected4, &(min_dfa->merge[4][5]), 1);

	for (int i = 0; i < dfa->size; i++)
		TEST_ASSERT_EQUAL_INT(i, min_dfa->numbers[i]);

	TEST_ASSERT_TRUE(set_is_empty(min_dfa->mem_region));

	destroy_cmpctrl(cc);
	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);
}

void test_distinguishable(void)
{
	CmpCtrl *cc = init_cmpctrl();
	read_line(cc, "abc|[bx]*", 9);
	NFA *nfa = parse(cc);
	index_states(nfa);
	DFA *dfa = convert_nfa_to_dfa(nfa);
	MinimalDFA *min_dfa = init_minimal_dfa(dfa);

	TEST_ASSERT_TRUE(distinguishable(0, 2, min_dfa, dfa));
	TEST_ASSERT_TRUE(distinguishable(0, 3, min_dfa, dfa));
	TEST_ASSERT_TRUE(distinguishable(0, 5, min_dfa, dfa));
	TEST_ASSERT_FALSE(distinguishable(2, 3, min_dfa, dfa));
	TEST_ASSERT_TRUE(distinguishable(2, 5, min_dfa, dfa));
	TEST_ASSERT_TRUE(distinguishable(3, 5, min_dfa, dfa));

	destroy_cmpctrl(cc);
	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_inits);
	RUN_TEST(test_distinguishable);

	return UNITY_END();
}