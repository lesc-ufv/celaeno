// Benchmark "s27.bench" written by ABC on Wed Mar 10 23:12:56 2021

module s27.bench   clock, 
    G0, G1, G2, G3,
    G17  ;
  input  clock;
  input  G0, G1, G2, G3;
  output G17;
  reg G5, G6, G7;
  wire new_n15_, new_n16_, new_n17_1_, new_n18_, new_n19_, n12, n17, n22;
  assign new_n15_ = ~G0 & G6;
  assign new_n16_ = ~G3 & ~new_n15_;
  assign new_n17_1_ = ~G1 & ~G7;
  assign new_n18_ = ~new_n15_ & ~new_n17_1_;
  assign new_n19_ = ~G5 & ~new_n16_;
  assign n17 = ~new_n18_ & new_n19_;
  assign n12 = G0 & ~n17;
  assign n22 = ~G2 & ~new_n17_1_;
  assign G17 = ~n17;
  always @ posedge clock begin
    G5 <= n12;
    G6 <= n17;
    G7 <= n22;
  end
  initial begin
    G5 <= 1'b0;
    G6 <= 1'b0;
    G7 <= 1'b0;
  end
endmodule


