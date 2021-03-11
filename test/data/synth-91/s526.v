// Benchmark "s526.bench" written by ABC on Wed Mar 10 23:12:57 2021

module s526.bench   clock, 
    G0, G1, G2,
    G198, G213, G147, G199, G214, G148  ;
  input  clock;
  input  G0, G1, G2;
  output G198, G213, G147, G199, G214, G148;
  reg G10, G11, G12, G13, G14, G15, G16, G17, G18, G19, G20, G21, G22, G23,
    G24, G25, G26, G27, G28, G29, G30;
  wire new_n74_, new_n75_1_, new_n76_, new_n77_, new_n78_, new_n79_,
    new_n81_, new_n82_, new_n83_, new_n84_, new_n85_1_, new_n86_, new_n87_,
    new_n88_, new_n89_, new_n90_1_, new_n91_, new_n92_, new_n93_, new_n94_,
    new_n95_1_, new_n96_, new_n97_, new_n98_, new_n99_, new_n100_1_,
    new_n101_, new_n102_, new_n103_, new_n105_1_, new_n106_, new_n107_,
    new_n108_, new_n109_, new_n110_1_, new_n111_, new_n112_, new_n113_,
    new_n114_, new_n115_1_, new_n116_, new_n117_, new_n119_, new_n120_1_,
    new_n121_, new_n122_, new_n123_, new_n125_, new_n126_, new_n127_,
    new_n128_, new_n129_, new_n131_, new_n132_, new_n133_, new_n134_,
    new_n135_, new_n136_, new_n137_, new_n138_, new_n139_, new_n141_,
    new_n142_, new_n143_, new_n144_, new_n145_, new_n146_, new_n147_,
    new_n148_, new_n149_, new_n150_, new_n151_, new_n152_, new_n153_,
    new_n154_, new_n155_, new_n157_, new_n158_, new_n159_, new_n160_,
    new_n161_, new_n162_, new_n163_, new_n164_, new_n165_, new_n167_,
    new_n168_, new_n169_, new_n170_, new_n171_, new_n172_, new_n173_,
    new_n174_, new_n175_, new_n176_, new_n177_, new_n178_, new_n179_,
    new_n181_, new_n182_, new_n183_, new_n184_, new_n185_, new_n186_,
    new_n187_, new_n188_, new_n190_, new_n191_, new_n192_, new_n193_,
    new_n194_, new_n195_, new_n196_, new_n197_, new_n198_, new_n199_,
    new_n200_, new_n202_, new_n203_, new_n204_, new_n205_, new_n206_,
    new_n207_, new_n208_, new_n210_, new_n211_, new_n212_, new_n213_,
    new_n214_, new_n216_, new_n217_, new_n218_, new_n219_, new_n220_,
    new_n221_, new_n222_, new_n223_, new_n224_, new_n226_, new_n227_,
    new_n228_, new_n229_, new_n230_, new_n232_, new_n233_, new_n234_,
    new_n235_, new_n236_, new_n237_, new_n238_, new_n239_, new_n240_,
    new_n241_, new_n243_, new_n244_, new_n245_, new_n246_, new_n247_,
    new_n248_, new_n249_, new_n251_, new_n252_, new_n253_, new_n254_,
    new_n255_, new_n257_, new_n258_, new_n259_, new_n261_, new_n262_,
    new_n263_, n20, n25, n30, n35, n40, n45, n50, n55, n60, n65, n70, n75,
    n80, n85, n90, n95, n100, n105, n110, n115, n120;
  assign n20 = ~G0 & ~G10;
  assign new_n74_ = ~G10 & ~G11;
  assign new_n75_1_ = G10 & ~G14;
  assign new_n76_ = G15 & new_n75_1_;
  assign new_n77_ = G10 & G11;
  assign new_n78_ = ~G0 & ~new_n74_;
  assign new_n79_ = ~new_n77_ & new_n78_;
  assign n25 = ~new_n76_ & new_n79_;
  assign new_n81_ = ~G17 & ~G18;
  assign new_n82_ = ~G12 & G21;
  assign new_n83_ = G19 & G20;
  assign new_n84_ = G10 & ~G11;
  assign new_n85_1_ = ~G14 & new_n84_;
  assign new_n86_ = G15 & new_n85_1_;
  assign new_n87_ = ~G30 & ~new_n86_;
  assign new_n88_ = ~G0 & G16;
  assign new_n89_ = new_n81_ & new_n88_;
  assign new_n90_1_ = new_n82_ & new_n83_;
  assign new_n91_ = new_n89_ & new_n90_1_;
  assign new_n92_ = ~new_n87_ & new_n91_;
  assign new_n93_ = ~G18 & G19;
  assign new_n94_ = G16 & ~G17;
  assign new_n95_1_ = new_n93_ & new_n94_;
  assign new_n96_ = ~G14 & G15;
  assign new_n97_ = new_n84_ & new_n96_;
  assign new_n98_ = ~G30 & ~new_n97_;
  assign new_n99_ = G20 & G21;
  assign new_n100_1_ = new_n95_1_ & new_n99_;
  assign new_n101_ = ~new_n98_ & new_n100_1_;
  assign new_n102_ = ~G0 & G12;
  assign new_n103_ = ~new_n101_ & new_n102_;
  assign n30 = new_n92_ | new_n103_;
  assign new_n105_1_ = G12 & ~G13;
  assign new_n106_ = ~G0 & new_n99_;
  assign new_n107_ = new_n105_1_ & new_n106_;
  assign new_n108_ = new_n95_1_ & new_n107_;
  assign new_n109_ = ~new_n87_ & new_n108_;
  assign new_n110_1_ = G12 & ~G21;
  assign new_n111_ = G16 & new_n81_;
  assign new_n112_ = ~new_n82_ & new_n83_;
  assign new_n113_ = ~new_n110_1_ & new_n112_;
  assign new_n114_ = new_n111_ & new_n113_;
  assign new_n115_1_ = ~new_n98_ & new_n114_;
  assign new_n116_ = ~G0 & G13;
  assign new_n117_ = ~new_n115_1_ & new_n116_;
  assign n35 = new_n109_ | new_n117_;
  assign new_n119_ = ~G11 & ~G14;
  assign new_n120_1_ = G14 & new_n77_;
  assign new_n121_ = ~G10 & ~G14;
  assign new_n122_ = ~G0 & ~new_n119_;
  assign new_n123_ = ~new_n121_ & new_n122_;
  assign n40 = ~new_n120_1_ & new_n123_;
  assign new_n125_ = ~G15 & ~new_n120_1_;
  assign new_n126_ = G14 & G15;
  assign new_n127_ = new_n77_ & new_n126_;
  assign new_n128_ = ~G0 & ~new_n85_1_;
  assign new_n129_ = ~new_n127_ & new_n128_;
  assign n45 = ~new_n125_ & new_n129_;
  assign new_n131_ = ~G0 & ~G16;
  assign new_n132_ = ~G0 & ~G30;
  assign new_n133_ = ~new_n131_ & ~new_n132_;
  assign new_n134_ = G15 & G16;
  assign new_n135_ = G10 & new_n119_;
  assign new_n136_ = new_n134_ & new_n135_;
  assign new_n137_ = ~G16 & ~G30;
  assign new_n138_ = ~new_n97_ & new_n137_;
  assign new_n139_ = ~new_n133_ & ~new_n136_;
  assign n50 = ~new_n138_ & new_n139_;
  assign new_n141_ = G16 & G30;
  assign new_n142_ = new_n93_ & new_n141_;
  assign new_n143_ = G17 & new_n141_;
  assign new_n144_ = ~G16 & ~G17;
  assign new_n145_ = ~G17 & ~G19;
  assign new_n146_ = ~G17 & G18;
  assign new_n147_ = new_n85_1_ & new_n134_;
  assign new_n148_ = ~new_n145_ & ~new_n146_;
  assign new_n149_ = new_n147_ & new_n148_;
  assign new_n150_ = ~G17 & ~G30;
  assign new_n151_ = ~new_n97_ & new_n150_;
  assign new_n152_ = ~G0 & ~new_n144_;
  assign new_n153_ = ~new_n142_ & new_n152_;
  assign new_n154_ = ~new_n143_ & new_n153_;
  assign new_n155_ = ~new_n151_ & new_n154_;
  assign n55 = ~new_n149_ & new_n155_;
  assign new_n157_ = G17 & new_n88_;
  assign new_n158_ = ~G0 & G18;
  assign new_n159_ = ~new_n157_ & ~new_n158_;
  assign new_n160_ = G16 & G17;
  assign new_n161_ = G18 & new_n160_;
  assign new_n162_ = ~new_n98_ & new_n161_;
  assign new_n163_ = ~G18 & ~G30;
  assign new_n164_ = ~new_n97_ & new_n163_;
  assign new_n165_ = ~new_n159_ & ~new_n164_;
  assign n60 = ~new_n162_ & new_n165_;
  assign new_n167_ = G18 & new_n157_;
  assign new_n168_ = ~G0 & G19;
  assign new_n169_ = ~new_n167_ & ~new_n168_;
  assign new_n170_ = G17 & ~G19;
  assign new_n171_ = G17 & ~G18;
  assign new_n172_ = ~new_n146_ & ~new_n170_;
  assign new_n173_ = ~new_n171_ & new_n172_;
  assign new_n174_ = new_n141_ & new_n173_;
  assign new_n175_ = new_n147_ & new_n173_;
  assign new_n176_ = ~G19 & ~G30;
  assign new_n177_ = ~new_n97_ & new_n176_;
  assign new_n178_ = ~new_n169_ & ~new_n177_;
  assign new_n179_ = ~new_n174_ & ~new_n175_;
  assign n65 = new_n178_ & new_n179_;
  assign new_n181_ = ~G20 & ~new_n95_1_;
  assign new_n182_ = ~G18 & new_n83_;
  assign new_n183_ = new_n94_ & new_n182_;
  assign new_n184_ = ~new_n98_ & new_n183_;
  assign new_n185_ = ~G20 & ~G30;
  assign new_n186_ = ~new_n97_ & new_n185_;
  assign new_n187_ = ~G0 & ~new_n181_;
  assign new_n188_ = ~new_n186_ & new_n187_;
  assign n70 = ~new_n184_ & new_n188_;
  assign new_n190_ = G20 & ~G21;
  assign new_n191_ = ~G12 & G13;
  assign new_n192_ = ~G0 & new_n93_;
  assign new_n193_ = new_n94_ & new_n190_;
  assign new_n194_ = ~new_n191_ & new_n193_;
  assign new_n195_ = new_n192_ & new_n194_;
  assign new_n196_ = ~new_n98_ & new_n195_;
  assign new_n197_ = G20 & new_n95_1_;
  assign new_n198_ = ~new_n98_ & new_n197_;
  assign new_n199_ = ~G0 & G21;
  assign new_n200_ = ~new_n198_ & new_n199_;
  assign n75 = new_n196_ | new_n200_;
  assign new_n202_ = ~G29 & new_n105_1_;
  assign new_n203_ = new_n190_ & new_n202_;
  assign new_n204_ = ~G20 & ~G21;
  assign new_n205_ = G29 & new_n105_1_;
  assign new_n206_ = new_n204_ & new_n205_;
  assign new_n207_ = ~G22 & ~new_n206_;
  assign new_n208_ = ~new_n203_ & ~new_n207_;
  assign n80 = ~G0 & new_n208_;
  assign new_n210_ = ~G12 & ~G21;
  assign new_n211_ = G13 & ~G23;
  assign new_n212_ = ~G12 & ~G13;
  assign new_n213_ = ~new_n210_ & ~new_n211_;
  assign new_n214_ = ~new_n212_ & new_n213_;
  assign n85 = ~new_n208_ & new_n214_;
  assign new_n216_ = new_n191_ & new_n204_;
  assign new_n217_ = ~G13 & new_n99_;
  assign new_n218_ = G13 & G21;
  assign new_n219_ = ~G24 & new_n218_;
  assign new_n220_ = G12 & ~G24;
  assign new_n221_ = ~new_n105_1_ & ~new_n220_;
  assign new_n222_ = ~new_n216_ & new_n221_;
  assign new_n223_ = ~new_n217_ & ~new_n219_;
  assign new_n224_ = new_n222_ & new_n223_;
  assign n90 = ~new_n208_ & new_n224_;
  assign new_n226_ = ~G25 & new_n218_;
  assign new_n227_ = G12 & ~G25;
  assign new_n228_ = ~new_n105_1_ & ~new_n227_;
  assign new_n229_ = ~new_n216_ & new_n228_;
  assign new_n230_ = ~new_n226_ & new_n229_;
  assign n95 = ~new_n208_ & new_n230_;
  assign new_n232_ = G26 & new_n218_;
  assign new_n233_ = G13 & new_n204_;
  assign new_n234_ = ~G12 & ~new_n232_;
  assign new_n235_ = ~new_n233_ & new_n234_;
  assign new_n236_ = ~new_n208_ & new_n235_;
  assign new_n237_ = G12 & G13;
  assign new_n238_ = ~G26 & new_n237_;
  assign new_n239_ = ~new_n208_ & new_n238_;
  assign new_n240_ = G18 & new_n208_;
  assign new_n241_ = ~new_n236_ & ~new_n239_;
  assign n100 = ~new_n240_ & new_n241_;
  assign new_n243_ = ~G12 & new_n190_;
  assign new_n244_ = G21 & ~G27;
  assign new_n245_ = G12 & ~G27;
  assign new_n246_ = G13 & ~new_n244_;
  assign new_n247_ = ~new_n245_ & new_n246_;
  assign new_n248_ = ~new_n243_ & new_n247_;
  assign new_n249_ = ~new_n208_ & ~new_n248_;
  assign n105 = ~new_n240_ & ~new_n249_;
  assign new_n251_ = G13 & ~G28;
  assign new_n252_ = ~G13 & ~G20;
  assign new_n253_ = ~new_n105_1_ & ~new_n210_;
  assign new_n254_ = ~new_n251_ & ~new_n252_;
  assign new_n255_ = new_n253_ & new_n254_;
  assign n110 = ~new_n208_ & new_n255_;
  assign new_n257_ = ~G2 & ~G29;
  assign new_n258_ = G2 & G29;
  assign new_n259_ = ~G0 & ~new_n257_;
  assign n115 = ~new_n258_ & new_n259_;
  assign new_n261_ = ~G1 & ~G30;
  assign new_n262_ = G1 & G30;
  assign new_n263_ = ~G0 & ~new_n261_;
  assign n120 = ~new_n262_ & new_n263_;
  assign G198 = G25;
  assign G213 = G27;
  assign G147 = G23;
  assign G199 = G26;
  assign G214 = G28;
  assign G148 = G24;
  always @ posedge clock begin
    G10 <= n20;
    G11 <= n25;
    G12 <= n30;
    G13 <= n35;
    G14 <= n40;
    G15 <= n45;
    G16 <= n50;
    G17 <= n55;
    G18 <= n60;
    G19 <= n65;
    G20 <= n70;
    G21 <= n75;
    G22 <= n80;
    G23 <= n85;
    G24 <= n90;
    G25 <= n95;
    G26 <= n100;
    G27 <= n105;
    G28 <= n110;
    G29 <= n115;
    G30 <= n120;
  end
  initial begin
    G10 <= 1'b0;
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
  end
endmodule


