// Benchmark "s382.bench" written by ABC on Wed Mar 10 23:12:56 2021

module s382.bench   clock, 
    FM, TEST, CLR,
    GRN1, GRN2, RED1, YLW2, RED2, YLW1  ;
  input  clock;
  input  FM, TEST, CLR;
  output GRN1, GRN2, RED1, YLW2, RED2, YLW1;
  reg TESTL, FML, OLATCH_Y2L, OLATCHVUC_6, OLATCHVUC_5, OLATCH_R1L,
    OLATCH_G2L, OLATCH_G1L, OLATCH_FEL, C3_Q3, C3_Q2, C3_Q1, C3_Q0, UC_16,
    UC_17, UC_18, UC_19, UC_8, UC_9, UC_10, UC_11;
  wire new_n73_, new_n74_, new_n75_1_, new_n77_, new_n78_, new_n79_,
    new_n81_, new_n82_, new_n83_, new_n85_1_, new_n86_, new_n87_, new_n88_,
    new_n89_, new_n90_1_, new_n91_, new_n92_, new_n93_, new_n94_, new_n96_,
    new_n97_, new_n98_, new_n99_, new_n100_1_, new_n101_, new_n102_,
    new_n103_, new_n104_, new_n106_, new_n107_, new_n108_, new_n109_,
    new_n110_1_, new_n112_, new_n113_, new_n114_, new_n115_1_, new_n117_,
    new_n118_, new_n119_, new_n120_1_, new_n121_, new_n123_, new_n124_,
    new_n125_, new_n126_, new_n127_, new_n128_, new_n129_, new_n131_,
    new_n132_, new_n133_, new_n134_, new_n135_, new_n136_, new_n137_,
    new_n138_, new_n139_, new_n140_, new_n141_, new_n142_, new_n143_,
    new_n144_, new_n145_, new_n146_, new_n147_, new_n149_, new_n150_,
    new_n151_, new_n152_, new_n153_, new_n155_, new_n156_, new_n157_,
    new_n158_, new_n160_, new_n161_, new_n163_, new_n164_, new_n165_,
    new_n166_, new_n167_, new_n169_, new_n170_, new_n171_, new_n172_,
    new_n173_, new_n175_, new_n176_, new_n177_, new_n178_, new_n180_,
    new_n181_, new_n183_, new_n184_, new_n185_, new_n186_, new_n188_,
    new_n189_, new_n190_, new_n191_, new_n192_, new_n194_, new_n195_,
    new_n196_, new_n198_, n20, n25, n30, n35, n40, n45, n50, n55, n60, n65,
    n70, n75, n80, n85, n90, n95, n100, n105, n110, n115, n120;
  assign new_n73_ = TEST & TESTL;
  assign new_n74_ = ~TEST & ~TESTL;
  assign new_n75_1_ = ~CLR & ~new_n73_;
  assign n20 = ~new_n74_ & new_n75_1_;
  assign new_n77_ = FM & FML;
  assign new_n78_ = ~FM & ~FML;
  assign new_n79_ = ~CLR & ~new_n77_;
  assign n25 = ~new_n78_ & new_n79_;
  assign new_n81_ = ~CLR & ~OLATCH_FEL;
  assign new_n82_ = ~C3_Q2 & C3_Q1;
  assign new_n83_ = C3_Q0 & new_n82_;
  assign n30 = new_n81_ & new_n83_;
  assign new_n85_1_ = ~CLR & OLATCH_FEL;
  assign new_n86_ = ~C3_Q1 & C3_Q0;
  assign new_n87_ = ~C3_Q3 & C3_Q2;
  assign new_n88_ = ~FML & new_n86_;
  assign new_n89_ = new_n87_ & new_n88_;
  assign new_n90_1_ = new_n85_1_ & ~new_n89_;
  assign new_n91_ = ~CLR & FML;
  assign new_n92_ = C3_Q2 & ~C3_Q1;
  assign new_n93_ = ~C3_Q0 & new_n92_;
  assign new_n94_ = new_n91_ & new_n93_;
  assign n60 = new_n90_1_ | new_n94_;
  assign new_n96_ = ~CLR & C3_Q3;
  assign new_n97_ = ~C3_Q2 & new_n96_;
  assign new_n98_ = new_n87_ & new_n91_;
  assign new_n99_ = ~new_n97_ & ~new_n98_;
  assign new_n100_1_ = ~C3_Q1 & ~C3_Q0;
  assign new_n101_ = ~new_n99_ & new_n100_1_;
  assign new_n102_ = ~new_n90_1_ & ~new_n101_;
  assign new_n103_ = ~n60 & ~new_n102_;
  assign new_n104_ = ~UC_17 & ~new_n102_;
  assign n35 = ~new_n103_ & ~new_n104_;
  assign new_n106_ = ~CLR & C3_Q2;
  assign new_n107_ = new_n96_ & new_n100_1_;
  assign new_n108_ = ~new_n106_ & ~new_n107_;
  assign new_n109_ = ~n60 & ~new_n108_;
  assign new_n110_1_ = ~UC_17 & n60;
  assign n40 = ~new_n109_ & ~new_n110_1_;
  assign new_n112_ = ~OLATCH_FEL & ~C3_Q2;
  assign new_n113_ = new_n86_ & new_n112_;
  assign new_n114_ = ~C3_Q3 & new_n112_;
  assign new_n115_1_ = ~CLR & ~new_n113_;
  assign n45 = new_n114_ | ~new_n115_1_;
  assign new_n117_ = ~C3_Q0 & new_n96_;
  assign new_n118_ = ~CLR & C3_Q1;
  assign new_n119_ = C3_Q0 & new_n118_;
  assign new_n120_1_ = ~new_n85_1_ & ~new_n106_;
  assign new_n121_ = ~new_n117_ & new_n120_1_;
  assign n50 = ~new_n119_ & new_n121_;
  assign new_n123_ = FML & C3_Q3;
  assign new_n124_ = FML & new_n100_1_;
  assign new_n125_ = ~FML & ~C3_Q3;
  assign new_n126_ = new_n86_ & new_n125_;
  assign new_n127_ = OLATCH_FEL & ~new_n126_;
  assign new_n128_ = new_n106_ & ~new_n123_;
  assign new_n129_ = ~new_n124_ & new_n128_;
  assign n55 = ~new_n127_ & new_n129_;
  assign new_n131_ = ~UC_17 & ~UC_18;
  assign new_n132_ = ~UC_19 & new_n131_;
  assign new_n133_ = ~UC_9 & ~UC_10;
  assign new_n134_ = ~UC_11 & new_n133_;
  assign new_n135_ = UC_8 & ~new_n134_;
  assign new_n136_ = ~TESTL & ~new_n135_;
  assign new_n137_ = UC_16 & ~new_n132_;
  assign new_n138_ = ~new_n136_ & new_n137_;
  assign new_n139_ = ~C3_Q2 & ~C3_Q1;
  assign new_n140_ = ~C3_Q0 & new_n139_;
  assign new_n141_ = C3_Q3 & ~new_n140_;
  assign new_n142_ = new_n138_ & new_n141_;
  assign new_n143_ = C3_Q2 & C3_Q1;
  assign new_n144_ = C3_Q0 & new_n143_;
  assign new_n145_ = new_n138_ & new_n144_;
  assign new_n146_ = ~C3_Q3 & ~new_n145_;
  assign new_n147_ = ~CLR & ~new_n142_;
  assign n65 = ~new_n146_ & new_n147_;
  assign new_n149_ = C3_Q1 & C3_Q0;
  assign new_n150_ = new_n138_ & new_n149_;
  assign new_n151_ = ~C3_Q2 & ~new_n150_;
  assign new_n152_ = C3_Q2 & new_n150_;
  assign new_n153_ = new_n147_ & ~new_n151_;
  assign n70 = ~new_n152_ & new_n153_;
  assign new_n155_ = C3_Q0 & new_n138_;
  assign new_n156_ = ~C3_Q1 & ~new_n155_;
  assign new_n157_ = C3_Q1 & new_n155_;
  assign new_n158_ = new_n147_ & ~new_n156_;
  assign n75 = ~new_n157_ & new_n158_;
  assign new_n160_ = ~C3_Q0 & ~new_n138_;
  assign new_n161_ = ~new_n155_ & ~new_n160_;
  assign n80 = new_n147_ & new_n161_;
  assign new_n163_ = UC_17 & UC_18;
  assign new_n164_ = UC_19 & new_n163_;
  assign new_n165_ = ~new_n136_ & new_n164_;
  assign new_n166_ = ~UC_16 & ~new_n165_;
  assign new_n167_ = ~CLR & ~new_n138_;
  assign n85 = ~new_n166_ & new_n167_;
  assign new_n169_ = UC_18 & UC_19;
  assign new_n170_ = ~new_n136_ & new_n169_;
  assign new_n171_ = ~UC_17 & ~new_n170_;
  assign new_n172_ = UC_17 & new_n170_;
  assign new_n173_ = new_n167_ & ~new_n171_;
  assign n90 = ~new_n172_ & new_n173_;
  assign new_n175_ = UC_19 & ~new_n136_;
  assign new_n176_ = ~UC_18 & ~new_n175_;
  assign new_n177_ = UC_18 & new_n175_;
  assign new_n178_ = new_n167_ & ~new_n176_;
  assign n95 = ~new_n177_ & new_n178_;
  assign new_n180_ = ~UC_19 & new_n136_;
  assign new_n181_ = ~new_n175_ & ~new_n180_;
  assign n100 = new_n167_ & new_n181_;
  assign new_n183_ = UC_9 & UC_10;
  assign new_n184_ = UC_11 & new_n183_;
  assign new_n185_ = ~UC_8 & ~new_n184_;
  assign new_n186_ = ~CLR & ~new_n135_;
  assign n105 = ~new_n185_ & new_n186_;
  assign new_n188_ = UC_10 & UC_11;
  assign new_n189_ = ~UC_9 & ~new_n188_;
  assign new_n190_ = UC_9 & new_n188_;
  assign new_n191_ = ~CLR & ~new_n189_;
  assign new_n192_ = ~new_n190_ & new_n191_;
  assign n110 = ~new_n135_ & new_n192_;
  assign new_n194_ = ~UC_10 & ~UC_11;
  assign new_n195_ = ~CLR & ~new_n188_;
  assign new_n196_ = ~new_n194_ & new_n195_;
  assign n115 = ~new_n135_ & new_n196_;
  assign new_n198_ = ~CLR & ~UC_11;
  assign n120 = ~new_n135_ & new_n198_;
  assign RED2 = ~OLATCHVUC_5;
  assign YLW1 = ~OLATCHVUC_6;
  assign GRN1 = OLATCH_G1L;
  assign GRN2 = OLATCH_G2L;
  assign RED1 = OLATCH_R1L;
  assign YLW2 = OLATCH_Y2L;
  always @ posedge clock begin
    TESTL <= n20;
    FML <= n25;
    OLATCH_Y2L <= n30;
    OLATCHVUC_6 <= n35;
    OLATCHVUC_5 <= n40;
    OLATCH_R1L <= n45;
    OLATCH_G2L <= n50;
    OLATCH_G1L <= n55;
    OLATCH_FEL <= n60;
    C3_Q3 <= n65;
    C3_Q2 <= n70;
    C3_Q1 <= n75;
    C3_Q0 <= n80;
    UC_16 <= n85;
    UC_17 <= n90;
    UC_18 <= n95;
    UC_19 <= n100;
    UC_8 <= n105;
    UC_9 <= n110;
    UC_10 <= n115;
    UC_11 <= n120;
  end
  initial begin
    TESTL <= 1'b0;
    FML <= 1'b0;
    OLATCH_Y2L <= 1'b0;
    OLATCHVUC_6 <= 1'b0;
    OLATCHVUC_5 <= 1'b0;
    OLATCH_R1L <= 1'b0;
    OLATCH_G2L <= 1'b0;
    OLATCH_G1L <= 1'b0;
    OLATCH_FEL <= 1'b0;
    C3_Q3 <= 1'b0;
    C3_Q2 <= 1'b0;
    C3_Q1 <= 1'b0;
    C3_Q0 <= 1'b0;
    UC_16 <= 1'b0;
    UC_17 <= 1'b0;
    UC_18 <= 1'b0;
    UC_19 <= 1'b0;
    UC_8 <= 1'b0;
    UC_9 <= 1'b0;
    UC_10 <= 1'b0;
    UC_11 <= 1'b0;
  end
endmodule


