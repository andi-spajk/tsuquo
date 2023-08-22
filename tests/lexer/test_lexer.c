#include "../../unity/unity.h"
#include "lexer.h"

#define EXAMPLE1_LEN 7
#define EXAMPLE2_LEN 210
#define ESCAPES_LEN  12

void setUp(void) {}
void tearDown(void) {}

void test_init_lexer(void)
{
	Lexer *lexer = init_lexer();
	TEST_ASSERT_NULL(lexer->buffer);
	TEST_ASSERT_EQUAL_INT(0, lexer->pos);
	TEST_ASSERT_EQUAL_INT(0, lexer->buffer_len);
	destroy_lexer(lexer);
}

void test_read_file(void)
{
	Lexer *lexer = init_lexer();
	TEST_ASSERT_EQUAL_INT(0, read_file(lexer, "example.txt"));
	TEST_ASSERT_EQUAL_INT(EXAMPLE1_LEN, lexer->buffer_len);
	TEST_ASSERT_EQUAL_MEMORY("a(b|c)*", lexer->buffer, lexer->buffer_len);
	TEST_ASSERT_EQUAL_INT(0, read_file(lexer, "example2.txt"));
	TEST_ASSERT_EQUAL_INT(EXAMPLE2_LEN, lexer->buffer_len);
	TEST_ASSERT_EQUAL_INT(0, read_file(lexer, "empty.txt"));
	TEST_ASSERT_EQUAL_INT(0, lexer->buffer_len);
	destroy_lexer(lexer);

	// read_file() twice, ignoring first call
	Lexer *lexer2 = init_lexer();
	TEST_ASSERT_EQUAL_INT(0, read_file(lexer2, "example.txt"));
	TEST_ASSERT_EQUAL_INT(0, read_file(lexer2, "example2.txt"));
	TEST_ASSERT_EQUAL_INT(EXAMPLE2_LEN, lexer2->buffer_len);
	destroy_lexer(lexer2);
}

void test_get_char(void)
{
	U8 ch;
	Lexer *lexer = init_lexer();
	TEST_ASSERT_EQUAL_INT(0, read_file(lexer, "example.txt"));
	char *example = "a(b|c)*";
	for (int i = 0; i < EXAMPLE1_LEN; i++) {
		ch = get_char(lexer);
		TEST_ASSERT_EQUAL_INT(i+1, lexer->pos);
		TEST_ASSERT_EQUAL_UINT8(example[i], ch);
	}
	ch = get_char(lexer);
	TEST_ASSERT_EQUAL_INT(EXAMPLE1_LEN, lexer->pos);
	TEST_ASSERT_EQUAL_UINT8(TK_EOF, ch);
	// get_char should continue to return TK_EOF
	ch = get_char(lexer);
	TEST_ASSERT_EQUAL_INT(EXAMPLE1_LEN, lexer->pos);
	TEST_ASSERT_EQUAL_UINT8(TK_EOF, ch);
	destroy_lexer(lexer);
}

void test_lex(void)
{
	U8 tk;
	Lexer *lexer = init_lexer();
	TEST_ASSERT_EQUAL_INT(0, read_file(lexer, "example.txt"));
	tk = lex(lexer);
	TEST_ASSERT_EQUAL_UINT8('a', tk);
	tk = lex(lexer);
	TEST_ASSERT_EQUAL_UINT8(TK_LPAREN, tk);
	tk = lex(lexer);
	TEST_ASSERT_EQUAL_UINT8('b', tk);
	tk = lex(lexer);
	TEST_ASSERT_EQUAL_UINT8(TK_PIPE, tk);
	tk = lex(lexer);
	TEST_ASSERT_EQUAL_UINT8('c', tk);
	tk = lex(lexer);
	TEST_ASSERT_EQUAL_UINT8(TK_RPAREN, tk);
	tk = lex(lexer);
	TEST_ASSERT_EQUAL_UINT8(TK_STAR, tk);
	tk = lex(lexer);
	TEST_ASSERT_EQUAL_UINT8(TK_EOF, tk);

	TEST_ASSERT_EQUAL_INT(0, read_file(lexer, "example2.txt"));
	for (int i = 0; i < EXAMPLE2_LEN; i++) {
		tk = lex(lexer);
		TEST_ASSERT_TRUE(tk < TK_EOF);
	}
	tk = lex(lexer);
	TEST_ASSERT_EQUAL_UINT8(TK_EOF, tk);

	TEST_ASSERT_EQUAL_INT(0, read_file(lexer, "escapes.txt"));
	tk = lex(lexer);
	TEST_ASSERT_EQUAL_UINT8('(', tk);
	tk = lex(lexer);
	TEST_ASSERT_EQUAL_UINT8('|', tk);
	tk = lex(lexer);
	TEST_ASSERT_EQUAL_UINT8(')', tk);
	tk = lex(lexer);
	TEST_ASSERT_EQUAL_UINT8('\t', tk);
	tk = lex(lexer);
	TEST_ASSERT_EQUAL_UINT8('*', tk);
	tk = lex(lexer);
	TEST_ASSERT_EQUAL_UINT8('\n', tk);
	tk = lex(lexer);
	TEST_ASSERT_EQUAL_UINT8('\\', tk);
	tk = lex(lexer);
	TEST_ASSERT_EQUAL_UINT8(TK_ILLEGAL, tk);
	tk = lex(lexer);
	TEST_ASSERT_EQUAL_UINT8(TK_EOF, tk);
	destroy_lexer(lexer);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_init_lexer);
	RUN_TEST(test_read_file);
	RUN_TEST(test_get_char);
	RUN_TEST(test_lex);

	return UNITY_END();
}