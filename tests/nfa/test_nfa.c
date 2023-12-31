#include <stddef.h>

#include "../../unity/unity.h"
#include "common.h"
#include "lexer.h"
#include "nfa.h"

void setUp(void) {}
void tearDown(void) {}

void test_inits(void)
{
	NFAState *state = init_nfastate();
	NFA *nfa = init_nfa();

	TEST_ASSERT_NULL(state->out1);
	TEST_ASSERT_NULL(state->out2);
	TEST_ASSERT_EQUAL_INT(EPSILON, state->ch);
	TEST_ASSERT_EQUAL_INT(-1, state->index);
	TEST_ASSERT_FALSE(state->seen);

	TEST_ASSERT_NULL(nfa->start);
	TEST_ASSERT_NULL(nfa->accept);
	TEST_ASSERT_EQUAL_UINT64(0, nfa->alphabet0_63);
	TEST_ASSERT_EQUAL_UINT64(0, nfa->alphabet64_127);
	TEST_ASSERT_EQUAL_UINT64(0, nfa->size);
	TEST_ASSERT_EQUAL_PTR(compare_nfastate_ptr, nfa->mem_region->compare);

	destroy_nfastate(state);
	destroy_nfa(nfa);
}

void test_init_thompson_nfa(void)
{
	NFA *t = init_thompson_nfa('a');
	TEST_ASSERT_NOT_NULL(t);
	TEST_ASSERT_EQUAL_PTR(compare_nfastate_ptr, t->mem_region->compare);

	TEST_ASSERT_EQUAL_UINT8('a', t->start->ch);
	TEST_ASSERT_EQUAL_PTR(t->accept, t->start->out1);
	TEST_ASSERT_NULL(t->start->out2);

	TEST_ASSERT_EQUAL_UINT8(EPSILON, t->accept->ch);

	TEST_ASSERT_EQUAL_UINT64(0, t->alphabet0_63);
	TEST_ASSERT_EQUAL_UINT64(1ULL << ('a' - 64), t->alphabet64_127);

	TEST_ASSERT_EQUAL_INT(2, t->size);

	TEST_ASSERT_TRUE(set_find(t->mem_region, t->start));
	TEST_ASSERT_TRUE(set_find(t->mem_region, t->accept));

	destroy_nfa_and_states(t);

	NFA *t2 = init_thompson_nfa('Q');
	TEST_ASSERT_NOT_NULL(t2);
	TEST_ASSERT_EQUAL_PTR(compare_nfastate_ptr, t2->mem_region->compare);

	TEST_ASSERT_EQUAL_UINT8('Q', t2->start->ch);
	TEST_ASSERT_EQUAL_PTR(t2->accept, t2->start->out1);
	TEST_ASSERT_NULL(t2->start->out2);

	TEST_ASSERT_EQUAL_UINT8(0, t2->accept->ch);

	TEST_ASSERT_EQUAL_UINT64(0, t2->alphabet0_63);
	TEST_ASSERT_EQUAL_UINT64(1ULL << ('Q' - 64), t2->alphabet64_127);
	TEST_ASSERT_EQUAL_INT(2, t2->size);

	TEST_ASSERT_TRUE(set_find(t2->mem_region, t2->start));
	TEST_ASSERT_TRUE(set_find(t2->mem_region, t2->accept));

	destroy_nfa_and_states(t2);

	NFA *tab = init_thompson_nfa('\t');
	TEST_ASSERT_NOT_NULL(tab);
	TEST_ASSERT_EQUAL_PTR(compare_nfastate_ptr, tab->mem_region->compare);

	TEST_ASSERT_EQUAL_UINT8('\t', tab->start->ch);
	TEST_ASSERT_EQUAL_PTR(tab->accept, tab->start->out1);
	TEST_ASSERT_NULL(tab->start->out2);

	TEST_ASSERT_EQUAL_UINT8(0, tab->accept->ch);

	TEST_ASSERT_EQUAL_UINT64(1ULL << '\t', tab->alphabet0_63);
	TEST_ASSERT_EQUAL_UINT64(0, tab->alphabet64_127);
	TEST_ASSERT_EQUAL_INT(2, tab->size);

	TEST_ASSERT_TRUE(set_find(tab->mem_region, tab->start));
	TEST_ASSERT_TRUE(set_find(tab->mem_region, tab->accept));

	destroy_nfa_and_states(tab);

	// it makes more sense to test wildcard NFAs in gen_nfa_graphviz
	// since wildcard would call nfa_union() and init_range_nfa()
	// by the time we generate a graphviz, those other functions have been
	// well tested. besides, the only way to practically examine an NFA for
	// the wildcard is by examining the graphviz
}

void test_nfa_union(void)
{
	NFA *regex = init_thompson_nfa('a');
	TEST_ASSERT_EQUAL_PTR(regex, nfa_union(regex, NULL));
	NFA *b = init_thompson_nfa('b');
	TEST_ASSERT_EQUAL_PTR(b, nfa_union(NULL, b));
	regex = nfa_union(regex, b);

	TEST_ASSERT_NOT_NULL(regex);
	U64 new_alphabet = 1ULL << ('a' - 64);
	new_alphabet |= (1ULL << ('b' - 64));
	TEST_ASSERT_EQUAL_UINT64(0, regex->alphabet0_63);
	TEST_ASSERT_EQUAL_UINT64(new_alphabet, regex->alphabet64_127);
	TEST_ASSERT_EQUAL_INT(6, regex->size);

	NFAState *curr = regex->start;
	TEST_ASSERT_EQUAL_UINT8(EPSILON, curr->ch);
	TEST_ASSERT_NOT_NULL(curr->out1);
	TEST_ASSERT_NOT_NULL(curr->out2);
	TEST_ASSERT_TRUE(set_find(regex->mem_region, curr));

	// 'a' path
	curr = curr->out1;
	TEST_ASSERT_EQUAL_UINT8('a', curr->ch);
	TEST_ASSERT_NULL(curr->out2);
	TEST_ASSERT_TRUE(set_find(regex->mem_region, curr));

	curr = curr->out1;
	TEST_ASSERT_EQUAL_UINT8(EPSILON, curr->ch);
	TEST_ASSERT_EQUAL_PTR(regex->accept, curr->out1);
	TEST_ASSERT_TRUE(set_find(regex->mem_region, curr));

	// go back to 'b' path
	curr = regex->start->out2;
	TEST_ASSERT_EQUAL_UINT8('b', curr->ch);
	TEST_ASSERT_NULL(curr->out2);
	TEST_ASSERT_TRUE(set_find(regex->mem_region, curr));

	curr = curr->out1;
	TEST_ASSERT_EQUAL_UINT8(EPSILON, curr->ch);
	TEST_ASSERT_EQUAL_PTR(regex->accept, curr->out1);
	TEST_ASSERT_TRUE(set_find(regex->mem_region, curr));

	destroy_nfa_and_states(regex);
}

void test_nfa_append(void)
{
	NFA *regex = init_thompson_nfa('x');
	NFA *y = init_thompson_nfa('y');
	regex = nfa_append(regex, y);
	TEST_ASSERT_NOT_NULL(regex);

	U64 new_alphabet = 1ULL << ('x' - 64);
	new_alphabet |= (1ULL << ('y' - 64));
	TEST_ASSERT_EQUAL_UINT64(0, regex->alphabet0_63);
	TEST_ASSERT_EQUAL_UINT64(new_alphabet, regex->alphabet64_127);
	TEST_ASSERT_EQUAL_INT(4, regex->size);

	NFAState *curr = regex->start;
	TEST_ASSERT_EQUAL_UINT8('x', curr->ch);
	TEST_ASSERT_NOT_NULL(curr->out1);
	TEST_ASSERT_NULL(curr->out2);
	TEST_ASSERT_TRUE(set_find(regex->mem_region, curr));

	curr = curr->out1;
	TEST_ASSERT_EQUAL_UINT8(EPSILON, curr->ch);
	TEST_ASSERT_NOT_NULL(curr->out1);
	TEST_ASSERT_NULL(curr->out2);
	TEST_ASSERT_TRUE(set_find(regex->mem_region, curr));

	curr = curr->out1;
	TEST_ASSERT_EQUAL_UINT8('y', curr->ch);
	TEST_ASSERT_NOT_NULL(curr->out1);
	TEST_ASSERT_NULL(curr->out2);
	TEST_ASSERT_TRUE(set_find(regex->mem_region, curr));

	curr = curr->out1;
	TEST_ASSERT_EQUAL_UINT8(EPSILON, curr->ch);
	TEST_ASSERT_NULL(curr->out1);
	TEST_ASSERT_NULL(curr->out2);
	TEST_ASSERT_EQUAL_PTR(regex->accept, curr);
	TEST_ASSERT_TRUE(set_find(regex->mem_region, curr));

	NFA *shallow_copy = NULL;
	shallow_copy = nfa_append(shallow_copy, regex);
	TEST_ASSERT_EQUAL_PTR(regex, shallow_copy);

	TEST_ASSERT_EQUAL_PTR(regex, nfa_append(regex, NULL));

	destroy_nfa_and_states(regex);
}

void test_transform(void)
{
	NFA *regex = init_thompson_nfa('$');
	TEST_ASSERT_NOT_NULL(regex);

	regex = transform(regex, '*');
	TEST_ASSERT_NOT_NULL(regex);
	TEST_ASSERT_EQUAL_UINT64((1ULL << '$'), regex->alphabet0_63);
	TEST_ASSERT_EQUAL_UINT64(0, regex->alphabet64_127);
	TEST_ASSERT_EQUAL_INT(4, regex->size);

	NFAState *curr = regex->start;
	TEST_ASSERT_EQUAL_UINT8(EPSILON, curr->ch);
	TEST_ASSERT_EQUAL_PTR(regex->accept, curr->out2);
	TEST_ASSERT_TRUE(set_find(regex->mem_region, curr));

	curr = curr->out1;
	TEST_ASSERT_EQUAL_UINT8('$', curr->ch);
	TEST_ASSERT_NULL(curr->out2);
	TEST_ASSERT_TRUE(set_find(regex->mem_region, curr));

	curr = curr->out1;
	TEST_ASSERT_EQUAL_UINT8(EPSILON, curr->ch);
	TEST_ASSERT_NOT_NULL(curr->out1);
	TEST_ASSERT_EQUAL_PTR(regex->start->out1, curr->out2);
	TEST_ASSERT_NOT_NULL(curr->out2);
	TEST_ASSERT_EQUAL_PTR(regex->accept, curr->out1);
	TEST_ASSERT_TRUE(set_find(regex->mem_region, curr));

	destroy_nfa_and_states(regex);


	regex = init_thompson_nfa('w');
	TEST_ASSERT_NOT_NULL(regex);

	regex = transform(regex, '?');
	TEST_ASSERT_NOT_NULL(regex);
	TEST_ASSERT_EQUAL_UINT64(0, regex->alphabet0_63);
	TEST_ASSERT_EQUAL_UINT64((1ULL << ('w'-64)), regex->alphabet64_127);
	TEST_ASSERT_EQUAL_INT(4, regex->size);

	curr = regex->start;
	TEST_ASSERT_EQUAL_UINT8(EPSILON, curr->ch);
	TEST_ASSERT_NOT_NULL(curr->out1);
	TEST_ASSERT_NOT_NULL(curr->out2);
	TEST_ASSERT_EQUAL_PTR(regex->accept, curr->out2);
	TEST_ASSERT_TRUE(set_find(regex->mem_region, curr));

	curr = curr->out1;
	TEST_ASSERT_EQUAL_UINT8('w', curr->ch);
	TEST_ASSERT_NULL(curr->out2);
	TEST_ASSERT_TRUE(set_find(regex->mem_region, curr));

	curr = curr->out1;
	TEST_ASSERT_EQUAL_UINT8(EPSILON, curr->ch);
	TEST_ASSERT_NOT_NULL(curr->out1);
	TEST_ASSERT_EQUAL_PTR(regex->accept, curr->out1);
	TEST_ASSERT_NULL(curr->out2);
	TEST_ASSERT_TRUE(set_find(regex->mem_region, curr));

	destroy_nfa_and_states(regex);


	regex = init_thompson_nfa(' ');
	TEST_ASSERT_NOT_NULL(regex);

	regex = transform(regex, '+');
	TEST_ASSERT_NOT_NULL(regex);
	TEST_ASSERT_NOT_NULL(regex);
	TEST_ASSERT_EQUAL_UINT64((1ULL << ' '), regex->alphabet0_63);
	TEST_ASSERT_EQUAL_UINT64(0, regex->alphabet64_127);
	TEST_ASSERT_EQUAL_INT(4, regex->size);

	curr = regex->start;
	TEST_ASSERT_EQUAL_UINT8(EPSILON, curr->ch);
	TEST_ASSERT_NULL(curr->out2);
	TEST_ASSERT_TRUE(set_find(regex->mem_region, curr));

	curr = curr->out1;
	TEST_ASSERT_EQUAL_UINT8(' ', curr->ch);
	TEST_ASSERT_NULL(curr->out2);
	TEST_ASSERT_TRUE(set_find(regex->mem_region, curr));

	curr = curr->out1;
	TEST_ASSERT_EQUAL_UINT8(EPSILON, curr->ch);
	TEST_ASSERT_NOT_NULL(curr->out1);
	TEST_ASSERT_EQUAL_PTR(regex->start->out1, curr->out2);
	TEST_ASSERT_NOT_NULL(curr->out2);
	TEST_ASSERT_EQUAL_PTR(regex->accept, curr->out1);
	TEST_ASSERT_TRUE(set_find(regex->mem_region, curr));

	destroy_nfa_and_states(regex);

	// (a|b)*
	NFA *a = init_thompson_nfa('a');
	NFA *b = init_thompson_nfa('b');
	TEST_ASSERT_NOT_NULL(a);
	TEST_ASSERT_NOT_NULL(b);
	regex = nfa_union(a, b);
	TEST_ASSERT_NOT_NULL(regex);
	regex = transform(regex, '*');
	TEST_ASSERT_NOT_NULL(regex);

	TEST_ASSERT_EQUAL_UINT64(0, regex->alphabet0_63);
	TEST_ASSERT_EQUAL_UINT64(((1ULL << ('a'-64)) | (1ULL << ('b'-64))), regex->alphabet64_127);
	TEST_ASSERT_EQUAL_INT(8, regex->size);

	curr = regex->start;
	TEST_ASSERT_EQUAL_PTR(regex->accept, curr->out2);
	TEST_ASSERT_TRUE(set_find(regex->mem_region, curr));

	curr = curr->out1;
	TEST_ASSERT_TRUE(set_find(regex->mem_region, curr));
	TEST_ASSERT_EQUAL_UINT8('a', curr->out1->ch);
	TEST_ASSERT_TRUE(set_find(regex->mem_region, curr->out1));
	TEST_ASSERT_EQUAL_UINT8('b', curr->out2->ch);
	TEST_ASSERT_TRUE(set_find(regex->mem_region, curr->out2));
	TEST_ASSERT_EQUAL_PTR(curr, curr->out1->out1->out1->out2); // cycle back

	destroy_nfa_and_states(regex);
}

void test_init_range_nfa(void)
{
	NFA *range = NULL;

	// [A-B]
	range = init_range_nfa('A', 'B');
	TEST_ASSERT_NOT_NULL(range);
	gen_nfa_graphviz(range, "dots/A-B.dot");
	destroy_nfa_and_states(range);
	// [A-C]
	range = init_range_nfa('A', 'C');
	TEST_ASSERT_NOT_NULL(range);
	gen_nfa_graphviz(range, "dots/A-C.dot");
	destroy_nfa_and_states(range);
	// [A-D]
	range = init_range_nfa('A', 'D');
	TEST_ASSERT_NOT_NULL(range);
	gen_nfa_graphviz(range, "dots/A-D.dot");
	destroy_nfa_and_states(range);
	// [A-E]
	range = init_range_nfa('A', 'E');
	TEST_ASSERT_NOT_NULL(range);
	gen_nfa_graphviz(range, "dots/A-E.dot");
	destroy_nfa_and_states(range);
	// [A-F]
	range = init_range_nfa('A', 'F');
	TEST_ASSERT_NOT_NULL(range);
	gen_nfa_graphviz(range, "dots/A-F.dot");
	destroy_nfa_and_states(range);
	// [A-G]
	range = init_range_nfa('A', 'G');
	TEST_ASSERT_NOT_NULL(range);
	gen_nfa_graphviz(range, "dots/A-G.dot");
	destroy_nfa_and_states(range);
	// [A-H]
	range = init_range_nfa('A', 'H');
	TEST_ASSERT_NOT_NULL(range);
	gen_nfa_graphviz(range, "dots/A-H.dot");
	destroy_nfa_and_states(range);
	// [A-Z]
	range = init_range_nfa('A', 'Z');
	TEST_ASSERT_NOT_NULL(range);
	gen_nfa_graphviz(range, "dots/A-Z.dot");
	destroy_nfa_and_states(range);
	// [0-9]
	range = init_range_nfa('0', '9');
	TEST_ASSERT_NOT_NULL(range);
	gen_nfa_graphviz(range, "dots/digit09.dot");
	range = transform(range, '+');
	gen_nfa_graphviz(range, "dots/digit09plus.dot");
	destroy_nfa_and_states(range);
	// [ -(]
	range = init_range_nfa(' ', '(');
	TEST_ASSERT_NOT_NULL(range);
	gen_nfa_graphviz(range, "dots/space_paren.dot");
	destroy_nfa_and_states(range);
	// [0-9]
	range = init_range_nfa('0', '7');
	TEST_ASSERT_NOT_NULL(range);
	gen_nfa_graphviz(range, "dots/digit07.dot");
	destroy_nfa_and_states(range);
	// [\[-`]
	range = init_range_nfa('[', '`');
	TEST_ASSERT_NOT_NULL(range);
	gen_nfa_graphviz(range, "dots/rbrak_grave.dot");
	destroy_nfa_and_states(range);
	// .
	range = init_range_nfa(' ', '~');
	TEST_ASSERT_NOT_NULL(range);
	gen_nfa_graphviz(range, "dots/matchall.dot");
	destroy_nfa_and_states(range);
}

void test_index_states_and_gen_graphviz(void)
{
	// a
	NFA *a = init_thompson_nfa('a');
	TEST_ASSERT_NOT_NULL(a);
	TEST_ASSERT_EQUAL_INT(a->size, index_states(a)+1);
	gen_nfa_graphviz(a, "dots/a.dot");

	// b
	NFA *b = init_thompson_nfa('b');
	TEST_ASSERT_NOT_NULL(b);
	TEST_ASSERT_EQUAL_INT(b->size, index_states(b)+1);
	gen_nfa_graphviz(b, "dots/b.dot");

	// a|b
	NFA *regex = nfa_union(a, b);
	TEST_ASSERT_NOT_NULL(regex);
	TEST_ASSERT_EQUAL_INT(regex->size, index_states(regex)+1);
	gen_nfa_graphviz(regex, "dots/a_or_b.dot");

	// (a|b)*
	regex = transform(regex, '*');
	TEST_ASSERT_NOT_NULL(regex);
	TEST_ASSERT_EQUAL_INT(regex->size, index_states(regex)+1);
	gen_nfa_graphviz(regex, "dots/a_or_b_closure.dot");

	destroy_nfa_and_states(regex);
}

void test_graphviz_other(void)
{
	// @+
	NFA *at = init_thompson_nfa('@');
	at = transform(at, '+');
	gen_nfa_graphviz(at, "dots/atplus.dot");
	destroy_nfa_and_states(at);

	// &?
	NFA *maybe_and = init_thompson_nfa('&');
	maybe_and = transform(maybe_and, '?');
	gen_nfa_graphviz(maybe_and, "dots/maybe_and.dot");
	destroy_nfa_and_states(maybe_and);

	// \t*
	NFA *any_tabs = init_thompson_nfa('\t');
	any_tabs = transform(any_tabs, '*');
	gen_nfa_graphviz(any_tabs, "dots/any_tabs.dot");
	destroy_nfa_and_states(any_tabs);

	// \n
	NFA *newline = init_thompson_nfa('\n');
	gen_nfa_graphviz(newline, "dots/newline.dot");
	destroy_nfa_and_states(newline);

	// .
	NFA *wildcard = init_thompson_nfa(TK_WILDCARD);
	gen_nfa_graphviz(wildcard, "dots/wildcard.dot");
	U64 exp0_63 = 0xFFFFFFFF00000000 | 1ULL << '\n' | 1ULL << '\t';
	U64 exp64_127 = 0x7FFFFFFFFFFFFFFF;
	TEST_ASSERT_EQUAL_UINT64(exp0_63, wildcard->alphabet0_63);
	TEST_ASSERT_EQUAL_UINT64(exp64_127, wildcard->alphabet64_127);
	destroy_nfa_and_states(wildcard);
}

void test_epsilon_closure(void)
{
	// a(b|c)*
	NFA *b = init_thompson_nfa('b');
	TEST_ASSERT_NOT_NULL(b);

	NFA *c = init_thompson_nfa('c');
	TEST_ASSERT_NOT_NULL(c);

	NFA *regex = nfa_union(b, c);
	TEST_ASSERT_NOT_NULL(regex);

	regex = transform(regex, '*');
	TEST_ASSERT_NOT_NULL(regex);

	NFA *a = init_thompson_nfa('a');
	TEST_ASSERT_NOT_NULL(a);

	regex = nfa_append(a, regex);
	TEST_ASSERT_NOT_NULL(regex);
	gen_nfa_graphviz(regex, "dots/cooper_torczon_example2.5.dot");

	TEST_ASSERT_EQUAL_UINT8('a', regex->start->ch);
	Set *eps = epsilon_closure(regex->start->out1);
	TEST_ASSERT_NOT_NULL(eps);
	TEST_ASSERT_EQUAL_INT(6, eps->size);

	int expected[] = {1, 2, 3, 4, 7, 8};
	Iterator *it = set_begin(eps);
	for (int i = 0; it; i++) {
		TEST_ASSERT_EQUAL_INT(expected[i], ((NFAState *)it->element)->index);
		advance_iter(&it);
	}

	destroy_nfa_and_states(regex);
	destroy_set(eps);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_inits);
	RUN_TEST(test_init_thompson_nfa);
	RUN_TEST(test_nfa_union);
	RUN_TEST(test_nfa_append);
	RUN_TEST(test_transform);
	RUN_TEST(test_init_range_nfa);
	RUN_TEST(test_index_states_and_gen_graphviz);
	RUN_TEST(test_graphviz_other);
	RUN_TEST(test_epsilon_closure);

	return UNITY_END();
}