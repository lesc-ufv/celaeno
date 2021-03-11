// Benchmark "z4ml" written by ABC on Wed Mar 10 23:12:58 2021

module z4ml  
    1 , 2, 3, 4, 5, 6, 7,
    24, 25, 26, 27  ;
  input  1 , 2, 3, 4, 5, 6, 7;
  output 24, 25, 26, 27;
  wire new_n12_, new_n13_, new_n14_, new_n15_, new_n16_, new_n17_, new_n18_,
    new_n19_, new_n20_, new_n21_, new_n22_, new_n23_, new_n24_, new_n26_,
    new_n27_, new_n28_, new_n29_, new_n30_, new_n31_, new_n32_, new_n33_,
    new_n34_, new_n35_, new_n36_, new_n37_, new_n38_, new_n39_, new_n40_,
    new_n42_, new_n43_, new_n44_, new_n45_, new_n46_, new_n47_, new_n49_,
    new_n50_, new_n51_, new_n52_, new_n53_, new_n54_;
  assign new_n12_ = ~4 & ~7;
  assign new_n13_ = 1  & ~new_n12_;
  assign new_n14_ = 4 & 7;
  assign new_n15_ = ~new_n13_ & ~new_n14_;
  assign new_n16_ = ~3 & ~6;
  assign new_n17_ = ~2 & ~5;
  assign new_n18_ = ~new_n16_ & ~new_n17_;
  assign new_n19_ = ~new_n15_ & new_n18_;
  assign new_n20_ = 3 & 6;
  assign new_n21_ = ~5 & ~new_n20_;
  assign new_n22_ = 2 & ~new_n21_;
  assign new_n23_ = 5 & new_n20_;
  assign new_n24_ = ~new_n22_ & ~new_n23_;
  assign 24 = new_n19_ | ~new_n24_;
  assign new_n26_ = 2 & 5;
  assign new_n27_ = ~new_n17_ & ~new_n26_;
  assign new_n28_ = ~new_n16_ & ~new_n27_;
  assign new_n29_ = ~new_n15_ & new_n28_;
  assign new_n30_ = new_n20_ & ~new_n27_;
  assign new_n31_ = ~1  & ~new_n14_;
  assign new_n32_ = ~new_n12_ & ~new_n31_;
  assign new_n33_ = ~2 & 5;
  assign new_n34_ = 2 & ~5;
  assign new_n35_ = ~new_n33_ & ~new_n34_;
  assign new_n36_ = ~new_n20_ & ~new_n35_;
  assign new_n37_ = ~new_n32_ & new_n36_;
  assign new_n38_ = new_n16_ & ~new_n35_;
  assign new_n39_ = ~new_n30_ & ~new_n38_;
  assign new_n40_ = ~new_n29_ & new_n39_;
  assign 25 = new_n37_ | ~new_n40_;
  assign new_n42_ = ~new_n16_ & ~new_n20_;
  assign new_n43_ = ~new_n15_ & ~new_n42_;
  assign new_n44_ = ~3 & 6;
  assign new_n45_ = 3 & ~6;
  assign new_n46_ = ~new_n44_ & ~new_n45_;
  assign new_n47_ = ~new_n32_ & ~new_n46_;
  assign 26 = new_n43_ | new_n47_;
  assign new_n49_ = ~new_n12_ & ~new_n14_;
  assign new_n50_ = 1  & ~new_n49_;
  assign new_n51_ = ~4 & 7;
  assign new_n52_ = 4 & ~7;
  assign new_n53_ = ~new_n51_ & ~new_n52_;
  assign new_n54_ = ~1  & ~new_n53_;
  assign 27 = new_n50_ | new_n54_;
endmodule


