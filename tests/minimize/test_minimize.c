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

	destroy_nfa_and_states(nfa);
	destroy_dfa(dfa);
	destroy_minimal_dfa(min_dfa);

	read_line(cc, "@*", 2);
	nfa = parse(cc);
	index_states(nfa);
	dfa = convert_nfa_to_dfa(nfa);
	min_dfa = init_minimal_dfa(dfa);

	int just_one_row[] = {1};
	TEST_ASSERT_EQUAL_INT_ARRAY(just_one_row, &(min_dfa->merge[0][1]), 1);

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
	index_states(nfa);
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
	index_states(nfa);
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
	index_states(nfa);
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
	index_states(nfa);
	dfa = convert_nfa_to_dfa(nfa);
	min_dfa = init_minimal_dfa(dfa);

	TEST_ASSERT_EQUAL_INT(0, quotient(min_dfa, dfa));

/*
  1 2 3 4 5
0 F F F F F
1   F F F F
2       F F
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
	index_states(nfa);
	dfa = convert_nfa_to_dfa(nfa);
	min_dfa = init_minimal_dfa(dfa);

	TEST_ASSERT_EQUAL_INT(0, quotient(min_dfa, dfa));

/*
  1 2 3 4 5 6
0 F F F F F F
1   F F F F F
2         F F
3         F F
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
	destroy_cmpctrl(cc);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_inits);
	RUN_TEST(test_distinguishable);
	RUN_TEST(test_quotient);

	return UNITY_END();
}