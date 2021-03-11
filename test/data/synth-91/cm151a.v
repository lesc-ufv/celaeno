// Benchmark "CM151" written by ABC on Wed Mar 10 23:12:52 2021

module CM151  
    a, b, c, d, e, f, g, h, i, j, k, l,
    m, n  ;
  input  a, b, c, d, e, f, g, h, i, j, k, l;
  output m, n;
  wire new_n15_, new_n16_, new_n17_, new_n18_, new_n19_, new_n20_, new_n21_,
    new_n22_, new_n23_, new_n24_, new_n25_, new_n26_, new_n27_, new_n28_,
    new_n29_, new_n30_, new_n31_, new_n32_, new_n33_, new_n34_, new_n35_,
    new_n36_, new_n37_, new_n38_, new_n39_, new_n40_, new_n41_, new_n42_;
  assign new_n15_ = b & i;
  assign new_n16_ = ~a & ~new_n15_;
  assign new_n17_ = ~b & i;
  assign new_n18_ = d & i;
  assign new_n19_ = ~c & ~new_n18_;
  assign new_n20_ = ~d & i;
  assign new_n21_ = ~new_n19_ & ~new_n20_;
  assign new_n22_ = j & ~new_n21_;
  assign new_n23_ = ~new_n16_ & ~new_n17_;
  assign new_n24_ = ~new_n22_ & new_n23_;
  assign new_n25_ = j & new_n21_;
  assign new_n26_ = f & i;
  assign new_n27_ = ~e & ~new_n26_;
  assign new_n28_ = ~f & i;
  assign new_n29_ = h & i;
  assign new_n30_ = ~g & ~new_n29_;
  assign new_n31_ = ~h & i;
  assign new_n32_ = ~new_n30_ & ~new_n31_;
  assign new_n33_ = j & ~new_n32_;
  assign new_n34_ = ~new_n27_ & ~new_n28_;
  assign new_n35_ = ~new_n33_ & new_n34_;
  assign new_n36_ = j & new_n32_;
  assign new_n37_ = ~new_n35_ & ~new_n36_;
  assign new_n38_ = k & ~new_n37_;
  assign new_n39_ = ~new_n24_ & ~new_n25_;
  assign new_n40_ = ~new_n38_ & new_n39_;
  assign new_n41_ = k & new_n37_;
  assign new_n42_ = ~l & ~new_n41_;
  assign m = ~new_n40_ & new_n42_;
  assign n = ~m;
endmodule


