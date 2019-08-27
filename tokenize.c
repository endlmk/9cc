#include "9cc.h"

// 入力プログラム
char *user_input;
// 現在着目しているトークン
Token *token;

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(1);
}

void error_at(char *loc, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(1);
}
bool is_alpha(char c)
{
    return ('a' <= c && c <= 'z') ||
           ('A' <= c && c <= 'Z');
}

bool is_alnum(char c)
{
    return ('a' <= c && c <= 'z') ||
           ('A' <= c && c <= 'Z') ||
           ('0' <= c && c <= '9') ||
           (c == '_');
}

// 次のトークンが期待している記号のときには、トークンを一つ読み進めて真を返す。
// それ以外の場合には偽を返す。
bool consume(char *op)
{
    if(token->kind != TK_RESERVED || 
       strlen(op) != token->len ||
       memcmp(token->str, op, token->len))
    {
        return false;
    }
    token = token->next;
    return true;
}

bool consume_return()
{
    if(token->kind != TK_RETURN)
    {
        return false;
    }
    token = token->next;
    return true;
}

Token* consume_ident()
{
    if(token->kind != TK_IDENT)
    {
        return NULL;
    }
    Token* tok = token;
    token = token->next;
    return tok;
}

// 次のトークンが期待している記号のときには、トークンを一つ読みすすめる。
// それ以外の場合には、エラーを報告する。
void expect(char *op)
{
    if(token->kind != TK_RESERVED || 
       strlen(op) != token->len ||
       memcmp(token->str, op, token->len))
    {
        error_at(token->str, "'%s'ではありません", op);
    }
    token = token->next;
}

// 次のトークンが数値の場合、トークンを一つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number() 
{
    if(token->kind != TK_NUM)
    {
        error_at(token->str, "数ではありません");
    }
    int val = token->val;
    token = token->next;
    return val;
}

// 現在のトークンがEOFかどうか
bool at_eof()
{
    return token->kind == TK_EOF;
}

// 新しいトークンを作成してcurにつなげる
Token *new_token(TokenKind kind, Token *cur, char *str)
{
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p)
{
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while(*p)
    {
        if(isspace(*p))
        {
            p++;
            continue;
        }
        if(strncmp(p, "return", 6) == 0 && !is_alnum(p[6]))
        {
            cur = new_token(TK_RETURN, cur, p);
            cur->len = 6;
            p += 6;
            continue;
        }
        if(strncmp(p, "==", 2) == 0 ||
           strncmp(p, "!=", 2) == 0 ||
           strncmp(p, "<=", 2) == 0 ||
           strncmp(p, ">=", 2) == 0)
        {
            cur = new_token(TK_RESERVED, cur, p);
            cur->len = 2;
            p += 2;
            continue;
        }
        if(*p == '+' ||
           *p == '-' ||
           *p == '*' ||
           *p == '/' ||
           *p == '(' ||
           *p == ')' ||
           *p == '<' ||
           *p == '>' ||
           *p == ';' ||
           *p == '=')
        {
            cur = new_token(TK_RESERVED, cur, p++);            
            cur->len = 1;
            continue;
        }
        if(is_alpha(*p))
        {
            char *q = p++;
            while(is_alnum(*p))
            {
                p++;
            }
            cur = new_token(TK_IDENT, cur, q);
            cur->len = p - q;
            continue;
        }
        if(isdigit(*p))
        {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error_at(p, "トークナイズできません");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}