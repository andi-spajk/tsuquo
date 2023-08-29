#include "../../unity/unity.h"
#include "dfa.h"
#include "nfa.h"
#include "set.h"

void setUp(void) {}
void tearDown(void) {}

void test_inits(void)
{
	NFA *nfa = init_thompson_nfa('$');
	DFA *dfa = init_dfa(nfa);

	TEST_ASSERT_EQUAL_INT(1, dfa->alphabet_size);
	TEST_ASSERT_TRUE(set_is_empty(dfa->accepts));
	TEST_ASSERT_TRUE(set_is_empty(dfa->mem_region));
	TEST_ASSERT_NULL(dfa->start);

	TEST_ASSERT_EQUAL_UINT8('$', dfa->alphabet[0]);
	TEST_ASSERT_EQUAL_INT(0, dfa->mappings['$']);

	DFAState *state = init_dfastate(dfa->alphabet_size);
	TEST_ASSERT_EQUAL_INT(-1, state->index);
	TEST_ASSERT_TRUE(set_is_empty(state->constituent_nfas));

	destroy_dfa(dfa);
	destroy_dfastate(state);

	nfa = nfa_union(nfa, init_thompson_nfa('x'));
	dfa = init_dfa(nfa);

	TEST_ASSERT_EQUAL_INT(2, dfa->alphabet_size);
	TEST_ASSERT_TRUE(set_is_empty(dfa->accepts));
	TEST_ASSERT_TRUE(set_is_empty(dfa->mem_region));
	TEST_ASSERT_NULL(dfa->start);

	TEST_ASSERT_EQUAL_UINT8('$', dfa->alphabet[0]);
	TEST_ASSERT_EQUAL_INT(0, dfa->mappings['$']);

	TEST_ASSERT_EQUAL_UINT8('x', dfa->alphabet[1]);
	TEST_ASSERT_EQUAL_INT(1, dfa->mappings['x']);

	state = init_dfastate(dfa->alphabet_size);
	TEST_ASSERT_EQUAL_INT(-1, state->index);
	TEST_ASSERT_TRUE(set_is_empty(state->constituent_nfas));

	destroy_dfa(dfa);
	destroy_dfastate(state);
	destroy_nfa_and_states(nfa);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_inits);

	return UNITY_END();
}