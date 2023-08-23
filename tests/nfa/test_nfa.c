#include "../../unity/unity.h"
#include "common.h"
#include "nfa.h"

void setUp(void) {}
void tearDown(void) {}

void test_inits(void)
{
	NFAState *state = init_nfastate();
	NFA *nfa = init_nfa();

	TEST_ASSERT_NULL(state->out1);
	TEST_ASSERT_NULL(state->out2);
	TEST_ASSERT_EQUAL_INT(0, state->ch);
	TEST_ASSERT_EQUAL_INT(-1, state->index);

	TEST_ASSERT_NULL(nfa->start);
	TEST_ASSERT_NULL(nfa->accept);
	TEST_ASSERT_EQUAL_UINT64(0, nfa->alphabet0_63);
	TEST_ASSERT_EQUAL_UINT64(0, nfa->alphabet64_127);
	TEST_ASSERT_EQUAL_UINT64(0, nfa->size);

	destroy_nfastate(state);
	destroy_nfa(nfa);
}

void test_init_thompson_nfa(void)
{
	NFA *t = init_thompson_nfa('a');
	TEST_ASSERT_NOT_NULL(t);
	TEST_ASSERT_EQUAL_UINT8('a', t->start->ch);
	TEST_ASSERT_EQUAL_PTR(t->accept, t->start->out1);
	TEST_ASSERT_NULL(t->start->out2);
	TEST_ASSERT_EQUAL_UINT8(0, t->accept->ch);
	TEST_ASSERT_EQUAL_UINT64(0, t->alphabet0_63);
	TEST_ASSERT_EQUAL_UINT64(1ULL << ('a' - 64), t->alphabet64_127);
	TEST_ASSERT_EQUAL_INT(2, t->size);
	destroy_nfastate(t->start);
	destroy_nfastate(t->accept);
	destroy_nfa(t);

	NFA *t2 = init_thompson_nfa('Q');
	TEST_ASSERT_NOT_NULL(t2);
	TEST_ASSERT_EQUAL_UINT8('Q', t2->start->ch);
	TEST_ASSERT_EQUAL_PTR(t2->accept, t2->start->out1);
	TEST_ASSERT_NULL(t2->start->out2);
	TEST_ASSERT_EQUAL_UINT8(0, t2->accept->ch);
	TEST_ASSERT_EQUAL_UINT64(0, t2->alphabet0_63);
	TEST_ASSERT_EQUAL_UINT64(1ULL << ('Q' - 64), t2->alphabet64_127);
	TEST_ASSERT_EQUAL_INT(2, t2->size);
	destroy_nfastate(t2->start);
	destroy_nfastate(t2->accept);
	destroy_nfa(t2);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_inits);
	RUN_TEST(test_init_thompson_nfa);

	return UNITY_END();
}