// Benchmark "CM85" written by ABC on Wed Mar 10 23:12:53 2021

module CM85  
    a, b, c, d, e, f, g, h, i, j, k,
    l, m, n  ;
  input  a, b, c, d, e, f, g, h, i, j, k;
  output l, m, n;
  wire new_n15_, new_n16_, new_n17_, new_n18_, new_n19_, new_n20_, new_n21_,
    new_n22_, new_n23_, new_n24_, new_n25_, new_n26_, new_n27_, new_n28_,
    new_n29_, new_n30_, new_n31_, new_n32_, new_n33_, new_n34_, new_n35_,
    new_n36_, new_n38_, new_n39_, new_n41_, new_n42_, new_n43_, new_n44_,
    new_n45_, new_n46_, new_n47_, new_n48_, new_n49_;
  assign new_n15_ = b & ~d;
  assign new_n16_ = e & new_n15_;
  assign new_n17_ = ~d & ~e;
  assign new_n18_ = d & e;
  assign new_n19_ = ~new_n17_ & ~new_n18_;
  assign new_n20_ = b & ~new_n19_;
  assign new_n21_ = ~f & g;
  assign new_n22_ = new_n20_ & new_n21_;
  assign new_n23_ = f & ~g;
  assign new_n24_ = ~new_n21_ & ~new_n23_;
  assign new_n25_ = new_n20_ & new_n24_;
  assign new_n26_ = ~h & i;
  assign new_n27_ = new_n25_ & new_n26_;
  assign new_n28_ = ~h & ~i;
  assign new_n29_ = h & i;
  assign new_n30_ = ~new_n28_ & ~new_n29_;
  assign new_n31_ = new_n25_ & ~new_n30_;
  assign new_n32_ = ~j & k;
  assign new_n33_ = new_n31_ & new_n32_;
  assign new_n34_ = ~a & ~new_n16_;
  assign new_n35_ = ~new_n22_ & new_n34_;
  assign new_n36_ = ~new_n27_ & new_n35_;
  assign l = new_n33_ | ~new_n36_;
  assign new_n38_ = j & ~k;
  assign new_n39_ = ~new_n32_ & ~new_n38_;
  assign m = new_n31_ & new_n39_;
  assign new_n41_ = b & d;
  assign new_n42_ = ~e & new_n41_;
  assign new_n43_ = new_n20_ & new_n23_;
  assign new_n44_ = h & ~i;
  assign new_n45_ = new_n25_ & new_n44_;
  assign new_n46_ = new_n31_ & new_n38_;
  assign new_n47_ = ~c & ~new_n42_;
  assign new_n48_ = ~new_n43_ & new_n47_;
  assign new_n49_ = ~new_n45_ & new_n48_;
  assign n = new_n46_ | ~new_n49_;
endmodule


