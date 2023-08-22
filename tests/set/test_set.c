#include <stdlib.h>

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
	TEST_ASSERT_EQUAL_PTR(node->data, &num);
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

	TEST_ASSERT_EQUAL_INT(0, set_insert(set, &n1));
	TEST_ASSERT_EQUAL_PTR(&n1, set->head->data);
	TEST_ASSERT_EQUAL_PTR(&n1, set->tail->data);
	TEST_ASSERT_NULL(set->head->next);
	TEST_ASSERT_NULL(set->tail->next);
	TEST_ASSERT_EQUAL_INT(1, set->size);

	// insert before head
	TEST_ASSERT_EQUAL_INT(0, set_insert(set, &n0));
	TEST_ASSERT_EQUAL_PTR(&n0, set->head->data);
	TEST_ASSERT_EQUAL_PTR(&n1, set->tail->data);
	TEST_ASSERT_EQUAL_PTR(set->head->next, set->tail);
	TEST_ASSERT_EQUAL_INT(2, set->size);

	// insert new tail
	TEST_ASSERT_EQUAL_INT(0, set_insert(set, &n3));
	TEST_ASSERT_EQUAL_PTR(&n0, set->head->data);
	TEST_ASSERT_EQUAL_PTR(&n3, set->tail->data);
	TEST_ASSERT_EQUAL_INT(3, set->size);

	// insert in the middle
	TEST_ASSERT_EQUAL_INT(0, set_insert(set, &n2));
	TEST_ASSERT_EQUAL_PTR(&n0, set->head->data);
	TEST_ASSERT_EQUAL_PTR(&n1, set->head->next->data);
	TEST_ASSERT_EQUAL_PTR(&n2, set->head->next->next->data);
	TEST_ASSERT_EQUAL_PTR(&n3, set->head->next->next->next->data);
	TEST_ASSERT_EQUAL_PTR(&n3, set->tail->data);
	TEST_ASSERT_EQUAL_INT(4, set->size);

	// insert duplicates
	TEST_ASSERT_EQUAL_INT(0, set_insert(set, &n3_again));
	TEST_ASSERT_EQUAL_PTR(&n3, set->tail->data);
	TEST_ASSERT_EQUAL_INT(4, set->size);
	TEST_ASSERT_EQUAL_INT(0, set_insert(set, &n1_again));
	TEST_ASSERT_EQUAL_PTR(&n1, set->head->next->data);
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
	TEST_ASSERT_NULL(set_find(set, &n99));
	TEST_ASSERT_NULL(set_find(set, &n123));

	destroy_set(set);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_init_set);
	RUN_TEST(test_init_node);
	RUN_TEST(test_set_insert);
	RUN_TEST(test_set_find);

	return UNITY_END();
}