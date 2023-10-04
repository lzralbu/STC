#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stc/crand.h>

#define i_key uint64_t
#define i_val int
#define i_tag i64
#include <stc/hmap.h>

static uint64_t seed = 12345;

static void test_repeats(void)
{
    enum {BITS = 46, BITS_TEST = BITS/2 + 2};
    static const uint64_t N = 1ull << BITS_TEST;
    static const uint64_t mask = (1ull << BITS) - 1;

    printf("birthday paradox: value range: 2^%d, testing repeats of 2^%d values\n", BITS, BITS_TEST);
    crand_t rng = crand_init(seed);

    hmap_i64 m = hmap_i64_with_capacity(N);
    c_forrange (i, N) {
        uint64_t k = crand_u64(&rng) & mask;
        int v = hmap_i64_insert(&m, k, 0).ref->second += 1;
        if (v > 1) printf("repeated value %" PRIu64 " (%d) at 2^%d\n",
                          k, v, (int)log2((double)i));
    }
    hmap_i64_drop(&m);
}

#define i_key uint32_t
#define i_val uint64_t
#define i_tag u32
#include <stc/hmap.h>

void test_distribution(void)
{
    enum {BITS = 26};
    printf("distribution test: 2^%d values\n", BITS);
    crand_t rng = crand_init(seed);
    const size_t N = 1ull << BITS ;

    hmap_u32 map = {0};
    c_forrange (N) {
        uint64_t k = crand_u64(&rng);
        hmap_u32_insert(&map, k & 0xf, 0).ref->second += 1;
    }

    uint64_t sum = 0;
    c_foreach (i, hmap_u32, map) sum += i.ref->second;
    sum /= (uint64_t)map.size;

    c_foreach (i, hmap_u32, map) {
        printf("%4" PRIu32 ": %" PRIu64 " - %" PRIu64 ": %11.8f\n",
                i.ref->first, i.ref->second, sum,
                (1.0 - (double)i.ref->second / (double)sum));
    }

    hmap_u32_drop(&map);
}

int main(void)
{
    seed = (uint64_t)time(NULL);
    test_distribution();
    test_repeats();
}
