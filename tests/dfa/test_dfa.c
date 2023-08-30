#include "../../unity/unity.h"
#include "control.h"
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
	TEST_ASSERT_FALSE(state->seen);
	TEST_ASSERT_NULL(state->outs[0]);

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
	TEST_ASSERT_FALSE(state->seen);
	TEST_ASSERT_NULL(state->outs[0]);
	TEST_ASSERT_NULL(state->outs[1]);

	destroy_dfa(dfa);
	destroy_dfastate(state);
	destroy_nfa_and_states(nfa);
}

void test_epsilon_closure_delta(void)
{
	// see /tests/nfa/svgs/cooper_torczon_example2.5.svg
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


	// ********!!!DO NOT FORGET THIS!!!********
	index_states(regex);
	// ********!!!DO NOT FORGET THIS!!!********

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

	// see /tests/parser/svgs/test_20.svg
	// gray|grey
	CmpCtrl *cc = init_cmpctrl();
	read_line(cc, "gray|grey", 9);
	regex = parse(cc);

	// ********!!!DO NOT FORGET THIS!!!********
	index_states(regex);
	// ********!!!DO NOT FORGET THIS!!!********

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

void test_subset(void)
{
	DFAState *d0, *d1, *d2, *d3, *d4, *d5, *d6, *d7, *d8;
	CmpCtrl *cc = init_cmpctrl();
	NFA *nfa;
	read_line(cc, "a(b|c)*", 7);
	nfa = parse(cc);

	// ********!!!DO NOT FORGET THIS!!!********
	index_states(nfa);
	// ********!!!DO NOT FORGET THIS!!!********

	DFA *dfa = subset(nfa);
	TEST_ASSERT_NOT_NULL(dfa);
	TEST_ASSERT_EQUAL_INT(3, dfa->alphabet_size);
	TEST_ASSERT_EQUAL_INT(3, dfa->accepts->size);
	TEST_ASSERT_EQUAL_INT(4, dfa->mem_region->size);
	int a = 0;
	int b = 1;
	int c = 2;
	TEST_ASSERT_EQUAL_UINT8('a', dfa->alphabet[a]);
	TEST_ASSERT_EQUAL_UINT8('b', dfa->alphabet[b]);
	TEST_ASSERT_EQUAL_UINT8('c', dfa->alphabet[c]);

	// the d0, d1, d2 etc numbering may not reflect the actual dfa state
	// indices that are produced by subset()
	// i number them for these tests just for convenience
	d0 = dfa->start;
	TEST_ASSERT_NOT_NULL(d0->outs[a]);
	TEST_ASSERT_NULL(d0->outs[b]);
	TEST_ASSERT_NULL(d0->outs[c]);
	TEST_ASSERT_EQUAL_INT(1, d0->constituent_nfastates->size);

	d1 = d0->outs[a];
	d2 = d1->outs[b];
	d3 = d1->outs[c];
	TEST_ASSERT_NULL(d1->outs[a]);
	TEST_ASSERT_NOT_NULL(d1->outs[b]);
	TEST_ASSERT_NOT_NULL(d1->outs[c]);
	TEST_ASSERT_EQUAL_INT(6, d1->constituent_nfastates->size);

	TEST_ASSERT_NULL(d2->outs[a]);
	TEST_ASSERT_NULL(d3->outs[a]);

	TEST_ASSERT_EQUAL_PTR(d2, d2->outs[b]);
	TEST_ASSERT_EQUAL_PTR(d2, d3->outs[b]);
	TEST_ASSERT_EQUAL_INT(6, d2->constituent_nfastates->size);
	TEST_ASSERT_EQUAL_PTR(d3, d3->outs[c]);
	TEST_ASSERT_EQUAL_PTR(d3, d2->outs[c]);
	TEST_ASSERT_EQUAL_INT(6, d3->constituent_nfastates->size);

	TEST_ASSERT_FALSE(set_find(dfa->accepts, d0));
	TEST_ASSERT_TRUE(set_find(dfa->accepts, d1));
	TEST_ASSERT_TRUE(set_find(dfa->accepts, d2));
	TEST_ASSERT_TRUE(set_find(dfa->accepts, d3));
	TEST_ASSERT_EQUAL_INT(3, dfa->accepts->size);

	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);

	read_line(cc, "who|what|where", 14);
	nfa = parse(cc);

	// ********!!!DO NOT FORGET THIS!!!********
	index_states(nfa);
	// ********!!!DO NOT FORGET THIS!!!********

	dfa = subset(nfa);
	TEST_ASSERT_NOT_NULL(dfa);
	TEST_ASSERT_EQUAL_INT(7, dfa->alphabet_size);
	TEST_ASSERT_EQUAL_INT(3, dfa->accepts->size);
	TEST_ASSERT_EQUAL_INT(9, dfa->mem_region->size);

	//             0 1 2 3 4 5 6
	// alphabet = {a,e,h,o,r,t,w}
	d0 = dfa->start;
	TEST_ASSERT_NOT_NULL(d0->outs[dfa->mappings['w']]);
	TEST_ASSERT_EACH_EQUAL_PTR(NULL, d0->outs, 6);
	TEST_ASSERT_EQUAL_INT(5, d0->constituent_nfastates->size);
	TEST_ASSERT_FALSE(set_find(dfa->accepts, d0));

	d1 = d0->outs[dfa->mappings['w']];
	TEST_ASSERT_NOT_NULL(d1->outs[dfa->mappings['h']]);
	TEST_ASSERT_EACH_EQUAL_PTR(NULL, d1->outs, 2);  // a e
	TEST_ASSERT_EACH_EQUAL_PTR(NULL, &(d1->outs[3]), 4);  // o r t w
	TEST_ASSERT_EQUAL_INT(6, d1->constituent_nfastates->size);
	TEST_ASSERT_FALSE(set_find(dfa->accepts, d1));

	d2 = d1->outs[dfa->mappings['h']];
	d3 = d2->outs[dfa->mappings['a']];
	d4 = d2->outs[dfa->mappings['e']];
	d5 = d2->outs[dfa->mappings['o']];
	TEST_ASSERT_NOT_NULL(d3);  // a
	TEST_ASSERT_NOT_NULL(d4);  // e
	TEST_ASSERT_NOT_NULL(d5);  // o
	TEST_ASSERT_NULL(d2->outs[dfa->mappings['h']]);
	TEST_ASSERT_EACH_EQUAL_PTR(NULL, &(d2->outs[4]), 3);  // r t w
	TEST_ASSERT_EQUAL_INT(6, d2->constituent_nfastates->size);
	TEST_ASSERT_FALSE(set_find(dfa->accepts, d2));

	d6 = d3->outs[dfa->mappings['t']];
	TEST_ASSERT_EACH_EQUAL_PTR(NULL, d3->outs, 5); // a e h o r
	TEST_ASSERT_NULL(d3->outs[dfa->mappings['w']]);
	// don't test constituent_nfastates since parse() may not group the
	// unions in the same way i do
	// let's hope that the future graphviz tests confirm the DFA correctness
	// with 100% certainty
	TEST_ASSERT_FALSE(set_find(dfa->accepts, d3));

	d7 = d4->outs[dfa->mappings['r']];
	TEST_ASSERT_EACH_EQUAL_PTR(NULL, d4->outs, 4); // a e h o
	TEST_ASSERT_EACH_EQUAL_PTR(NULL, &(d4->outs[5]), 2); // t w
	TEST_ASSERT_FALSE(set_find(dfa->accepts, d4));

	TEST_ASSERT_EACH_EQUAL_PTR(NULL, d5->outs, 7);
	TEST_ASSERT_TRUE(set_find(dfa->accepts, d5));

	TEST_ASSERT_EACH_EQUAL_PTR(NULL, d6->outs, 7);
	TEST_ASSERT_TRUE(set_find(dfa->accepts, d6));

	d8 = d7->outs[dfa->mappings['e']];
	TEST_ASSERT_NULL(d7->outs[dfa->mappings['a']]);
	TEST_ASSERT_EACH_EQUAL_PTR(NULL, &(d7->outs[2]), 5); // h o r t w
	TEST_ASSERT_FALSE(set_find(dfa->accepts, d7));

	TEST_ASSERT_EACH_EQUAL_PTR(NULL, d8->outs, 7);
	TEST_ASSERT_TRUE(set_find(dfa->accepts, d8));

	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);

	read_line(cc, "(ab|ac)*", 8);
	nfa = parse(cc);

	// ********!!!DO NOT FORGET THIS!!!********
	index_states(nfa);
	// ********!!!DO NOT FORGET THIS!!!********

	dfa = subset(nfa);
	TEST_ASSERT_NOT_NULL(dfa);
	TEST_ASSERT_EQUAL_INT(3, dfa->alphabet_size);
	TEST_ASSERT_EQUAL_INT(3, dfa->accepts->size);
	TEST_ASSERT_EQUAL_INT(4, dfa->mem_region->size);

	d0 = dfa->start;
	TEST_ASSERT_NOT_NULL(d0->outs[a]);
	TEST_ASSERT_NULL(d0->outs[b]);
	TEST_ASSERT_NULL(d0->outs[c]);
	TEST_ASSERT_TRUE(set_find(dfa->accepts, d0));

	d1 = d0->outs[a];
	d2 = d1->outs[b];
	d3 = d1->outs[c];
	TEST_ASSERT_NULL(d1->outs[a]);
	TEST_ASSERT_NOT_NULL(d2);
	TEST_ASSERT_NOT_NULL(d3);
	TEST_ASSERT_FALSE(set_find(dfa->accepts, d1));

	TEST_ASSERT_EQUAL_PTR(d1, d2->outs[a]);
	TEST_ASSERT_NULL(d2->outs[b]);
	TEST_ASSERT_NULL(d2->outs[c]);
	TEST_ASSERT_TRUE(set_find(dfa->accepts, d2));

	TEST_ASSERT_EQUAL_PTR(d1, d3->outs[a]);
	TEST_ASSERT_NULL(d3->outs[b]);
	TEST_ASSERT_NULL(d3->outs[c]);
	TEST_ASSERT_TRUE(set_find(dfa->accepts, d3));

	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);

	read_line(cc, "(0|1)*11001*", 12);
	nfa = parse(cc);

	// ********!!!DO NOT FORGET THIS!!!********
	index_states(nfa);
	// ********!!!DO NOT FORGET THIS!!!********

	dfa = subset(nfa);
	TEST_ASSERT_NOT_NULL(dfa);
	TEST_ASSERT_EQUAL_INT(3, dfa->accepts->size);
	TEST_ASSERT_EQUAL_INT(8, dfa->mem_region->size);

	// too lazy

	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_cmpctrl(cc);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_inits);
	RUN_TEST(test_epsilon_closure_delta);
	RUN_TEST(test_subset);

	return UNITY_END();
}