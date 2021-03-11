// Benchmark "CM162" written by ABC on Wed Mar 10 23:12:52 2021

module CM162  
    a, b, c, d, e, f, g, h, i, j, k, l, m, n,
    o, p, q, r, s  ;
  input  a, b, c, d, e, f, g, h, i, j, k, l, m, n;
  output o, p, q, r, s;
  wire new_n20_, new_n21_, new_n22_, new_n23_, new_n24_, new_n25_, new_n26_,
    new_n27_, new_n28_, new_n29_, new_n30_, new_n31_, new_n32_, new_n34_,
    new_n35_, new_n36_, new_n37_, new_n38_, new_n39_, new_n41_, new_n42_,
    new_n43_, new_n44_, new_n45_, new_n46_, new_n48_, new_n49_, new_n50_,
    new_n51_, new_n52_, new_n53_, new_n54_, new_n55_;
  assign new_n20_ = c & d;
  assign new_n21_ = e & new_n20_;
  assign new_n22_ = i & new_n21_;
  assign new_n23_ = ~i & ~new_n21_;
  assign new_n24_ = ~new_n22_ & ~new_n23_;
  assign new_n25_ = j & n;
  assign new_n26_ = new_n21_ & new_n25_;
  assign new_n27_ = d & f;
  assign new_n28_ = ~new_n26_ & new_n27_;
  assign new_n29_ = ~new_n24_ & new_n28_;
  assign new_n30_ = ~d & f;
  assign new_n31_ = ~new_n26_ & new_n30_;
  assign new_n32_ = a & new_n31_;
  assign o = ~new_n29_ & ~new_n32_;
  assign new_n34_ = ~i & new_n21_;
  assign new_n35_ = k & new_n34_;
  assign new_n36_ = ~k & ~new_n34_;
  assign new_n37_ = ~new_n35_ & ~new_n36_;
  assign new_n38_ = new_n28_ & ~new_n37_;
  assign new_n39_ = b & new_n31_;
  assign p = ~new_n38_ & ~new_n39_;
  assign new_n41_ = ~k & new_n34_;
  assign new_n42_ = l & new_n41_;
  assign new_n43_ = ~l & ~new_n41_;
  assign new_n44_ = ~new_n42_ & ~new_n43_;
  assign new_n45_ = new_n28_ & ~new_n44_;
  assign new_n46_ = g & new_n31_;
  assign q = ~new_n45_ & ~new_n46_;
  assign new_n48_ = ~i & ~k;
  assign new_n49_ = ~l & new_n48_;
  assign new_n50_ = new_n21_ & new_n49_;
  assign new_n51_ = m & new_n50_;
  assign new_n52_ = ~m & ~new_n50_;
  assign new_n53_ = ~new_n51_ & ~new_n52_;
  assign new_n54_ = new_n28_ & ~new_n53_;
  assign new_n55_ = h & new_n31_;
  assign r = ~new_n54_ & ~new_n55_;
  assign s = e & new_n25_;
endmodule


