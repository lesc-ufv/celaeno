// Benchmark "x2" written by ABC on Wed Mar 10 23:12:58 2021

module x2  
    a, b, c, d, e, f, g, h, i, j,
    k, l, m, n, o, p, q  ;
  input  a, b, c, d, e, f, g, h, i, j;
  output k, l, m, n, o, p, q;
  wire new_n18_, new_n19_, new_n20_, new_n21_, new_n22_, new_n23_, new_n24_,
    new_n25_, new_n26_, new_n28_, new_n29_, new_n31_, new_n33_, new_n34_,
    new_n35_, new_n36_, new_n37_, new_n38_, new_n40_, new_n41_, new_n42_,
    new_n44_, new_n45_, new_n46_, new_n47_, new_n48_, new_n49_, new_n50_,
    new_n51_, new_n52_, new_n53_, new_n54_, new_n55_, new_n56_, new_n57_,
    new_n59_, new_n60_, new_n61_, new_n62_, new_n63_, new_n64_, new_n65_,
    new_n66_, new_n67_;
  assign new_n18_ = i & j;
  assign new_n19_ = ~h & j;
  assign new_n20_ = ~new_n18_ & ~new_n19_;
  assign new_n21_ = ~h & i;
  assign new_n22_ = h & ~i;
  assign new_n23_ = j & new_n22_;
  assign new_n24_ = ~i & ~j;
  assign new_n25_ = ~new_n21_ & ~new_n24_;
  assign new_n26_ = new_n20_ & new_n25_;
  assign k = new_n23_ | ~new_n26_;
  assign new_n28_ = h & ~j;
  assign new_n29_ = ~new_n21_ & ~new_n28_;
  assign l = ~new_n20_ | ~new_n29_;
  assign new_n31_ = ~h & ~i;
  assign m = ~j & new_n31_;
  assign new_n33_ = ~a & ~b;
  assign new_n34_ = ~c & ~new_n18_;
  assign new_n35_ = ~new_n19_ & ~new_n28_;
  assign new_n36_ = new_n33_ & new_n35_;
  assign new_n37_ = ~new_n23_ & new_n34_;
  assign new_n38_ = ~m & new_n37_;
  assign n = ~new_n36_ | ~new_n38_;
  assign new_n40_ = g & ~new_n18_;
  assign new_n41_ = ~new_n19_ & ~new_n21_;
  assign new_n42_ = new_n40_ & new_n41_;
  assign o = m | ~new_n42_;
  assign new_n44_ = c & ~h;
  assign new_n45_ = new_n33_ & new_n44_;
  assign new_n46_ = j & ~new_n45_;
  assign new_n47_ = ~i & ~new_n46_;
  assign new_n48_ = c & new_n33_;
  assign new_n49_ = new_n18_ & new_n48_;
  assign new_n50_ = d & ~e;
  assign new_n51_ = ~j & new_n50_;
  assign new_n52_ = ~new_n49_ & ~new_n51_;
  assign new_n53_ = h & ~new_n52_;
  assign new_n54_ = f & ~h;
  assign new_n55_ = new_n18_ & new_n54_;
  assign new_n56_ = g & ~new_n55_;
  assign new_n57_ = ~new_n47_ & new_n56_;
  assign p = new_n53_ | ~new_n57_;
  assign new_n59_ = ~c & j;
  assign new_n60_ = new_n33_ & new_n59_;
  assign new_n61_ = d & e;
  assign new_n62_ = i & ~j;
  assign new_n63_ = new_n61_ & new_n62_;
  assign new_n64_ = ~new_n60_ & ~new_n63_;
  assign new_n65_ = h & ~new_n64_;
  assign new_n66_ = ~new_n23_ & ~m;
  assign new_n67_ = new_n56_ & new_n66_;
  assign q = new_n65_ | ~new_n67_;
endmodule


