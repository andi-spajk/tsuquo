#include "../../unity/unity.h"
#include "common.h"
#include "control.h"

#define EXAMPLE1_LEN 7
#define EXAMPLE2_LEN 210

void setUp(void) {}
void tearDown(void) {}

void test_init_cmpctrl(void)
{
	CmpCtrl *cc = init_cmpctrl();
	TEST_ASSERT_EQUAL_UINT8(0, cc->token);
	TEST_ASSERT_NULL(cc->buffer);
	TEST_ASSERT_EQUAL_INT(0, cc->pos);
	TEST_ASSERT_EQUAL_INT(0, cc->buffer_len);
	TEST_ASSERT_EQUAL_INT(0, cc->flags);
	destroy_cmpctrl(cc);
}

void test_read_file(void)
{
	CmpCtrl *cc = init_cmpctrl();
	TEST_ASSERT_EQUAL_INT(0, read_file(cc, "../example.txt"));
	TEST_ASSERT_EQUAL_INT(EXAMPLE1_LEN, cc->buffer_len);
	TEST_ASSERT_EQUAL_MEMORY("a(b|c)*", cc->buffer, cc->buffer_len);
	TEST_ASSERT_EQUAL_INT(0, read_file(cc, "../example2.txt"));
	TEST_ASSERT_EQUAL_INT(EXAMPLE2_LEN, cc->buffer_len);
	TEST_ASSERT_EQUAL_INT(0, read_file(cc, "../empty.txt"));
	TEST_ASSERT_EQUAL_INT(0, cc->buffer_len);
	destroy_cmpctrl(cc);

	// read_file() twice, ignoring first call
	CmpCtrl *cc2 = init_cmpctrl();
	TEST_ASSERT_EQUAL_INT(0, read_file(cc2, "../example.txt"));
	TEST_ASSERT_EQUAL_INT(0, read_file(cc2, "../example2.txt"));
	TEST_ASSERT_EQUAL_INT(EXAMPLE2_LEN, cc2->buffer_len);
	destroy_cmpctrl(cc2);
}

void test_read_line(void)
{
	CmpCtrl *cc = init_cmpctrl();

	// declare with [] so sizeof(str) works intuitively
	const char test_line1[] = "abc";
	const char test_line2[] = "\\\\";
	const char test_line3[] = "x(y|z)*";
	const char test_line4[] = "OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO";

	TEST_ASSERT_EQUAL_INT(0, read_line(cc, test_line1, sizeof(test_line1)-1));
	TEST_ASSERT_EQUAL_INT(0, cc->pos);
	TEST_ASSERT_EQUAL_INT(3, cc->buffer_len);
	TEST_ASSERT_EQUAL_INT8_ARRAY(test_line1, cc->buffer, cc->buffer_len);
	TEST_ASSERT_EQUAL_INT(0, read_line(cc, test_line2, sizeof(test_line2)-1));
	TEST_ASSERT_EQUAL_INT(0, cc->pos);
	TEST_ASSERT_EQUAL_INT(2, cc->buffer_len);
	TEST_ASSERT_EQUAL_INT8_ARRAY(test_line2, cc->buffer, cc->buffer_len);
	TEST_ASSERT_EQUAL_INT(0, read_line(cc, test_line3, sizeof(test_line3)-1));
	TEST_ASSERT_EQUAL_INT(0, cc->pos);
	TEST_ASSERT_EQUAL_INT(7, cc->buffer_len);
	TEST_ASSERT_EQUAL_INT8_ARRAY(test_line3, cc->buffer, cc->buffer_len);
	TEST_ASSERT_EQUAL_INT(0, read_line(cc, test_line4, sizeof(test_line4)-1));
	TEST_ASSERT_EQUAL_INT(0, cc->pos);
	TEST_ASSERT_EQUAL_INT(46, cc->buffer_len);
	TEST_ASSERT_EQUAL_INT8_ARRAY(test_line4, cc->buffer, cc->buffer_len);

	destroy_cmpctrl(cc);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_init_cmpctrl);
	RUN_TEST(test_read_file);
	RUN_TEST(test_read_line);

	return UNITY_END();
}