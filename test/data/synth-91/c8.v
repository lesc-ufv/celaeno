// Benchmark "c8" written by ABC on Wed Mar 10 23:12:52 2021

module c8  
    a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, u, v, w, x, y,
    z, a0, b0, c0,
    d0, e0, f0, g0, h0, i0, j0, k0, l0, m0, n0, o0, p0, q0, r0, s0, t0, u0  ;
  input  a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, u, v,
    w, x, y, z, a0, b0, c0;
  output d0, e0, f0, g0, h0, i0, j0, k0, l0, m0, n0, o0, p0, q0, r0, s0, t0,
    u0;
  wire new_n47_, new_n48_, new_n49_, new_n51_, new_n52_, new_n53_, new_n55_,
    new_n56_, new_n57_, new_n59_, new_n60_, new_n61_, new_n63_, new_n64_,
    new_n65_, new_n67_, new_n68_, new_n69_, new_n71_, new_n72_, new_n73_,
    new_n75_, new_n76_, new_n77_, new_n79_, new_n80_, new_n81_, new_n82_,
    new_n83_, new_n84_, new_n85_, new_n86_, new_n87_, new_n88_, new_n89_,
    new_n91_, new_n92_, new_n93_, new_n94_, new_n95_, new_n96_, new_n97_,
    new_n98_, new_n99_, new_n100_, new_n102_, new_n103_, new_n104_,
    new_n105_, new_n106_, new_n107_, new_n108_, new_n109_, new_n110_,
    new_n111_, new_n112_, new_n113_, new_n115_, new_n116_, new_n117_,
    new_n118_, new_n119_, new_n120_, new_n121_, new_n122_, new_n123_,
    new_n124_, new_n125_, new_n126_, new_n128_, new_n129_, new_n130_,
    new_n131_, new_n132_, new_n133_, new_n134_, new_n135_, new_n136_,
    new_n137_, new_n138_, new_n139_, new_n140_, new_n142_, new_n143_,
    new_n144_, new_n145_, new_n146_, new_n147_, new_n148_, new_n149_,
    new_n150_, new_n151_, new_n152_, new_n153_, new_n154_, new_n155_,
    new_n156_, new_n158_, new_n159_, new_n160_, new_n161_, new_n162_,
    new_n163_, new_n164_, new_n165_, new_n166_, new_n167_, new_n168_,
    new_n169_, new_n170_, new_n171_, new_n172_, new_n174_, new_n175_,
    new_n176_, new_n177_, new_n178_, new_n179_, new_n180_, new_n181_,
    new_n182_, new_n183_, new_n184_, new_n185_, new_n186_, new_n187_,
    new_n188_, new_n189_, new_n191_, new_n192_, new_n193_, new_n194_,
    new_n195_, new_n196_, new_n197_, new_n198_;
  assign new_n47_ = u & ~c0;
  assign new_n48_ = ~i & ~new_n47_;
  assign new_n49_ = ~u & ~c0;
  assign d0 = new_n48_ | new_n49_;
  assign new_n51_ = v & ~c0;
  assign new_n52_ = ~j & ~new_n51_;
  assign new_n53_ = ~v & ~c0;
  assign e0 = new_n52_ | new_n53_;
  assign new_n55_ = w & ~c0;
  assign new_n56_ = ~k & ~new_n55_;
  assign new_n57_ = ~w & ~c0;
  assign f0 = new_n56_ | new_n57_;
  assign new_n59_ = x & ~c0;
  assign new_n60_ = ~l & ~new_n59_;
  assign new_n61_ = ~x & ~c0;
  assign g0 = new_n60_ | new_n61_;
  assign new_n63_ = y & ~c0;
  assign new_n64_ = ~m & ~new_n63_;
  assign new_n65_ = ~y & ~c0;
  assign h0 = new_n64_ | new_n65_;
  assign new_n67_ = z & ~c0;
  assign new_n68_ = ~n & ~new_n67_;
  assign new_n69_ = ~z & ~c0;
  assign i0 = new_n68_ | new_n69_;
  assign new_n71_ = a0 & ~c0;
  assign new_n72_ = ~o & ~new_n71_;
  assign new_n73_ = ~a0 & ~c0;
  assign j0 = new_n72_ | new_n73_;
  assign new_n75_ = b0 & ~c0;
  assign new_n76_ = ~p & ~new_n75_;
  assign new_n77_ = ~b0 & ~c0;
  assign k0 = new_n76_ | new_n77_;
  assign new_n79_ = r & u;
  assign new_n80_ = ~r & ~u;
  assign new_n81_ = ~new_n79_ & ~new_n80_;
  assign new_n82_ = ~a & s;
  assign new_n83_ = i & ~new_n82_;
  assign new_n84_ = a & s;
  assign new_n85_ = ~q & ~new_n84_;
  assign new_n86_ = ~new_n83_ & new_n85_;
  assign new_n87_ = ~new_n81_ & ~new_n86_;
  assign new_n88_ = ~new_n83_ & ~new_n84_;
  assign new_n89_ = ~q & ~new_n88_;
  assign m0 = new_n87_ | new_n89_;
  assign new_n91_ = ~b & s;
  assign new_n92_ = ~j & ~s;
  assign new_n93_ = ~new_n91_ & ~new_n92_;
  assign new_n94_ = ~q & ~new_n93_;
  assign new_n95_ = q & ~r;
  assign new_n96_ = ~u & v;
  assign new_n97_ = new_n95_ & new_n96_;
  assign new_n98_ = q & ~v;
  assign new_n99_ = ~new_n80_ & new_n98_;
  assign new_n100_ = ~new_n97_ & ~new_n99_;
  assign n0 = ~new_n94_ & new_n100_;
  assign new_n102_ = k & ~s;
  assign new_n103_ = ~c & ~new_n102_;
  assign new_n104_ = ~k & ~s;
  assign new_n105_ = ~new_n103_ & ~new_n104_;
  assign new_n106_ = ~q & ~new_n105_;
  assign new_n107_ = ~u & ~v;
  assign new_n108_ = ~r & new_n107_;
  assign new_n109_ = ~w & ~new_n108_;
  assign new_n110_ = ~v & w;
  assign new_n111_ = new_n80_ & new_n110_;
  assign new_n112_ = ~new_n109_ & ~new_n111_;
  assign new_n113_ = q & ~new_n112_;
  assign o0 = ~new_n106_ & ~new_n113_;
  assign new_n115_ = l & ~s;
  assign new_n116_ = ~d & ~new_n115_;
  assign new_n117_ = ~l & ~s;
  assign new_n118_ = ~new_n116_ & ~new_n117_;
  assign new_n119_ = ~q & ~new_n118_;
  assign new_n120_ = ~v & ~w;
  assign new_n121_ = new_n80_ & new_n120_;
  assign new_n122_ = ~x & ~new_n121_;
  assign new_n123_ = x & new_n120_;
  assign new_n124_ = new_n80_ & new_n123_;
  assign new_n125_ = ~new_n122_ & ~new_n124_;
  assign new_n126_ = q & ~new_n125_;
  assign p0 = ~new_n119_ & ~new_n126_;
  assign new_n128_ = m & ~s;
  assign new_n129_ = ~e & ~new_n128_;
  assign new_n130_ = ~m & ~s;
  assign new_n131_ = ~new_n129_ & ~new_n130_;
  assign new_n132_ = ~q & ~new_n131_;
  assign new_n133_ = ~w & ~x;
  assign new_n134_ = new_n108_ & new_n133_;
  assign new_n135_ = ~y & ~new_n134_;
  assign new_n136_ = ~x & y;
  assign new_n137_ = new_n120_ & new_n136_;
  assign new_n138_ = new_n80_ & new_n137_;
  assign new_n139_ = ~new_n135_ & ~new_n138_;
  assign new_n140_ = q & ~new_n139_;
  assign q0 = ~new_n132_ & ~new_n140_;
  assign new_n142_ = n & ~s;
  assign new_n143_ = ~f & ~new_n142_;
  assign new_n144_ = ~n & ~s;
  assign new_n145_ = ~new_n143_ & ~new_n144_;
  assign new_n146_ = ~q & ~new_n145_;
  assign new_n147_ = ~r & ~w;
  assign new_n148_ = ~x & ~y;
  assign new_n149_ = new_n147_ & new_n148_;
  assign new_n150_ = new_n107_ & new_n149_;
  assign new_n151_ = ~z & ~new_n150_;
  assign new_n152_ = z & new_n148_;
  assign new_n153_ = new_n107_ & new_n147_;
  assign new_n154_ = new_n152_ & new_n153_;
  assign new_n155_ = ~new_n151_ & ~new_n154_;
  assign new_n156_ = q & ~new_n155_;
  assign r0 = ~new_n146_ & ~new_n156_;
  assign new_n158_ = o & ~s;
  assign new_n159_ = ~g & ~new_n158_;
  assign new_n160_ = ~o & ~s;
  assign new_n161_ = ~new_n159_ & ~new_n160_;
  assign new_n162_ = ~q & ~new_n161_;
  assign new_n163_ = ~y & ~z;
  assign new_n164_ = ~x & new_n147_;
  assign new_n165_ = new_n107_ & new_n163_;
  assign new_n166_ = new_n164_ & new_n165_;
  assign new_n167_ = ~a0 & ~new_n166_;
  assign new_n168_ = ~z & a0;
  assign new_n169_ = new_n148_ & new_n168_;
  assign new_n170_ = new_n121_ & new_n169_;
  assign new_n171_ = ~new_n167_ & ~new_n170_;
  assign new_n172_ = q & ~new_n171_;
  assign s0 = ~new_n162_ & ~new_n172_;
  assign new_n174_ = p & ~s;
  assign new_n175_ = ~h & ~new_n174_;
  assign new_n176_ = ~p & ~s;
  assign new_n177_ = ~new_n175_ & ~new_n176_;
  assign new_n178_ = ~q & ~new_n177_;
  assign new_n179_ = ~z & ~a0;
  assign new_n180_ = new_n148_ & new_n179_;
  assign new_n181_ = new_n121_ & new_n180_;
  assign new_n182_ = ~b0 & ~new_n181_;
  assign new_n183_ = ~v & ~y;
  assign new_n184_ = b0 & new_n179_;
  assign new_n185_ = new_n80_ & new_n183_;
  assign new_n186_ = new_n133_ & new_n185_;
  assign new_n187_ = new_n184_ & new_n186_;
  assign new_n188_ = ~new_n182_ & ~new_n187_;
  assign new_n189_ = q & ~new_n188_;
  assign t0 = ~new_n178_ & ~new_n189_;
  assign new_n191_ = ~r & u;
  assign new_n192_ = ~v & ~a0;
  assign new_n193_ = ~b0 & new_n192_;
  assign new_n194_ = new_n133_ & new_n191_;
  assign new_n195_ = new_n163_ & new_n194_;
  assign new_n196_ = new_n193_ & new_n195_;
  assign new_n197_ = r & c0;
  assign new_n198_ = ~new_n196_ & ~new_n197_;
  assign u0 = q & ~new_n198_;
  assign l0 = c0;
endmodule


