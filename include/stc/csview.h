/* MIT License
 *
 * Copyright (c) 2024 Tyge Løvset
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#define i_header // external linkage by default. override with i_static.
#include "priv/linkage.h"

// csview is a non-zero-terminated string view.

#ifndef STC_CSVIEW_H_INCLUDED
#define STC_CSVIEW_H_INCLUDED

#include "common.h"
#include "types.h"
#include "priv/utf8_prv.h"

#define             csview_init() c_sv_1("")
#define             csview_drop(p) c_default_drop(p)
#define             csview_clone(sv) c_default_clone(sv)

STC_API csview_iter csview_advance(csview_iter it, isize u8pos);
STC_API isize       csview_find_sv(csview sv, csview search);
STC_API size_t      csview_hash(const csview *self);
STC_API csview      csview_slice_ex(csview sv, isize p1, isize p2);
STC_API csview      csview_subview_ex(csview sv, isize pos, isize n);
STC_API csview      csview_token(csview sv, const char* sep, isize* pos);

STC_INLINE csview   csview_from(const char* str)
    { return c_literal(csview){str, c_strlen(str)}; }
STC_INLINE csview   csview_from_n(const char* str, isize n)
    { return c_literal(csview){str, n}; }

STC_INLINE void     csview_clear(csview* self) { *self = csview_init(); }
STC_INLINE isize    csview_size(csview sv) { return sv.size; }
STC_INLINE bool     csview_is_empty(csview sv) { return sv.size == 0; }

STC_INLINE bool csview_equals_sv(csview sv1, csview sv2)
    { return sv1.size == sv2.size && !c_memcmp(sv1.buf, sv2.buf, sv1.size); }

STC_INLINE bool csview_equals(csview sv, const char* str)
    { return csview_equals_sv(sv, c_sv_2(str, c_strlen(str))); }

STC_INLINE isize csview_find(csview sv, const char* str)
    { return csview_find_sv(sv, c_sv_2(str, c_strlen(str))); }

STC_INLINE bool csview_contains(csview sv, const char* str)
    { return csview_find(sv, str) != c_NPOS; }

STC_INLINE bool csview_starts_with(csview sv, const char* str) {
    isize n = c_strlen(str);
    return n <= sv.size && !c_memcmp(sv.buf, str, n);
}

STC_INLINE bool csview_ends_with(csview sv, const char* str) {
    isize n = c_strlen(str);
    return n <= sv.size && !c_memcmp(sv.buf + sv.size - n, str, n);
}

STC_INLINE csview csview_subview(csview sv, isize pos, isize n) {
    if (pos + n > sv.size) n = sv.size - pos;
    sv.buf += pos, sv.size = n;
    return sv;
}

STC_INLINE csview csview_slice(csview sv, isize p1, isize p2) {
    if (p2 > sv.size) p2 = sv.size;
    sv.buf += p1, sv.size = p2 - p1;
    return sv;
}

STC_INLINE csview csview_trim_left(csview sv)
    { while (sv.size && *sv.buf <= ' ') ++sv.buf, --sv.size; return sv; }

STC_INLINE csview csview_trim_right(csview sv)
    { while (sv.size && sv.buf[sv.size - 1] <= ' ') --sv.size; return sv; }

STC_INLINE csview csview_trim(csview sv)
    { return csview_trim_right(csview_trim_left(sv)); }

STC_INLINE csview csview_right(csview sv, isize len)
    { return csview_subview(sv, sv.size - len, len); }

STC_INLINE const char* csview_at(csview sv, isize idx)
    { c_assert(c_uless(idx, sv.size)); return sv.buf + idx; }

/* utf8 iterator */
STC_INLINE csview_iter csview_begin(const csview* self) {
    return c_literal(csview_iter){.u8 = {{self->buf, utf8_chr_size(self->buf)},
                                          self->buf + self->size}};
}
STC_INLINE csview_iter csview_end(const csview* self) {
    return c_literal(csview_iter){.u8 = {{0}, self->buf + self->size}};
}
STC_INLINE void csview_next(csview_iter* it) {
    it->ref += it->chr.size;
    it->chr.size = utf8_chr_size(it->ref);
    if (it->ref == it->u8.end) it->ref = NULL;
}

/* utf8 */
STC_INLINE csview csview_u8_from(const char* str, isize u8pos, isize u8len)
    { return utf8_span(str, u8pos, u8len); }

STC_INLINE csview csview_u8_subview(csview sv, isize u8pos, isize u8len) {
    const char* s, *end = &sv.buf[sv.size];
    while ((u8pos > 0) & (sv.buf != end))
        u8pos -= (*++sv.buf & 0xC0) != 0x80;
    s = sv.buf;
    while ((u8len > 0) & (s != end))
        u8len -= (*++s & 0xC0) != 0x80;
    sv.size = s - sv.buf; return sv;
}

STC_INLINE csview csview_u8_right(csview sv, isize u8len) {
    const char* p = &sv.buf[sv.size];
    while (u8len && p != sv.buf)
        u8len -= (*--p & 0xC0) != 0x80;
    sv.size -= p - sv.buf, sv.buf = p;
    return sv;
}

STC_INLINE csview csview_u8_chr(csview sv, isize u8pos) {
    const char *end = &sv.buf[sv.size];
    while ((u8pos > 0) & (sv.buf != end))
        u8pos -= (*++sv.buf & 0xC0) != 0x80;
    sv.size = sv.buf != end ? utf8_chr_size(sv.buf) : 0;
    return sv;
}

STC_INLINE isize csview_u8_size(csview sv)
    { return utf8_count_n(sv.buf, sv.size); }

STC_INLINE bool csview_u8_valid(csview sv) // requires linking with utf8 symbols
    { return utf8_valid_n(sv.buf, sv.size); }

#define c_fortoken_sv(it, separator, sv) \
    for (struct { csview input, token; const char* sep; isize pos; } \
         it = {.input=sv, .sep=separator} ; \
         it.pos <= it.input.size && (it.token = csview_token(it.input, it.sep, &it.pos)).buf ; )

#define c_fortoken(it, separator, str) \
    c_fortoken_sv(it, separator, csview_from(str))

/* ---- Container helper functions ---- */

STC_INLINE int csview_cmp(const csview* x, const csview* y) {
    isize n = x->size < y->size ? x->size : y->size;
    int c = c_memcmp(x->buf, y->buf, n);
    return c ? c : c_default_cmp(&x->size, &y->size);
}

STC_INLINE bool csview_eq(const csview* x, const csview* y)
    { return x->size == y->size && !c_memcmp(x->buf, y->buf, x->size); }

/* ---- case insensitive ---- */

STC_INLINE bool csview_iequals_sv(csview sv1, csview sv2)
    { return sv1.size == sv2.size && !utf8_icompare(sv1, sv2); }

STC_INLINE bool csview_iequals(csview sv, const char* str)
    { return csview_iequals_sv(sv, c_sv(str, c_strlen(str))); }

STC_INLINE bool csview_ieq(const csview* x, const csview* y)
    { return csview_iequals_sv(*x, *y); }

STC_INLINE int csview_icmp(const csview* x, const csview* y)
    { return utf8_icompare(*x, *y); }

STC_INLINE bool csview_istarts_with(csview sv, const char* str) {
    isize n = c_strlen(str);
    return n <= sv.size && !utf8_icompare(sv, c_sv(str, n));
}

STC_INLINE bool csview_iends_with(csview sv, const char* str) {
    isize n = c_strlen(str);
    return n <= sv.size && !utf8_icmp(sv.buf + sv.size - n, str);
}

#endif // STC_CSVIEW_H_INCLUDED

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined i_implement || defined i_static
#ifndef STC_CSVIEW_C_INCLUDED
#define STC_CSVIEW_C_INCLUDED

STC_DEF size_t csview_hash(const csview *self)
    { return c_hash_n(self->buf, self->size); }

STC_DEF csview_iter csview_advance(csview_iter it, isize u8pos) {
    int inc = 1;
    if (u8pos < 0) u8pos = -u8pos, inc = -1;
    while (u8pos && it.ref != it.u8.end)
        u8pos -= (*(it.ref += inc) & 0xC0) != 0x80;
    it.chr.size = utf8_chr_size(it.ref);
    if (it.ref == it.u8.end) it.ref = NULL;
    return it;
}

STC_DEF isize csview_find_sv(csview sv, csview search) {
    char* res = c_strnstrn(sv.buf, sv.size, search.buf, search.size);
    return res ? (res - sv.buf) : c_NPOS;
}

STC_DEF csview csview_subview_ex(csview sv, isize pos, isize n) {
    if (pos < 0) {
        pos += sv.size;
        if (pos < 0) pos = 0;
    }
    if (pos > sv.size) pos = sv.size;
    if (pos + n > sv.size) n = sv.size - pos;
    sv.buf += pos, sv.size = n;
    return sv;
}

STC_DEF csview csview_slice_ex(csview sv, isize p1, isize p2) {
    if (p1 < 0) {
        p1 += sv.size;
        if (p1 < 0) p1 = 0;
    }
    if (p2 < 0) p2 += sv.size;
    if (p2 > sv.size) p2 = sv.size;
    sv.buf += p1, sv.size = (p2 > p1 ? p2 - p1 : 0);
    return sv;
}

STC_DEF csview csview_token(csview sv, const char* sep, isize* pos) {
    isize sep_size = c_strlen(sep);
    csview slice = {sv.buf + *pos, sv.size - *pos};
    const char* res = c_strnstrn(slice.buf, slice.size, sep, sep_size);
    csview tok = {slice.buf, res ? (res - slice.buf) : slice.size};
    *pos += tok.size + sep_size;
    return tok;
}
#endif // STC_CSVIEW_C_INCLUDED
#endif // i_implement

#if defined i_import
  #include "priv/utf8_prv.c"
#endif
#include "priv/linkage2.h"
