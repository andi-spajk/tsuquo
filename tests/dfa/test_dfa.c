#include "../../unity/unity.h"
#include "dfa.h"
#include "nfa.h"
#include "parser.h"
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
	TEST_ASSERT_FALSE(state->seen);

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
	TEST_ASSERT_FALSE(state->seen);

	destroy_dfa(dfa);
	destroy_dfastate(state);
	destroy_nfa_and_states(nfa);
}

void test_epsilon_closure_delta(void)
{
	// see /nfa/svgs/cooper_torczon_example2.5.svg
	// a(b|c)*
	// build this in steps without parser because I need to save a reference
	// to one of the states in the middle and I'd rather not access it by
	// doing regex->start->out1->out1->out1->out1->blah->blah->blah
	NFA *regex = init_thompson_nfa('a');
	NFA *b = init_thompson_nfa('b');
	NFAState *n4 = b->start;
	NFA *c = init_thompson_nfa('c');
	NFA *b_c = nfa_union(b, c);
	b_c = transform(b_c, '*');
	regex = nfa_append(regex, b_c);
	index_states(regex);

	Set *q0 = epsilon_closure(regex->start);
	Set *eps_on_a = epsilon_closure_delta(q0, 'b');
	TEST_ASSERT_TRUE(set_is_empty(eps_on_a));

	destroy_set(eps_on_a);

	eps_on_a = epsilon_closure_delta(q0, 'a');
	TEST_ASSERT_FALSE(set_is_empty(eps_on_a));
	int expected_on_a[] = {1, 2, 3, 4, 7, 8};
	Iterator *it = set_begin(eps_on_a);
	for (int i = 0; it; i++) {
		TEST_ASSERT_EQUAL_INT(expected_on_a[i], ((NFAState *)(it->element))->index);
		advance_iter(&it);
	}

	Set *eps_on_b = epsilon_closure_delta(eps_on_a, 'b');
	TEST_ASSERT_FALSE(set_is_empty(eps_on_b));
	int expected_on_b[] = {3, 4, 5, 6, 7, 8};
	it = set_begin(eps_on_b);
	for (int i = 0; it; i++) {
		TEST_ASSERT_EQUAL_INT(expected_on_b[i], ((NFAState *)(it->element))->index);
		advance_iter(&it);
	}

	destroy_set(q0);
	destroy_set(eps_on_a);
	destroy_set(eps_on_b);

	TEST_ASSERT_EQUAL_INT(4, n4->index);
	q0 = epsilon_closure(n4);
	TEST_ASSERT_EQUAL_INT(1, q0->size);
	Set *eps_on_c = epsilon_closure_delta(q0, 'c');
	TEST_ASSERT_TRUE(set_is_empty(eps_on_c));

	destroy_set(eps_on_c);

	eps_on_b = epsilon_closure_delta(q0, 'b');
	TEST_ASSERT_FALSE(set_is_empty(eps_on_b));
	it = set_begin(eps_on_b);
	for (int i = 0; it; i++) {
		TEST_ASSERT_EQUAL_INT(expected_on_b[i], ((NFAState *)(it->element))->index);
		advance_iter(&it);
	}

	destroy_nfa_and_states(regex);
	destroy_set(q0);
	destroy_set(eps_on_b);

	CmpCtrl *cc = init_cmpctrl();
	read_line(cc, "gray|grey", 9);
	regex = parse(cc);
	index_states(regex);

	q0 = epsilon_closure(regex->start);
	TEST_ASSERT_EQUAL_INT(3, q0->size);
	Set *eps_on_g = epsilon_closure_delta(q0, 'g');
	TEST_ASSERT_FALSE(set_is_empty(eps_on_g));
	int expected_on_g[] = {2, 3, 11, 12};
	for (int i = 0; it; i++) {
		TEST_ASSERT_EQUAL_INT(expected_on_g[i], ((NFAState *)(it->element))->index);
		advance_iter(&it);
	}

	destroy_cmpctrl(cc);
	destroy_nfa_and_states(regex);
	destroy_set(q0);
	destroy_set(eps_on_g);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_inits);
	RUN_TEST(test_epsilon_closure_delta);

	return UNITY_END();
}