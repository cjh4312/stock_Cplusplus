function decode(t) {
	var v,w,D,_,C,R,A,O,U,y,E;
    var e, i, n, r, a, o, s, l = (arguments,
    864e5), u = 7657, c = [], d = [], h = ~(3 << 30), f = 1 << 30, m = [0, 3, 5, 6, 9, 10, 12, 15, 17, 18, 20, 23, 24, 27, 29, 30], p = Math, g = function() {
        var l, u;
		
        for (l = 0; 64 > l; l++)
            d[l] = p.pow(2, l),
            26 > l && (c[l] = v(l + 65),
            c[l + 26] = v(l + 97),
            10 > l && (c[l + 52] = v(l + 48)));
        for (c.push("+", "/"),
        c = c.join(""),
        i = t.split(""),
        n = i.length,
        l = 0; n > l; l++)
            i[l] = c.indexOf(i[l]);
        return r = {},
        e = o = 0,
        a = {},
        u = w([12, 6]),
        s = 63 ^ u[1],
        {
            _1479: D,
            _136: _,
            _200: C,
            _139: R,
            _197: A,
            _3466: O
        }["_" + u[0]] || function() {
            return []
        }
    }, v = String.fromCharCode, b = function(t) {
        return t === {}._
    }, N = function() {
        var t, e;
        for (t = y(),
        e = 1; ; ) {
            if (!y())
                return e * (2 * t - 1);
            e++
        }
    }, y = function() {
        var t;
        return e >= n ? 0 : (t = i[e] & 1 << o,
        o++,
        o >= 6 && (o -= 6,
        e++),
        !!t)
    }, w = function(t, r, a) {
        var s, l, u, c, h;
        for (l = [],
        u = 0,
        r || (r = []),
        a || (a = []),
        s = 0; s < t.length; s++)
            if (c = t[s],
            u = 0,
            c) {
                if (e >= n)
                    return l;
                if (t[s] <= 0)
                    u = 0;
                else if (t[s] <= 30) {
                    for (; h = 6 - o,
                    h = c > h ? h : c,
                    u |= (i[e] >> o & (1 << h) - 1) << t[s] - c,
                    o += h,
                    o >= 6 && (o -= 6,
                    e++),
                    c -= h,
                    !(0 >= c); )
                        ;
                    r[s] && u >= d[t[s] - 1] && (u -= d[t[s]])
                } else
                    u = w([30, t[s] - 30], [0, r[s]]),
                    a[s] || (u = u[0] + u[1] * d[30]);
                l[s] = u
            } else
                l[s] = 0;
        return l
    }, x = function() {
        var t;
        return t = w([3])[0],
        1 == t ? (r.d = w([18], [1])[0],
        t = 0) : t || (t = w([6])[0]),
        t
    }, S = function(t) {
        var e, i, n;
        for (t > 1 && (e = 0),
        e = 0; t > e; e++)
            r.d++,
            n = r.d % 7,
            (3 == n || 4 == n) && (r.d += 5 - n);
        return i = new Date,
        i.setTime((u + r.d) * l),
        i
    }, k = function(t) {
        var e, i, n;
        for (n = r.wd || 62,
        e = 0; t > e; e++)
            do
                r.d++;
            while (!(n & 1 << (r.d % 7 + 10) % 7));
        return i = new Date,
        i.setTime((u + r.d) * l),
        i
    }, T = function(t) {
        var e, i, n;
        return t ? 0 > t ? (e = T(-t),
        [-e[0], -e[1]]) : (e = t % 3,
        i = (t - e) / 3,
        n = [i, i],
        e && n[e - 1]++,
        n) : [0, 0]
    }, P = function(t, e, i) {
        var n, r, a;
        for (r = "number" == typeof e ? T(e) : e,
        a = T(i),
        n = [a[0] - r[0], a[1] - r[1]],
        r = 1; n[0] < n[1]; )
            r *= 5,
            n[1]--;
        for (; n[1] < n[0]; )
            r *= 2,
            n[0]--;
        if (r > 1 && (t *= r),
        n = n[0],
        t = E(t),
        0 > n) {
            for (; t.length + n <= 0; )
                t = "0" + t;
            return n += t.length,
            r = t.substr(0, n) - 0,
            void 0 === i ? r + "." + t.substr(n) - 0 : (a = t.charAt(n) - 0,
            a > 5 ? r++ : 5 == a && (t.substr(n + 1) - 0 > 0 ? r++ : r += 1 & r),
            r)
        }
        for (; n > 0; n--)
            t += "0";
        return t - 0
    }, C = function() {
        var t, i, a, o, l;
        if (s >= 1)
            return [];
        for (r.d = w([18], [1])[0] - 1,
        a = w([3, 3, 30, 6]),
        r.p = a[0],
        r.ld = a[1],
        r.cd = a[2],
        r.c = a[3],
        r.m = p.pow(10, r.p),
        r.pc = r.cd / r.m,
        i = [],
        t = 0; o = {
            d: 1
        },
        y() && (a = w([3])[0],
        0 == a ? o.d = w([6])[0] : 1 == a ? (r.d = w([18])[0],
        o.d = 0) : o.d = a),
        l = {
            date: S(o.d)
        },
        y() && (r.ld += N()),
        a = w([3 * r.ld], [1]),
        r.cd += a[0],
        l.close = r.cd / r.m,
        i.push(l),
        !(e >= n) && (e != n - 1 || 63 & (r.c ^ t + 1)); t++)
            ;
        return i[0].prevclose = r.pc,
        i
    }, _ = function() {
        var t, i, a, o, l, u, c, d, h, f, m;
        if (s > 2)
            return [];
        for (c = [],
        h = {
            v: "volume",
            p: "price",
            a: "avg_price"
        },
        r.d = w([18], [1])[0] - 1,
        d = {
            date: S(1)
        },
        a = w(1 > s ? [3, 3, 4, 1, 1, 1, 5] : [4, 4, 4, 1, 1, 1, 3]),
        t = 0; 7 > t; t++)
            r[["la", "lp", "lv", "tv", "rv", "zv", "pp"][t]] = a[t];
        for (r.m = p.pow(10, r.pp),
        s >= 1 ? (a = w([3, 3]),
        r.c = a[0],
        a = a[1]) : (a = 5,
        r.c = 2),
        r.pc = w([6 * a])[0],
        d.pc = r.pc / r.m,
        r.cp = r.pc,
        r.da = 0,
        r.sa = r.sv = 0,
        t = 0; !(e >= n) && (e != n - 1 || 7 & (r.c ^ t)); t++) {
            for (l = {},
            o = {},
            f = r.tv ? y() : 1,
            i = 0; 3 > i; i++)
                if (m = ["v", "p", "a"][i],
                (f ? y() : 0) && (a = N(),
                r["l" + m] += a),
                u = "v" == m && r.rv ? y() : 1,
                a = w([3 * r["l" + m] + ("v" == m ? 7 * u : 0)], [!!i])[0] * (u ? 1 : 100),
                o[m] = a,
                "v" == m) {
                    if (!(l[h[m]] = a) && (s > 1 || 241 > t) && (r.zv ? !y() : 1)) {
                        o.p = 0;
                        break
                    }
                } else
                    "a" == m && (r.da = (1 > s ? 0 : r.da) + o.a);
            r.sv += o.v,
            l[h.p] = (r.cp += o.p) / r.m,
            r.sa += o.v * r.cp,
            l[h.a] = b(o.a) ? t ? c[t - 1][h.a] : l[h.p] : r.sv ? ((p.floor((r.sa * (2e3 / r.m) + r.sv) / r.sv) >> 1) + r.da) / 1e3 : l[h.p] + r.da / 1e3,
            c.push(l)
        }
        return c[0].date = d.date,
        c[0].prevclose = d.pc,
        c
    }, D = function() {
        var t, e, i, n, a, o, l;
        if (s >= 1)
            return [];
        for (r.lv = 0,
        r.ld = 0,
        r.cd = 0,
        r.cv = [0, 0],
        r.p = w([6])[0],
        r.d = w([18], [1])[0] - 1,
        r.m = p.pow(10, r.p),
        a = w([3, 3]),
        r.md = a[0],
        r.mv = a[1],
        t = []; a = w([6]),
        a.length; ) {
            if (i = {
                c: a[0]
            },
            n = {},
            i.d = 1,
            32 & i.c)
                for (; ; ) {
                    if (a = w([6])[0],
                    63 == (16 | a)) {
                        l = 16 & a ? "x" : "u",
                        a = w([3, 3]),
                        i[l + "_d"] = a[0] + r.md,
                        i[l + "_v"] = a[1] + r.mv;
                        break
                    }
                    if (32 & a) {
                        o = 8 & a ? "d" : "v",
                        l = 16 & a ? "x" : "u",
                        i[l + "_" + o] = (7 & a) + r["m" + o];
                        break
                    }
                    if (o = 15 & a,
                    0 == o ? i.d = w([6])[0] : 1 == o ? (r.d = o = w([18])[0],
                    i.d = 0) : i.d = o,
                    !(16 & a))
                        break
                }
            n.date = S(i.d);
            for (o in {
                v: 0,
                d: 0
            })
                b(i["x_" + o]) || (r["l" + o] = i["x_" + o]),
                b(i["u_" + o]) && (i["u_" + o] = r["l" + o]);
            for (i.l_l = [i.u_d, i.u_d, i.u_d, i.u_d, i.u_v],
            l = m[15 & i.c],
            1 & i.u_v && (l = 31 - l),
            16 & i.c && (i.l_l[4] += 2),
            e = 0; 5 > e; e++)
                l & 1 << 4 - e && i.l_l[e]++,
                i.l_l[e] *= 3;
            i.d_v = w(i.l_l, [1, 0, 0, 1, 1], [0, 0, 0, 0, 1]),
            o = r.cd + i.d_v[0],
            n.open = o / r.m,
            n.high = (o + i.d_v[1]) / r.m,
            n.low = (o - i.d_v[2]) / r.m,
            n.close = (o + i.d_v[3]) / r.m,
            a = i.d_v[4],
            "number" == typeof a && (a = [a, a >= 0 ? 0 : -1]),
            r.cd = o + i.d_v[3],
            l = r.cv[0] + a[0],
            r.cv = [l & h, r.cv[1] + a[1] + !!((r.cv[0] & h) + (a[0] & h) & f)],
            n.volume = (r.cv[0] & f - 1) + r.cv[1] * f,
            t.push(n)
        }
        return t
    }, R = function() {
        var t, e, i, n;
        if (s > 1)
            return [];
        for (r.l = 0,
        n = -1,
        r.d = w([18])[0] - 1,
        i = w([18])[0]; r.d < i; )
            e = S(1),
            0 >= n ? (y() && (r.l += N()),
            n = w([3 * r.l], [0])[0] + 1,
            t || (t = [e],
            n--)) : t.push(e),
            n--;
        return t
    }, A = function() {
        var t, i, a, o;
        if (s >= 1)
            return [];
        for (r.f = w([6])[0],
        r.c = w([6])[0],
        a = [],
        r.dv = [],
        r.dl = [],
        t = 0; t < r.f; t++)
            r.dv[t] = 0,
            r.dl[t] = 0;
        for (t = 0; !(e >= n) && (e != n - 1 || 7 & (r.c ^ t)); t++) {
            for (o = [],
            i = 0; i < r.f; i++)
                y() && (r.dl[i] += N()),
                r.dv[i] += w([3 * r.dl[i]], [1])[0],
                o[i] = r.dv[i];
            a.push(o)
        }
        return a
    }, O = function() {
        if (r = {
            b_avp: 1,
            b_ph: 0,
            b_phx: 0,
            b_sep: 0,
            p_p: 6,
            p_v: 0,
            p_a: 0,
            p_e: 0,
            p_t: 0,
            l_o: 3,
            l_h: 3,
            l_l: 3,
            l_c: 3,
            l_v: 5,
            l_a: 5,
            l_e: 3,
            l_t: 0,
            u_p: 0,
            u_v: 0,
            u_a: 0,
            wd: 62,
            d: 0
        },
        s > 0)
            return [];
        var t, i, a, o, l, u, c;
        for (t = []; ; ) {
            if (e >= n)
                return void 0;
            if (a = {
                d: 1,
                c: 0
            },
            y())
                if (y()) {
                    if (y()) {
                        for (a.c++,
                        a.a = r.b_avp,
                        y() && (r.b_avp ^= y(),
                        r.b_ph ^= y(),
                        r.b_phx ^= y(),
                        a.s = r.b_sep,
                        r.b_sep ^= y(),
                        y() && (r.wd = w([7])[0]),
                        a.s ^ r.b_sep && (a.s ? r.u_p = r.u_c : r.u_o = r.u_h = r.u_l = r.u_c = r.u_p)),
                        u = 0; u < 3 + 2 * r.b_ph; u++)
                            if (y() && (l = "pvaet".charAt(u),
                            o = r["p_" + l],
                            r["p_" + l] += N(),
                            r["u_" + l] = P(r["u_" + l], o, r["p_" + l]) - 0,
                            r.b_sep && !u))
                                for (c = 0; 4 > c; c++)
                                    l = "ohlc".charAt(c),
                                    r["u_" + l] = P(r["u_" + l], o, r.p_p) - 0;
                        !r.b_avp && a.a && (r.u_a = P(i && i.amount || 0, 0, r.p_a))
                    }
                    if (y())
                        for (a.c++,
                        u = 0; u < 7 + r.b_ph + r.b_phx; u++)
                            y() && (6 == u ? a.d = x() : r["l_" + "ohlcva*et".charAt(u)] += N());
                    if (y() && (a.c++,
                    l = r.l_o + (y() && N()),
                    o = w([3 * l], [1])[0],
                    a.p = r.b_sep ? r.u_c + o : r.u_p += o),
                    !a.c)
                        break
                } else
                    y() ? y() ? y() ? a.d = x() : r.l_v += N() : r.b_ph && y() ? r["l_" + "et".charAt(r.b_phx && y())] += N() : r.l_a += N() : r["l_" + "ohlc".charAt(w([2])[0])] += N();
            for (u = 0; u < 6 + r.b_ph + r.b_phx; u++)
                c = "ohlcvaet".charAt(u),
                o = (r.b_sep ? 191 : 185) >> u & 1,
                a["v_" + c] = w([3 * r["l_" + c]], [o])[0];
            i = {
                date: k(a.d)
            },
            a.p && (i.prevclose = P(a.p, r.p_p)),
            r.b_sep ? (i.open = P(r.u_o += a.v_o, r.p_p),
            i.high = P(r.u_h += a.v_h, r.p_p),
            i.low = P(r.u_l += a.v_l, r.p_p),
            i.close = P(r.u_c += a.v_c, r.p_p)) : (a.o = r.u_p + a.v_o,
            i.open = P(a.o, r.p_p),
            i.high = P(a.o + a.v_h, r.p_p),
            i.low = P(a.o - a.v_l, r.p_p),
            i.close = P(r.u_p = a.o + a.v_c, r.p_p)),
            i.volume = P(r.u_v += a.v_v, r.p_v),
            r.b_avp ? (o = T(r.p_p),
            l = T(r.p_v),
            i.amount = P(P(Math.floor((r.b_sep ? (r.u_o + r.u_h + r.u_l + r.u_c) / 4 : a.o + (a.v_h - a.v_l + a.v_c) / 4) * r.u_v + .5), [o[0] + l[0], o[1] + l[1]], r.p_a) + a.v_a, r.p_a)) : (r.u_a += a.v_a,
            i.amount = P(r.u_a, r.p_a)),
            r.b_ph && (i.postVol = P(a.v_e, r.p_e),
            i.postAmt = P(Math.floor(i.postVol * i.close + (r.b_phx ? P(a.v_t, r.p_t) : 0) + .5), 0)),
            t.push(i)
        }
        return t
    }, E = function(t) {
        var e, i, n;
        if (t = (t || 0).toString(),
        n = [],
        i = t.toLowerCase().indexOf("e"),
        i > 0) {
            for (e = t.substr(i + 1) - 0; e >= 0; e--)
                n.push(Math.floor(e * Math.pow(10, -e) + .5) - 0);
            return n.join("")
        }
        return t
    };
    return g()()
}