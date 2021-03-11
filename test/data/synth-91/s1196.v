// Benchmark "s1196.bench" written by ABC on Wed Mar 10 23:12:55 2021

module s1196.bench   clock, 
    G0, G1, G2, G3, G4, G5, G6, G7, G8, G9, G10, G11, G12, G13,
    G546, G539, G550, G551, G552, G547, G548, G549, G530, G45, G542, G532,
    G535, G537  ;
  input  clock;
  input  G0, G1, G2, G3, G4, G5, G6, G7, G8, G9, G10, G11, G12, G13;
  output G546, G539, G550, G551, G552, G547, G548, G549, G530, G45, G542,
    G532, G535, G537;
  reg G29, G30, G31, G32, G33, G34, G35, G36, G37, G38, G39, G40, G41, G42,
    G43, G44, G45, G46;
  wire new_n83_1_, new_n84_, new_n85_, new_n86_, new_n87_, new_n88_1_,
    new_n89_, new_n90_, new_n91_, new_n92_, new_n93_1_, new_n94_, new_n95_,
    new_n96_, new_n97_, new_n98_1_, new_n99_, new_n100_, new_n101_,
    new_n102_, new_n103_1_, new_n104_, new_n105_, new_n106_, new_n107_,
    new_n108_1_, new_n109_, new_n110_, new_n111_, new_n112_, new_n113_1_,
    new_n114_, new_n115_, new_n116_, new_n117_, new_n118_1_, new_n119_,
    new_n120_, new_n121_, new_n122_, new_n123_1_, new_n124_, new_n125_,
    new_n126_, new_n127_, new_n128_1_, new_n129_, new_n130_, new_n131_,
    new_n132_, new_n133_1_, new_n134_, new_n135_, new_n136_, new_n137_,
    new_n138_1_, new_n139_, new_n140_, new_n141_, new_n142_1_, new_n143_,
    new_n144_, new_n145_, new_n146_, new_n147_, new_n148_, new_n149_,
    new_n150_, new_n151_, new_n152_, new_n153_, new_n154_, new_n155_,
    new_n156_, new_n157_, new_n158_, new_n159_, new_n160_, new_n161_,
    new_n162_, new_n163_, new_n164_, new_n165_, new_n166_, new_n167_,
    new_n168_, new_n169_, new_n170_, new_n171_, new_n172_, new_n173_,
    new_n174_, new_n175_, new_n176_, new_n177_, new_n178_, new_n179_,
    new_n180_, new_n181_, new_n182_, new_n183_, new_n184_, new_n185_,
    new_n186_, new_n187_, new_n188_, new_n189_, new_n190_, new_n191_,
    new_n192_, new_n193_, new_n194_, new_n195_, new_n196_, new_n197_,
    new_n198_, new_n199_, new_n200_, new_n201_, new_n202_, new_n203_,
    new_n204_, new_n205_, new_n206_, new_n207_, new_n208_, new_n209_,
    new_n210_, new_n211_, new_n212_, new_n213_, new_n214_, new_n215_,
    new_n216_, new_n217_, new_n218_, new_n219_, new_n220_, new_n221_,
    new_n222_, new_n223_, new_n224_, new_n225_, new_n226_, new_n227_,
    new_n228_, new_n229_, new_n230_, new_n231_, new_n232_, new_n233_,
    new_n234_, new_n235_, new_n236_, new_n237_, new_n238_, new_n239_,
    new_n240_, new_n241_, new_n242_, new_n243_, new_n245_, new_n246_,
    new_n247_, new_n248_, new_n249_, new_n250_, new_n251_, new_n252_,
    new_n253_, new_n254_, new_n255_, new_n256_, new_n257_, new_n258_,
    new_n259_, new_n260_, new_n261_, new_n262_, new_n263_, new_n264_,
    new_n265_, new_n266_, new_n267_, new_n269_, new_n270_, new_n271_,
    new_n272_, new_n273_, new_n274_, new_n275_, new_n276_, new_n277_,
    new_n278_, new_n279_, new_n280_, new_n281_, new_n282_, new_n283_,
    new_n284_, new_n285_, new_n286_, new_n287_, new_n288_, new_n289_,
    new_n290_, new_n291_, new_n292_, new_n293_, new_n294_, new_n295_,
    new_n297_, new_n298_, new_n299_, new_n300_, new_n301_, new_n302_,
    new_n303_, new_n304_, new_n305_, new_n306_, new_n307_, new_n308_,
    new_n309_, new_n310_, new_n311_, new_n312_, new_n313_, new_n314_,
    new_n316_, new_n317_, new_n318_, new_n319_, new_n320_, new_n321_,
    new_n322_, new_n323_, new_n324_, new_n325_, new_n326_, new_n327_,
    new_n328_, new_n329_, new_n330_, new_n331_, new_n332_, new_n333_,
    new_n335_, new_n336_, new_n337_, new_n338_, new_n339_, new_n340_,
    new_n341_, new_n342_, new_n344_, new_n345_, new_n346_, new_n347_,
    new_n348_, new_n349_, new_n350_, new_n351_, new_n352_, new_n353_,
    new_n354_, new_n355_, new_n356_, new_n357_, new_n358_, new_n359_,
    new_n361_, new_n362_, new_n363_, new_n364_, new_n365_, new_n366_,
    new_n367_, new_n368_, new_n369_, new_n370_, new_n371_, new_n372_,
    new_n374_, new_n375_, new_n376_, new_n377_, new_n378_, new_n379_,
    new_n380_, new_n381_, new_n382_, new_n383_, new_n384_, new_n385_,
    new_n386_, new_n387_, new_n388_, new_n389_, new_n391_, new_n392_,
    new_n393_, new_n394_, new_n395_, new_n396_, new_n397_, new_n398_,
    new_n399_, new_n400_, new_n401_, new_n402_, new_n403_, new_n404_,
    new_n405_, new_n406_, new_n407_, new_n408_, new_n409_, new_n410_,
    new_n411_, new_n412_, new_n413_, new_n414_, new_n415_, new_n416_,
    new_n417_, new_n418_, new_n419_, new_n420_, new_n422_, new_n423_,
    new_n424_, new_n425_, new_n426_, new_n427_, new_n428_, new_n429_,
    new_n430_, new_n431_, new_n432_, new_n433_, new_n434_, new_n435_,
    new_n436_, new_n437_, new_n439_, new_n440_, new_n441_, new_n442_,
    new_n443_, new_n444_, new_n445_, new_n446_, new_n447_, new_n448_,
    new_n449_, new_n450_, new_n451_, new_n452_, new_n453_, new_n454_,
    new_n455_, new_n456_, new_n457_, new_n458_, new_n459_, new_n460_,
    new_n461_, new_n462_, new_n464_, new_n465_, new_n466_, new_n468_,
    new_n470_, new_n471_, new_n473_, new_n474_, new_n475_, new_n477_,
    new_n478_, new_n479_, new_n480_, new_n482_, new_n483_, new_n484_,
    new_n485_, new_n487_, new_n488_, new_n489_, new_n491_, new_n492_,
    new_n494_, new_n497_, new_n499_, new_n500_, new_n501_, new_n502_,
    new_n503_, new_n504_, new_n505_, new_n506_, new_n507_, new_n509_,
    new_n510_, new_n511_, new_n512_, new_n514_, new_n515_, new_n516_,
    new_n517_, new_n518_, new_n519_, new_n520_, new_n521_, new_n523_,
    new_n524_, new_n525_, new_n526_, new_n527_, new_n528_, new_n529_,
    new_n530_, new_n531_, new_n533_, new_n534_, new_n535_, new_n536_,
    new_n537_, new_n539_, new_n540_, new_n542_, new_n543_, new_n544_,
    new_n545_, new_n546_, new_n547_, new_n548_, new_n549_, new_n550_,
    new_n551_, new_n552_, new_n553_, new_n554_, new_n555_, new_n556_, n58,
    n63, n68, n73, n78, n83, n88, n93, n98, n103, n108, n113, n118, n123,
    n128, n133, n138, n142;
  assign new_n83_1_ = G1 & G3;
  assign new_n84_ = ~G4 & new_n83_1_;
  assign new_n85_ = G6 & new_n84_;
  assign new_n86_ = G4 & G6;
  assign new_n87_ = G3 & new_n86_;
  assign new_n88_1_ = ~G1 & new_n87_;
  assign new_n89_ = ~new_n85_ & ~new_n88_1_;
  assign new_n90_ = ~G8 & G10;
  assign new_n91_ = G9 & G11;
  assign new_n92_ = new_n90_ & new_n91_;
  assign new_n93_1_ = ~new_n89_ & new_n92_;
  assign new_n94_ = G8 & new_n91_;
  assign new_n95_ = ~G10 & new_n94_;
  assign new_n96_ = new_n88_1_ & new_n95_;
  assign new_n97_ = ~new_n93_1_ & ~new_n96_;
  assign new_n98_1_ = ~G7 & ~new_n97_;
  assign new_n99_ = ~G1 & G3;
  assign new_n100_ = ~G4 & ~G6;
  assign new_n101_ = ~G8 & new_n99_;
  assign new_n102_ = new_n100_ & new_n101_;
  assign new_n103_1_ = G8 & ~new_n89_;
  assign new_n104_ = ~new_n102_ & ~new_n103_1_;
  assign new_n105_ = G7 & ~G10;
  assign new_n106_ = ~G9 & new_n105_;
  assign new_n107_ = G11 & new_n106_;
  assign new_n108_1_ = ~new_n104_ & new_n107_;
  assign new_n109_ = ~new_n98_1_ & ~new_n108_1_;
  assign new_n110_ = G2 & ~G5;
  assign new_n111_ = ~new_n109_ & new_n110_;
  assign new_n112_ = G3 & G5;
  assign new_n113_1_ = G1 & new_n112_;
  assign new_n114_ = G7 & G10;
  assign new_n115_ = new_n94_ & new_n114_;
  assign new_n116_ = G9 & ~G10;
  assign new_n117_ = ~G7 & ~G11;
  assign new_n118_1_ = ~G8 & new_n117_;
  assign new_n119_ = new_n116_ & new_n118_1_;
  assign new_n120_ = ~new_n115_ & ~new_n119_;
  assign new_n121_ = G2 & new_n86_;
  assign new_n122_ = new_n113_1_ & new_n121_;
  assign new_n123_1_ = ~new_n120_ & new_n122_;
  assign new_n124_ = ~new_n111_ & ~new_n123_1_;
  assign new_n125_ = G7 & new_n116_;
  assign new_n126_ = ~G7 & G8;
  assign new_n127_ = G30 & new_n126_;
  assign new_n128_1_ = ~new_n94_ & new_n114_;
  assign new_n129_ = ~new_n125_ & ~new_n127_;
  assign new_n130_ = ~new_n128_1_ & new_n129_;
  assign new_n131_ = ~G4 & G6;
  assign new_n132_ = ~G5 & ~new_n131_;
  assign new_n133_1_ = ~G1 & G2;
  assign new_n134_ = ~new_n132_ & new_n133_1_;
  assign new_n135_ = G3 & ~new_n86_;
  assign new_n136_ = ~new_n100_ & ~new_n135_;
  assign new_n137_ = G5 & ~new_n136_;
  assign new_n138_1_ = G4 & ~G5;
  assign new_n139_ = ~G3 & new_n138_1_;
  assign new_n140_ = G4 & ~new_n112_;
  assign new_n141_ = ~G2 & G3;
  assign new_n142_1_ = G2 & ~G3;
  assign new_n143_ = ~new_n141_ & ~new_n142_1_;
  assign new_n144_ = ~new_n140_ & new_n143_;
  assign new_n145_ = G6 & ~new_n144_;
  assign new_n146_ = ~new_n137_ & ~new_n139_;
  assign new_n147_ = ~new_n145_ & new_n146_;
  assign new_n148_ = G1 & ~new_n147_;
  assign new_n149_ = G1 & G4;
  assign new_n150_ = G2 & ~G6;
  assign new_n151_ = new_n149_ & new_n150_;
  assign new_n152_ = ~new_n134_ & ~new_n151_;
  assign new_n153_ = ~new_n148_ & new_n152_;
  assign new_n154_ = ~new_n130_ & ~new_n153_;
  assign new_n155_ = ~G12 & G13;
  assign new_n156_ = ~new_n154_ & new_n155_;
  assign new_n157_ = new_n124_ & new_n156_;
  assign new_n158_ = G32 & ~new_n130_;
  assign new_n159_ = ~G13 & ~new_n158_;
  assign new_n160_ = ~G6 & G36;
  assign new_n161_ = G5 & new_n86_;
  assign new_n162_ = ~G7 & G9;
  assign new_n163_ = new_n90_ & new_n162_;
  assign new_n164_ = G8 & new_n106_;
  assign new_n165_ = ~new_n163_ & ~new_n164_;
  assign new_n166_ = G11 & new_n161_;
  assign new_n167_ = ~new_n165_ & new_n166_;
  assign new_n168_ = ~new_n160_ & ~new_n167_;
  assign new_n169_ = ~G3 & ~new_n168_;
  assign new_n170_ = ~G2 & new_n169_;
  assign new_n171_ = ~G5 & G11;
  assign new_n172_ = new_n87_ & new_n171_;
  assign new_n173_ = new_n116_ & new_n126_;
  assign new_n174_ = new_n165_ & ~new_n173_;
  assign new_n175_ = ~G2 & new_n172_;
  assign new_n176_ = ~new_n174_ & new_n175_;
  assign new_n177_ = G5 & new_n119_;
  assign new_n178_ = new_n87_ & new_n177_;
  assign new_n179_ = new_n86_ & new_n112_;
  assign new_n180_ = new_n115_ & new_n179_;
  assign new_n181_ = G3 & ~G4;
  assign new_n182_ = G35 & new_n171_;
  assign new_n183_ = new_n181_ & new_n182_;
  assign new_n184_ = ~new_n180_ & ~new_n183_;
  assign new_n185_ = ~new_n178_ & new_n184_;
  assign new_n186_ = G2 & ~new_n185_;
  assign new_n187_ = ~new_n176_ & ~new_n186_;
  assign new_n188_ = ~new_n170_ & new_n187_;
  assign new_n189_ = ~G12 & new_n159_;
  assign new_n190_ = new_n188_ & new_n189_;
  assign new_n191_ = ~G0 & ~G4;
  assign new_n192_ = G6 & ~G9;
  assign new_n193_ = ~G3 & ~G5;
  assign new_n194_ = ~G7 & new_n193_;
  assign new_n195_ = new_n90_ & new_n192_;
  assign new_n196_ = new_n194_ & new_n195_;
  assign new_n197_ = G8 & G37;
  assign new_n198_ = new_n105_ & new_n197_;
  assign new_n199_ = new_n112_ & new_n198_;
  assign new_n200_ = ~new_n196_ & ~new_n199_;
  assign new_n201_ = G11 & new_n191_;
  assign new_n202_ = ~new_n200_ & new_n201_;
  assign new_n203_ = G0 & new_n180_;
  assign new_n204_ = ~new_n202_ & ~new_n203_;
  assign new_n205_ = G1 & G2;
  assign new_n206_ = ~new_n204_ & new_n205_;
  assign new_n207_ = G3 & new_n191_;
  assign new_n208_ = ~G1 & G5;
  assign new_n209_ = ~new_n140_ & ~new_n208_;
  assign new_n210_ = G2 & ~new_n209_;
  assign new_n211_ = ~G3 & ~new_n210_;
  assign new_n212_ = ~G5 & new_n211_;
  assign new_n213_ = ~new_n207_ & ~new_n212_;
  assign new_n214_ = ~new_n138_1_ & ~new_n213_;
  assign new_n215_ = ~G7 & ~G8;
  assign new_n216_ = ~G9 & new_n215_;
  assign new_n217_ = G8 & ~G31;
  assign new_n218_ = ~G6 & G7;
  assign new_n219_ = G30 & new_n218_;
  assign new_n220_ = G8 & G10;
  assign new_n221_ = G9 & ~new_n220_;
  assign new_n222_ = new_n217_ & ~new_n219_;
  assign new_n223_ = ~new_n221_ & new_n222_;
  assign new_n224_ = ~G6 & ~G30;
  assign new_n225_ = ~new_n216_ & ~new_n224_;
  assign new_n226_ = ~new_n223_ & new_n225_;
  assign new_n227_ = G11 & ~new_n226_;
  assign new_n228_ = G8 & G31;
  assign new_n229_ = ~G10 & ~G11;
  assign new_n230_ = ~G9 & new_n229_;
  assign new_n231_ = ~new_n219_ & ~new_n228_;
  assign new_n232_ = new_n230_ & new_n231_;
  assign new_n233_ = ~new_n90_ & ~new_n217_;
  assign new_n234_ = G9 & new_n117_;
  assign new_n235_ = ~new_n233_ & new_n234_;
  assign new_n236_ = G46 & ~new_n235_;
  assign new_n237_ = ~new_n232_ & new_n236_;
  assign new_n238_ = ~new_n227_ & new_n237_;
  assign new_n239_ = ~new_n214_ & new_n238_;
  assign new_n240_ = G12 & ~G13;
  assign new_n241_ = ~new_n239_ & new_n240_;
  assign new_n242_ = ~new_n206_ & new_n241_;
  assign new_n243_ = ~new_n157_ & ~new_n190_;
  assign G539 = new_n242_ | ~new_n243_;
  assign new_n245_ = G5 & ~new_n149_;
  assign new_n246_ = new_n154_ & new_n245_;
  assign new_n247_ = new_n149_ & new_n154_;
  assign new_n248_ = ~G5 & new_n247_;
  assign new_n249_ = ~new_n246_ & ~new_n248_;
  assign new_n250_ = G2 & new_n155_;
  assign new_n251_ = ~new_n249_ & new_n250_;
  assign new_n252_ = ~G12 & new_n158_;
  assign new_n253_ = ~G13 & new_n252_;
  assign new_n254_ = G2 & G4;
  assign new_n255_ = new_n112_ & ~new_n254_;
  assign new_n256_ = new_n253_ & new_n255_;
  assign new_n257_ = ~G0 & new_n149_;
  assign new_n258_ = G3 & new_n257_;
  assign new_n259_ = G0 & ~G29;
  assign new_n260_ = ~new_n258_ & ~new_n259_;
  assign new_n261_ = G12 & new_n239_;
  assign new_n262_ = ~G13 & new_n261_;
  assign new_n263_ = ~new_n260_ & new_n262_;
  assign new_n264_ = G3 & ~G13;
  assign new_n265_ = ~G33 & new_n264_;
  assign new_n266_ = ~new_n256_ & ~new_n265_;
  assign new_n267_ = ~new_n263_ & new_n266_;
  assign G550 = new_n251_ | ~new_n267_;
  assign new_n269_ = ~G1 & new_n254_;
  assign new_n270_ = new_n154_ & new_n269_;
  assign new_n271_ = new_n141_ & new_n247_;
  assign new_n272_ = ~G3 & new_n86_;
  assign new_n273_ = G6 & new_n141_;
  assign new_n274_ = ~G5 & new_n273_;
  assign new_n275_ = ~new_n138_1_ & ~new_n272_;
  assign new_n276_ = ~new_n274_ & new_n275_;
  assign new_n277_ = G1 & new_n154_;
  assign new_n278_ = ~new_n276_ & new_n277_;
  assign new_n279_ = ~new_n270_ & ~new_n271_;
  assign new_n280_ = ~new_n278_ & new_n279_;
  assign new_n281_ = new_n155_ & ~new_n280_;
  assign new_n282_ = G4 & G39;
  assign new_n283_ = new_n253_ & new_n282_;
  assign new_n284_ = G0 & G3;
  assign new_n285_ = ~new_n149_ & new_n284_;
  assign new_n286_ = G0 & G2;
  assign new_n287_ = G1 & ~new_n286_;
  assign new_n288_ = G4 & new_n286_;
  assign new_n289_ = ~new_n287_ & ~new_n288_;
  assign new_n290_ = ~G3 & ~new_n289_;
  assign new_n291_ = ~new_n257_ & ~new_n285_;
  assign new_n292_ = ~new_n290_ & new_n291_;
  assign new_n293_ = G5 & ~new_n292_;
  assign new_n294_ = new_n262_ & new_n293_;
  assign new_n295_ = ~new_n283_ & ~new_n294_;
  assign G551 = new_n281_ | ~new_n295_;
  assign new_n297_ = ~new_n138_1_ & new_n273_;
  assign new_n298_ = ~G4 & G5;
  assign new_n299_ = ~new_n138_1_ & ~new_n298_;
  assign new_n300_ = G6 & ~new_n299_;
  assign new_n301_ = ~new_n272_ & ~new_n300_;
  assign new_n302_ = G2 & ~new_n301_;
  assign new_n303_ = ~new_n297_ & ~new_n302_;
  assign new_n304_ = new_n253_ & ~new_n303_;
  assign new_n305_ = G2 & G5;
  assign new_n306_ = new_n86_ & ~new_n305_;
  assign new_n307_ = G2 & ~new_n149_;
  assign new_n308_ = ~new_n112_ & ~new_n307_;
  assign new_n309_ = G6 & ~new_n308_;
  assign new_n310_ = ~new_n306_ & ~new_n309_;
  assign new_n311_ = new_n155_ & ~new_n310_;
  assign new_n312_ = new_n154_ & new_n311_;
  assign new_n313_ = ~G40 & new_n262_;
  assign new_n314_ = ~new_n304_ & ~new_n312_;
  assign G552 = new_n313_ | ~new_n314_;
  assign new_n316_ = ~G7 & new_n220_;
  assign new_n317_ = G7 & ~new_n220_;
  assign new_n318_ = ~new_n316_ & ~new_n317_;
  assign new_n319_ = G9 & G34;
  assign new_n320_ = ~new_n318_ & new_n319_;
  assign new_n321_ = ~G6 & new_n114_;
  assign new_n322_ = G9 & new_n321_;
  assign new_n323_ = new_n116_ & ~new_n126_;
  assign new_n324_ = ~G8 & new_n91_;
  assign new_n325_ = G9 & G10;
  assign new_n326_ = ~G10 & G11;
  assign new_n327_ = ~new_n325_ & ~new_n326_;
  assign new_n328_ = new_n126_ & ~new_n327_;
  assign new_n329_ = ~new_n323_ & ~new_n324_;
  assign new_n330_ = ~new_n328_ & new_n329_;
  assign new_n331_ = G6 & ~new_n330_;
  assign new_n332_ = ~new_n322_ & ~new_n331_;
  assign new_n333_ = new_n262_ & ~new_n332_;
  assign G547 = new_n320_ | new_n333_;
  assign new_n335_ = ~G42 & new_n262_;
  assign new_n336_ = new_n91_ & new_n317_;
  assign new_n337_ = ~G9 & new_n114_;
  assign new_n338_ = ~new_n116_ & new_n126_;
  assign new_n339_ = ~new_n337_ & ~new_n338_;
  assign new_n340_ = G11 & ~new_n339_;
  assign new_n341_ = ~new_n336_ & ~new_n340_;
  assign new_n342_ = G34 & ~new_n341_;
  assign G548 = new_n335_ | new_n342_;
  assign new_n344_ = new_n149_ & ~new_n284_;
  assign new_n345_ = new_n262_ & new_n344_;
  assign new_n346_ = G3 & G4;
  assign new_n347_ = new_n305_ & ~new_n346_;
  assign new_n348_ = new_n253_ & new_n347_;
  assign new_n349_ = ~G5 & ~new_n142_1_;
  assign new_n350_ = ~G4 & ~new_n349_;
  assign new_n351_ = ~new_n141_ & ~new_n272_;
  assign new_n352_ = G5 & ~new_n351_;
  assign new_n353_ = G2 & new_n138_1_;
  assign new_n354_ = ~new_n350_ & ~new_n353_;
  assign new_n355_ = ~new_n352_ & new_n354_;
  assign new_n356_ = new_n155_ & ~new_n355_;
  assign new_n357_ = new_n277_ & new_n356_;
  assign new_n358_ = ~new_n265_ & ~new_n348_;
  assign new_n359_ = ~new_n357_ & new_n358_;
  assign G549 = new_n345_ | ~new_n359_;
  assign new_n361_ = ~G0 & G1;
  assign new_n362_ = ~G5 & new_n83_1_;
  assign new_n363_ = ~G4 & new_n112_;
  assign new_n364_ = ~new_n362_ & ~new_n363_;
  assign new_n365_ = new_n209_ & new_n364_;
  assign new_n366_ = G0 & ~new_n365_;
  assign new_n367_ = ~new_n361_ & ~new_n366_;
  assign new_n368_ = G2 & ~new_n367_;
  assign new_n369_ = new_n262_ & new_n368_;
  assign new_n370_ = new_n159_ & ~new_n188_;
  assign new_n371_ = ~G12 & new_n370_;
  assign new_n372_ = new_n169_ & new_n371_;
  assign G530 = new_n369_ | new_n372_;
  assign new_n374_ = G6 & new_n262_;
  assign new_n375_ = G8 & G34;
  assign new_n376_ = ~new_n374_ & ~new_n375_;
  assign new_n377_ = new_n125_ & ~new_n376_;
  assign new_n378_ = G8 & G9;
  assign new_n379_ = G34 & new_n114_;
  assign new_n380_ = ~new_n378_ & new_n379_;
  assign new_n381_ = G7 & ~G8;
  assign new_n382_ = ~G10 & ~new_n381_;
  assign new_n383_ = ~G9 & ~new_n382_;
  assign new_n384_ = G7 & G9;
  assign new_n385_ = new_n220_ & ~new_n384_;
  assign new_n386_ = ~new_n92_ & ~new_n385_;
  assign new_n387_ = ~new_n383_ & new_n386_;
  assign new_n388_ = new_n374_ & ~new_n387_;
  assign new_n389_ = ~new_n380_ & ~new_n388_;
  assign G542 = new_n377_ | ~new_n389_;
  assign new_n391_ = ~G2 & new_n112_;
  assign new_n392_ = G2 & ~new_n83_1_;
  assign new_n393_ = ~new_n193_ & ~new_n391_;
  assign new_n394_ = ~new_n392_ & new_n393_;
  assign new_n395_ = G4 & ~new_n394_;
  assign new_n396_ = G1 & ~G2;
  assign new_n397_ = ~G3 & new_n396_;
  assign new_n398_ = ~new_n84_ & ~new_n397_;
  assign new_n399_ = ~new_n395_ & new_n398_;
  assign new_n400_ = G0 & ~new_n399_;
  assign new_n401_ = new_n262_ & new_n400_;
  assign new_n402_ = G13 & ~new_n124_;
  assign new_n403_ = ~new_n370_ & ~new_n402_;
  assign new_n404_ = ~G4 & ~new_n165_;
  assign new_n405_ = ~new_n177_ & ~new_n404_;
  assign new_n406_ = G6 & ~new_n405_;
  assign new_n407_ = ~new_n403_ & new_n406_;
  assign new_n408_ = new_n87_ & new_n370_;
  assign new_n409_ = ~new_n402_ & ~new_n408_;
  assign new_n410_ = new_n92_ & ~new_n409_;
  assign new_n411_ = G13 & ~G43;
  assign new_n412_ = new_n154_ & new_n411_;
  assign new_n413_ = new_n107_ & new_n161_;
  assign new_n414_ = ~new_n100_ & ~new_n413_;
  assign new_n415_ = ~G3 & ~new_n414_;
  assign new_n416_ = new_n370_ & new_n415_;
  assign new_n417_ = ~new_n412_ & ~new_n416_;
  assign new_n418_ = ~new_n407_ & new_n417_;
  assign new_n419_ = ~new_n410_ & new_n418_;
  assign new_n420_ = ~G12 & ~new_n419_;
  assign G532 = new_n401_ | new_n420_;
  assign new_n422_ = new_n206_ & new_n241_;
  assign new_n423_ = new_n113_1_ & new_n422_;
  assign new_n424_ = G37 & G38;
  assign new_n425_ = new_n423_ & new_n424_;
  assign new_n426_ = ~new_n106_ & ~new_n116_;
  assign new_n427_ = new_n138_1_ & ~new_n426_;
  assign new_n428_ = new_n402_ & new_n427_;
  assign new_n429_ = ~new_n403_ & new_n404_;
  assign new_n430_ = ~new_n428_ & ~new_n429_;
  assign new_n431_ = G6 & ~G12;
  assign new_n432_ = ~new_n430_ & new_n431_;
  assign new_n433_ = new_n172_ & ~new_n426_;
  assign new_n434_ = ~G3 & ~G44;
  assign new_n435_ = ~new_n433_ & ~new_n434_;
  assign new_n436_ = new_n371_ & ~new_n435_;
  assign new_n437_ = ~new_n425_ & ~new_n436_;
  assign G535 = new_n432_ | ~new_n437_;
  assign new_n439_ = G6 & new_n106_;
  assign new_n440_ = ~new_n112_ & ~new_n439_;
  assign new_n441_ = G8 & ~new_n440_;
  assign new_n442_ = ~new_n403_ & new_n441_;
  assign new_n443_ = new_n87_ & ~new_n165_;
  assign new_n444_ = new_n402_ & new_n443_;
  assign new_n445_ = ~new_n442_ & ~new_n444_;
  assign new_n446_ = ~G12 & ~new_n445_;
  assign new_n447_ = G38 & new_n192_;
  assign new_n448_ = ~new_n114_ & ~new_n447_;
  assign new_n449_ = new_n423_ & ~new_n448_;
  assign new_n450_ = ~new_n446_ & ~new_n449_;
  assign new_n451_ = G2 & ~new_n450_;
  assign new_n452_ = new_n94_ & new_n138_1_;
  assign new_n453_ = new_n321_ & new_n452_;
  assign new_n454_ = ~G5 & new_n100_;
  assign new_n455_ = new_n229_ & new_n454_;
  assign new_n456_ = G10 & new_n91_;
  assign new_n457_ = new_n161_ & new_n456_;
  assign new_n458_ = ~new_n455_ & ~new_n457_;
  assign new_n459_ = new_n215_ & ~new_n458_;
  assign new_n460_ = ~new_n443_ & ~new_n453_;
  assign new_n461_ = ~new_n459_ & new_n460_;
  assign new_n462_ = new_n371_ & ~new_n461_;
  assign G537 = new_n451_ | new_n462_;
  assign new_n464_ = ~new_n138_1_ & new_n141_;
  assign new_n465_ = ~new_n298_ & ~new_n346_;
  assign new_n466_ = new_n133_1_ & ~new_n465_;
  assign n58 = ~new_n464_ & ~new_n466_;
  assign new_n468_ = ~G9 & G11;
  assign n63 = G10 | new_n468_;
  assign new_n470_ = G10 & ~new_n91_;
  assign new_n471_ = ~G7 & G11;
  assign n68 = new_n470_ | new_n471_;
  assign new_n473_ = ~new_n87_ & new_n305_;
  assign new_n474_ = ~new_n132_ & new_n141_;
  assign new_n475_ = ~new_n353_ & ~new_n473_;
  assign n73 = new_n474_ | ~new_n475_;
  assign new_n477_ = G0 & G1;
  assign new_n478_ = ~G4 & new_n477_;
  assign new_n479_ = new_n261_ & new_n478_;
  assign new_n480_ = new_n252_ & new_n353_;
  assign n78 = ~new_n479_ & ~new_n480_;
  assign new_n482_ = G13 & ~new_n153_;
  assign new_n483_ = ~G13 & G32;
  assign new_n484_ = ~new_n482_ & ~new_n483_;
  assign new_n485_ = ~G12 & ~new_n130_;
  assign n83 = ~new_n484_ & new_n485_;
  assign new_n487_ = ~G6 & ~G8;
  assign new_n488_ = new_n106_ & new_n487_;
  assign new_n489_ = G6 & ~new_n165_;
  assign n88 = new_n488_ | new_n489_;
  assign new_n491_ = ~G10 & new_n118_1_;
  assign new_n492_ = ~new_n115_ & ~new_n491_;
  assign n93 = ~G5 & ~new_n492_;
  assign new_n494_ = ~G6 & G9;
  assign n98 = new_n192_ | new_n494_;
  assign n103 = new_n105_ & new_n191_;
  assign new_n497_ = G2 & ~new_n112_;
  assign n108 = new_n391_ | new_n497_;
  assign new_n499_ = G6 & G9;
  assign new_n500_ = ~G11 & new_n499_;
  assign new_n501_ = ~G6 & G30;
  assign new_n502_ = ~new_n500_ & ~new_n501_;
  assign new_n503_ = G7 & ~new_n502_;
  assign new_n504_ = G6 & G31;
  assign new_n505_ = ~new_n503_ & ~new_n504_;
  assign new_n506_ = G8 & ~new_n505_;
  assign new_n507_ = G6 & new_n95_;
  assign n113 = ~new_n506_ & ~new_n507_;
  assign new_n509_ = new_n114_ & ~new_n499_;
  assign new_n510_ = new_n262_ & new_n509_;
  assign new_n511_ = G34 & new_n385_;
  assign new_n512_ = ~new_n510_ & ~new_n511_;
  assign n118 = ~new_n377_ & new_n512_;
  assign new_n514_ = G6 & new_n91_;
  assign new_n515_ = ~new_n114_ & new_n514_;
  assign new_n516_ = ~new_n116_ & new_n218_;
  assign new_n517_ = G6 & new_n220_;
  assign new_n518_ = ~new_n381_ & ~new_n517_;
  assign new_n519_ = ~G9 & ~new_n518_;
  assign new_n520_ = ~new_n516_ & ~new_n519_;
  assign new_n521_ = G11 & ~new_n520_;
  assign n123 = ~new_n515_ & ~new_n521_;
  assign new_n523_ = G3 & new_n134_;
  assign new_n524_ = G1 & new_n273_;
  assign new_n525_ = G5 & ~new_n86_;
  assign new_n526_ = ~G6 & new_n254_;
  assign new_n527_ = G6 & new_n138_1_;
  assign new_n528_ = ~new_n525_ & ~new_n526_;
  assign new_n529_ = ~new_n527_ & new_n528_;
  assign new_n530_ = new_n83_1_ & ~new_n529_;
  assign new_n531_ = ~new_n523_ & ~new_n524_;
  assign n128 = ~new_n530_ & new_n531_;
  assign new_n533_ = new_n115_ & new_n454_;
  assign new_n534_ = ~G5 & ~G6;
  assign new_n535_ = new_n230_ & new_n534_;
  assign new_n536_ = ~new_n457_ & ~new_n535_;
  assign new_n537_ = new_n215_ & ~new_n536_;
  assign n133 = ~new_n533_ & ~new_n537_;
  assign new_n539_ = ~new_n124_ & new_n156_;
  assign new_n540_ = ~new_n371_ & ~new_n422_;
  assign n138 = new_n539_ | ~new_n540_;
  assign new_n542_ = new_n138_1_ & new_n141_;
  assign new_n543_ = ~new_n138_1_ & new_n211_;
  assign new_n544_ = G0 & ~new_n542_;
  assign new_n545_ = ~new_n543_ & new_n544_;
  assign new_n546_ = ~G1 & ~new_n545_;
  assign new_n547_ = ~G10 & new_n224_;
  assign new_n548_ = G0 & ~G3;
  assign new_n549_ = G0 & G4;
  assign new_n550_ = ~new_n99_ & ~new_n548_;
  assign new_n551_ = ~new_n549_ & new_n550_;
  assign new_n552_ = G2 & new_n209_;
  assign new_n553_ = ~new_n551_ & new_n552_;
  assign new_n554_ = ~G6 & ~G7;
  assign new_n555_ = ~new_n547_ & ~new_n554_;
  assign new_n556_ = ~new_n553_ & new_n555_;
  assign n142 = ~new_n546_ & new_n556_;
  assign G546 = ~G41;
  always @ posedge clock begin
    G29 <= n58;
    G30 <= n63;
    G31 <= n68;
    G32 <= n73;
    G33 <= n78;
    G34 <= n83;
    G35 <= n88;
    G36 <= n93;
    G37 <= n98;
    G38 <= n103;
    G39 <= n108;
    G40 <= n113;
    G41 <= n118;
    G42 <= n123;
    G43 <= n128;
    G44 <= n133;
    G45 <= n138;
    G46 <= n142;
  end
  initial begin
    G29 <= 1'b0;
    G30 <= 1'b0;
    G31 <= 1'b0;
    G32 <= 1'b0;
    G33 <= 1'b0;
    G34 <= 1'b0;
    G35 <= 1'b0;
    G36 <= 1'b0;
    G37 <= 1'b0;
    G38 <= 1'b0;
    G39 <= 1'b0;
    G40 <= 1'b0;
    G41 <= 1'b0;
    G42 <= 1'b0;
    G43 <= 1'b0;
    G44 <= 1'b0;
    G45 <= 1'b0;
    G46 <= 1'b0;
  end
endmodule


