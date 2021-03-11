// Benchmark "cordic" written by ABC on Wed Mar 10 23:12:53 2021

module cordic  
    a6, a4, a3, a2, a5, v, x0, x1, x2, x3, y0, y1, y2, y3, z0, z1, z2, ex0,
    ex1, ex2, ey0, ey1, ey2,
    d, dn  ;
  input  a6, a4, a3, a2, a5, v, x0, x1, x2, x3, y0, y1, y2, y3, z0, z1,
    z2, ex0, ex1, ex2, ey0, ey1, ey2;
  output d, dn;
  wire new_n26_, new_n27_, new_n28_, new_n29_, new_n30_, new_n31_, new_n32_,
    new_n33_, new_n34_, new_n35_, new_n36_, new_n37_, new_n38_, new_n39_,
    new_n40_, new_n41_, new_n42_, new_n43_, new_n44_, new_n45_, new_n46_,
    new_n47_, new_n48_, new_n49_, new_n50_, new_n51_, new_n52_, new_n53_,
    new_n54_, new_n55_, new_n56_, new_n57_, new_n58_, new_n59_, new_n60_,
    new_n61_, new_n62_, new_n63_, new_n64_, new_n65_, new_n66_, new_n67_,
    new_n68_, new_n69_, new_n70_, new_n71_, new_n72_, new_n73_, new_n74_,
    new_n76_, new_n77_, new_n78_, new_n79_, new_n80_, new_n81_, new_n82_,
    new_n83_, new_n84_, new_n85_, new_n86_, new_n87_, new_n88_, new_n89_,
    new_n90_, new_n91_, new_n92_, new_n93_, new_n94_, new_n95_, new_n96_,
    new_n97_, new_n98_, new_n99_, new_n100_, new_n101_, new_n102_,
    new_n103_, new_n104_;
  assign new_n26_ = ~a6 & ~a4;
  assign new_n27_ = ~a3 & ~a2;
  assign new_n28_ = ~a5 & new_n27_;
  assign new_n29_ = new_n26_ & new_n28_;
  assign new_n30_ = a6 & a4;
  assign new_n31_ = new_n27_ & new_n30_;
  assign new_n32_ = ~ey0 & ~ey2;
  assign new_n33_ = ~ey1 & ~new_n32_;
  assign new_n34_ = a6 & ~a4;
  assign new_n35_ = a3 & ~a2;
  assign new_n36_ = new_n34_ & new_n35_;
  assign new_n37_ = y0 & ~y1;
  assign new_n38_ = ~y0 & y1;
  assign new_n39_ = ~new_n37_ & ~new_n38_;
  assign new_n40_ = y2 & ~y3;
  assign new_n41_ = ~y2 & y3;
  assign new_n42_ = ~new_n40_ & ~new_n41_;
  assign new_n43_ = new_n39_ & ~new_n42_;
  assign new_n44_ = x2 & ~x3;
  assign new_n45_ = ~x2 & x3;
  assign new_n46_ = ~new_n44_ & ~new_n45_;
  assign new_n47_ = x0 & ~x1;
  assign new_n48_ = ~x0 & x1;
  assign new_n49_ = ~new_n47_ & ~new_n48_;
  assign new_n50_ = new_n46_ & ~new_n49_;
  assign new_n51_ = ~z0 & z1;
  assign new_n52_ = z2 & new_n51_;
  assign new_n53_ = ~new_n39_ & new_n42_;
  assign new_n54_ = ~z1 & ~z2;
  assign new_n55_ = z0 & new_n54_;
  assign new_n56_ = ~new_n46_ & new_n49_;
  assign new_n57_ = ~new_n52_ & ~new_n55_;
  assign new_n58_ = ~new_n43_ & new_n57_;
  assign new_n59_ = ~new_n50_ & ~new_n53_;
  assign new_n60_ = ~new_n56_ & new_n59_;
  assign new_n61_ = new_n58_ & new_n60_;
  assign new_n62_ = ~new_n36_ & ~new_n61_;
  assign new_n63_ = ey0 & ey2;
  assign new_n64_ = ey1 & ~new_n63_;
  assign new_n65_ = ex0 & ex2;
  assign new_n66_ = ex1 & ~new_n65_;
  assign new_n67_ = ~ex0 & ~ex2;
  assign new_n68_ = ~ex1 & ~new_n67_;
  assign new_n69_ = ~new_n33_ & ~new_n64_;
  assign new_n70_ = ~new_n66_ & ~new_n68_;
  assign new_n71_ = new_n69_ & new_n70_;
  assign new_n72_ = ~new_n62_ & new_n71_;
  assign new_n73_ = ~new_n31_ & ~new_n72_;
  assign new_n74_ = ~v & ~new_n73_;
  assign d = ~new_n29_ & ~new_n74_;
  assign new_n76_ = ~y0 & ~y1;
  assign new_n77_ = y0 & y1;
  assign new_n78_ = ~new_n76_ & ~new_n77_;
  assign new_n79_ = new_n42_ & ~new_n78_;
  assign new_n80_ = ~new_n42_ & new_n78_;
  assign new_n81_ = ~new_n79_ & ~new_n80_;
  assign new_n82_ = z0 & ~new_n54_;
  assign new_n83_ = z1 & z2;
  assign new_n84_ = ~z0 & ~new_n83_;
  assign new_n85_ = ~new_n82_ & ~new_n84_;
  assign new_n86_ = ~x0 & ~x1;
  assign new_n87_ = x0 & x1;
  assign new_n88_ = ~new_n86_ & ~new_n87_;
  assign new_n89_ = new_n46_ & ~new_n88_;
  assign new_n90_ = ~new_n46_ & new_n88_;
  assign new_n91_ = ~new_n89_ & ~new_n90_;
  assign new_n92_ = ~ex1 & new_n67_;
  assign new_n93_ = ex1 & new_n65_;
  assign new_n94_ = ~new_n92_ & ~new_n93_;
  assign new_n95_ = ~ey1 & new_n32_;
  assign new_n96_ = ey1 & new_n63_;
  assign new_n97_ = ~new_n95_ & ~new_n96_;
  assign new_n98_ = ~new_n85_ & ~new_n94_;
  assign new_n99_ = ~new_n97_ & new_n98_;
  assign new_n100_ = ~new_n81_ & ~new_n91_;
  assign new_n101_ = new_n99_ & new_n100_;
  assign new_n102_ = ~new_n31_ & ~new_n36_;
  assign new_n103_ = ~new_n101_ & new_n102_;
  assign new_n104_ = ~v & ~new_n103_;
  assign dn = new_n29_ | new_n104_;
endmodule


