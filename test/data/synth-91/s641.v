// Benchmark "s641.bench" written by ABC on Wed Mar 10 23:12:57 2021

module s641.bench   clock, 
    G1, G2, G3, G4, G5, G6, G8, G9, G10, G11, G12, G13, G14, G15, G16, G17,
    G18, G19, G20, G21, G22, G23, G24, G25, G26, G27, G28, G29, G30, G31,
    G32, G33, G34, G35, G36,
    G91, G94, G107, G83, G84, G85, G100BF, G98BF, G96BF, G92, G87BF, G89BF,
    G101BF, G106BF, G97BF, G104BF, G88BF, G99BF, G105BF, G86BF, G95BF,
    G103BF, G90  ;
  input  clock;
  input  G1, G2, G3, G4, G5, G6, G8, G9, G10, G11, G12, G13, G14, G15,
    G16, G17, G18, G19, G20, G21, G22, G23, G24, G25, G26, G27, G28, G29,
    G30, G31, G32, G33, G34, G35, G36;
  output G91, G94, G107, G83, G84, G85, G100BF, G98BF, G96BF, G92, G87BF,
    G89BF, G101BF, G106BF, G97BF, G104BF, G88BF, G99BF, G105BF, G86BF,
    G95BF, G103BF, G90;
  reg G64, G65, G66, G67, G68, G69, G70, G71, G72, G73, G74, G75, G76, G77,
    G78, G79, G80, G81, G82;
  wire new_n116_, new_n118_1_, new_n120_, new_n122_, new_n124_, new_n126_,
    new_n128_1_, new_n130_, new_n131_, new_n133_1_, new_n134_, new_n135_,
    new_n136_, new_n137_, new_n138_1_, new_n140_, new_n141_, new_n142_,
    new_n143_1_, new_n147_, new_n148_1_, new_n150_, new_n152_, new_n153_1_,
    new_n155_, new_n156_, new_n157_, new_n158_1_, new_n159_, new_n160_,
    new_n162_, new_n164_, new_n167_, new_n168_1_, new_n169_, new_n170_,
    new_n171_, new_n172_, new_n173_1_, new_n175_, new_n177_, new_n180_,
    new_n181_, new_n182_, new_n183_1_, new_n184_, new_n185_, new_n186_,
    new_n187_, new_n188_1_, new_n189_, new_n190_, new_n191_, new_n192_,
    new_n193_1_, new_n194_, new_n195_, new_n196_, new_n197_, new_n198_1_,
    new_n199_, new_n200_, new_n201_, new_n202_, new_n203_1_, new_n204_,
    new_n205_, new_n206_, new_n208_1_, new_n209_, new_n212_, new_n213_,
    new_n216_, new_n217_, new_n220_, new_n221_, new_n224_, new_n225_,
    new_n226_, new_n227_, new_n228_, new_n229_, new_n230_, new_n231_,
    new_n233_, new_n234_, new_n235_, new_n236_, new_n237_, new_n238_,
    new_n239_, new_n241_, new_n242_, new_n243_, new_n244_, new_n245_,
    new_n246_, new_n247_, new_n248_, new_n250_, new_n251_, new_n252_,
    new_n253_, new_n254_, new_n255_, new_n257_, n118, n123, n128, n133,
    n138, n143, n148, n153, n158, n163, n168, n173, n178, n183, n188, n193,
    n198, n203, n208;
  assign new_n116_ = ~G4 & G18;
  assign G107 = G79 & new_n116_;
  assign new_n118_1_ = ~G4 & G19;
  assign G83 = G80 & new_n118_1_;
  assign new_n120_ = ~G4 & G20;
  assign G84 = G81 & new_n120_;
  assign new_n122_ = ~G4 & G21;
  assign G85 = G82 & new_n122_;
  assign new_n124_ = ~G4 & G69;
  assign G100BF = ~G35 | ~new_n124_;
  assign new_n126_ = ~G4 & G71;
  assign G98BF = ~G33 | ~new_n126_;
  assign new_n128_1_ = ~G4 & G73;
  assign G96BF = ~G31 | ~new_n128_1_;
  assign new_n130_ = G11 & G12;
  assign new_n131_ = G13 & G28;
  assign G92 = new_n130_ & new_n131_;
  assign new_n133_1_ = ~G3 & ~G11;
  assign new_n134_ = ~G3 & ~G9;
  assign new_n135_ = G10 & ~G13;
  assign new_n136_ = new_n134_ & new_n135_;
  assign new_n137_ = G23 & ~G65;
  assign new_n138_1_ = ~new_n133_1_ & new_n137_;
  assign G87BF = new_n136_ | ~new_n138_1_;
  assign new_n140_ = ~G3 & G9;
  assign new_n141_ = new_n135_ & new_n140_;
  assign new_n142_ = G25 & ~G67;
  assign new_n143_1_ = ~new_n133_1_ & new_n142_;
  assign G89BF = new_n141_ | ~new_n143_1_;
  assign n138 = G68 & ~G89BF;
  assign G101BF = ~G36 | ~n138;
  assign new_n147_ = ~G3 & G89BF;
  assign new_n148_1_ = G78 & ~new_n147_;
  assign G106BF = ~G17 | ~new_n148_1_;
  assign new_n150_ = G72 & ~G87BF;
  assign G97BF = ~G32 | ~new_n150_;
  assign new_n152_ = ~G3 & G87BF;
  assign new_n153_1_ = G76 & ~new_n152_;
  assign G104BF = ~G15 | ~new_n153_1_;
  assign new_n155_ = ~G10 & ~G13;
  assign new_n156_ = new_n140_ & new_n155_;
  assign new_n157_ = ~G2 & G66;
  assign new_n158_1_ = ~new_n148_1_ & new_n157_;
  assign new_n159_ = G24 & ~new_n133_1_;
  assign new_n160_ = ~new_n156_ & new_n159_;
  assign G88BF = new_n158_1_ | ~new_n160_;
  assign new_n162_ = G70 & ~G88BF;
  assign G99BF = ~G34 | ~new_n162_;
  assign new_n164_ = ~G3 & G88BF;
  assign n128 = G77 & ~new_n164_;
  assign G105BF = ~G16 | ~n128;
  assign new_n167_ = ~new_n148_1_ & ~n128;
  assign new_n168_1_ = ~G2 & G64;
  assign new_n169_ = ~new_n153_1_ & new_n168_1_;
  assign new_n170_ = new_n167_ & new_n169_;
  assign new_n171_ = new_n134_ & new_n155_;
  assign new_n172_ = G22 & ~new_n133_1_;
  assign new_n173_1_ = ~new_n171_ & new_n172_;
  assign G86BF = new_n170_ | ~new_n173_1_;
  assign new_n175_ = G74 & ~G86BF;
  assign G95BF = ~G30 | ~new_n175_;
  assign new_n177_ = ~G3 & G86BF;
  assign n118 = G75 & ~new_n177_;
  assign G103BF = ~G14 | ~n118;
  assign new_n180_ = ~G9 & G10;
  assign new_n181_ = ~G13 & new_n180_;
  assign new_n182_ = new_n126_ & new_n181_;
  assign new_n183_1_ = new_n150_ & new_n182_;
  assign new_n184_ = G9 & G10;
  assign new_n185_ = ~G13 & new_n184_;
  assign new_n186_ = n138 & new_n185_;
  assign new_n187_ = ~G11 & new_n124_;
  assign new_n188_1_ = new_n126_ & new_n128_1_;
  assign new_n189_ = new_n187_ & new_n188_1_;
  assign new_n190_ = n138 & new_n189_;
  assign new_n191_ = new_n150_ & new_n190_;
  assign new_n192_ = new_n162_ & new_n191_;
  assign new_n193_1_ = new_n175_ & new_n192_;
  assign new_n194_ = G9 & ~G10;
  assign new_n195_ = ~G13 & new_n194_;
  assign new_n196_ = new_n124_ & new_n195_;
  assign new_n197_ = new_n162_ & new_n196_;
  assign new_n198_1_ = ~G9 & ~G10;
  assign new_n199_ = ~G13 & new_n198_1_;
  assign new_n200_ = new_n128_1_ & new_n199_;
  assign new_n201_ = new_n175_ & new_n200_;
  assign new_n202_ = G12 & G26;
  assign new_n203_1_ = ~new_n183_1_ & new_n202_;
  assign new_n204_ = ~new_n186_ & new_n203_1_;
  assign new_n205_ = ~new_n197_ & new_n204_;
  assign new_n206_ = ~new_n193_1_ & new_n205_;
  assign G90 = ~new_n201_ & new_n206_;
  assign new_n208_1_ = ~G2 & new_n153_1_;
  assign new_n209_ = ~new_n148_1_ & new_n208_1_;
  assign n123 = ~n128 & new_n209_;
  assign n133 = ~G2 & new_n148_1_;
  assign new_n212_ = ~new_n124_ & ~new_n162_;
  assign new_n213_ = ~G88BF & ~new_n162_;
  assign n143 = ~new_n212_ & ~new_n213_;
  assign n148 = ~new_n124_ | new_n162_;
  assign new_n216_ = ~new_n126_ & ~new_n150_;
  assign new_n217_ = ~G87BF & ~new_n150_;
  assign n153 = ~new_n216_ & ~new_n217_;
  assign n158 = ~new_n126_ | new_n150_;
  assign new_n220_ = ~new_n128_1_ & ~new_n175_;
  assign new_n221_ = ~G86BF & ~new_n175_;
  assign n163 = ~new_n220_ & ~new_n221_;
  assign n168 = ~new_n128_1_ | new_n175_;
  assign new_n224_ = G8 & new_n128_1_;
  assign new_n225_ = ~new_n148_1_ & new_n224_;
  assign new_n226_ = ~new_n153_1_ & new_n225_;
  assign new_n227_ = ~n128 & new_n226_;
  assign new_n228_ = new_n175_ & new_n227_;
  assign new_n229_ = ~G2 & n118;
  assign new_n230_ = G8 & n118;
  assign new_n231_ = ~new_n228_ & ~new_n229_;
  assign n173 = new_n230_ | ~new_n231_;
  assign new_n233_ = G5 & new_n126_;
  assign new_n234_ = new_n150_ & new_n233_;
  assign new_n235_ = ~new_n148_1_ & new_n234_;
  assign new_n236_ = ~n128 & new_n235_;
  assign new_n237_ = ~n118 & new_n236_;
  assign new_n238_ = G5 & new_n153_1_;
  assign new_n239_ = ~new_n208_1_ & ~new_n238_;
  assign n178 = new_n237_ | ~new_n239_;
  assign new_n241_ = G6 & new_n124_;
  assign new_n242_ = ~new_n148_1_ & new_n241_;
  assign new_n243_ = ~new_n153_1_ & new_n242_;
  assign new_n244_ = new_n162_ & new_n243_;
  assign new_n245_ = ~n118 & new_n244_;
  assign new_n246_ = ~G2 & n128;
  assign new_n247_ = G6 & n128;
  assign new_n248_ = ~new_n246_ & ~new_n247_;
  assign n183 = new_n245_ | ~new_n248_;
  assign new_n250_ = G1 & n138;
  assign new_n251_ = ~new_n153_1_ & new_n250_;
  assign new_n252_ = ~n128 & new_n251_;
  assign new_n253_ = ~n118 & new_n252_;
  assign new_n254_ = G1 & new_n148_1_;
  assign new_n255_ = ~n133 & ~new_n254_;
  assign n188 = new_n253_ | ~new_n255_;
  assign new_n257_ = ~new_n153_1_ & new_n167_;
  assign n193 = new_n229_ & new_n257_;
  assign n203 = ~new_n148_1_ & new_n246_;
  assign G91 = G27;
  assign G94 = G29;
  assign n198 = n123;
  assign n208 = n133;
  always @ posedge clock begin
    G64 <= n118;
    G65 <= n123;
    G66 <= n128;
    G67 <= n133;
    G68 <= n138;
    G69 <= n143;
    G70 <= n148;
    G71 <= n153;
    G72 <= n158;
    G73 <= n163;
    G74 <= n168;
    G75 <= n173;
    G76 <= n178;
    G77 <= n183;
    G78 <= n188;
    G79 <= n193;
    G80 <= n198;
    G81 <= n203;
    G82 <= n208;
  end
  initial begin
    G64 <= 1'b0;
    G65 <= 1'b0;
    G66 <= 1'b0;
    G67 <= 1'b0;
    G68 <= 1'b0;
    G69 <= 1'b0;
    G70 <= 1'b0;
    G71 <= 1'b0;
    G72 <= 1'b0;
    G73 <= 1'b0;
    G74 <= 1'b0;
    G75 <= 1'b0;
    G76 <= 1'b0;
    G77 <= 1'b0;
    G78 <= 1'b0;
    G79 <= 1'b0;
    G80 <= 1'b0;
    G81 <= 1'b0;
    G82 <= 1'b0;
  end
endmodule


