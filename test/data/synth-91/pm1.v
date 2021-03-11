// Benchmark "pm1" written by ABC on Wed Mar 10 23:12:55 2021

module pm1  
    a, b, c, d, e, g, h, i, j, k, l, m, n, o, p, q,
    r, s, t, u, v, w, x, y, z, a0, b0, c0, d0  ;
  input  a, b, c, d, e, g, h, i, j, k, l, m, n, o, p, q;
  output r, s, t, u, v, w, x, y, z, a0, b0, c0, d0;
  wire new_n30_, new_n33_, new_n34_, new_n35_, new_n36_, new_n37_, new_n39_,
    new_n40_, new_n41_, new_n43_, new_n44_, new_n45_, new_n46_, new_n48_,
    new_n49_, new_n50_, new_n51_, new_n52_, new_n53_, new_n55_, new_n56_,
    new_n58_, new_n59_, new_n60_, new_n62_, new_n63_, new_n64_, new_n65_,
    new_n66_, new_n68_;
  assign new_n30_ = ~b & ~m;
  assign r = n | ~new_n30_;
  assign s = m | ~n;
  assign new_n33_ = m & n;
  assign new_n34_ = g & h;
  assign new_n35_ = i & j;
  assign new_n36_ = k & new_n35_;
  assign new_n37_ = new_n33_ & new_n34_;
  assign t = ~new_n36_ | ~new_n37_;
  assign new_n39_ = new_n34_ & new_n35_;
  assign new_n40_ = k & m;
  assign new_n41_ = n & new_n40_;
  assign u = new_n39_ | ~new_n41_;
  assign new_n43_ = d & e;
  assign new_n44_ = c & s;
  assign new_n45_ = new_n43_ & new_n44_;
  assign new_n46_ = b & k;
  assign x = ~new_n45_ & new_n46_;
  assign new_n48_ = c & d;
  assign new_n49_ = e & k;
  assign new_n50_ = n & new_n49_;
  assign new_n51_ = new_n48_ & new_n50_;
  assign new_n52_ = a & ~l;
  assign new_n53_ = m & new_n52_;
  assign z = new_n51_ | ~new_n53_;
  assign new_n55_ = m & ~n;
  assign new_n56_ = s & new_n52_;
  assign a0 = ~new_n55_ & new_n56_;
  assign new_n58_ = c & new_n43_;
  assign new_n59_ = k & new_n52_;
  assign new_n60_ = new_n33_ & new_n59_;
  assign b0 = ~new_n58_ & new_n60_;
  assign new_n62_ = ~b & n;
  assign new_n63_ = k & n;
  assign new_n64_ = ~new_n58_ & new_n63_;
  assign new_n65_ = b & ~new_n64_;
  assign new_n66_ = new_n53_ & ~new_n62_;
  assign c0 = ~new_n65_ & new_n66_;
  assign new_n68_ = ~k & new_n52_;
  assign d0 = new_n33_ & new_n68_;
  assign v = ~p;
  assign w = ~o;
  assign y = ~q;
endmodule


