#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <time.h>
#include <gmp.h>

typedef mpz_t* (*ack_func_t)(int, mpz_t);

typedef struct _ack_jmp_node_t
{
    mpz_t m;
    mpz_t n;
    jmp_buf jmp;
} ack_jmp_node_t;

typedef struct _ack_node_t
{
    int m;
    mpz_t run_length;
} ack_node_t;

mpz_t ZERO;
mpz_t ONE;

/*
mpz_t ack(mpz_t m, mpz_t n)
{
    if (m == 0L) {
        return n + 1L;
    } else if (n == 0L) {
        return ack(m - 1L, 1L);
    } else {
        return ack(m - 1L, ack(m, n - 1L));
    }
}
*/

mpz_t* while_ack(int m, mpz_t n)
{
    ack_node_t* stack;
    ack_node_t* node;
    ack_node_t* next_node;
    long long sp;
    long long max_sp;
    mpz_t* result;
    long long i;

    stack = (ack_node_t*)malloc(sizeof(ack_node_t) * 1024 * 1024);
    if (stack == NULL) {
        return &ZERO;
    }

    result = (mpz_t*)malloc(sizeof(mpz_t));
    if (result == NULL) {
        free(stack);
        return &ZERO;
    }
    mpz_init(*result);

    sp = 0L;
    max_sp = -1L;

    while (1) {
        if (m == 0) {
            if (sp == 0L) {
                mpz_add(*result, n, ONE);
                for (i = 0; i < max_sp; ++i) {
                    mpz_clear(stack[i].run_length);
                }
                free(stack);
                return result;
            } else {
                node = &(stack[sp - 1]);
                m = node->m - 1;
                if (mpz_cmp(node->run_length, ZERO) == 0) {
                    sp--;
                } else {
                    mpz_sub(node->run_length, node->run_length, ONE);
                }
                mpz_add(n, n, ONE);
            }
        } else if (mpz_cmp(n, ZERO) == 0) {
            m--;
            mpz_set(n, ONE);
        } else {
            node = &(stack[sp - 1]);
            if (sp > 0 && node->m == m) {
                mpz_add(node->run_length, node->run_length, ONE);
            } else {
                next_node = &(stack[sp]);
                if (sp > max_sp) {
                    max_sp = sp;
                    mpz_init(next_node->run_length);
                }
                next_node->m = m;
                mpz_set(next_node->run_length, ZERO);
                sp++;
            }
            mpz_sub(n, n, ONE);
        }
    }
}

/*
mpz_t goto_ack(mpz_t m, mpz_t n)
{
    ack_node_t* stack;
    ack_node_t* node;
    mpz_t sp;

    stack = (ack_node_t*)malloc(sizeof(ack_node_t) * 6 * 1024 * 1024);
    if (stack == NULL) {
        return 0;
    }

    sp = 0L;

    start:
    if (m == 0L) {
        if (sp == 0L) {
            return n + 1L;
        } else {
            sp--;
            node = &(stack[sp]);
            m = node->m - 1L;
            n = n + 1L;
            goto start;
        }
    } else if (n == 0L) {
        m = m - 1L;
        n = 1L;
        goto start;
    } else {
        stack[sp].m = m;
        stack[sp].n = n;
        m = m;
        n = n - 1L;
        sp++;
        goto start;
    }
}

mpz_t jmp_ack(mpz_t m, mpz_t n)
{
    ack_node_t* stack;
    ack_node_t* node;
    mpz_t sp;
    jmp_buf start;

    stack = (ack_node_t*)malloc(sizeof(ack_node_t) * 6 * 1024 * 1024);
    if (stack == NULL) {
        return 0;
    }

    sp = 0L;

    setjmp(start);
    if (m == 0L) {
        if (sp == 0L) {
            return n + 1L;
        } else {
            sp--;
            node = &(stack[sp]);
            m = node->m - 1L;
            n = n + 1L;
            longjmp(start, 1);
        }
    } else if (n == 0L) {
        m = m - 1L;
        n = 1L;
        longjmp(start, 1);
    } else {
        stack[sp].m = m;
        stack[sp].n = n;
        m = m;
        n = n - 1L;
        sp++;
        longjmp(start, 1);
    }
}

mpz_t jmp_general_ack(mpz_t m, mpz_t n)
{
    ack_jmp_node_t* stack;
    ack_jmp_node_t* node;
    mpz_t sp;
    mpz_t result;
    jmp_buf start;

    stack = (ack_jmp_node_t*)malloc(sizeof(ack_jmp_node_t) * 6 * 1024 * 1024);
    if (stack == NULL) {
        return 0;
    }

    sp = 0L;

    setjmp(start);
    if (m == 0L) {
        if (sp == 0L) {
            return n + 1L;
        } else {
            sp--;
            node = &(stack[sp]);
            result = n + 1;
            m = node->m;
            n = node->n;
            longjmp(node->jmp, 1);
        }
    } else if (n == 0L) {
        m = m - 1L;
        n = 1L;
        longjmp(start, 1);
    } else {
        if (setjmp(stack[sp].jmp) == 0) {
            stack[sp].m = m;
            stack[sp].n = n;
            m = m;
            n = n - 1L;
            sp++;
            longjmp(start, 1);
        }
        m = m - 1L;
        n = result;
        longjmp(start, 1);
    }
}
*/


int run_ack(char* func_name, ack_func_t func, int m, mpz_t n)
{
    clock_t start;
    clock_t end;
    mpz_t* result;
    long double seconds;

    gmp_printf("start: %s(%d, %Zd)\n", func_name, m, n);
    start = clock();
    result = func(m, n);
    end = clock();
    gmp_printf("result: %Zd\n", *result);
    mpz_clear(*result);
    free(result);
    seconds = (long double)(end - start) / CLOCKS_PER_SEC;
    printf("%.8Lf\n", seconds);

    return 0;
}

#define run_ack(f, m, n) run_ack(#f, f, m, n)


int main(void)
{
    int m_init;
    mpz_t n_init;

    mpz_init(ZERO);
    mpz_init(ONE);

    mpz_set_ui(ZERO, 0L);
    mpz_set_ui(ONE, 1L);

    mpz_init(n_init);

    m_init =           4;
    mpz_set_ui(n_init, 1L);

//    run_ack(ack, m_init, n_init);
//    run_ack(goto_ack, m_init, n_init);
    run_ack(while_ack, m_init, n_init);
//    run_ack(jmp_general_ack, m_init, n_init);
//    run_ack(jmp_ack, m_init, n_init);

    mpz_clear(n_init);
    mpz_clear(ZERO);
    mpz_clear(ONE);

    return 0;
}

