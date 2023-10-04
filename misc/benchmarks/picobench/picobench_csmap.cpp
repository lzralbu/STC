#include <iostream>
#define i_static
#include <stc/crand.h>
#define i_static
#include <stc/cstr.h>
#include <cmath>
#include <string>
#include <map>

#define PICOBENCH_IMPLEMENT_WITH_MAIN
#include "picobench.hpp"

enum {N1 = 1000000, S1 = 1};
uint64_t seed = time(NULL); // 18237129837891;

using omap_i = std::map<int, int>;
using omap_x = std::map<uint64_t, uint64_t>;
using omap_s = std::map<std::string, std::string>;

#define i_key int
#define i_val int
#define i_tag i
#include <stc/smap.h>

#define i_key size_t
#define i_val size_t
#define i_tag x
#include <stc/smap.h>

#define i_key_str
#define i_val_str
#include <stc/smap.h>

PICOBENCH_SUITE("Map1");

template <class MapInt>
static void ctor_and_ins_one_i(picobench::state& s)
{
    size_t result = 0;
    picobench::scope scope(s);
    c_forrange (n, s.iterations()) {
        MapInt map;
        map[n];
        result += map.size();
    }
    s.set_result(result);
}
/*
static void ctor_and_ins_one_smap_i(picobench::state& s)
{
    size_t result = 0;
    picobench::scope scope(s);
    c_forrange (n, s.iterations()) {
        smap_i map = smap_i_init();
        smap_i_insert(&map, n, 0);
        result += smap_i_size(&map);
        smap_i_drop(&map);
    }
    s.set_result(result);
}

#define P samples(S1).iterations({N1})
PICOBENCH(ctor_and_ins_one_i<omap_i>).P;
PICOBENCH(ctor_and_ins_one_smap_i).P;
#undef P
*/

PICOBENCH_SUITE("Map_insert_only");

template <class MapInt>
static void insert_i(picobench::state& s)
{
    MapInt map;
    csrand(seed);
    picobench::scope scope(s);
    c_forrange (n, s.iterations())
        map.emplace(crand() & 0xfffffff, n);
    s.set_result(map.size());
}

static void insert_smap_i(picobench::state& s)
{
    smap_i map = smap_i_init();
    csrand(seed);
    picobench::scope scope(s);
    c_forrange (n, s.iterations())
        smap_i_insert(&map, crand() & 0xfffffff, n);
    s.set_result(smap_i_size(&map));
    smap_i_drop(&map);
}

#define P samples(S1).iterations({N1})
PICOBENCH(insert_i<omap_i>).P;
PICOBENCH(insert_smap_i).P;
#undef P


PICOBENCH_SUITE("Map2");

template <class MapInt>
static void ins_and_erase_i(picobench::state& s)
{
    size_t result = 0;
    uint64_t mask = (1ull << s.arg()) - 1;
    MapInt map;
    csrand(seed);

    picobench::scope scope(s);
    c_forrange (i, s.iterations())
        map.emplace(crand() & mask, i);
    result = map.size();

    map.clear();
    csrand(seed);
    c_forrange (i, s.iterations())
        map[crand() & mask] = i;

    csrand(seed);
    c_forrange (s.iterations())
        map.erase(crand() & mask);
    s.set_result(result);
}

static void ins_and_erase_smap_i(picobench::state& s)
{
    size_t result = 0;
    uint64_t mask = (1ull << s.arg()) - 1;
    smap_i map = smap_i_init();
    csrand(seed);

    picobench::scope scope(s);
    c_forrange (i, s.iterations())
        smap_i_insert(&map, crand() & mask, i);
    result = smap_i_size(&map);

    smap_i_clear(&map);
    csrand(seed);
    c_forrange (i, s.iterations())
        smap_i_insert_or_assign(&map, crand() & mask, i);

    csrand(seed);
    c_forrange (s.iterations())
        smap_i_erase(&map, crand() & mask);
    s.set_result(result);
    smap_i_drop(&map);
}

#define P samples(S1).iterations({N1/2, N1/2, N1/2, N1/2}).args({18, 23, 25, 31})
PICOBENCH(ins_and_erase_i<omap_i>).P;
PICOBENCH(ins_and_erase_smap_i).P;
#undef P

PICOBENCH_SUITE("Map3");

template <class MapInt>
static void ins_and_access_i(picobench::state& s)
{
    uint64_t mask = (1ull << s.arg()) - 1;
    size_t result = 0;
    MapInt map;
    csrand(seed);

    picobench::scope scope(s);
    c_forrange (s.iterations()) {
        result += ++map[crand() & mask];
        auto it = map.find(crand() & mask);
        if (it != map.end()) map.erase(it->first);
    }
    s.set_result(result + map.size());
}

static void ins_and_access_smap_i(picobench::state& s)
{
    uint64_t mask = (1ull << s.arg()) - 1;
    size_t result = 0;
    smap_i map = smap_i_init();
    csrand(seed);

    picobench::scope scope(s);
    c_forrange (s.iterations()) {
        result += ++smap_i_insert(&map, crand() & mask, 0).ref->second;
        const smap_i_value* val = smap_i_get(&map, crand() & mask);
        if (val) smap_i_erase(&map, val->first);
    }
    s.set_result(result + smap_i_size(&map));
    smap_i_drop(&map);
}

#define P samples(S1).iterations({N1, N1, N1, N1}).args({18, 23, 25, 31})
PICOBENCH(ins_and_access_i<omap_i>).P;
PICOBENCH(ins_and_access_smap_i).P;
#undef P

PICOBENCH_SUITE("Map4");

static void randomize(char* str, int len) {
    union {uint64_t i; char c[8];} r = {.i = crand()};
    for (int i = len - 7, j = 0; i < len; ++j, ++i)
        str[i] = (r.c[j] & 63) + 48;
}

template <class MapStr>
static void ins_and_access_s(picobench::state& s)
{
    std::string str(s.arg(), 'x');
    size_t result = 0;
    MapStr map;

    picobench::scope scope(s);
    csrand(seed);
    c_forrange (s.iterations()) {
        randomize(&str[0], str.size());
        map.emplace(str, str);
    }
    csrand(seed);
    c_forrange (s.iterations()) {
        randomize(&str[0], str.size());
        result += map.erase(str);
    }
    s.set_result(result + map.size());
}

static void ins_and_access_smap_s(picobench::state& s)
{
    cstr str = cstr_with_size(s.arg(), 'x');
    char* buf = cstr_data(&str);
    size_t result = 0;
    smap_str map = smap_str_init();

    picobench::scope scope(s);
    csrand(seed);
    c_forrange (s.iterations()) {
        randomize(buf, s.arg());
        smap_str_emplace(&map, buf, buf);
    }
    csrand(seed);
    c_forrange (s.iterations()) {
        randomize(buf, s.arg());
        result += smap_str_erase(&map, buf);
        /*smap_str_iter it = smap_str_find(&map, buf);
        if (it.ref) {
            ++result;
            smap_str_erase(&map, cstr_str(&it.ref->first));
        }*/
    }
    s.set_result(result + smap_str_size(&map));
    cstr_drop(&str);
    smap_str_drop(&map);
}

#define P samples(S1).iterations({N1/5, N1/5, N1/5, N1/10, N1/40}).args({13, 7, 8, 100, 1000})
PICOBENCH(ins_and_access_s<omap_s>).P;
PICOBENCH(ins_and_access_smap_s).P;
#undef P

PICOBENCH_SUITE("Map5");

template <class MapX>
static void iterate_x(picobench::state& s)
{
    MapX map;
    uint64_t K = (1ull << s.arg()) - 1;

    picobench::scope scope(s);
    csrand(seed);
    size_t result = 0;

    // measure insert then iterate whole map
    c_forrange (n, s.iterations()) {
        map[crand()] = n;
        if (!(n & K)) for (auto const& keyVal : map)
            result += keyVal.second;
    }

    // reset rng back to inital state
    csrand(seed);

    // measure erase then iterate whole map
    c_forrange (n, s.iterations()) {
        map.erase(crand());
        if (!(n & K)) for (auto const& keyVal : map)
            result += keyVal.second;
    }
    s.set_result(result);
}
/*
static void iterate_smap_x(picobench::state& s)
{
    smap_x map = smap_x_init();
    uint64_t K = (1ull << s.arg()) - 1;

    picobench::scope scope(s);
    csrand(seed);
    size_t result = 0;

    // measure insert then iterate whole map
    c_forrange (n, s.iterations()) {
        smap_x_insert_or_assign(&map, crand(), n);
        if (!(n & K)) c_foreach (i, smap_x, map)
            result += i.ref->second;
    }

    // reset rng back to inital state
    csrand(seed);

    // measure erase then iterate whole map
    c_forrange (n, s.iterations()) {
        smap_x_erase(&map, crand());
        if (!(n & K)) c_foreach (i, smap_x, map)
            result += i.ref->second;
    }
    s.set_result(result);
    smap_x_drop(&map);
}

#define P samples(S1).iterations({N1/20}).args({12})
PICOBENCH(iterate_x<omap_x>).P;
PICOBENCH(iterate_smap_x).P;
#undef P
*/