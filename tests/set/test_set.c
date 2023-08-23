#include "../../unity/unity.h"
#include "set.h"

void setUp(void) {}
void tearDown(void) {}

int compare_ints(const void *num1, const void *num2)
{
	return *(int *)num1 - *(int *)num2;
}

void test_init_set(void)
{
	Set *set = init_set(compare_ints);
	TEST_ASSERT_NULL(set->id);
	TEST_ASSERT_EQUAL_PTR(compare_ints, set->compare);
	TEST_ASSERT_NULL(set->head);
	TEST_ASSERT_NULL(set->tail);
	TEST_ASSERT_EQUAL_INT(0, set->size);

	TEST_ASSERT_TRUE(set_is_empty(set));
	destroy_set(set);
}

void test_init_node(void)
{
	int num = 123;
	Node *node = init_node(&num);
	TEST_ASSERT_EQUAL_PTR(node->element, &num);
	TEST_ASSERT_NULL(node->next);
	destroy_node(node);
}

void test_set_insert(void)
{
	Set *set = init_set(compare_ints);
	int n0 = 0;
	int n1 = 1;
	int n2 = 2;
	int n3 = 3;
	int n3_again = 3;
	int n1_again = 1;

	TEST_ASSERT_EQUAL_INT(INSERT_SUCCESS, set_insert(set, &n1));
	TEST_ASSERT_EQUAL_PTR(&n1, set->head->element);
	TEST_ASSERT_EQUAL_PTR(&n1, set->tail->element);
	TEST_ASSERT_NULL(set->head->next);
	TEST_ASSERT_NULL(set->tail->next);
	TEST_ASSERT_EQUAL_INT(1, set->size);
	TEST_ASSERT_FALSE(set_is_empty(set));

	// insert before head
	TEST_ASSERT_EQUAL_INT(INSERT_SUCCESS, set_insert(set, &n0));
	TEST_ASSERT_EQUAL_PTR(&n0, set->head->element);
	TEST_ASSERT_EQUAL_PTR(&n1, set->tail->element);
	TEST_ASSERT_EQUAL_PTR(set->head->next, set->tail);
	TEST_ASSERT_EQUAL_INT(2, set->size);

	// insert new tail
	TEST_ASSERT_EQUAL_INT(INSERT_SUCCESS, set_insert(set, &n3));
	TEST_ASSERT_EQUAL_PTR(&n0, set->head->element);
	TEST_ASSERT_EQUAL_PTR(&n3, set->tail->element);
	TEST_ASSERT_EQUAL_INT(3, set->size);

	// insert in the middle
	TEST_ASSERT_EQUAL_INT(INSERT_SUCCESS, set_insert(set, &n2));
	TEST_ASSERT_EQUAL_PTR(&n0, set->head->element);
	TEST_ASSERT_EQUAL_PTR(&n1, set->head->next->element);
	TEST_ASSERT_EQUAL_PTR(&n2, set->head->next->next->element);
	TEST_ASSERT_EQUAL_PTR(&n3, set->head->next->next->next->element);
	TEST_ASSERT_EQUAL_PTR(&n3, set->tail->element);
	TEST_ASSERT_EQUAL_INT(4, set->size);

	// insert duplicates
	TEST_ASSERT_EQUAL_INT(INSERT_DUPLICATE, set_insert(set, &n3_again));
	TEST_ASSERT_EQUAL_PTR(&n3, set->tail->element);
	TEST_ASSERT_EQUAL_INT(4, set->size);
	TEST_ASSERT_EQUAL_INT(INSERT_DUPLICATE, set_insert(set, &n1_again));
	TEST_ASSERT_EQUAL_PTR(&n1, set->head->next->element);
	TEST_ASSERT_EQUAL_INT(4, set->size);

	destroy_set(set);
}

void test_set_find(void)
{
	Set *set = init_set(compare_ints);
	int nneg1 = -1;
	int n1 = 1;
	int n2 = 2;
	int n3 = 3;
	int n3_another = 3;
	int n99 = 99;
	int n123 = 123;
	set_insert(set, &nneg1);
	set_insert(set, &n1);
	set_insert(set, &n2);
	set_insert(set, &n3);

	TEST_ASSERT_EQUAL_PTR(&nneg1, set_find(set, &nneg1));
	TEST_ASSERT_EQUAL_PTR(&n1, set_find(set, &n1));
	TEST_ASSERT_EQUAL_PTR(&n2, set_find(set, &n2));
	TEST_ASSERT_EQUAL_PTR(&n3, set_find(set, &n3));
	TEST_ASSERT_EQUAL_PTR(&n3, set_find(set, &n3_another));
	TEST_ASSERT_NULL(set_find(set, &n99));
	TEST_ASSERT_NULL(set_find(set, &n123));

	destroy_set(set);
}

void test_set_decapitate(void)
{
	Set *set = init_set(compare_ints);
	TEST_ASSERT_NULL(set_decapitate(set));

	int n0 = 0;
	int n1 = 1;
	int n2 = 2;
	int n3 = 3;

	set_insert(set, &n0);
	set_insert(set, &n1);
	set_insert(set, &n2);
	set_insert(set, &n3);
	TEST_ASSERT_EQUAL_INT(4, set->size);
	TEST_ASSERT_FALSE(set_is_empty(set));

	TEST_ASSERT_EQUAL_PTR(&n0, set->head->element);
	TEST_ASSERT_EQUAL_PTR(&n3, set->tail->element);
	TEST_ASSERT_EQUAL_PTR(&n0, set_decapitate(set));
	TEST_ASSERT_EQUAL_INT(3, set->size);
	TEST_ASSERT_FALSE(set_is_empty(set));

	TEST_ASSERT_EQUAL_PTR(&n1, set->head->element);
	TEST_ASSERT_EQUAL_PTR(&n3, set->tail->element);
	TEST_ASSERT_EQUAL_PTR(&n1, set_decapitate(set));
	TEST_ASSERT_EQUAL_INT(2, set->size);
	TEST_ASSERT_FALSE(set_is_empty(set));

	TEST_ASSERT_EQUAL_PTR(&n2, set->head->element);
	TEST_ASSERT_EQUAL_PTR(&n3, set->tail->element);
	TEST_ASSERT_EQUAL_PTR(&n2, set_decapitate(set));
	TEST_ASSERT_EQUAL_INT(1, set->size);
	TEST_ASSERT_FALSE(set_is_empty(set));

	TEST_ASSERT_EQUAL_PTR(&n3, set->head->element);
	TEST_ASSERT_EQUAL_PTR(&n3, set->tail->element);
	TEST_ASSERT_EQUAL_PTR(&n3, set_decapitate(set));
	TEST_ASSERT_EQUAL_INT(0, set->size);
	TEST_ASSERT_TRUE(set_is_empty(set));
	TEST_ASSERT_NULL(set->head);
	TEST_ASSERT_NULL(set->tail);

	TEST_ASSERT_NULL(set_decapitate(set));
	TEST_ASSERT_EQUAL_INT(0, set->size);
	TEST_ASSERT_TRUE(set_is_empty(set));
	TEST_ASSERT_NULL(set->head);
	TEST_ASSERT_NULL(set->tail);

	destroy_set(set);
}

void test_set_equals(void)
{
	Set *s1 = init_set(compare_ints);
	Set *s2 = init_set(compare_ints);
	TEST_ASSERT_TRUE(set_equals(s1, s2));

	int n0_1 = 0;
	int n1_1 = 1;
	int n2_1 = 2;
	int n3_1 = 3;
	int n0_2 = 0;
	int n1_2 = 1;
	int n2_2 = 2;
	int n3_2 = 3;

	set_insert(s1, &n0_1);
	TEST_ASSERT_FALSE(set_equals(s1, s2));
	set_insert(s1, &n1_1);
	set_insert(s1, &n2_1);
	set_insert(s1, &n3_1);

	set_insert(s2, &n0_2);
	set_insert(s2, &n1_2);
	set_insert(s2, &n2_2);
	set_insert(s2, &n3_2);

	TEST_ASSERT_TRUE(set_equals(s1, s2));
	TEST_ASSERT_TRUE(set_equals(s2, s1));

	set_decapitate(s2);

	TEST_ASSERT_FALSE(set_equals(s1, s2));

	destroy_set(s1);
	destroy_set(s2);
}

void test_set_union(void)
{

	Set *s1 = init_set(compare_ints);
	Set *s2 = init_set(compare_ints);

	int n0 = 0;
	int n1 = 1;
	int n2 = 2;
	int n99 = 99;

	set_insert(s1, &n0);
	//  s1 |= s2
	// {0} |= {}
	TEST_ASSERT_EQUAL_PTR(s1, set_union(s1, s2));
	TEST_ASSERT_EQUAL_PTR(&n0, s1->head->element);
	TEST_ASSERT_NULL(s1->head->next);
	TEST_ASSERT_EQUAL_PTR(&n0, s1->tail->element);
	TEST_ASSERT_NULL(s1->tail->next);
	TEST_ASSERT_EQUAL_INT(1, s1->size);
	TEST_ASSERT_EQUAL_INT(0, s2->size);

	TEST_ASSERT_NULL(s2->head);
	TEST_ASSERT_NULL(s2->tail);

	set_insert(s1, &n2);
	// s2 |=   s1
	// {} |= {0, 2}
	TEST_ASSERT_EQUAL_INT(2, s1->size);
	TEST_ASSERT_EQUAL_INT(0, s2->size);
	TEST_ASSERT_EQUAL_PTR(s2, set_union(s2, s1));
	TEST_ASSERT_EQUAL_INT(2, s1->size);
	TEST_ASSERT_EQUAL_INT(2, s2->size);
	TEST_ASSERT_EQUAL_PTR(&n0, s2->head->element);
	TEST_ASSERT_EQUAL_PTR(&n2, s2->tail->element);
	TEST_ASSERT_TRUE(set_equals(s2, s1));

	set_insert(s1, &n1);
	set_insert(s1, &n99);
	//   s2   |=      s1
	// {0, 2} |= {0, 1, 2, 99}
	TEST_ASSERT_EQUAL_PTR(s2, set_union(s2, s1));
	TEST_ASSERT_TRUE(set_equals(s2, s1));
	TEST_ASSERT_EQUAL_PTR(&n0, s2->head->element);
	TEST_ASSERT_EQUAL_PTR(&n99, s2->tail->element);
	TEST_ASSERT_EQUAL_INT(4, s2->size);

	// s1 |= s2
	// no effect
	TEST_ASSERT_EQUAL_PTR(s1, set_union(s1, s2));
	TEST_ASSERT_TRUE(set_equals(s1, s2));
	TEST_ASSERT_EQUAL_INT(4, s1->size);
	TEST_ASSERT_EQUAL_INT(4, s2->size);

	destroy_set(s1);
	destroy_set(s2);
}

void test_iterators(void)
{
	Set *s = init_set(compare_ints);

	int n4 = 4;
	int n3 = 3;
	int n2 = 2;
	int n1 = 1;

	set_insert(s, &n4);
	set_insert(s, &n3);
	set_insert(s, &n2);
	set_insert(s, &n1);

	int i = 1;
	Iterator *it = set_begin(s);
	for (; it; advance_iter(&it)) {
		TEST_ASSERT_EQUAL_INT(i, *(int *)it->element);
		i++;
	}
	TEST_ASSERT_NULL(it);

	destroy_set(s);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_init_set);
	RUN_TEST(test_init_node);
	RUN_TEST(test_set_insert);
	RUN_TEST(test_set_find);
	RUN_TEST(test_set_decapitate);
	RUN_TEST(test_set_equals);
	RUN_TEST(test_set_union);
	RUN_TEST(test_iterators);

	return UNITY_END();
}