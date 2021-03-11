// Benchmark "lal" written by ABC on Wed Mar 10 23:12:54 2021

module lal  
    a, b, c, d, e, f, g, h, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y,
    z, a0,
    b0, c0, d0, e0, f0, g0, h0, i0, j0, k0, l0, m0, n0, o0, p0, q0, r0, s0,
    t0  ;
  input  a, b, c, d, e, f, g, h, j, k, l, m, n, o, p, q, r, s, t, u, v,
    w, x, y, z, a0;
  output b0, c0, d0, e0, f0, g0, h0, i0, j0, k0, l0, m0, n0, o0, p0, q0, r0,
    s0, t0;
  wire new_n47_, new_n48_, new_n49_, new_n50_, new_n51_, new_n52_, new_n53_,
    new_n54_, new_n55_, new_n56_, new_n57_, new_n59_, new_n60_, new_n61_,
    new_n62_, new_n64_, new_n65_, new_n66_, new_n67_, new_n68_, new_n69_,
    new_n70_, new_n71_, new_n72_, new_n73_, new_n74_, new_n75_, new_n76_,
    new_n77_, new_n78_, new_n83_, new_n85_, new_n87_, new_n88_, new_n89_,
    new_n91_, new_n92_, new_n93_, new_n94_, new_n96_, new_n97_, new_n98_,
    new_n100_, new_n101_, new_n102_, new_n103_, new_n104_, new_n105_,
    new_n107_, new_n108_, new_n109_, new_n110_, new_n112_, new_n113_,
    new_n114_, new_n115_, new_n116_, new_n117_, new_n119_, new_n120_,
    new_n121_, new_n122_, new_n123_, new_n124_, new_n125_, new_n127_,
    new_n128_, new_n129_, new_n130_, new_n131_;
  assign b0 = j & ~r;
  assign new_n47_ = ~z & ~a0;
  assign new_n48_ = e & f;
  assign new_n49_ = ~s & ~t;
  assign new_n50_ = ~u & new_n49_;
  assign new_n51_ = ~v & ~new_n50_;
  assign new_n52_ = w & x;
  assign new_n53_ = ~new_n51_ & new_n52_;
  assign new_n54_ = ~y & ~a0;
  assign new_n55_ = ~new_n53_ & new_n54_;
  assign new_n56_ = ~h & ~new_n47_;
  assign new_n57_ = ~new_n48_ & new_n56_;
  assign c0 = new_n55_ | ~new_n57_;
  assign new_n59_ = y & z;
  assign new_n60_ = z & new_n52_;
  assign new_n61_ = ~new_n51_ & new_n60_;
  assign new_n62_ = ~a0 & ~new_n59_;
  assign e0 = ~new_n61_ & new_n62_;
  assign new_n64_ = ~b & l;
  assign new_n65_ = ~c & m;
  assign new_n66_ = ~d & n;
  assign new_n67_ = ~a & k;
  assign new_n68_ = c & ~m;
  assign new_n69_ = d & ~n;
  assign new_n70_ = a & ~k;
  assign new_n71_ = b & ~l;
  assign new_n72_ = ~new_n64_ & ~new_n65_;
  assign new_n73_ = ~new_n66_ & ~new_n67_;
  assign new_n74_ = ~new_n68_ & ~new_n69_;
  assign new_n75_ = ~new_n70_ & ~new_n71_;
  assign new_n76_ = new_n74_ & new_n75_;
  assign new_n77_ = new_n72_ & new_n73_;
  assign new_n78_ = new_n76_ & new_n77_;
  assign f0 = ~j & ~new_n78_;
  assign g0 = ~j & ~o;
  assign h0 = ~j & p;
  assign i0 = ~g | j;
  assign new_n83_ = ~h & ~q;
  assign k0 = new_n48_ & new_n83_;
  assign new_n85_ = ~new_n48_ & new_n83_;
  assign l0 = ~s & new_n85_;
  assign new_n87_ = ~s & t;
  assign new_n88_ = s & ~t;
  assign new_n89_ = ~new_n87_ & ~new_n88_;
  assign m0 = new_n85_ & ~new_n89_;
  assign new_n91_ = s & t;
  assign new_n92_ = u & new_n91_;
  assign new_n93_ = ~u & ~new_n91_;
  assign new_n94_ = new_n85_ & ~new_n92_;
  assign n0 = ~new_n93_ & new_n94_;
  assign new_n96_ = ~v & new_n92_;
  assign new_n97_ = v & ~new_n92_;
  assign new_n98_ = new_n85_ & ~new_n96_;
  assign o0 = new_n97_ | ~new_n98_;
  assign new_n100_ = ~v & ~w;
  assign new_n101_ = new_n92_ & new_n100_;
  assign new_n102_ = u & ~v;
  assign new_n103_ = new_n91_ & new_n102_;
  assign new_n104_ = w & ~new_n103_;
  assign new_n105_ = new_n85_ & ~new_n101_;
  assign p0 = new_n104_ | ~new_n105_;
  assign new_n107_ = ~w & ~x;
  assign new_n108_ = new_n103_ & new_n107_;
  assign new_n109_ = x & ~new_n101_;
  assign new_n110_ = new_n85_ & ~new_n108_;
  assign q0 = new_n109_ | ~new_n110_;
  assign new_n112_ = ~x & ~y;
  assign new_n113_ = new_n101_ & new_n112_;
  assign new_n114_ = ~v & new_n107_;
  assign new_n115_ = new_n92_ & new_n114_;
  assign new_n116_ = y & ~new_n115_;
  assign new_n117_ = new_n85_ & ~new_n113_;
  assign r0 = new_n116_ | ~new_n117_;
  assign new_n119_ = ~y & ~z;
  assign new_n120_ = new_n115_ & new_n119_;
  assign new_n121_ = ~w & new_n91_;
  assign new_n122_ = new_n102_ & new_n112_;
  assign new_n123_ = new_n121_ & new_n122_;
  assign new_n124_ = z & ~new_n123_;
  assign new_n125_ = new_n85_ & ~new_n120_;
  assign s0 = new_n124_ | ~new_n125_;
  assign new_n127_ = new_n47_ & new_n123_;
  assign new_n128_ = ~x & new_n119_;
  assign new_n129_ = new_n101_ & new_n128_;
  assign new_n130_ = a0 & ~new_n129_;
  assign new_n131_ = new_n85_ & ~new_n127_;
  assign t0 = new_n130_ | ~new_n131_;
  assign j0 = ~e0;
  assign d0 = r;
endmodule


