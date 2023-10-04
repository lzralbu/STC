#include <stdio.h>

#define i_key int
#define i_use_cmp
#include <stc/vec.h>

#define i_key int
#include <stc/sset.h>

int main(void)
{
    // TEST SORTED VECTOR
    {
        int key, *res;
        vec_int vec = c_init(vec_int, {40, 600, 1, 7000, 2, 500, 30});

        vec_int_sort(&vec);

        key = 100;
        res = vec_int_lower_bound(&vec, key).ref;
        if (res)
            printf("Sorted Vec %d: lower bound: %d\n", key, *res); // 500

        key = 10;
        vec_int_iter it1 = vec_int_lower_bound(&vec, key);
        if (it1.ref)
            printf("Sorted Vec %3d: lower_bound: %d\n", key, *it1.ref); // 30

        key = 600;
        vec_int_iter it2 = vec_int_binary_search(&vec, key);
        if (it2.ref)
            printf("Sorted Vec %d: bin. search: %d\n", key, *it2.ref); // 600

        c_foreach (i, vec_int, it1, it2)
            printf("  %d\n", *i.ref);

        puts("");
        vec_int_drop(&vec);
    }
    
    // TEST SORTED SET
    {
        int key, *res;
        sset_int set = c_init(sset_int, {40, 600, 1, 7000, 2, 500, 30});

        key = 100;
        res = sset_int_lower_bound(&set, key).ref;
        if (res)
            printf("Sorted Set %d: lower bound: %d\n", key, *res); // 500

        key = 10;
        sset_int_iter it1 = sset_int_lower_bound(&set, key);
        if (it1.ref)
            printf("Sorted Set %3d: lower bound: %d\n", key, *it1.ref); // 30

        key = 600;
        sset_int_iter it2 = sset_int_find(&set, key);
        if (it2.ref)
            printf("Sorted Set %d: find       : %d\n", key, *it2.ref); // 600

        c_foreach (i, sset_int, it1, it2)
            printf("  %d\n", *i.ref);

        sset_int_drop(&set);
    }
}
