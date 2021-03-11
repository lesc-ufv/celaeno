// Benchmark "b9" written by ABC on Wed Mar 10 23:12:51 2021

module b9  
    a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x,
    y, z, a0, b0, c0, d0, e0, f0, g0, h0, i0, j0, k0, l0, m0, n0, o0,
    p0, q0, r0, s0, t0, u0, v0, w0, x0, y0, z0, a1, b1, c1, d1, e1, f1, g1,
    h1, i1, j1  ;
  input  a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u,
    v, w, x, y, z, a0, b0, c0, d0, e0, f0, g0, h0, i0, j0, k0, l0, m0, n0,
    o0;
  output p0, q0, r0, s0, t0, u0, v0, w0, x0, y0, z0, a1, b1, c1, d1, e1, f1,
    g1, h1, i1, j1;
  wire new_n63_, new_n64_, new_n65_, new_n66_, new_n67_, new_n68_, new_n69_,
    new_n70_, new_n71_, new_n73_, new_n75_, new_n77_, new_n78_, new_n79_,
    new_n80_, new_n81_, new_n82_, new_n83_, new_n84_, new_n86_, new_n87_,
    new_n88_, new_n89_, new_n90_, new_n91_, new_n93_, new_n94_, new_n95_,
    new_n98_, new_n100_, new_n101_, new_n102_, new_n103_, new_n104_,
    new_n105_, new_n106_, new_n107_, new_n108_, new_n110_, new_n111_,
    new_n113_, new_n114_, new_n115_, new_n116_, new_n117_, new_n118_,
    new_n119_, new_n120_, new_n121_, new_n122_, new_n123_, new_n124_,
    new_n125_, new_n127_, new_n128_, new_n129_, new_n130_, new_n131_,
    new_n132_, new_n133_, new_n135_, new_n136_, new_n137_, new_n138_,
    new_n139_, new_n140_, new_n141_, new_n142_, new_n143_, new_n144_,
    new_n146_, new_n148_, new_n150_, new_n152_, new_n153_, new_n154_,
    new_n155_;
  assign new_n63_ = c0 & j0;
  assign new_n64_ = ~l0 & ~new_n63_;
  assign new_n65_ = b0 & ~new_n64_;
  assign new_n66_ = ~e & k;
  assign new_n67_ = ~c0 & j0;
  assign new_n68_ = ~k0 & ~new_n67_;
  assign new_n69_ = ~e & b0;
  assign new_n70_ = ~new_n68_ & new_n69_;
  assign new_n71_ = ~new_n66_ & ~new_n70_;
  assign x0 = n0 & o0;
  assign new_n73_ = ~new_n71_ & x0;
  assign b1 = ~new_n65_ & ~new_n73_;
  assign new_n75_ = ~p & ~b1;
  assign p0 = ~q | new_n75_;
  assign new_n77_ = ~e0 & ~g0;
  assign new_n78_ = e & ~new_n77_;
  assign new_n79_ = e & b0;
  assign new_n80_ = k0 & new_n79_;
  assign new_n81_ = b0 & j0;
  assign new_n82_ = e & ~c0;
  assign new_n83_ = new_n81_ & new_n82_;
  assign new_n84_ = ~new_n78_ & ~new_n80_;
  assign q0 = ~new_n83_ & new_n84_;
  assign new_n86_ = i & d0;
  assign new_n87_ = ~c & ~new_n86_;
  assign new_n88_ = ~i & ~b0;
  assign new_n89_ = new_n67_ & ~new_n88_;
  assign new_n90_ = e & ~new_n89_;
  assign new_n91_ = x0 & ~new_n87_;
  assign r0 = new_n90_ | ~new_n91_;
  assign new_n93_ = ~b0 & l0;
  assign new_n94_ = b0 & new_n63_;
  assign new_n95_ = ~new_n93_ & ~new_n94_;
  assign s0 = ~v | new_n95_;
  assign t0 = v | new_n95_;
  assign new_n98_ = b0 & c0;
  assign v0 = ~l0 & ~new_n98_;
  assign new_n100_ = ~p & r;
  assign new_n101_ = ~f0 & ~h0;
  assign new_n102_ = ~new_n100_ & new_n101_;
  assign new_n103_ = o & ~new_n102_;
  assign new_n104_ = ~o & z;
  assign new_n105_ = a & ~z;
  assign new_n106_ = m0 & ~new_n104_;
  assign new_n107_ = ~new_n105_ & new_n106_;
  assign new_n108_ = d & ~new_n103_;
  assign w0 = ~new_n107_ & new_n108_;
  assign new_n110_ = a0 & i0;
  assign new_n111_ = l & b0;
  assign y0 = new_n110_ & new_n111_;
  assign new_n113_ = h & new_n69_;
  assign new_n114_ = ~x0 & new_n113_;
  assign new_n115_ = ~new_n68_ & new_n114_;
  assign new_n116_ = g & b0;
  assign new_n117_ = l0 & new_n116_;
  assign new_n118_ = b0 & o0;
  assign new_n119_ = ~new_n68_ & new_n118_;
  assign new_n120_ = o0 & ~new_n77_;
  assign new_n121_ = ~new_n119_ & ~new_n120_;
  assign new_n122_ = ~e & f;
  assign new_n123_ = n0 & new_n122_;
  assign new_n124_ = ~new_n121_ & new_n123_;
  assign new_n125_ = ~new_n115_ & ~new_n117_;
  assign z0 = new_n124_ | ~new_n125_;
  assign new_n127_ = i & ~b0;
  assign new_n128_ = new_n67_ & new_n127_;
  assign new_n129_ = ~j & ~e0;
  assign new_n130_ = new_n67_ & new_n69_;
  assign new_n131_ = x0 & new_n86_;
  assign new_n132_ = ~new_n128_ & new_n129_;
  assign new_n133_ = ~new_n130_ & ~new_n131_;
  assign a1 = new_n132_ & new_n133_;
  assign new_n135_ = ~s & ~t;
  assign new_n136_ = u & new_n135_;
  assign new_n137_ = new_n65_ & new_n136_;
  assign new_n138_ = j0 & k0;
  assign new_n139_ = c0 & ~new_n138_;
  assign new_n140_ = b0 & ~new_n139_;
  assign new_n141_ = new_n77_ & ~new_n140_;
  assign new_n142_ = ~e & ~n;
  assign new_n143_ = x0 & new_n142_;
  assign new_n144_ = ~new_n141_ & new_n143_;
  assign d1 = ~new_n137_ & ~new_n144_;
  assign new_n146_ = m & b0;
  assign e1 = new_n110_ & new_n146_;
  assign new_n148_ = b & w;
  assign f1 = ~x & new_n148_;
  assign new_n150_ = x & ~y;
  assign g1 = new_n148_ & new_n150_;
  assign new_n152_ = i & ~c0;
  assign new_n153_ = j0 & new_n152_;
  assign new_n154_ = ~new_n81_ & new_n129_;
  assign new_n155_ = ~new_n131_ & new_n154_;
  assign j1 = ~new_n153_ & new_n155_;
  assign u0 = ~v0;
  assign c1 = ~d1;
  assign h1 = ~a1;
  assign i1 = a1;
endmodule


