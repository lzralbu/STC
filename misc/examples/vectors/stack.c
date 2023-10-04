
#include <stdio.h>

#define i_tag i
#define i_capacity 100
#define i_key int
#include <stc/stack.h>

#define i_tag c
#define i_key char
#include <stc/stack.h>

int main(void) {
    stack_i stack = {0};
    stack_c chars = {0};

    c_forrange (i, 101)
        stack_i_push(&stack, (int)(i*i));

    printf("%d\n", *stack_i_top(&stack));

    c_forrange (i, 90)
        stack_i_pop(&stack);

    c_foreach (i, stack_i, stack)
        printf(" %d", *i.ref);
    puts("");
    printf("top: %d\n", *stack_i_top(&stack));

    stack_i_drop(&stack);
    stack_c_drop(&chars);
}
