// Benchmark "cc" written by ABC on Wed Mar 10 23:12:52 2021

module cc  
    a, b, c, d, e, f, g, h, i, j, k, l, m, o, p, q, r, s, t, u, v,
    w, x, y, z, a0, b0, c0, d0, e0, f0, g0, h0, i0, j0, k0, l0, m0, n0, o0,
    p0  ;
  input  a, b, c, d, e, f, g, h, i, j, k, l, m, o, p, q, r, s, t, u, v;
  output w, x, y, z, a0, b0, c0, d0, e0, f0, g0, h0, i0, j0, k0, l0, m0, n0,
    o0, p0;
  wire new_n43_, new_n44_, new_n46_, new_n47_, new_n48_, new_n49_, new_n51_,
    new_n52_, new_n55_, new_n56_, new_n57_, new_n58_, new_n60_, new_n61_,
    new_n62_, new_n63_, new_n64_, new_n66_, new_n67_, new_n68_, new_n69_,
    new_n71_, new_n72_, new_n73_, new_n74_, new_n75_, new_n77_, new_n78_,
    new_n79_, new_n80_, new_n82_, new_n83_, new_n84_, new_n85_, new_n87_,
    new_n88_, new_n89_, new_n90_, new_n92_, new_n93_, new_n94_, new_n95_;
  assign w = l & v;
  assign new_n43_ = i & k;
  assign new_n44_ = ~p & q;
  assign x = new_n43_ & new_n44_;
  assign new_n46_ = k & ~q;
  assign new_n47_ = i & new_n46_;
  assign new_n48_ = ~p & ~new_n47_;
  assign new_n49_ = ~l & m;
  assign y = ~new_n48_ & new_n49_;
  assign new_n51_ = m & ~p;
  assign new_n52_ = q & new_n51_;
  assign z = new_n43_ & new_n52_;
  assign f0 = i & j;
  assign new_n55_ = ~a & i;
  assign new_n56_ = new_n46_ & new_n55_;
  assign new_n57_ = ~o & ~new_n47_;
  assign new_n58_ = m & ~new_n56_;
  assign i0 = ~new_n57_ & new_n58_;
  assign new_n60_ = b & i;
  assign new_n61_ = new_n46_ & new_n60_;
  assign new_n62_ = q & new_n43_;
  assign new_n63_ = ~p & ~new_n62_;
  assign new_n64_ = m & ~new_n61_;
  assign j0 = ~new_n63_ & new_n64_;
  assign new_n66_ = ~q & ~new_n43_;
  assign new_n67_ = c & ~q;
  assign new_n68_ = new_n43_ & ~new_n67_;
  assign new_n69_ = m & ~new_n66_;
  assign k0 = ~new_n68_ & new_n69_;
  assign new_n71_ = ~d & i;
  assign new_n72_ = new_n46_ & new_n71_;
  assign new_n73_ = ~p & r;
  assign new_n74_ = ~new_n47_ & ~new_n73_;
  assign new_n75_ = m & ~new_n72_;
  assign l0 = ~new_n74_ & new_n75_;
  assign new_n77_ = ~e & i;
  assign new_n78_ = new_n46_ & new_n77_;
  assign new_n79_ = ~s & ~new_n47_;
  assign new_n80_ = m & ~new_n78_;
  assign m0 = ~new_n79_ & new_n80_;
  assign new_n82_ = ~f & i;
  assign new_n83_ = new_n46_ & new_n82_;
  assign new_n84_ = ~t & ~new_n47_;
  assign new_n85_ = m & ~new_n83_;
  assign n0 = ~new_n84_ & new_n85_;
  assign new_n87_ = ~g & i;
  assign new_n88_ = new_n46_ & new_n87_;
  assign new_n89_ = ~u & ~new_n47_;
  assign new_n90_ = m & ~new_n88_;
  assign o0 = ~new_n89_ & new_n90_;
  assign new_n92_ = ~h & i;
  assign new_n93_ = new_n46_ & new_n92_;
  assign new_n94_ = ~v & ~new_n47_;
  assign new_n95_ = m & ~new_n93_;
  assign p0 = ~new_n94_ & new_n95_;
  assign a0 = ~t;
  assign g0 = ~f0;
  assign b0 = u;
  assign c0 = q;
  assign d0 = s;
  assign e0 = r;
  assign h0 = p;
endmodule


