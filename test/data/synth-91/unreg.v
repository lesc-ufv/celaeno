// Benchmark "unreg" written by ABC on Wed Mar 10 23:12:58 2021

module unreg  
    a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, s, t, u, v, w, x, y,
    z, a0, b0, c0, d0, e0, f0, g0, h0, i0, j0, k0,
    l0, m0, n0, o0, p0, q0, r0, s0, t0, u0, v0, w0, x0, y0, z0, a1  ;
  input  a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, s, t, u, v,
    w, x, y, z, a0, b0, c0, d0, e0, f0, g0, h0, i0, j0, k0;
  output l0, m0, n0, o0, p0, q0, r0, s0, t0, u0, v0, w0, x0, y0, z0, a1;
  wire new_n53_, new_n54_, new_n55_, new_n56_, new_n57_, new_n58_, new_n60_,
    new_n61_, new_n62_, new_n63_, new_n64_, new_n66_, new_n67_, new_n68_,
    new_n69_, new_n70_, new_n72_, new_n73_, new_n74_, new_n75_, new_n76_,
    new_n78_, new_n79_, new_n80_, new_n81_, new_n82_, new_n84_, new_n85_,
    new_n86_, new_n87_, new_n88_, new_n90_, new_n91_, new_n92_, new_n93_,
    new_n94_, new_n96_, new_n97_, new_n98_, new_n99_, new_n100_, new_n102_,
    new_n103_, new_n104_, new_n105_, new_n106_, new_n108_, new_n109_,
    new_n110_, new_n111_, new_n112_, new_n114_, new_n115_, new_n116_,
    new_n117_, new_n118_, new_n120_, new_n121_, new_n122_, new_n123_,
    new_n124_, new_n126_, new_n127_, new_n128_, new_n129_, new_n130_,
    new_n132_, new_n133_, new_n134_, new_n135_, new_n136_, new_n138_,
    new_n139_, new_n140_, new_n141_, new_n142_, new_n144_, new_n145_,
    new_n146_, new_n147_, new_n148_;
  assign new_n53_ = ~u & ~v;
  assign new_n54_ = t & w;
  assign new_n55_ = ~d & ~t;
  assign new_n56_ = ~s & u;
  assign new_n57_ = ~new_n54_ & new_n56_;
  assign new_n58_ = ~new_n55_ & new_n57_;
  assign l0 = new_n53_ | new_n58_;
  assign new_n60_ = ~u & ~w;
  assign new_n61_ = t & x;
  assign new_n62_ = ~c & ~t;
  assign new_n63_ = new_n56_ & ~new_n61_;
  assign new_n64_ = ~new_n62_ & new_n63_;
  assign m0 = new_n60_ | new_n64_;
  assign new_n66_ = ~u & ~x;
  assign new_n67_ = t & y;
  assign new_n68_ = ~b & ~t;
  assign new_n69_ = new_n56_ & ~new_n67_;
  assign new_n70_ = ~new_n68_ & new_n69_;
  assign n0 = new_n66_ | new_n70_;
  assign new_n72_ = ~u & ~y;
  assign new_n73_ = ~a & ~t;
  assign new_n74_ = ~q & t;
  assign new_n75_ = new_n56_ & ~new_n73_;
  assign new_n76_ = ~new_n74_ & new_n75_;
  assign o0 = new_n72_ | new_n76_;
  assign new_n78_ = ~u & ~z;
  assign new_n79_ = t & a0;
  assign new_n80_ = ~h & ~t;
  assign new_n81_ = new_n56_ & ~new_n79_;
  assign new_n82_ = ~new_n80_ & new_n81_;
  assign p0 = new_n78_ | new_n82_;
  assign new_n84_ = ~u & ~a0;
  assign new_n85_ = t & b0;
  assign new_n86_ = ~g & ~t;
  assign new_n87_ = new_n56_ & ~new_n85_;
  assign new_n88_ = ~new_n86_ & new_n87_;
  assign q0 = new_n84_ | new_n88_;
  assign new_n90_ = ~u & ~b0;
  assign new_n91_ = t & c0;
  assign new_n92_ = ~f & ~t;
  assign new_n93_ = new_n56_ & ~new_n91_;
  assign new_n94_ = ~new_n92_ & new_n93_;
  assign r0 = new_n90_ | new_n94_;
  assign new_n96_ = ~u & ~c0;
  assign new_n97_ = t & v;
  assign new_n98_ = ~e & ~t;
  assign new_n99_ = new_n56_ & ~new_n97_;
  assign new_n100_ = ~new_n98_ & new_n99_;
  assign s0 = new_n96_ | new_n100_;
  assign new_n102_ = ~u & ~d0;
  assign new_n103_ = t & e0;
  assign new_n104_ = ~l & ~t;
  assign new_n105_ = new_n56_ & ~new_n103_;
  assign new_n106_ = ~new_n104_ & new_n105_;
  assign t0 = new_n102_ | new_n106_;
  assign new_n108_ = ~u & ~e0;
  assign new_n109_ = t & f0;
  assign new_n110_ = ~k & ~t;
  assign new_n111_ = new_n56_ & ~new_n109_;
  assign new_n112_ = ~new_n110_ & new_n111_;
  assign u0 = new_n108_ | new_n112_;
  assign new_n114_ = ~u & ~f0;
  assign new_n115_ = t & g0;
  assign new_n116_ = ~j & ~t;
  assign new_n117_ = new_n56_ & ~new_n115_;
  assign new_n118_ = ~new_n116_ & new_n117_;
  assign v0 = new_n114_ | new_n118_;
  assign new_n120_ = ~u & ~g0;
  assign new_n121_ = t & z;
  assign new_n122_ = ~i & ~t;
  assign new_n123_ = new_n56_ & ~new_n121_;
  assign new_n124_ = ~new_n122_ & new_n123_;
  assign w0 = new_n120_ | new_n124_;
  assign new_n126_ = ~u & ~h0;
  assign new_n127_ = t & i0;
  assign new_n128_ = ~p & ~t;
  assign new_n129_ = new_n56_ & ~new_n127_;
  assign new_n130_ = ~new_n128_ & new_n129_;
  assign x0 = new_n126_ | new_n130_;
  assign new_n132_ = ~u & ~i0;
  assign new_n133_ = t & j0;
  assign new_n134_ = ~o & ~t;
  assign new_n135_ = new_n56_ & ~new_n133_;
  assign new_n136_ = ~new_n134_ & new_n135_;
  assign y0 = new_n132_ | new_n136_;
  assign new_n138_ = ~u & ~j0;
  assign new_n139_ = t & k0;
  assign new_n140_ = ~n & ~t;
  assign new_n141_ = new_n56_ & ~new_n139_;
  assign new_n142_ = ~new_n140_ & new_n141_;
  assign z0 = new_n138_ | new_n142_;
  assign new_n144_ = ~u & ~k0;
  assign new_n145_ = t & d0;
  assign new_n146_ = ~m & ~t;
  assign new_n147_ = new_n56_ & ~new_n145_;
  assign new_n148_ = ~new_n146_ & new_n147_;
  assign a1 = new_n144_ | new_n148_;
endmodule


