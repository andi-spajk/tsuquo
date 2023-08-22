#include <stdlib.h>

#include "../../unity/unity.h"
#include "set.h"

void setUp(void) {}
void tearDown(void) {}

int compare_int(const void *num1, const void *num2)
{
	return *(int *)num1 - *(int *)num2;
}

void test_init_set(void)
{
	Set *set = init_set(compare_int);
	TEST_ASSERT_NULL(set->id);
	TEST_ASSERT_EQUAL_PTR(compare_int, set->compare);
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

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_init_set);
	RUN_TEST(test_init_node);

	return UNITY_END();
}