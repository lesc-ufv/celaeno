// Benchmark "i4" written by ABC on Wed Mar 10 23:12:54 2021

module i4  
    V560 , V280 , V561 , V281 , V562 , V282 , V566 ,
    V286 , V5610 , V2810 , V5614 , V2814 , V5618 ,
    V2818 , V5622 , V2822 , V5626 , V2826 , V1202 ,
    V882 , V1206 , V886 , V12010 , V8810 , V12014 ,
    V8814 , V12018 , V8818 , V12022 , V8822 , V12026 ,
    V8826 , V12030 , V8830 , V1322 , V1262 , V1440 ,
    V283 , V563 , V1441 , V564 , V284 , V1442 , V285 ,
    V565 , V1444 , V287 , V567 , V1445 , V568 , V288 ,
    V1446 , V289 , V569 , V1448 , V2811 , V5611 ,
    V1449 , V5612 , V2812 , V14410 , V2813 , V5613 ,
    V14412 , V2815 , V5615 , V14413 , V5616 , V2816 ,
    V14414 , V2817 , V5617 , V1560 , V2819 , V5619 ,
    V1561 , V5620 , V2820 , V1562 , V2821 , V5621 ,
    V1564 , V2823 , V5623 , V1565 , V5624 , V2824 ,
    V1566 , V2825 , V5625 , V1568 , V2827 , V5627 ,
    V1569 , V1200 , V880 , V15610 , V881 , V1201 ,
    V15612 , V883 , V1203 , V15613 , V1204 , V884 ,
    V15614 , V885 , V1205 , V1680 , V887 , V1207 ,
    V1681 , V1208 , V888 , V1682 , V889 , V1209 ,
    V1684 , V8811 , V12011 , V1685 , V12012 , V8812 ,
    V1686 , V8813 , V12013 , V1688 , V8815 , V12015 ,
    V1689 , V12016 , V8816 , V16810 , V8817 , V12017 ,
    V16812 , V8819 , V12019 , V16813 , V12020 , V8820 ,
    V16814 , V8821 , V12021 , V1800 , V8823 , V12023 ,
    V1801 , V12024 , V8824 , V1802 , V8825 , V12025 ,
    V1804 , V8827 , V12027 , V1805 , V12028 , V8828 ,
    V1806 , V8829 , V12029 , V1808 , V8831 , V12031 ,
    V1809 , V1320 , V1260 , V18010 , V1261 , V1321 ,
    V18012 , V1263 , V1323 , V18013 , V1324 , V1264 ,
    V18014 , V1265 , V1325 , V1830 , V1831 , V1832 ,
    V1860 , V1861 , V1862 , V1890 , V1891 , V1892 ,
    V1920 , V1921 , V1922 ,
    V1940 , V1941 , V1980 , V1981 , V1982 , V1983   ;
  input  V560 , V280 , V561 , V281 , V562 , V282 ,
    V566 , V286 , V5610 , V2810 , V5614 , V2814 ,
    V5618 , V2818 , V5622 , V2822 , V5626 , V2826 ,
    V1202 , V882 , V1206 , V886 , V12010 , V8810 ,
    V12014 , V8814 , V12018 , V8818 , V12022 , V8822 ,
    V12026 , V8826 , V12030 , V8830 , V1322 , V1262 ,
    V1440 , V283 , V563 , V1441 , V564 , V284 ,
    V1442 , V285 , V565 , V1444 , V287 , V567 ,
    V1445 , V568 , V288 , V1446 , V289 , V569 ,
    V1448 , V2811 , V5611 , V1449 , V5612 , V2812 ,
    V14410 , V2813 , V5613 , V14412 , V2815 , V5615 ,
    V14413 , V5616 , V2816 , V14414 , V2817 , V5617 ,
    V1560 , V2819 , V5619 , V1561 , V5620 , V2820 ,
    V1562 , V2821 , V5621 , V1564 , V2823 , V5623 ,
    V1565 , V5624 , V2824 , V1566 , V2825 , V5625 ,
    V1568 , V2827 , V5627 , V1569 , V1200 , V880 ,
    V15610 , V881 , V1201 , V15612 , V883 , V1203 ,
    V15613 , V1204 , V884 , V15614 , V885 , V1205 ,
    V1680 , V887 , V1207 , V1681 , V1208 , V888 ,
    V1682 , V889 , V1209 , V1684 , V8811 , V12011 ,
    V1685 , V12012 , V8812 , V1686 , V8813 , V12013 ,
    V1688 , V8815 , V12015 , V1689 , V12016 , V8816 ,
    V16810 , V8817 , V12017 , V16812 , V8819 , V12019 ,
    V16813 , V12020 , V8820 , V16814 , V8821 , V12021 ,
    V1800 , V8823 , V12023 , V1801 , V12024 , V8824 ,
    V1802 , V8825 , V12025 , V1804 , V8827 , V12027 ,
    V1805 , V12028 , V8828 , V1806 , V8829 , V12029 ,
    V1808 , V8831 , V12031 , V1809 , V1320 , V1260 ,
    V18010 , V1261 , V1321 , V18012 , V1263 , V1323 ,
    V18013 , V1324 , V1264 , V18014 , V1265 , V1325 ,
    V1830 , V1831 , V1832 , V1860 , V1861 , V1862 ,
    V1890 , V1891 , V1892 , V1920 , V1921 , V1922 ;
  output V1940 , V1941 , V1980 , V1981 , V1982 , V1983 ;
  wire new_n201_, new_n202_, new_n203_, new_n204_, new_n205_, new_n206_,
    new_n207_, new_n208_, new_n209_, new_n210_, new_n211_, new_n212_,
    new_n213_, new_n214_, new_n215_, new_n216_, new_n217_, new_n218_,
    new_n219_, new_n220_, new_n221_, new_n222_, new_n223_, new_n224_,
    new_n225_, new_n226_, new_n227_, new_n228_, new_n229_, new_n230_,
    new_n231_, new_n232_, new_n233_, new_n234_, new_n235_, new_n236_,
    new_n237_, new_n238_, new_n239_, new_n240_, new_n241_, new_n242_,
    new_n243_, new_n244_, new_n245_, new_n246_, new_n247_, new_n248_,
    new_n249_, new_n250_, new_n251_, new_n252_, new_n253_, new_n254_,
    new_n255_, new_n257_, new_n258_, new_n259_, new_n260_, new_n261_,
    new_n262_, new_n263_, new_n264_, new_n265_, new_n266_, new_n267_,
    new_n268_, new_n269_, new_n270_, new_n271_, new_n272_, new_n273_,
    new_n274_, new_n275_, new_n276_, new_n277_, new_n278_, new_n279_,
    new_n280_, new_n281_, new_n282_, new_n283_, new_n284_, new_n285_,
    new_n286_, new_n287_, new_n288_, new_n289_, new_n290_, new_n291_,
    new_n292_, new_n293_, new_n294_, new_n295_, new_n296_, new_n297_,
    new_n298_, new_n299_, new_n300_, new_n301_, new_n302_, new_n303_,
    new_n304_, new_n305_, new_n306_, new_n307_, new_n308_, new_n309_,
    new_n310_, new_n311_, new_n313_, new_n314_, new_n315_, new_n316_,
    new_n317_, new_n318_, new_n319_, new_n320_, new_n321_, new_n322_,
    new_n323_, new_n324_, new_n325_, new_n326_, new_n327_, new_n328_,
    new_n329_, new_n330_, new_n331_, new_n332_, new_n333_, new_n334_,
    new_n335_, new_n336_, new_n337_, new_n338_, new_n339_, new_n340_,
    new_n341_, new_n342_, new_n343_, new_n344_, new_n345_, new_n346_,
    new_n347_, new_n348_, new_n349_, new_n350_, new_n351_, new_n352_,
    new_n353_, new_n354_, new_n355_, new_n356_, new_n357_, new_n358_,
    new_n359_, new_n360_, new_n361_, new_n362_, new_n363_, new_n364_,
    new_n365_, new_n366_, new_n367_, new_n369_, new_n370_, new_n371_,
    new_n372_, new_n373_, new_n374_, new_n375_, new_n376_, new_n377_,
    new_n378_, new_n379_, new_n380_, new_n381_, new_n382_, new_n383_,
    new_n384_, new_n385_, new_n386_, new_n387_, new_n388_, new_n389_,
    new_n390_, new_n391_, new_n392_, new_n393_, new_n394_, new_n395_,
    new_n396_, new_n397_, new_n398_, new_n399_, new_n400_, new_n401_,
    new_n402_, new_n403_, new_n404_, new_n405_, new_n406_, new_n407_,
    new_n408_, new_n409_, new_n410_, new_n411_, new_n412_, new_n413_,
    new_n414_, new_n415_, new_n416_, new_n417_, new_n418_, new_n419_,
    new_n420_, new_n421_, new_n422_, new_n423_;
  assign V1940  = V560  & V280 ;
  assign V1941  = V561  & V281 ;
  assign new_n201_ = V1440  & V1441 ;
  assign new_n202_ = V564  & V284 ;
  assign new_n203_ = new_n201_ & new_n202_;
  assign new_n204_ = V562  & V282 ;
  assign new_n205_ = V5614  & V2814 ;
  assign new_n206_ = V14412  & V14413 ;
  assign new_n207_ = V5616  & V2816 ;
  assign new_n208_ = new_n206_ & new_n207_;
  assign new_n209_ = V14414  & V2817 ;
  assign new_n210_ = V5617  & new_n209_;
  assign new_n211_ = new_n206_ & new_n210_;
  assign new_n212_ = V14412  & V2815 ;
  assign new_n213_ = V5615  & new_n212_;
  assign new_n214_ = ~new_n205_ & ~new_n208_;
  assign new_n215_ = ~new_n213_ & new_n214_;
  assign new_n216_ = ~new_n211_ & new_n215_;
  assign new_n217_ = V1830  & V1831 ;
  assign new_n218_ = V1832  & new_n217_;
  assign new_n219_ = ~new_n216_ & new_n218_;
  assign new_n220_ = V566  & V286 ;
  assign new_n221_ = V1444  & V1445 ;
  assign new_n222_ = V568  & V288 ;
  assign new_n223_ = new_n221_ & new_n222_;
  assign new_n224_ = V1446  & V289 ;
  assign new_n225_ = V569  & new_n224_;
  assign new_n226_ = new_n221_ & new_n225_;
  assign new_n227_ = V1444  & V287 ;
  assign new_n228_ = V567  & new_n227_;
  assign new_n229_ = ~new_n220_ & ~new_n223_;
  assign new_n230_ = ~new_n228_ & new_n229_;
  assign new_n231_ = ~new_n226_ & new_n230_;
  assign new_n232_ = V1830  & ~new_n231_;
  assign new_n233_ = V5610  & V2810 ;
  assign new_n234_ = V1448  & V1449 ;
  assign new_n235_ = V5612  & V2812 ;
  assign new_n236_ = new_n234_ & new_n235_;
  assign new_n237_ = V14410  & V2813 ;
  assign new_n238_ = V5613  & new_n237_;
  assign new_n239_ = new_n234_ & new_n238_;
  assign new_n240_ = V1448  & V2811 ;
  assign new_n241_ = V5611  & new_n240_;
  assign new_n242_ = ~new_n233_ & ~new_n236_;
  assign new_n243_ = ~new_n241_ & new_n242_;
  assign new_n244_ = ~new_n239_ & new_n243_;
  assign new_n245_ = new_n217_ & ~new_n244_;
  assign new_n246_ = V1440  & V283 ;
  assign new_n247_ = V563  & new_n246_;
  assign new_n248_ = V1442  & V285 ;
  assign new_n249_ = V565  & new_n248_;
  assign new_n250_ = new_n201_ & new_n249_;
  assign new_n251_ = ~new_n203_ & ~new_n204_;
  assign new_n252_ = ~new_n247_ & new_n251_;
  assign new_n253_ = ~new_n250_ & new_n252_;
  assign new_n254_ = ~new_n219_ & new_n253_;
  assign new_n255_ = ~new_n232_ & ~new_n245_;
  assign V1980  = ~new_n254_ | ~new_n255_;
  assign new_n257_ = V1560  & V1561 ;
  assign new_n258_ = V5620  & V2820 ;
  assign new_n259_ = new_n257_ & new_n258_;
  assign new_n260_ = V5618  & V2818 ;
  assign new_n261_ = V1202  & V882 ;
  assign new_n262_ = V15612  & V15613 ;
  assign new_n263_ = V1204  & V884 ;
  assign new_n264_ = new_n262_ & new_n263_;
  assign new_n265_ = V15614  & V885 ;
  assign new_n266_ = V1205  & new_n265_;
  assign new_n267_ = new_n262_ & new_n266_;
  assign new_n268_ = V15612  & V883 ;
  assign new_n269_ = V1203  & new_n268_;
  assign new_n270_ = ~new_n261_ & ~new_n264_;
  assign new_n271_ = ~new_n269_ & new_n270_;
  assign new_n272_ = ~new_n267_ & new_n271_;
  assign new_n273_ = V1860  & V1861 ;
  assign new_n274_ = V1862  & new_n273_;
  assign new_n275_ = ~new_n272_ & new_n274_;
  assign new_n276_ = V5622  & V2822 ;
  assign new_n277_ = V1564  & V1565 ;
  assign new_n278_ = V5624  & V2824 ;
  assign new_n279_ = new_n277_ & new_n278_;
  assign new_n280_ = V1566  & V2825 ;
  assign new_n281_ = V5625  & new_n280_;
  assign new_n282_ = new_n277_ & new_n281_;
  assign new_n283_ = V1564  & V2823 ;
  assign new_n284_ = V5623  & new_n283_;
  assign new_n285_ = ~new_n276_ & ~new_n279_;
  assign new_n286_ = ~new_n284_ & new_n285_;
  assign new_n287_ = ~new_n282_ & new_n286_;
  assign new_n288_ = V1860  & ~new_n287_;
  assign new_n289_ = V5626  & V2826 ;
  assign new_n290_ = V1568  & V1569 ;
  assign new_n291_ = V1200  & V880 ;
  assign new_n292_ = new_n290_ & new_n291_;
  assign new_n293_ = V15610  & V881 ;
  assign new_n294_ = V1201  & new_n293_;
  assign new_n295_ = new_n290_ & new_n294_;
  assign new_n296_ = V1568  & V2827 ;
  assign new_n297_ = V5627  & new_n296_;
  assign new_n298_ = ~new_n289_ & ~new_n292_;
  assign new_n299_ = ~new_n297_ & new_n298_;
  assign new_n300_ = ~new_n295_ & new_n299_;
  assign new_n301_ = new_n273_ & ~new_n300_;
  assign new_n302_ = V1560  & V2819 ;
  assign new_n303_ = V5619  & new_n302_;
  assign new_n304_ = V1562  & V2821 ;
  assign new_n305_ = V5621  & new_n304_;
  assign new_n306_ = new_n257_ & new_n305_;
  assign new_n307_ = ~new_n259_ & ~new_n260_;
  assign new_n308_ = ~new_n303_ & new_n307_;
  assign new_n309_ = ~new_n306_ & new_n308_;
  assign new_n310_ = ~new_n275_ & new_n309_;
  assign new_n311_ = ~new_n288_ & ~new_n301_;
  assign V1981  = ~new_n310_ | ~new_n311_;
  assign new_n313_ = V1680  & V1681 ;
  assign new_n314_ = V1208  & V888 ;
  assign new_n315_ = new_n313_ & new_n314_;
  assign new_n316_ = V1206  & V886 ;
  assign new_n317_ = V12018  & V8818 ;
  assign new_n318_ = V16812  & V16813 ;
  assign new_n319_ = V12020  & V8820 ;
  assign new_n320_ = new_n318_ & new_n319_;
  assign new_n321_ = V16814  & V8821 ;
  assign new_n322_ = V12021  & new_n321_;
  assign new_n323_ = new_n318_ & new_n322_;
  assign new_n324_ = V16812  & V8819 ;
  assign new_n325_ = V12019  & new_n324_;
  assign new_n326_ = ~new_n317_ & ~new_n320_;
  assign new_n327_ = ~new_n325_ & new_n326_;
  assign new_n328_ = ~new_n323_ & new_n327_;
  assign new_n329_ = V1890  & V1891 ;
  assign new_n330_ = V1892  & new_n329_;
  assign new_n331_ = ~new_n328_ & new_n330_;
  assign new_n332_ = V12010  & V8810 ;
  assign new_n333_ = V1684  & V1685 ;
  assign new_n334_ = V12012  & V8812 ;
  assign new_n335_ = new_n333_ & new_n334_;
  assign new_n336_ = V1686  & V8813 ;
  assign new_n337_ = V12013  & new_n336_;
  assign new_n338_ = new_n333_ & new_n337_;
  assign new_n339_ = V1684  & V8811 ;
  assign new_n340_ = V12011  & new_n339_;
  assign new_n341_ = ~new_n332_ & ~new_n335_;
  assign new_n342_ = ~new_n340_ & new_n341_;
  assign new_n343_ = ~new_n338_ & new_n342_;
  assign new_n344_ = V1890  & ~new_n343_;
  assign new_n345_ = V12014  & V8814 ;
  assign new_n346_ = V1688  & V1689 ;
  assign new_n347_ = V12016  & V8816 ;
  assign new_n348_ = new_n346_ & new_n347_;
  assign new_n349_ = V16810  & V8817 ;
  assign new_n350_ = V12017  & new_n349_;
  assign new_n351_ = new_n346_ & new_n350_;
  assign new_n352_ = V1688  & V8815 ;
  assign new_n353_ = V12015  & new_n352_;
  assign new_n354_ = ~new_n345_ & ~new_n348_;
  assign new_n355_ = ~new_n353_ & new_n354_;
  assign new_n356_ = ~new_n351_ & new_n355_;
  assign new_n357_ = new_n329_ & ~new_n356_;
  assign new_n358_ = V1680  & V887 ;
  assign new_n359_ = V1207  & new_n358_;
  assign new_n360_ = V1682  & V889 ;
  assign new_n361_ = V1209  & new_n360_;
  assign new_n362_ = new_n313_ & new_n361_;
  assign new_n363_ = ~new_n315_ & ~new_n316_;
  assign new_n364_ = ~new_n359_ & new_n363_;
  assign new_n365_ = ~new_n362_ & new_n364_;
  assign new_n366_ = ~new_n331_ & new_n365_;
  assign new_n367_ = ~new_n344_ & ~new_n357_;
  assign V1982  = ~new_n366_ | ~new_n367_;
  assign new_n369_ = V1800  & V1801 ;
  assign new_n370_ = V12024  & V8824 ;
  assign new_n371_ = new_n369_ & new_n370_;
  assign new_n372_ = V12022  & V8822 ;
  assign new_n373_ = V1322  & V1262 ;
  assign new_n374_ = V18012  & V18013 ;
  assign new_n375_ = V1324  & V1264 ;
  assign new_n376_ = new_n374_ & new_n375_;
  assign new_n377_ = V18014  & V1265 ;
  assign new_n378_ = V1325  & new_n377_;
  assign new_n379_ = new_n374_ & new_n378_;
  assign new_n380_ = V18012  & V1263 ;
  assign new_n381_ = V1323  & new_n380_;
  assign new_n382_ = ~new_n373_ & ~new_n376_;
  assign new_n383_ = ~new_n381_ & new_n382_;
  assign new_n384_ = ~new_n379_ & new_n383_;
  assign new_n385_ = V1920  & V1921 ;
  assign new_n386_ = V1922  & new_n385_;
  assign new_n387_ = ~new_n384_ & new_n386_;
  assign new_n388_ = V12026  & V8826 ;
  assign new_n389_ = V1804  & V1805 ;
  assign new_n390_ = V12028  & V8828 ;
  assign new_n391_ = new_n389_ & new_n390_;
  assign new_n392_ = V1806  & V8829 ;
  assign new_n393_ = V12029  & new_n392_;
  assign new_n394_ = new_n389_ & new_n393_;
  assign new_n395_ = V1804  & V8827 ;
  assign new_n396_ = V12027  & new_n395_;
  assign new_n397_ = ~new_n388_ & ~new_n391_;
  assign new_n398_ = ~new_n396_ & new_n397_;
  assign new_n399_ = ~new_n394_ & new_n398_;
  assign new_n400_ = V1920  & ~new_n399_;
  assign new_n401_ = V12030  & V8830 ;
  assign new_n402_ = V1808  & V1809 ;
  assign new_n403_ = V1320  & V1260 ;
  assign new_n404_ = new_n402_ & new_n403_;
  assign new_n405_ = V18010  & V1261 ;
  assign new_n406_ = V1321  & new_n405_;
  assign new_n407_ = new_n402_ & new_n406_;
  assign new_n408_ = V1808  & V8831 ;
  assign new_n409_ = V12031  & new_n408_;
  assign new_n410_ = ~new_n401_ & ~new_n404_;
  assign new_n411_ = ~new_n409_ & new_n410_;
  assign new_n412_ = ~new_n407_ & new_n411_;
  assign new_n413_ = new_n385_ & ~new_n412_;
  assign new_n414_ = V1800  & V8823 ;
  assign new_n415_ = V12023  & new_n414_;
  assign new_n416_ = V1802  & V8825 ;
  assign new_n417_ = V12025  & new_n416_;
  assign new_n418_ = new_n369_ & new_n417_;
  assign new_n419_ = ~new_n371_ & ~new_n372_;
  assign new_n420_ = ~new_n415_ & new_n419_;
  assign new_n421_ = ~new_n418_ & new_n420_;
  assign new_n422_ = ~new_n387_ & new_n421_;
  assign new_n423_ = ~new_n400_ & ~new_n413_;
  assign V1983  = ~new_n422_ | ~new_n423_;
endmodule


