#!/bin/bash
assert() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert_with_funccall() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    cc -o tmp tmp.s funccalltest.o alloctesthelper.o
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 "tests/test-aa.c"
assert 42 "tests/test-ab.c"
assert 21 "tests/test-ac.c"
assert 41 "tests/test-ad.c"
assert 47 "tests/test-ae.c"
assert 15 "tests/test-af.c"
assert 4 "tests/test-ag.c"
assert 10 "tests/test-ah.c"
assert 2 "tests/test-ai.c"
assert 1 "tests/test-aj.c"
assert 0 "tests/test-ak.c"
assert 1 "tests/test-al.c"
assert 0 "tests/test-am.c"
assert 1 "tests/test-an.c"
assert 0 "tests/test-ao.c"
assert 16 "tests/test-ap.c"
assert 44 "tests/test-aq.c"
assert 25 "tests/test-ar.c"
assert 44 "tests/test-as.c"
assert 40 "tests/test-at.c"
assert 15 "tests/test-au.c"
assert 15 "tests/test-av.c"
assert 10 "tests/test-aw.c"
assert 15 "tests/test-ax.c"
assert 15 "tests/test-ay.c"
assert 15 "tests/test-az.c"
assert 10 "tests/test-ba.c"
assert 20 "tests/test-bb.c"
assert_with_funccall 0 "tests/test-bc.c"
assert_with_funccall 13 "tests/test-bd.c"
assert_with_funccall 25 "tests/test-be.c"
assert_with_funccall 36 "tests/test-bf.c"
assert_with_funccall 70 "tests/test-bg.c"
assert_with_funccall 76 "tests/test-bh.c"
assert_with_funccall 81 "tests/test-bi.c"
assert_with_funccall 85 "tests/test-bj.c"
assert 1 "tests/test-bk.c"
assert 3 "tests/test-bl.c"
assert 6 "tests/test-bm.c"
assert 10 "tests/test-bn.c"
assert 15 "tests/test-bo.c"
assert 21 "tests/test-bp.c"
assert 28 "tests/test-bq.c"
assert 36 "tests/test-br.c"
assert 45 "tests/test-bs.c"
assert 55 "tests/test-bt.c"
assert 6 "tests/test-bu.c"
# assert 110 "tests/test-bv.c"
assert 1 "tests/test-bw.c"
assert 2 "tests/test-bx.c"
assert 3 "tests/test-by.c"
assert_with_funccall 1 "tests/test-bz.c"
assert_with_funccall 2 "tests/test-ca.c"
assert_with_funccall 4 "tests/test-cb.c"
assert_with_funccall 8 "tests/test-cc.c"
assert_with_funccall 1 "tests/test-cd.c"
assert_with_funccall 2 "tests/test-ce.c"
assert_with_funccall 4 "tests/test-cf.c"
assert_with_funccall 8 "tests/test-cg.c"
assert_with_funccall 1 "tests/test-ch.c"
assert_with_funccall 2 "tests/test-ci.c"
assert_with_funccall 4 "tests/test-cj.c"
assert_with_funccall 8 "tests/test-ck.c"
assert_with_funccall 1 "tests/test-cl.c"
assert_with_funccall 2 "tests/test-cm.c"
assert_with_funccall 4 "tests/test-cn.c"
assert_with_funccall 8 "tests/test-co.c"
assert 4 "tests/test-cp.c"
assert 8 "tests/test-cq.c"
assert 0 "tests/test-cr.c"
assert 64 "tests/test-cs.c"
assert_with_funccall 3 "tests/test-ct.c"
assert 1 "tests/test-cu.c"
assert 2 "tests/test-cv.c"
assert 3 "tests/test-cw.c"
assert 3 "tests/test-cx.c"
assert 3 "tests/test-cy.c"
assert 3 "tests/test-cz.c"
assert 1 "tests/test-da.c"
assert 3 "tests/test-db.c"
assert 3 "tests/test-dc.c"
assert 1 "tests/test-dd.c"
assert 1 "tests/test-de.c"
assert 5 "tests/test-df.c"
assert 3 "tests/test-dg.c"
assert 97 "tests/test-dh.c"
assert 98 "tests/test-di.c"
assert 99 "tests/test-dj.c"
assert 100 "tests/test-dk.c"
assert 101 "tests/test-dl.c"
assert 102 "tests/test-dm.c"

echo OK

