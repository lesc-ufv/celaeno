// Benchmark "i3" written by ABC on Wed Mar 10 23:12:54 2021

module i3  
    V560 , V280 , V561 , V281 , V562 , V282 , V563 ,
    V283 , V564 , V284 , V565 , V285 , V566 , V286 ,
    V567 , V287 , V568 , V288 , V569 , V289 , V5610 ,
    V2810 , V5611 , V2811 , V5612 , V2812 , V5613 ,
    V2813 , V5614 , V2814 , V5615 , V2815 , V5616 ,
    V2816 , V5617 , V2817 , V5618 , V2818 , V5619 ,
    V2819 , V5620 , V2820 , V5621 , V2821 , V5622 ,
    V2822 , V5623 , V2823 , V5624 , V2824 , V5625 ,
    V2825 , V5626 , V2826 , V5627 , V2827 , V1200 ,
    V880 , V1201 , V881 , V1202 , V882 , V1203 ,
    V883 , V1204 , V884 , V1205 , V885 , V1206 ,
    V886 , V1207 , V887 , V1208 , V888 , V1209 ,
    V889 , V12010 , V8810 , V12011 , V8811 , V12012 ,
    V8812 , V12013 , V8813 , V12014 , V8814 , V12015 ,
    V8815 , V12016 , V8816 , V12017 , V8817 , V12018 ,
    V8818 , V12019 , V8819 , V12020 , V8820 , V12021 ,
    V8821 , V12022 , V8822 , V12023 , V8823 , V12024 ,
    V8824 , V12025 , V8825 , V12026 , V8826 , V12027 ,
    V8827 , V12028 , V8828 , V12029 , V8829 , V12030 ,
    V8830 , V12031 , V8831 , V1320 , V1260 , V1321 ,
    V1261 , V1322 , V1262 , V1323 , V1263 , V1324 ,
    V1264 , V1325 , V1265 ,
    V1340 , V1341 , V1380 , V1381 , V1382 , V1383   ;
  input  V560 , V280 , V561 , V281 , V562 , V282 ,
    V563 , V283 , V564 , V284 , V565 , V285 , V566 ,
    V286 , V567 , V287 , V568 , V288 , V569 , V289 ,
    V5610 , V2810 , V5611 , V2811 , V5612 , V2812 ,
    V5613 , V2813 , V5614 , V2814 , V5615 , V2815 ,
    V5616 , V2816 , V5617 , V2817 , V5618 , V2818 ,
    V5619 , V2819 , V5620 , V2820 , V5621 , V2821 ,
    V5622 , V2822 , V5623 , V2823 , V5624 , V2824 ,
    V5625 , V2825 , V5626 , V2826 , V5627 , V2827 ,
    V1200 , V880 , V1201 , V881 , V1202 , V882 ,
    V1203 , V883 , V1204 , V884 , V1205 , V885 ,
    V1206 , V886 , V1207 , V887 , V1208 , V888 ,
    V1209 , V889 , V12010 , V8810 , V12011 , V8811 ,
    V12012 , V8812 , V12013 , V8813 , V12014 , V8814 ,
    V12015 , V8815 , V12016 , V8816 , V12017 , V8817 ,
    V12018 , V8818 , V12019 , V8819 , V12020 , V8820 ,
    V12021 , V8821 , V12022 , V8822 , V12023 , V8823 ,
    V12024 , V8824 , V12025 , V8825 , V12026 , V8826 ,
    V12027 , V8827 , V12028 , V8828 , V12029 , V8829 ,
    V12030 , V8830 , V12031 , V8831 , V1320 , V1260 ,
    V1321 , V1261 , V1322 , V1262 , V1323 , V1263 ,
    V1324 , V1264 , V1325 , V1265 ;
  output V1340 , V1341 , V1380 , V1381 , V1382 , V1383 ;
  wire new_n141_, new_n142_, new_n143_, new_n144_, new_n145_, new_n146_,
    new_n147_, new_n148_, new_n149_, new_n150_, new_n151_, new_n152_,
    new_n153_, new_n154_, new_n155_, new_n156_, new_n157_, new_n158_,
    new_n159_, new_n160_, new_n161_, new_n162_, new_n163_, new_n164_,
    new_n165_, new_n166_, new_n167_, new_n168_, new_n169_, new_n170_,
    new_n172_, new_n173_, new_n174_, new_n175_, new_n176_, new_n177_,
    new_n178_, new_n179_, new_n180_, new_n181_, new_n182_, new_n183_,
    new_n184_, new_n185_, new_n186_, new_n187_, new_n188_, new_n189_,
    new_n190_, new_n191_, new_n192_, new_n193_, new_n194_, new_n195_,
    new_n196_, new_n197_, new_n198_, new_n199_, new_n200_, new_n201_,
    new_n203_, new_n204_, new_n205_, new_n206_, new_n207_, new_n208_,
    new_n209_, new_n210_, new_n211_, new_n212_, new_n213_, new_n214_,
    new_n215_, new_n216_, new_n217_, new_n218_, new_n219_, new_n220_,
    new_n221_, new_n222_, new_n223_, new_n224_, new_n225_, new_n226_,
    new_n227_, new_n228_, new_n229_, new_n230_, new_n231_, new_n232_,
    new_n234_, new_n235_, new_n236_, new_n237_, new_n238_, new_n239_,
    new_n240_, new_n241_, new_n242_, new_n243_, new_n244_, new_n245_,
    new_n246_, new_n247_, new_n248_, new_n249_, new_n250_, new_n251_,
    new_n252_, new_n253_, new_n254_, new_n255_, new_n256_, new_n257_,
    new_n258_, new_n259_, new_n260_, new_n261_, new_n262_, new_n263_;
  assign V1340  = V560  | V280 ;
  assign V1341  = V561  | V281 ;
  assign new_n141_ = ~V5613  & ~V2813 ;
  assign new_n142_ = ~V5612  & ~V2812 ;
  assign new_n143_ = ~V5610  & ~V2810 ;
  assign new_n144_ = ~V5611  & ~V2811 ;
  assign new_n145_ = ~V569  & ~V289 ;
  assign new_n146_ = ~V567  & ~V287 ;
  assign new_n147_ = ~V566  & ~V286 ;
  assign new_n148_ = ~V568  & ~V288 ;
  assign new_n149_ = ~V563  & ~V283 ;
  assign new_n150_ = ~V562  & ~V282 ;
  assign new_n151_ = ~V564  & ~V284 ;
  assign new_n152_ = ~V565  & ~V285 ;
  assign new_n153_ = ~V5615  & ~V2815 ;
  assign new_n154_ = ~V5614  & ~V2814 ;
  assign new_n155_ = ~V5617  & ~V2817 ;
  assign new_n156_ = ~V5616  & ~V2816 ;
  assign new_n157_ = ~new_n141_ & ~new_n142_;
  assign new_n158_ = ~new_n143_ & ~new_n144_;
  assign new_n159_ = ~new_n145_ & ~new_n146_;
  assign new_n160_ = ~new_n147_ & ~new_n148_;
  assign new_n161_ = ~new_n149_ & ~new_n150_;
  assign new_n162_ = ~new_n151_ & ~new_n152_;
  assign new_n163_ = ~new_n153_ & ~new_n154_;
  assign new_n164_ = ~new_n155_ & ~new_n156_;
  assign new_n165_ = new_n163_ & new_n164_;
  assign new_n166_ = new_n161_ & new_n162_;
  assign new_n167_ = new_n159_ & new_n160_;
  assign new_n168_ = new_n157_ & new_n158_;
  assign new_n169_ = new_n167_ & new_n168_;
  assign new_n170_ = new_n165_ & new_n166_;
  assign V1380  = new_n169_ & new_n170_;
  assign new_n172_ = ~V1201  & ~V881 ;
  assign new_n173_ = ~V1200  & ~V880 ;
  assign new_n174_ = ~V5626  & ~V2826 ;
  assign new_n175_ = ~V5627  & ~V2827 ;
  assign new_n176_ = ~V5625  & ~V2825 ;
  assign new_n177_ = ~V5623  & ~V2823 ;
  assign new_n178_ = ~V5622  & ~V2822 ;
  assign new_n179_ = ~V5624  & ~V2824 ;
  assign new_n180_ = ~V5619  & ~V2819 ;
  assign new_n181_ = ~V5618  & ~V2818 ;
  assign new_n182_ = ~V5620  & ~V2820 ;
  assign new_n183_ = ~V5621  & ~V2821 ;
  assign new_n184_ = ~V1203  & ~V883 ;
  assign new_n185_ = ~V1202  & ~V882 ;
  assign new_n186_ = ~V1205  & ~V885 ;
  assign new_n187_ = ~V1204  & ~V884 ;
  assign new_n188_ = ~new_n172_ & ~new_n173_;
  assign new_n189_ = ~new_n174_ & ~new_n175_;
  assign new_n190_ = ~new_n176_ & ~new_n177_;
  assign new_n191_ = ~new_n178_ & ~new_n179_;
  assign new_n192_ = ~new_n180_ & ~new_n181_;
  assign new_n193_ = ~new_n182_ & ~new_n183_;
  assign new_n194_ = ~new_n184_ & ~new_n185_;
  assign new_n195_ = ~new_n186_ & ~new_n187_;
  assign new_n196_ = new_n194_ & new_n195_;
  assign new_n197_ = new_n192_ & new_n193_;
  assign new_n198_ = new_n190_ & new_n191_;
  assign new_n199_ = new_n188_ & new_n189_;
  assign new_n200_ = new_n198_ & new_n199_;
  assign new_n201_ = new_n196_ & new_n197_;
  assign V1381  = new_n200_ & new_n201_;
  assign new_n203_ = ~V12017  & ~V8817 ;
  assign new_n204_ = ~V12016  & ~V8816 ;
  assign new_n205_ = ~V12014  & ~V8814 ;
  assign new_n206_ = ~V12015  & ~V8815 ;
  assign new_n207_ = ~V12013  & ~V8813 ;
  assign new_n208_ = ~V12011  & ~V8811 ;
  assign new_n209_ = ~V12010  & ~V8810 ;
  assign new_n210_ = ~V12012  & ~V8812 ;
  assign new_n211_ = ~V1207  & ~V887 ;
  assign new_n212_ = ~V1206  & ~V886 ;
  assign new_n213_ = ~V1208  & ~V888 ;
  assign new_n214_ = ~V1209  & ~V889 ;
  assign new_n215_ = ~V12019  & ~V8819 ;
  assign new_n216_ = ~V12018  & ~V8818 ;
  assign new_n217_ = ~V12021  & ~V8821 ;
  assign new_n218_ = ~V12020  & ~V8820 ;
  assign new_n219_ = ~new_n203_ & ~new_n204_;
  assign new_n220_ = ~new_n205_ & ~new_n206_;
  assign new_n221_ = ~new_n207_ & ~new_n208_;
  assign new_n222_ = ~new_n209_ & ~new_n210_;
  assign new_n223_ = ~new_n211_ & ~new_n212_;
  assign new_n224_ = ~new_n213_ & ~new_n214_;
  assign new_n225_ = ~new_n215_ & ~new_n216_;
  assign new_n226_ = ~new_n217_ & ~new_n218_;
  assign new_n227_ = new_n225_ & new_n226_;
  assign new_n228_ = new_n223_ & new_n224_;
  assign new_n229_ = new_n221_ & new_n222_;
  assign new_n230_ = new_n219_ & new_n220_;
  assign new_n231_ = new_n229_ & new_n230_;
  assign new_n232_ = new_n227_ & new_n228_;
  assign V1382  = new_n231_ & new_n232_;
  assign new_n234_ = ~V1321  & ~V1261 ;
  assign new_n235_ = ~V1320  & ~V1260 ;
  assign new_n236_ = ~V12030  & ~V8830 ;
  assign new_n237_ = ~V12031  & ~V8831 ;
  assign new_n238_ = ~V12029  & ~V8829 ;
  assign new_n239_ = ~V12027  & ~V8827 ;
  assign new_n240_ = ~V12026  & ~V8826 ;
  assign new_n241_ = ~V12028  & ~V8828 ;
  assign new_n242_ = ~V12023  & ~V8823 ;
  assign new_n243_ = ~V12022  & ~V8822 ;
  assign new_n244_ = ~V12024  & ~V8824 ;
  assign new_n245_ = ~V12025  & ~V8825 ;
  assign new_n246_ = ~V1323  & ~V1263 ;
  assign new_n247_ = ~V1322  & ~V1262 ;
  assign new_n248_ = ~V1325  & ~V1265 ;
  assign new_n249_ = ~V1324  & ~V1264 ;
  assign new_n250_ = ~new_n234_ & ~new_n235_;
  assign new_n251_ = ~new_n236_ & ~new_n237_;
  assign new_n252_ = ~new_n238_ & ~new_n239_;
  assign new_n253_ = ~new_n240_ & ~new_n241_;
  assign new_n254_ = ~new_n242_ & ~new_n243_;
  assign new_n255_ = ~new_n244_ & ~new_n245_;
  assign new_n256_ = ~new_n246_ & ~new_n247_;
  assign new_n257_ = ~new_n248_ & ~new_n249_;
  assign new_n258_ = new_n256_ & new_n257_;
  assign new_n259_ = new_n254_ & new_n255_;
  assign new_n260_ = new_n252_ & new_n253_;
  assign new_n261_ = new_n250_ & new_n251_;
  assign new_n262_ = new_n260_ & new_n261_;
  assign new_n263_ = new_n258_ & new_n259_;
  assign V1383  = new_n262_ & new_n263_;
endmodule


