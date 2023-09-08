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

	TEST_ASSERT_FALSE(min_dfa->collected_last_state);

	for (int i = 0; i < dfa->size; i++)
		TEST_ASSERT_EQUAL_INT(i, min_dfa->numbers[i]);

	TEST_ASSERT_TRUE(set_is_empty(min_dfa->mem_region));

	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);

	read_line(cc, "@*", 2);
	nfa = parse(cc);
	dfa = convert_nfa_to_dfa(nfa);
	min_dfa = init_minimal_dfa(dfa);

	int just_one_row[] = {1};
	TEST_ASSERT_EQUAL_INT_ARRAY(just_one_row, &(min_dfa->merge[0][1]), 1);

	TEST_ASSERT_FALSE(min_dfa->collected_last_state);

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
	DFA *dfa = convert_nfa_to_dfa(nfa);
	MinimalDFA *min_dfa = init_minimal_dfa(dfa);

/*
Table of Indistinguishable States
--------------+--------------
     Final    |    Initial
--------------+--------------
  1 2 3 4 5   |     1 2 3 4 5
0 F F F F F   |   0 F     F
1   F F F F   |   1   F F   F
2       F F   |   2       F
3       F F   |   3       F
4         F   |   4         F
*/

	// rows/cols with 1 xor 4 were distinguished during init_minimal_dfa(),
	// so don't test them
	TEST_ASSERT_TRUE(distinguishable(0, 2, min_dfa, dfa));
	TEST_ASSERT_TRUE(distinguishable(0, 3, min_dfa, dfa));
	TEST_ASSERT_TRUE(distinguishable(0, 5, min_dfa, dfa));
	TEST_ASSERT_TRUE(distinguishable(1, 4, min_dfa, dfa));
	TEST_ASSERT_TRUE(distinguishable(2, 5, min_dfa, dfa));
	TEST_ASSERT_TRUE(distinguishable(3, 5, min_dfa, dfa));

	TEST_ASSERT_FALSE(distinguishable(2, 3, min_dfa, dfa));

	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);

	read_line(cc, "for|[f-h]*", 10);
	nfa = parse(cc);
	dfa = convert_nfa_to_dfa(nfa);
	min_dfa = init_minimal_dfa(dfa);

/*
Table of Indistinguishable States
----------------+----------------
     Final      |     Initial
----------------+----------------
  1 2 3 4 5 6   |     1 2 3 4 5 6
0 F F F F F F   |   0         F
1   F F F F F   |   1         F
2         F F   |   2         F
3         F F   |   3         F
4         F F   |   4         F
5           F   |   5           F
*/

	// rows/cols with state 5 are distinguished during init_minimal_dfa(),
	// so don't test them
	TEST_ASSERT_TRUE(distinguishable(0, 1, min_dfa, dfa));
	TEST_ASSERT_TRUE(distinguishable(0, 2, min_dfa, dfa));
	TEST_ASSERT_TRUE(distinguishable(0, 3, min_dfa, dfa));
	TEST_ASSERT_TRUE(distinguishable(0, 4, min_dfa, dfa));
	TEST_ASSERT_TRUE(distinguishable(0, 6, min_dfa, dfa));
	TEST_ASSERT_TRUE(distinguishable(1, 2, min_dfa, dfa));
	TEST_ASSERT_TRUE(distinguishable(1, 3, min_dfa, dfa));
	TEST_ASSERT_TRUE(distinguishable(1, 4, min_dfa, dfa));
	TEST_ASSERT_TRUE(distinguishable(1, 6, min_dfa, dfa));
	TEST_ASSERT_TRUE(distinguishable(2, 6, min_dfa, dfa));
	TEST_ASSERT_TRUE(distinguishable(3, 6, min_dfa, dfa));
	TEST_ASSERT_TRUE(distinguishable(4, 6, min_dfa, dfa));

	TEST_ASSERT_FALSE(distinguishable(2, 3, min_dfa, dfa));
	TEST_ASSERT_FALSE(distinguishable(2, 4, min_dfa, dfa));
	TEST_ASSERT_FALSE(distinguishable(3, 4, min_dfa, dfa));

	destroy_cmpctrl(cc);
	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);
}

void test_quotient(void)
{
	CmpCtrl *cc = init_cmpctrl();
	read_line(cc, "(ab|ac)*", 8);
	NFA *nfa = parse(cc);
	DFA *dfa = convert_nfa_to_dfa(nfa);
	MinimalDFA *min_dfa = init_minimal_dfa(dfa);

	TEST_ASSERT_EQUAL_INT(0, quotient(min_dfa, dfa));

/*
  1 2 3
0 F T T
1   F F
2     T
*/
	int expected0[] = {0, 1, 1};
	int expected1[] =    {0, 0};
	int expected2[] =       {1};

	TEST_ASSERT_EQUAL_INT_ARRAY(expected0, &(min_dfa->merge[0][1]), 3);
	TEST_ASSERT_EQUAL_INT_ARRAY(expected1, &(min_dfa->merge[1][2]), 2);
	TEST_ASSERT_EQUAL_INT_ARRAY(expected2, &(min_dfa->merge[2][3]), 1);

	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);


	read_line(cc, "(0|(1(01*(00)*0)*1)*)*", 22);
	nfa = parse(cc);
	dfa = convert_nfa_to_dfa(nfa);
	min_dfa = init_minimal_dfa(dfa);

	TEST_ASSERT_EQUAL_INT(0, quotient(min_dfa, dfa));

/*
  1 2 3 4 5 6 7
0 T F F T F F F
1   F F T F F F
2     F F T F F
3       F F T T
4         F F F
5           F F
6             T
*/
	int exp0[] = {1,0,0,1,0,0,0};
	int exp1[] =   {0,0,1,0,0,0};
	int exp2[] =     {0,0,1,0,0};
	int exp3[] =       {0,0,1,1};
	int exp4[] =         {0,0,0};
	int exp5[] =           {0,0};
	int exp6[] =             {1};

	TEST_ASSERT_EQUAL_INT_ARRAY(exp0, &(min_dfa->merge[0][1]), 7);
	TEST_ASSERT_EQUAL_INT_ARRAY(exp1, &(min_dfa->merge[1][2]), 6);
	TEST_ASSERT_EQUAL_INT_ARRAY(exp2, &(min_dfa->merge[2][3]), 5);
	TEST_ASSERT_EQUAL_INT_ARRAY(exp3, &(min_dfa->merge[3][4]), 4);
	TEST_ASSERT_EQUAL_INT_ARRAY(exp4, &(min_dfa->merge[4][5]), 3);
	TEST_ASSERT_EQUAL_INT_ARRAY(exp5, &(min_dfa->merge[5][6]), 2);
	TEST_ASSERT_EQUAL_INT_ARRAY(exp6, &(min_dfa->merge[6][7]), 1);

	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);


	read_line(cc, "abc|[bx]*", 9);
	nfa = parse(cc);
	dfa = convert_nfa_to_dfa(nfa);
	min_dfa = init_minimal_dfa(dfa);

	TEST_ASSERT_EQUAL_INT(0, quotient(min_dfa, dfa));

/*
  1 2 3 4 5
0 F F F F F
1   F F F F
2     T F F
3       F F
4         F
*/
	int abx_exp0[] = {0,0,0,0,0};
	int abx_exp1[] =   {0,0,0,0};
	int abx_exp2[] =     {1,0,0};
	int abx_exp3[] =       {0,0};
	int abx_exp4[] =         {0};

	TEST_ASSERT_EQUAL_INT_ARRAY(abx_exp0, &(min_dfa->merge[0][1]), 5);
	TEST_ASSERT_EQUAL_INT_ARRAY(abx_exp1, &(min_dfa->merge[1][2]), 4);
	TEST_ASSERT_EQUAL_INT_ARRAY(abx_exp2, &(min_dfa->merge[2][3]), 3);
	TEST_ASSERT_EQUAL_INT_ARRAY(abx_exp3, &(min_dfa->merge[3][4]), 2);
	TEST_ASSERT_EQUAL_INT_ARRAY(abx_exp4, &(min_dfa->merge[4][5]), 1);

	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);


	read_line(cc, "for|[f-h]*", 10);
	nfa = parse(cc);
	dfa = convert_nfa_to_dfa(nfa);
	min_dfa = init_minimal_dfa(dfa);

	TEST_ASSERT_EQUAL_INT(0, quotient(min_dfa, dfa));

/*
  1 2 3 4 5 6
0 F F F F F F
1   F F F F F
2     T T F F
3       T F F
4         F F
5           F
*/
	int forfgh_exp0[] = {0,0,0,0,0,0};
	int forfgh_exp1[] =   {0,0,0,0,0};
	int forfgh_exp2[] =     {1,1,0,0};
	int forfgh_exp3[] =       {1,0,0};
	int forfgh_exp4[] =         {0,0};
	int forfgh_exp5[] =           {0};

	TEST_ASSERT_EQUAL_INT_ARRAY(forfgh_exp0, &(min_dfa->merge[0][1]), 6);
	TEST_ASSERT_EQUAL_INT_ARRAY(forfgh_exp1, &(min_dfa->merge[1][2]), 5);
	TEST_ASSERT_EQUAL_INT_ARRAY(forfgh_exp2, &(min_dfa->merge[2][3]), 4);
	TEST_ASSERT_EQUAL_INT_ARRAY(forfgh_exp3, &(min_dfa->merge[3][4]), 3);
	TEST_ASSERT_EQUAL_INT_ARRAY(forfgh_exp4, &(min_dfa->merge[4][5]), 2);
	TEST_ASSERT_EQUAL_INT_ARRAY(forfgh_exp5, &(min_dfa->merge[4][5]), 1);

	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);


	read_line(cc, "there|here", 10);
	nfa = parse(cc);
	dfa = convert_nfa_to_dfa(nfa);
	min_dfa = init_minimal_dfa(dfa);
	TEST_ASSERT_EQUAL_INT(0, quotient(min_dfa, dfa));
/*
equivalence classes:
{0}
{1,3}
{2}
{4,7}
{5,8}
{6,9}
  | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |
0 | F | F | F | F | F | F | F | F | F |
1 |   | F |   | F | F | F | F | F | F |
2 |   |   | F | F | F | F | F | F | F |
3 |   |   |   | F | F | F | F | F | F |
4 |   |   |   |   | F | F |   | F | F |
5 |   |   |   |   |   | F | F |   | F |
6 |   |   |   |   |   |   | F | F |   |
7 |   |   |   |   |   |   |   | F | F |
8 |   |   |   |   |   |   |   |   | F |
*/

	int there_exp0[] = {0,0,0,0,0,0,0,0,0};
	int there_exp1[] =   {0,1,0,0,0,0,0,0};
	int there_exp2[] =     {0,0,0,0,0,0,0};
	int there_exp3[] =       {0,0,0,0,0,0};
	int there_exp4[] =         {0,0,1,0,0};
	int there_exp5[] =           {0,0,1,0};
	int there_exp6[] =             {0,0,1};
	int there_exp7[] =               {0,0};
	int there_exp8[] =                 {0};

	TEST_ASSERT_EQUAL_INT_ARRAY(there_exp0, &(min_dfa->merge[0][1]), 9);
	TEST_ASSERT_EQUAL_INT_ARRAY(there_exp1, &(min_dfa->merge[1][2]), 8);
	TEST_ASSERT_EQUAL_INT_ARRAY(there_exp2, &(min_dfa->merge[2][3]), 7);
	TEST_ASSERT_EQUAL_INT_ARRAY(there_exp3, &(min_dfa->merge[3][4]), 6);
	TEST_ASSERT_EQUAL_INT_ARRAY(there_exp4, &(min_dfa->merge[4][5]), 5);
	TEST_ASSERT_EQUAL_INT_ARRAY(there_exp5, &(min_dfa->merge[5][6]), 4);
	TEST_ASSERT_EQUAL_INT_ARRAY(there_exp6, &(min_dfa->merge[6][7]), 3);
	TEST_ASSERT_EQUAL_INT_ARRAY(there_exp7, &(min_dfa->merge[7][8]), 2);
	TEST_ASSERT_EQUAL_INT_ARRAY(there_exp8, &(min_dfa->merge[8][9]), 1);

	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);
	destroy_cmpctrl(cc);
}

void test_construct_minimal_states(void)
{
	CmpCtrl *cc = init_cmpctrl();
	NFA *nfa;
	DFA *dfa;
	MinimalDFA *min_dfa;
	Iterator *it;
	int i;
	Set *curr;
	MinimalDFAState *currq;
	int n0 = 0;
	int n1 = 1;
	int n2 = 2;
	int n3 = 3;
	int n4 = 4;
	int n5 = 5;
	int n6 = 6;
	int n7 = 7;

	read_line(cc, "(ab|ac)*", 8);
	nfa = parse(cc);
	dfa = convert_nfa_to_dfa(nfa);
	min_dfa = init_minimal_dfa(dfa);
	quotient(min_dfa, dfa);
	TEST_ASSERT_NOT_NULL(construct_minimal_states(min_dfa, dfa));
	TEST_ASSERT_EQUAL_INT(2, min_dfa->size);
	TEST_ASSERT_EQUAL_INT(1, min_dfa->accepts->size);
/*
  1 2 3
0 F T T
1   F F
2     T
*/
	Set *abac_equ_class0 = init_set(compare_ints);
	set_insert(abac_equ_class0, &n0);
	set_insert(abac_equ_class0, &n2);
	set_insert(abac_equ_class0, &n3);
	Set *abac_equ_class1 = init_set(compare_ints);
	set_insert(abac_equ_class1, &n1);
	Set *abac_equ_classes[] = {abac_equ_class0, abac_equ_class1};

	i = 0;
	it = set_begin(min_dfa->mem_region);
	for (; it; advance_iter(&it), i++) {
		curr = (Set *)(it->element);
		currq = (MinimalDFAState *)(curr->id);
		TEST_ASSERT_TRUE(set_equals(abac_equ_classes[i], curr));
		// due to the nature of the minimal state construction algorithm
		// all equivalence classes are discovered in sorted order, and
		// the corresponding minimal state is created simultaneously
		// so state index can be checked using i
		TEST_ASSERT_EQUAL_INT(i, currq->index);
		destroy_set(abac_equ_classes[i]);
	}

	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);


	read_line(cc, "(0|(1(01*(00)*0)*1)*)*", 22);
	nfa = parse(cc);
	dfa = convert_nfa_to_dfa(nfa);
	min_dfa = init_minimal_dfa(dfa);
	quotient(min_dfa, dfa);
	TEST_ASSERT_NOT_NULL(construct_minimal_states(min_dfa, dfa));
	TEST_ASSERT_EQUAL_INT(3, min_dfa->size);
	TEST_ASSERT_EQUAL_INT(1, min_dfa->accepts->size);
/*
  1 2 3 4 5 6 7
0 T F F T F F F
1   F F T F F F
2     F F T F F
3       F F T T
4         F F F
5           F F
6             T
*/
	Set *modulo3_equ_class0 = init_set(compare_ints);
	set_insert(modulo3_equ_class0, &n0);
	set_insert(modulo3_equ_class0, &n1);
	set_insert(modulo3_equ_class0, &n4);
	Set *modulo3_equ_class2 = init_set(compare_ints);
	set_insert(modulo3_equ_class2, &n2);
	set_insert(modulo3_equ_class2, &n5);
	Set *modulo3_equ_class3 = init_set(compare_ints);
	set_insert(modulo3_equ_class3, &n3);
	set_insert(modulo3_equ_class3, &n6);
	set_insert(modulo3_equ_class3, &n7);
	Set *modulo3_equ_classes[] = {modulo3_equ_class0, modulo3_equ_class2,
	                              modulo3_equ_class3};

	i = 0;
	it = set_begin(min_dfa->mem_region);
	for (; it; advance_iter(&it), i++) {
		curr = (Set *)(it->element);
		currq = (MinimalDFAState *)(curr->id);
		TEST_ASSERT_TRUE(set_equals(modulo3_equ_classes[i], curr));
		TEST_ASSERT_EQUAL_INT(i, currq->index);
		destroy_set(modulo3_equ_classes[i]);
	}

	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);


	read_line(cc, "abc|[bx]*", 9);
	nfa = parse(cc);
	dfa = convert_nfa_to_dfa(nfa);
	min_dfa = init_minimal_dfa(dfa);
	quotient(min_dfa, dfa);
	TEST_ASSERT_NOT_NULL(construct_minimal_states(min_dfa, dfa));
	TEST_ASSERT_EQUAL_INT(5, min_dfa->size);
	TEST_ASSERT_EQUAL_INT(3, min_dfa->accepts->size);
/*
  1 2 3 4 5
0 F F F F F
1   F F F F
2     T F F
3       F F
4         F
5
*/
	Set *abcx_equ_class0 = init_set(compare_ints);
	set_insert(abcx_equ_class0, &n0);
	Set *abcx_equ_class1 = init_set(compare_ints);
	set_insert(abcx_equ_class1, &n1);
	Set *abcx_equ_class2 = init_set(compare_ints);
	set_insert(abcx_equ_class2, &n2);
	set_insert(abcx_equ_class2, &n3);
	Set *abcx_equ_class4 = init_set(compare_ints);
	set_insert(abcx_equ_class4, &n4);
	Set *abcx_equ_class5 = init_set(compare_ints);
	set_insert(abcx_equ_class5, &n5);
	Set *abcx_equ_classes[] = {abcx_equ_class0, abcx_equ_class1,
	                           abcx_equ_class2, abcx_equ_class4,
	                           abcx_equ_class5};

	i = 0;
	it = set_begin(min_dfa->mem_region);
	for (; it; advance_iter(&it), i++) {
		curr = (Set *)(it->element);
		currq = (MinimalDFAState *)(curr->id);
		TEST_ASSERT_TRUE(set_equals(abcx_equ_classes[i], curr));
		TEST_ASSERT_EQUAL_INT(i, currq->index);
		destroy_set(abcx_equ_classes[i]);
	}

	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);


	read_line(cc, "for|[f-h]*", 10);
	nfa = parse(cc);
	dfa = convert_nfa_to_dfa(nfa);
	min_dfa = init_minimal_dfa(dfa);
	quotient(min_dfa, dfa);
	TEST_ASSERT_NOT_NULL(construct_minimal_states(min_dfa, dfa));
/*
  1 2 3 4 5 6
0 F F F F F F
1   F F F F F
2     T T F F
3       T F F
4         F F
5           F
*/
	Set *forfgh_equ_class0 = init_set(compare_ints);
	set_insert(forfgh_equ_class0, &n0);
	Set *forfgh_equ_class1 = init_set(compare_ints);
	set_insert(forfgh_equ_class1, &n1);
	Set *forfgh_equ_class2 = init_set(compare_ints);
	set_insert(forfgh_equ_class2, &n2);
	set_insert(forfgh_equ_class2, &n3);
	set_insert(forfgh_equ_class2, &n4);
	Set *forfgh_equ_class5 = init_set(compare_ints);
	set_insert(forfgh_equ_class5, &n5);
	Set *forfgh_equ_class6 = init_set(compare_ints);
	set_insert(forfgh_equ_class6, &n6);
	Set *forfgh_equ_classes[] = {forfgh_equ_class0, forfgh_equ_class1,
	                             forfgh_equ_class2, forfgh_equ_class5,
	                             forfgh_equ_class6};

	i = 0;
	it = set_begin(min_dfa->mem_region);
	for (; it; advance_iter(&it), i++) {
		curr = (Set *)(it->element);
		currq = (MinimalDFAState *)(curr->id);
		TEST_ASSERT_TRUE(set_equals(forfgh_equ_classes[i], curr));
		TEST_ASSERT_EQUAL_INT(i, currq->index);
		destroy_set(forfgh_equ_classes[i]);
	}

	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);
	destroy_cmpctrl(cc);
}

void test_construct_transition_table(void)
{
	CmpCtrl *cc = init_cmpctrl();
	NFA *nfa;
	DFA *dfa;
	MinimalDFA *min_dfa;
	U64 exp;

	read_line(cc, "(ab|ac)*", 8);
	nfa = parse(cc);
	dfa = convert_nfa_to_dfa(nfa);
	min_dfa = init_minimal_dfa(dfa);
	quotient(min_dfa, dfa);
	construct_minimal_states(min_dfa, dfa);
	construct_transition_table(min_dfa, dfa);
/*
  0   | 1
0     | a
1 b,c |
*/
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[0][0][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[0][0][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[0][1][ASCII0_63]);
	exp = 1ULL << ('a'-64);
	TEST_ASSERT_EQUAL_UINT64(exp, min_dfa->delta[0][1][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[1][0][ASCII0_63]);
	exp = 1ULL << ('b'-64);
	exp |= 1ULL << ('c'-64);
	TEST_ASSERT_EQUAL_UINT64(exp, min_dfa->delta[1][0][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[1][1][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[1][1][ASCII64_127]);

	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);


	read_line(cc, "(0|(1(01*(00)*0)*1)*)*", 22);
	nfa = parse(cc);
	dfa = convert_nfa_to_dfa(nfa);
	min_dfa = init_minimal_dfa(dfa);
	quotient(min_dfa, dfa);
	construct_minimal_states(min_dfa, dfa);
	construct_transition_table(min_dfa, dfa);
/*
  0 | 1 | 2
0 0 | 1 |
1 1 |   | 0
2   | 0 | 1
*/
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[0][0][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[0][1][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[0][2][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[0][2][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[1][0][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[1][1][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[1][1][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[1][2][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[2][0][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[2][0][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[2][1][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[2][2][ASCII64_127]);

	exp = 1ULL << '1';
	TEST_ASSERT_EQUAL_UINT64(exp, min_dfa->delta[0][1][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(exp, min_dfa->delta[1][0][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(exp, min_dfa->delta[2][2][ASCII0_63]);
	exp = 1ULL << '0';
	TEST_ASSERT_EQUAL_UINT64(exp, min_dfa->delta[0][0][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(exp, min_dfa->delta[1][2][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(exp, min_dfa->delta[2][1][ASCII0_63]);

	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);


	read_line(cc, "abc|[bx]*", 9);
	nfa = parse(cc);
	dfa = convert_nfa_to_dfa(nfa);
	min_dfa = init_minimal_dfa(dfa);
	quotient(min_dfa, dfa);
	construct_minimal_states(min_dfa, dfa);
	construct_transition_table(min_dfa, dfa);

/*
  0 | 1 |  2  | 3 | 4
0   | a | b,x |   |
1   |   |     | b |
2   |   | b,x |   |
3   |   |     |   | c
4
*/
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[0][1][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[0][2][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[1][3][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[2][2][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[3][4][ASCII0_63]);

	exp = 1ULL << ('a'-64);
	TEST_ASSERT_EQUAL_UINT64(exp, min_dfa->delta[0][1][ASCII64_127]);
	exp = 1ULL << ('b'-64);
	TEST_ASSERT_EQUAL_UINT64(exp, min_dfa->delta[1][3][ASCII64_127]);
	exp |= 1ULL << ('x'-64);
	TEST_ASSERT_EQUAL_UINT64(exp, min_dfa->delta[0][2][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(exp, min_dfa->delta[2][2][ASCII64_127]);
	exp = 1ULL << ('c'-64);
	TEST_ASSERT_EQUAL_UINT64(exp, min_dfa->delta[3][4][ASCII64_127]);

	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[0][0][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[0][0][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[0][3][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[0][3][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[0][4][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[0][4][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[1][0][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[1][0][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[1][1][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[1][1][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[1][2][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[1][2][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[1][4][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[1][4][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[2][0][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[2][0][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[2][1][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[2][1][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[2][3][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[2][3][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[2][4][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[2][4][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[3][0][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[3][0][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[3][1][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[3][1][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[3][2][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[3][2][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[3][3][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[3][3][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[4][0][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[4][0][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[4][1][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[4][1][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[4][2][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[4][2][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[4][3][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[4][3][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[4][4][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[4][4][ASCII64_127]);

	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);


	read_line(cc, "for|[f-h]*", 10);
	nfa = parse(cc);
	dfa = convert_nfa_to_dfa(nfa);
	min_dfa = init_minimal_dfa(dfa);
	quotient(min_dfa, dfa);
	construct_minimal_states(min_dfa, dfa);
	construct_transition_table(min_dfa, dfa);
/*
  0 | 1 |   2   | 3 | 4
0   | f |  g,h  |   |
1   |   | f,g,h | o |
2   |   | f,g,h |   |
3   |   |       |   | r
4
*/
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[0][1][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[0][2][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[1][2][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[1][3][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[2][2][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[3][4][ASCII0_63]);

	exp = 1ULL << ('f'-64);
	TEST_ASSERT_EQUAL_UINT64(exp, min_dfa->delta[0][1][ASCII64_127]);
	exp = 1ULL << ('g'-64);
	exp |= 1ULL << ('h'-64);
	TEST_ASSERT_EQUAL_UINT64(exp, min_dfa->delta[0][2][ASCII64_127]);
	exp |= 1ULL << ('f'-64);
	TEST_ASSERT_EQUAL_UINT64(exp, min_dfa->delta[1][2][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(exp, min_dfa->delta[2][2][ASCII64_127]);
	exp = 1ULL << ('o'-64);
	TEST_ASSERT_EQUAL_UINT64(exp, min_dfa->delta[1][3][ASCII64_127]);
	exp = 1ULL << ('r'-64);
	TEST_ASSERT_EQUAL_UINT64(exp, min_dfa->delta[3][4][ASCII64_127]);

	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[0][0][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[0][0][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[0][3][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[0][3][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[0][4][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[0][4][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[1][0][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[1][0][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[1][1][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[1][1][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[1][4][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[1][4][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[2][0][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[2][0][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[2][1][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[2][1][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[2][3][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[2][3][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[2][4][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[2][4][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[3][0][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[3][0][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[3][1][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[3][1][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[3][2][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[3][2][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[3][3][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[3][3][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[4][0][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[4][0][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[4][1][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[4][1][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[4][2][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[4][2][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[4][3][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[4][3][ASCII64_127]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[4][4][ASCII0_63]);
	TEST_ASSERT_EQUAL_UINT64(0, min_dfa->delta[4][4][ASCII64_127]);

	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);
	destroy_cmpctrl(cc);
}

void test_minimize_and_gen_graphviz(void)
{
	CmpCtrl *cc = init_cmpctrl();
	NFA *nfa;
	DFA *dfa;
	MinimalDFA *min_dfa;

	read_line(cc, "(0|(1(01*(00)*0)*1)*)*", 22);
	nfa = parse(cc);
	dfa = convert_nfa_to_dfa(nfa);

	min_dfa = minimize(dfa);
	TEST_ASSERT_NOT_NULL(min_dfa);
	TEST_ASSERT_EQUAL_INT(0, gen_minimal_dfa_graphviz(min_dfa, "dots/modulo3.dot"));

	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);


	read_line(cc, "(ab|ac)*", 8);
	nfa = parse(cc);
	dfa = convert_nfa_to_dfa(nfa);

	min_dfa = minimize(dfa);
	TEST_ASSERT_NOT_NULL(min_dfa);
	TEST_ASSERT_EQUAL_INT(0, gen_minimal_dfa_graphviz(min_dfa, "dots/abac.dot"));

	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);


	read_line(cc, "for|[f-h]*", 10);
	nfa = parse(cc);
	dfa = convert_nfa_to_dfa(nfa);

	min_dfa = minimize(dfa);
	TEST_ASSERT_NOT_NULL(min_dfa);
	TEST_ASSERT_EQUAL_INT(0, gen_minimal_dfa_graphviz(min_dfa, "dots/forfgh.dot"));

	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);


	read_line(cc, "for|[fh]*", 9);
	nfa = parse(cc);
	dfa = convert_nfa_to_dfa(nfa);

	min_dfa = minimize(dfa);
	TEST_ASSERT_NOT_NULL(min_dfa);
	TEST_ASSERT_EQUAL_INT(0, gen_minimal_dfa_graphviz(min_dfa, "dots/forfh.dot"));

	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);


	read_line(cc, "[A-Za-z_][A-Za-z0-9_]*", 22);
	nfa = parse(cc);
	dfa = convert_nfa_to_dfa(nfa);

	min_dfa = minimize(dfa);
	TEST_ASSERT_NOT_NULL(min_dfa);
	TEST_ASSERT_EQUAL_INT(0, gen_minimal_dfa_graphviz(min_dfa, "dots/C_ident.dot"));

	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);


	read_line(cc, "there|here", 10);
	nfa = parse(cc);
	dfa = convert_nfa_to_dfa(nfa);

	min_dfa = minimize(dfa);
	TEST_ASSERT_NOT_NULL(min_dfa);
	TEST_ASSERT_EQUAL_INT(0, gen_minimal_dfa_graphviz(min_dfa, "dots/there.dot"));

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
	RUN_TEST(test_quotient);
	RUN_TEST(test_construct_minimal_states);
	RUN_TEST(test_construct_transition_table);
	RUN_TEST(test_minimize_and_gen_graphviz);

	return UNITY_END();
}