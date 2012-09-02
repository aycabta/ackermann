#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <time.h>

typedef unsigned long long ack_val_t;
typedef ack_val_t (*ack_func_t)(ack_val_t, ack_val_t);

typedef struct _ack_jmp_node_t
{
    ack_val_t m;
    ack_val_t n;
    jmp_buf jmp;
} ack_jmp_node_t;

typedef struct _ack_node_t
{
    ack_val_t m;
    ack_val_t n;
} ack_node_t;

int int_ack(int m, int n)
{
    if (m == 0) {
        return n + 1;
    } else if (n == 0) {
        return int_ack(m - 1, 1);
    } else {
        return int_ack(m - 1, int_ack(m, n - 1));
    }
}

ack_val_t ack(ack_val_t m, ack_val_t n)
{
    if (m == 0L) {
        return n + 1L;
    } else if (n == 0L) {
        return ack(m - 1L, 1L);
    } else {
        return ack(m - 1L, ack(m, n - 1L));
    }
}

ack_val_t while_ack(ack_val_t m, ack_val_t n)
{
    ack_node_t* stack;
    ack_node_t* node;
    ack_val_t sp;

    stack = (ack_node_t*)malloc(sizeof(ack_node_t) * 6 * 1024 * 1024);
    if (stack == NULL) {
        return 0;
    }

    sp = 0L;

    while (1) {
        if (m == 0L) {
            if (sp == 0L) {
                return n + 1L;
            } else {
                sp--;
                node = &(stack[sp]);
                m = node->m - 1L;
                n = n + 1L;
            }
        } else if (n == 0L) {
            m = m - 1L;
            n = 1L;
        } else {
            stack[sp].m = m;
            stack[sp].n = n;
            m = m;
            n = n - 1L;
            sp++;
        }
    }
}

ack_val_t goto_ack(ack_val_t m, ack_val_t n)
{
    ack_node_t* stack;
    ack_node_t* node;
    ack_val_t sp;

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

ack_val_t jmp_ack(ack_val_t m, ack_val_t n)
{
    ack_node_t* stack;
    ack_node_t* node;
    ack_val_t sp;
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

ack_val_t jmp_general_ack(ack_val_t m, ack_val_t n)
{
    ack_jmp_node_t* stack;
    ack_jmp_node_t* node;
    ack_val_t sp;
    ack_val_t result;
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


int run_ack(char* func_name, ack_func_t func, ack_val_t m, ack_val_t n)
{
    clock_t start;
    clock_t end;
    ack_val_t result;
    long double seconds;

    printf("start: %s(%llu, %llu)\n", func_name, m, n);
    start = clock();
    result = func(m, n);
    end = clock();
    printf("result: %llu\n", result);
    seconds = (long double)(end - start) / CLOCKS_PER_SEC;
    printf("%.8Lf\n", seconds);

    return 0;
}

#define run_ack(f, m, n) run_ack(#f, f, m, n)


int main(void)
{
    ack_val_t m_init;
    ack_val_t n_init;

    m_init = 4L;
    n_init = 1L;

    run_ack(ack, m_init, n_init);
    run_ack(goto_ack, m_init, n_init);
    run_ack(while_ack, m_init, n_init);
    run_ack(jmp_general_ack, m_init, n_init);
    run_ack(jmp_ack, m_init, n_init);

    return 0;
}

