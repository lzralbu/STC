#include <stdio.h>
#include <string.h>

void int_drop(int* x) {
    printf("drop: %d\n", *x);
}

// arc implements its own clone method using reference counting,
// so 'i_keyclone' is not required to be defined (ignored).

#define i_type Arc          // set type name to be defined (instead of 'arc_int')
#define i_key int
#define i_keydrop int_drop  // optional, just to display the elements destroyed
#define i_use_cmp           // use int comparison (x < y, x == y).
#include <stc/arc.h>       // Arc

#define i_key_arcbox Arc      // note: use i_key_arcbox instead of i_key for arc/box elements
#include <stc/sset.h>      // sset_Arc (like: std::set<std::shared_ptr<int>>)

#define i_key_arcbox Arc      // note: as above.
#define i_use_cmp
#include <stc/vec.h>       // vec_Arc (like: std::vector<std::shared_ptr<int>>)

int main(void)
{
    const int years[] = {2021, 2012, 2022, 2015};

    vec_Arc vec = {0};
    c_forrange (i, c_arraylen(years)) {
        vec_Arc_emplace(&vec, years[i]);
        // vec_Arc_push(&vec, Arc_from(years[i])); // alt.
    }

    vec_Arc_sort(&vec);

    printf("vec:");
    c_foreach (i, vec_Arc, vec)
        printf(" %d", *i.ref->get);
    puts("");

    // add odd numbers from vec to set
    sset_Arc set = {0};
    c_foreach (i, vec_Arc, vec)
        if (*i.ref->get & 1)
            sset_Arc_insert(&set, Arc_clone(*i.ref)); // copy shared pointer => increments counter.

    // erase the two last elements in vec
    vec_Arc_pop_back(&vec);
    vec_Arc_pop_back(&vec);

    printf("vec:");
    c_foreach (i, vec_Arc, vec) printf(" %d", *i.ref->get);

    printf("\nset:");
    c_foreach (i, sset_Arc, set) printf(" %d", *i.ref->get);

    Arc p = Arc_clone(vec.data[0]);
    printf("\n%d is now owned by %ld objects\n", *p.get, *p.use_count);

    Arc_drop(&p);
    vec_Arc_drop(&vec);
    sset_Arc_drop(&set);
}
