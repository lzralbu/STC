#define i_implement
#include <stc/cstr.h>

void stringdemo1(void)
{
    cstr cs = cstr_lit("one-nine-three-seven-five");
    printf("%s.\n", cstr_str(&cs));

    cstr_insert(&cs, 3, "-two");
    printf("%s.\n", cstr_str(&cs));

    cstr_erase(&cs, 7, 5); // -nine
    printf("%s.\n", cstr_str(&cs));

    cstr_replace(&cs, "seven", "four", 1);
    printf("%s.\n", cstr_str(&cs));

    cstr_take(&cs, cstr_from_fmt("%s *** %s", cstr_str(&cs), cstr_str(&cs)));
    printf("%s.\n", cstr_str(&cs));

    printf("find \"four\": %s\n", cstr_str(&cs) + cstr_find(&cs, "four"));

    // reassign:
    cstr_assign(&cs, "one two three four five six seven");
    cstr_append(&cs, " eight");
    printf("append: %s\n", cstr_str(&cs));

    cstr_drop(&cs);
}

#define i_key long long
#define i_tag ll
#include <stc/vec.h>

void vectordemo1(void)
{
    vec_ll bignums = vec_ll_with_capacity(100);
    vec_ll_reserve(&bignums, 100);
    for (int i = 10; i <= 100; i += 10)
        vec_ll_push(&bignums, i * i);

    printf("erase - %d: %lld\n", 3, bignums.data[3]);
    vec_ll_erase_n(&bignums, 3, 1); // erase index 3

    vec_ll_pop(&bignums);           // erase the last
    vec_ll_erase_n(&bignums, 0, 1); // erase the first

    for (int i = 0; i < vec_ll_size(&bignums); ++i) {
        printf("%d: %lld\n", i, bignums.data[i]);
    }

    vec_ll_drop(&bignums);
}

#define i_key_str
#define i_use_cmp
#include <stc/vec.h>

void vectordemo2(void)
{
    vec_str names = {0};
    vec_str_emplace_back(&names, "Mary");
    vec_str_emplace_back(&names, "Joe");
    vec_str_emplace_back(&names, "Chris");
    cstr_assign(&names.data[1], "Jane"); // replace Joe
    printf("names[1]: %s\n", cstr_str(&names.data[1]));

    vec_str_sort(&names);               // Sort the array
    
    c_foreach (i, vec_str, names)
        printf("sorted: %s\n", cstr_str(i.ref));

    vec_str_drop(&names);
}

#define i_key int
#define i_tag i
#define i_use_cmp
#include <stc/list.h>

void listdemo1(void)
{
    list_i nums = {0}, nums2 = {0};
    for (int i = 0; i < 10; ++i)
        list_i_push_back(&nums, i);
    for (int i = 100; i < 110; ++i)
        list_i_push_back(&nums2, i);

    /* splice nums2 to front of nums */
    list_i_splice(&nums, list_i_begin(&nums), &nums2);
    c_foreach (i, list_i, nums)
        printf("spliced: %d\n", *i.ref);
    puts("");

    *list_i_find(&nums, 104).ref += 50;
    list_i_remove(&nums, 103);
    list_i_iter it = list_i_begin(&nums);
    list_i_erase_range(&nums, list_i_advance(it, 5), list_i_advance(it, 15));
    list_i_pop_front(&nums);
    list_i_push_back(&nums, -99);
    list_i_sort(&nums);

    c_foreach (i, list_i, nums)
        printf("sorted: %d\n", *i.ref);

    c_drop(list_i, &nums, &nums2);
}

#define i_key int
#define i_tag i
#include <stc/hset.h>

void setdemo1(void)
{
    hset_i nums = {0};
    hset_i_insert(&nums, 8);
    hset_i_insert(&nums, 11);

    c_foreach (i, hset_i, nums)
        printf("set: %d\n", *i.ref);
    hset_i_drop(&nums);
}

#define i_key int
#define i_val int
#define i_tag ii
#include <stc/hmap.h>

void mapdemo1(void)
{
    hmap_ii nums = {0};
    hmap_ii_insert(&nums, 8, 64);
    hmap_ii_insert(&nums, 11, 121);
    printf("val 8: %d\n", *hmap_ii_at(&nums, 8));
    hmap_ii_drop(&nums);
}

#define i_key_str
#define i_val int
#define i_tag si
#include <stc/hmap.h>

void mapdemo2(void)
{
    hmap_si nums = {0};
    hmap_si_emplace_or_assign(&nums, "Hello", 64);
    hmap_si_emplace_or_assign(&nums, "Groovy", 121);
    hmap_si_emplace_or_assign(&nums, "Groovy", 200); // overwrite previous

    // iterate the map:
    for (hmap_si_iter i = hmap_si_begin(&nums); i.ref; hmap_si_next(&i))
        printf("long: %s: %d\n", cstr_str(&i.ref->first), i.ref->second);

    // or rather use the short form:
    c_foreach (i, hmap_si, nums)
        printf("short: %s: %d\n", cstr_str(&i.ref->first), i.ref->second);

    hmap_si_drop(&nums);
}

#define i_key_str
#define i_val_str
#include <stc/hmap.h>

void mapdemo3(void)
{
    hmap_str table = {0};
    hmap_str_emplace(&table, "Map", "test");
    hmap_str_emplace(&table, "Make", "my");
    hmap_str_emplace(&table, "Sunny", "day");
    hmap_str_iter it = hmap_str_find(&table, "Make");
    c_foreach (i, hmap_str, table)
        printf("entry: %s: %s\n", cstr_str(&i.ref->first), cstr_str(&i.ref->second));
    printf("size %" c_ZI ": remove: Make: %s\n", hmap_str_size(&table), cstr_str(&it.ref->second));
    //hmap_str_erase(&table, "Make");
    hmap_str_erase_at(&table, it);

    printf("size %" c_ZI "\n", hmap_str_size(&table));
    c_foreach (i, hmap_str, table)
        printf("entry: %s: %s\n", cstr_str(&i.ref->first), cstr_str(&i.ref->second));
    
    hmap_str_drop(&table); // frees key and value cstrs, and hash table.
}

int main(void)
{
    printf("\nSTRINGDEMO1\n"); stringdemo1();
    printf("\nVECTORDEMO1\n"); vectordemo1();
    printf("\nVECTORDEMO2\n"); vectordemo2();
    printf("\nLISTDEMO1\n"); listdemo1();
    printf("\nSETDEMO1\n"); setdemo1();
    printf("\nMAPDEMO1\n"); mapdemo1();
    printf("\nMAPDEMO2\n"); mapdemo2();
    printf("\nMAPDEMO3\n"); mapdemo3();
}
