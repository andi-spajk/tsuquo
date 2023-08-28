#include "../../unity/unity.h"
#include "common.h"
#include "control.h"
#include "lexer.h"

#define EXAMPLE1_LEN 7
#define EXAMPLE2_LEN 210

void setUp(void) {}
void tearDown(void) {}

void test_get_char(void)
{
	U8 ch;
	CmpCtrl *cc = init_cmpctrl();
	TEST_ASSERT_EQUAL_INT(0, read_file(cc, "../example.txt"));
	char *example = "a(b|c)*";
	for (int i = 0; i < EXAMPLE1_LEN; i++) {
		ch = get_char(cc);
		TEST_ASSERT_EQUAL_INT(i+1, cc->pos);
		TEST_ASSERT_EQUAL_UINT8(example[i], ch);
	}
	ch = get_char(cc);
	TEST_ASSERT_EQUAL_INT(EXAMPLE1_LEN, cc->pos);
	TEST_ASSERT_EQUAL_UINT8(TK_EOF, ch);
	// get_char should continue to return TK_EOF
	ch = get_char(cc);
	TEST_ASSERT_EQUAL_INT(EXAMPLE1_LEN, cc->pos);
	TEST_ASSERT_EQUAL_UINT8(TK_EOF, ch);
	destroy_cmpctrl(cc);
}

void test_lex(void)
{
	CmpCtrl *cc = init_cmpctrl();
	TEST_ASSERT_EQUAL_INT(0, read_file(cc, "../example.txt"));
	TEST_ASSERT_EQUAL_UINT8('a', lex(cc));
	TEST_ASSERT_EQUAL_UINT8('a', cc->token);
	cc->flags |= CC_DISABLE_INSTEAD_FOUND;
	print_error(cc, "blblblblbl");
	TEST_ASSERT_EQUAL_UINT8(TK_LPAREN, lex(cc));
	TEST_ASSERT_EQUAL_UINT8(TK_LPAREN, cc->token);
	TEST_ASSERT_EQUAL_UINT8('b', lex(cc));
	TEST_ASSERT_EQUAL_UINT8('b', cc->token);
	TEST_ASSERT_EQUAL_UINT8(TK_PIPE, lex(cc));
	TEST_ASSERT_EQUAL_UINT8(TK_PIPE, cc->token);
	TEST_ASSERT_EQUAL_UINT8('c', lex(cc));
	TEST_ASSERT_EQUAL_UINT8('c', cc->token);
	TEST_ASSERT_EQUAL_UINT8(TK_RPAREN, lex(cc));
	TEST_ASSERT_EQUAL_UINT8(TK_RPAREN, cc->token);
	TEST_ASSERT_EQUAL_UINT8(TK_STAR, lex(cc));
	TEST_ASSERT_EQUAL_UINT8(TK_STAR, cc->token);
	cc->flags = 0 | CC_DISABLE_LINE_PRINT;
	print_error(cc, "fake error, expected '(' or smth lol");
	TEST_ASSERT_EQUAL_UINT8(TK_EOF, lex(cc));
	TEST_ASSERT_EQUAL_UINT8(TK_EOF, cc->token);

	TEST_ASSERT_EQUAL_INT(0, read_file(cc, "../example2.txt"));
	for (int i = 0; i < EXAMPLE2_LEN; i++) {
		lex(cc);
		TEST_ASSERT_TRUE(cc->token < TK_EOF);
	}
	// read EOF again
	TEST_ASSERT_EQUAL_UINT8(TK_EOF, lex(cc));
	TEST_ASSERT_EQUAL_UINT8(TK_EOF, cc->token);

	TEST_ASSERT_EQUAL_INT(0, read_file(cc, "../escapes.txt"));
	TEST_ASSERT_EQUAL_UINT8('(', lex(cc));
	TEST_ASSERT_EQUAL_UINT8('(', cc->token);
	TEST_ASSERT_EQUAL_UINT8('|', lex(cc));
	TEST_ASSERT_EQUAL_UINT8('|', cc->token);
	TEST_ASSERT_EQUAL_UINT8(')', lex(cc));
	TEST_ASSERT_EQUAL_UINT8(')', cc->token);
	cc->flags |= CC_DISABLE_ERROR_MSG;
	print_error(cc, "you can't see this");
	TEST_ASSERT_EQUAL_UINT8('\t', lex(cc));
	TEST_ASSERT_EQUAL_UINT8('\t', cc->token);
	TEST_ASSERT_EQUAL_UINT8('*', lex(cc));
	TEST_ASSERT_EQUAL_UINT8('*', cc->token);
	TEST_ASSERT_EQUAL_UINT8('\n', lex(cc));
	TEST_ASSERT_EQUAL_UINT8('\n', cc->token);
	TEST_ASSERT_EQUAL_UINT8('\\', lex(cc));
	TEST_ASSERT_EQUAL_UINT8('\\', cc->token);
	TEST_ASSERT_EQUAL_UINT8(TK_ILLEGAL, lex(cc));
	TEST_ASSERT_EQUAL_UINT8(TK_ILLEGAL, cc->token);
	cc->flags = 0;
	print_error(cc, "unknown character");
	// read past illegal token
	TEST_ASSERT_EQUAL_UINT8(TK_EOF, lex(cc));
	TEST_ASSERT_EQUAL_UINT8(TK_EOF, cc->token);
	destroy_cmpctrl(cc);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_get_char);
	RUN_TEST(test_lex);

	return UNITY_END();
}