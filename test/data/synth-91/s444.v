// Benchmark "s444.bench" written by ABC on Wed Mar 10 23:12:57 2021

module s444.bench   clock, 
    G0, G1, G2,
    G118, G167, G107, G119, G168, G108  ;
  input  clock;
  input  G0, G1, G2;
  output G118, G167, G107, G119, G168, G108;
  reg G11, G12, G13, G14, G15, G16, G17, G18, G19, G20, G21, G22, G23, G24,
    G25, G26, G27, G28, G29, G30, G31;
  wire new_n73_, new_n74_, new_n75_1_, new_n76_, new_n78_, new_n79_,
    new_n80_1_, new_n81_, new_n82_, new_n84_, new_n85_1_, new_n86_,
    new_n87_, new_n89_, new_n90_1_, new_n91_, new_n92_, new_n94_,
    new_n95_1_, new_n96_, new_n97_, new_n98_, new_n99_, new_n100_1_,
    new_n101_, new_n102_, new_n103_, new_n105_1_, new_n106_, new_n107_,
    new_n108_, new_n110_1_, new_n111_, new_n112_, new_n113_, new_n114_,
    new_n115_1_, new_n117_, new_n118_, new_n119_, new_n120_1_, new_n121_,
    new_n122_, new_n124_, new_n125_, new_n126_, new_n127_, new_n128_,
    new_n129_, new_n130_, new_n131_, new_n132_, new_n134_, new_n135_,
    new_n136_, new_n137_, new_n139_, new_n140_, new_n141_, new_n142_,
    new_n143_, new_n144_, new_n146_, new_n147_, new_n148_, new_n149_,
    new_n150_, new_n151_, new_n153_, new_n154_, new_n155_, new_n157_,
    new_n158_, new_n159_, new_n160_, new_n161_, new_n162_, new_n163_,
    new_n164_, new_n165_, new_n166_, new_n168_, new_n169_, new_n170_,
    new_n171_, new_n172_, new_n173_, new_n174_, new_n175_, new_n177_,
    new_n178_, new_n179_, new_n180_, new_n181_, new_n182_, new_n184_,
    new_n185_, new_n186_, new_n187_, new_n189_, new_n190_, new_n191_,
    new_n192_, new_n193_, new_n195_, new_n196_, new_n197_, new_n198_,
    new_n199_, new_n200_, new_n201_, new_n202_, new_n203_, new_n205_,
    new_n206_, new_n208_, new_n209_, new_n210_, n20, n25, n30, n35, n40,
    n45, n50, n55, n60, n65, n70, n75, n80, n85, n90, n95, n100, n105,
    n110, n115, n120;
  assign new_n73_ = ~G11 & ~G12;
  assign new_n74_ = ~G13 & new_n73_;
  assign new_n75_1_ = G14 & ~new_n74_;
  assign new_n76_ = ~G0 & ~G11;
  assign n20 = ~new_n75_1_ & new_n76_;
  assign new_n78_ = G11 & G12;
  assign new_n79_ = G12 & ~new_n78_;
  assign new_n80_1_ = G11 & ~new_n78_;
  assign new_n81_ = ~new_n79_ & ~new_n80_1_;
  assign new_n82_ = ~G0 & ~new_n75_1_;
  assign n25 = ~new_n81_ & new_n82_;
  assign new_n84_ = G13 & new_n78_;
  assign new_n85_1_ = G13 & ~new_n84_;
  assign new_n86_ = new_n78_ & ~new_n84_;
  assign new_n87_ = ~new_n85_1_ & ~new_n86_;
  assign n30 = new_n82_ & ~new_n87_;
  assign new_n89_ = G14 & new_n84_;
  assign new_n90_1_ = G14 & ~new_n89_;
  assign new_n91_ = new_n84_ & ~new_n89_;
  assign new_n92_ = ~new_n90_1_ & ~new_n91_;
  assign n35 = new_n82_ & ~new_n92_;
  assign new_n94_ = ~G15 & ~G16;
  assign new_n95_1_ = ~G17 & new_n94_;
  assign new_n96_ = ~G31 & ~new_n75_1_;
  assign new_n97_ = G18 & ~new_n95_1_;
  assign new_n98_ = ~new_n96_ & new_n97_;
  assign new_n99_ = G15 & ~new_n96_;
  assign new_n100_1_ = G15 & ~new_n99_;
  assign new_n101_ = ~new_n96_ & ~new_n99_;
  assign new_n102_ = ~new_n100_1_ & ~new_n101_;
  assign new_n103_ = ~G0 & ~new_n98_;
  assign n40 = ~new_n102_ & new_n103_;
  assign new_n105_1_ = G16 & new_n99_;
  assign new_n106_ = G16 & ~new_n105_1_;
  assign new_n107_ = new_n99_ & ~new_n105_1_;
  assign new_n108_ = ~new_n106_ & ~new_n107_;
  assign n45 = new_n103_ & ~new_n108_;
  assign new_n110_1_ = G15 & G16;
  assign new_n111_ = ~new_n96_ & new_n110_1_;
  assign new_n112_ = G17 & new_n111_;
  assign new_n113_ = G17 & ~new_n112_;
  assign new_n114_ = new_n111_ & ~new_n112_;
  assign new_n115_1_ = ~new_n113_ & ~new_n114_;
  assign n50 = new_n103_ & ~new_n115_1_;
  assign new_n117_ = G17 & new_n110_1_;
  assign new_n118_ = ~new_n96_ & new_n117_;
  assign new_n119_ = G18 & new_n118_;
  assign new_n120_1_ = G18 & ~new_n119_;
  assign new_n121_ = new_n118_ & ~new_n119_;
  assign new_n122_ = ~new_n120_1_ & ~new_n121_;
  assign n55 = new_n103_ & ~new_n122_;
  assign new_n124_ = ~G19 & ~G20;
  assign new_n125_ = ~G21 & new_n124_;
  assign new_n126_ = G22 & ~new_n125_;
  assign new_n127_ = new_n98_ & new_n126_;
  assign new_n128_ = G19 & new_n98_;
  assign new_n129_ = G19 & ~new_n128_;
  assign new_n130_ = new_n98_ & ~new_n128_;
  assign new_n131_ = ~new_n129_ & ~new_n130_;
  assign new_n132_ = ~G0 & ~new_n127_;
  assign n60 = ~new_n131_ & new_n132_;
  assign new_n134_ = G20 & new_n128_;
  assign new_n135_ = G20 & ~new_n134_;
  assign new_n136_ = new_n128_ & ~new_n134_;
  assign new_n137_ = ~new_n135_ & ~new_n136_;
  assign n65 = new_n132_ & ~new_n137_;
  assign new_n139_ = G19 & G20;
  assign new_n140_ = new_n98_ & new_n139_;
  assign new_n141_ = G21 & new_n140_;
  assign new_n142_ = G21 & ~new_n141_;
  assign new_n143_ = new_n140_ & ~new_n141_;
  assign new_n144_ = ~new_n142_ & ~new_n143_;
  assign n70 = new_n132_ & ~new_n144_;
  assign new_n146_ = G21 & new_n139_;
  assign new_n147_ = new_n98_ & new_n146_;
  assign new_n148_ = G22 & new_n147_;
  assign new_n149_ = G22 & ~new_n148_;
  assign new_n150_ = new_n147_ & ~new_n148_;
  assign new_n151_ = ~new_n149_ & ~new_n150_;
  assign n75 = new_n132_ & ~new_n151_;
  assign new_n153_ = ~G2 & ~G23;
  assign new_n154_ = G2 & G23;
  assign new_n155_ = ~G0 & ~new_n153_;
  assign n80 = ~new_n154_ & new_n155_;
  assign new_n157_ = ~G0 & ~G19;
  assign new_n158_ = ~G20 & G21;
  assign new_n159_ = G23 & new_n158_;
  assign new_n160_ = new_n157_ & new_n159_;
  assign new_n161_ = G21 & ~G22;
  assign new_n162_ = G19 & ~G20;
  assign new_n163_ = ~G23 & new_n161_;
  assign new_n164_ = new_n162_ & new_n163_;
  assign new_n165_ = ~G0 & G24;
  assign new_n166_ = ~new_n164_ & new_n165_;
  assign n85 = new_n160_ | new_n166_;
  assign new_n168_ = ~G22 & ~G23;
  assign new_n169_ = new_n162_ & new_n168_;
  assign new_n170_ = G24 & ~new_n169_;
  assign new_n171_ = G23 & new_n124_;
  assign new_n172_ = G22 & G23;
  assign new_n173_ = ~G0 & G21;
  assign new_n174_ = ~new_n172_ & new_n173_;
  assign new_n175_ = ~new_n171_ & new_n174_;
  assign n90 = ~new_n170_ & new_n175_;
  assign new_n177_ = ~G0 & G22;
  assign new_n178_ = ~G19 & new_n177_;
  assign new_n179_ = ~G0 & G19;
  assign new_n180_ = G20 & new_n179_;
  assign new_n181_ = ~new_n165_ & ~new_n173_;
  assign new_n182_ = ~new_n178_ & new_n181_;
  assign n95 = ~new_n180_ & new_n182_;
  assign new_n184_ = ~G21 & ~G24;
  assign new_n185_ = new_n162_ & new_n184_;
  assign new_n186_ = ~G22 & new_n184_;
  assign new_n187_ = ~G0 & ~new_n185_;
  assign n100 = new_n186_ | ~new_n187_;
  assign new_n189_ = new_n124_ & new_n177_;
  assign new_n190_ = new_n181_ & ~new_n189_;
  assign new_n191_ = ~G17 & ~new_n190_;
  assign new_n192_ = n85 & new_n191_;
  assign new_n193_ = ~n85 & ~new_n190_;
  assign n105 = ~new_n192_ & ~new_n193_;
  assign new_n195_ = ~G0 & G23;
  assign new_n196_ = new_n161_ & new_n195_;
  assign new_n197_ = ~G21 & new_n177_;
  assign new_n198_ = ~new_n196_ & ~new_n197_;
  assign new_n199_ = new_n124_ & ~new_n198_;
  assign new_n200_ = ~new_n166_ & ~new_n199_;
  assign new_n201_ = ~G17 & n85;
  assign new_n202_ = ~new_n200_ & new_n201_;
  assign new_n203_ = ~n85 & ~new_n200_;
  assign n110 = ~new_n202_ & ~new_n203_;
  assign new_n205_ = G20 & ~G21;
  assign new_n206_ = ~G24 & new_n205_;
  assign n115 = new_n179_ & new_n206_;
  assign new_n208_ = ~G1 & ~G31;
  assign new_n209_ = G1 & G31;
  assign new_n210_ = ~G0 & ~new_n208_;
  assign n120 = ~new_n209_ & new_n210_;
  assign G167 = ~G29;
  assign G119 = ~G28;
  assign G118 = G27;
  assign G107 = G25;
  assign G168 = G30;
  assign G108 = G26;
  always @ posedge clock begin
    G11 <= n20;
    G12 <= n25;
    G13 <= n30;
    G14 <= n35;
    G15 <= n40;
    G16 <= n45;
    G17 <= n50;
    G18 <= n55;
    G19 <= n60;
    G20 <= n65;
    G21 <= n70;
    G22 <= n75;
    G23 <= n80;
    G24 <= n85;
    G25 <= n90;
    G26 <= n95;
    G27 <= n100;
    G28 <= n105;
    G29 <= n110;
    G30 <= n115;
    G31 <= n120;
  end
  initial begin
    G11 <= 1'b0;
    G12 <= 1'b0;
    G13 <= 1'b0;
    G14 <= 1'b0;
    G15 <= 1'b0;
    G16 <= 1'b0;
    G17 <= 1'b0;
    G18 <= 1'b0;
    G19 <= 1'b0;
    G20 <= 1'b0;
    G21 <= 1'b0;
    G22 <= 1'b0;
    G23 <= 1'b0;
    G24 <= 1'b0;
    G25 <= 1'b0;
    G26 <= 1'b0;
    G27 <= 1'b0;
    G28 <= 1'b0;
    G29 <= 1'b0;
    G30 <= 1'b0;
    G31 <= 1'b0;
  end
endmodule


