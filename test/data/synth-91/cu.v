// Benchmark "cu" written by ABC on Wed Mar 10 23:12:53 2021

module cu  
    a, b, c, d, e, f, g, i, j, k, l, m, n, o,
    p, q, r, s, t, u, v, w, x, y, z  ;
  input  a, b, c, d, e, f, g, i, j, k, l, m, n, o;
  output p, q, r, s, t, u, v, w, x, y, z;
  wire new_n26_, new_n27_, new_n28_, new_n29_, new_n30_, new_n31_, new_n32_,
    new_n33_, new_n34_, new_n36_, new_n37_, new_n38_, new_n39_, new_n41_,
    new_n42_, new_n44_, new_n45_, new_n47_, new_n48_, new_n50_, new_n51_,
    new_n52_, new_n53_, new_n54_, new_n55_, new_n56_, new_n57_, new_n58_,
    new_n59_, new_n60_, new_n61_, new_n62_, new_n63_, new_n64_, new_n65_,
    new_n66_, new_n69_, new_n70_, new_n71_, new_n74_, new_n75_;
  assign new_n26_ = e & ~f;
  assign new_n27_ = c & ~new_n26_;
  assign new_n28_ = ~e & f;
  assign new_n29_ = ~c & ~new_n28_;
  assign new_n30_ = ~e & ~f;
  assign new_n31_ = e & f;
  assign new_n32_ = ~d & ~new_n30_;
  assign new_n33_ = ~new_n31_ & new_n32_;
  assign new_n34_ = ~new_n27_ & ~new_n29_;
  assign q = new_n33_ & new_n34_;
  assign new_n36_ = ~c & ~d;
  assign new_n37_ = ~o & new_n28_;
  assign new_n38_ = ~a & ~b;
  assign new_n39_ = new_n36_ & new_n38_;
  assign r = new_n37_ & new_n39_;
  assign new_n41_ = a & ~b;
  assign new_n42_ = new_n36_ & new_n41_;
  assign s = new_n37_ & new_n42_;
  assign new_n44_ = ~a & b;
  assign new_n45_ = new_n36_ & new_n44_;
  assign t = new_n37_ & new_n45_;
  assign new_n47_ = a & b;
  assign new_n48_ = new_n36_ & new_n47_;
  assign u = new_n37_ & new_n48_;
  assign new_n50_ = ~f & ~o;
  assign new_n51_ = c & ~new_n50_;
  assign new_n52_ = ~d & e;
  assign new_n53_ = ~new_n51_ & new_n52_;
  assign new_n54_ = ~b & k;
  assign new_n55_ = b & m;
  assign new_n56_ = ~new_n54_ & ~new_n55_;
  assign new_n57_ = a & ~new_n56_;
  assign new_n58_ = j & new_n38_;
  assign new_n59_ = l & new_n44_;
  assign new_n60_ = f & ~i;
  assign new_n61_ = ~n & o;
  assign new_n62_ = new_n60_ & new_n61_;
  assign new_n63_ = ~new_n58_ & new_n62_;
  assign new_n64_ = ~new_n59_ & new_n63_;
  assign new_n65_ = ~new_n57_ & new_n64_;
  assign new_n66_ = ~c & ~new_n65_;
  assign v = new_n53_ & ~new_n66_;
  assign w = new_n36_ & new_n37_;
  assign new_n69_ = f & ~n;
  assign new_n70_ = o & new_n69_;
  assign new_n71_ = ~c & ~new_n70_;
  assign x = new_n53_ & ~new_n71_;
  assign y = g & o;
  assign new_n74_ = c & f;
  assign new_n75_ = ~d & g;
  assign z = ~new_n74_ & new_n75_;
  assign p = ~q;
endmodule


