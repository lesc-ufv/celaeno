// Benchmark "apex7" written by ABC on Wed Mar 10 23:12:51 2021

module apex7  
    CAPSD, CAT0, CAT1, CAT2, CAT3, CAT4, CAT5, VACC, MMERR, IBT0, IBT1,
    IBT2, ICLR, LSD, ACCRPY, VERR_N, RATR, MARSSR, VLENESR, VSUMESR,
    PLUTO0, PLUTO1, PLUTO2, PLUTO3, PLUTO4, PLUTO5, ORWD_N, OWL_N, PY, END,
    FBI, WATCH, OVACC, KBG_N, DEL1, COMPPAR, VST0, VST1, STAR0, STAR1,
    STAR2, STAR3, BULL0, BULL1, BULL2, BULL3, BULL4, BULL5, BULL6,
    SDO, LSD_P, ACCRPY_P, VERR_F, RATR_P, MARSSR_P, VLENESR_P, VSUMESR_P,
    PLUTO0_P, PLUTO1_P, PLUTO2_P, PLUTO3_P, PLUTO4_P, PLUTO5_P, ORWD_F,
    OWL_F, PY_P, END_P, FBI_P, WATCH_P, OVACC_P, KBG_F, DEL1_P, COMPPAR_P,
    VST0_P, VST1_P, STAR0_P, STAR1_P, STAR2_P, STAR3_P, BULL0_P, BULL1_P,
    BULL2_P, BULL3_P, BULL4_P, BULL5_P, BULL6_P  ;
  input  CAPSD, CAT0, CAT1, CAT2, CAT3, CAT4, CAT5, VACC, MMERR, IBT0,
    IBT1, IBT2, ICLR, LSD, ACCRPY, VERR_N, RATR, MARSSR, VLENESR, VSUMESR,
    PLUTO0, PLUTO1, PLUTO2, PLUTO3, PLUTO4, PLUTO5, ORWD_N, OWL_N, PY, END,
    FBI, WATCH, OVACC, KBG_N, DEL1, COMPPAR, VST0, VST1, STAR0, STAR1,
    STAR2, STAR3, BULL0, BULL1, BULL2, BULL3, BULL4, BULL5, BULL6;
  output SDO, LSD_P, ACCRPY_P, VERR_F, RATR_P, MARSSR_P, VLENESR_P, VSUMESR_P,
    PLUTO0_P, PLUTO1_P, PLUTO2_P, PLUTO3_P, PLUTO4_P, PLUTO5_P, ORWD_F,
    OWL_F, PY_P, END_P, FBI_P, WATCH_P, OVACC_P, KBG_F, DEL1_P, COMPPAR_P,
    VST0_P, VST1_P, STAR0_P, STAR1_P, STAR2_P, STAR3_P, BULL0_P, BULL1_P,
    BULL2_P, BULL3_P, BULL4_P, BULL5_P, BULL6_P;
  wire new_n87_, new_n88_, new_n89_, new_n90_, new_n91_, new_n92_, new_n93_,
    new_n94_, new_n95_, new_n96_, new_n97_, new_n98_, new_n99_, new_n100_,
    new_n101_, new_n102_, new_n103_, new_n104_, new_n105_, new_n106_,
    new_n107_, new_n108_, new_n109_, new_n110_, new_n111_, new_n112_,
    new_n114_, new_n116_, new_n117_, new_n118_, new_n119_, new_n121_,
    new_n122_, new_n123_, new_n124_, new_n125_, new_n126_, new_n127_,
    new_n128_, new_n129_, new_n130_, new_n131_, new_n132_, new_n133_,
    new_n134_, new_n135_, new_n136_, new_n137_, new_n138_, new_n140_,
    new_n141_, new_n142_, new_n143_, new_n144_, new_n145_, new_n147_,
    new_n148_, new_n150_, new_n152_, new_n153_, new_n155_, new_n156_,
    new_n157_, new_n158_, new_n159_, new_n160_, new_n161_, new_n162_,
    new_n163_, new_n165_, new_n166_, new_n168_, new_n169_, new_n170_,
    new_n171_, new_n173_, new_n174_, new_n175_, new_n177_, new_n178_,
    new_n179_, new_n181_, new_n182_, new_n183_, new_n185_, new_n186_,
    new_n188_, new_n189_, new_n190_, new_n191_, new_n194_, new_n195_,
    new_n196_, new_n197_, new_n198_, new_n199_, new_n200_, new_n201_,
    new_n202_, new_n203_, new_n204_, new_n205_, new_n206_, new_n207_,
    new_n208_, new_n209_, new_n211_, new_n212_, new_n215_, new_n216_,
    new_n219_, new_n220_, new_n221_, new_n222_, new_n223_, new_n225_,
    new_n226_, new_n228_, new_n229_, new_n231_, new_n232_, new_n233_,
    new_n234_, new_n235_, new_n237_, new_n238_, new_n239_, new_n240_,
    new_n241_, new_n242_, new_n243_, new_n245_, new_n246_, new_n247_,
    new_n248_, new_n249_, new_n251_, new_n252_, new_n253_, new_n254_,
    new_n255_, new_n256_, new_n258_, new_n259_, new_n260_, new_n262_,
    new_n263_, new_n264_, new_n265_, new_n266_, new_n267_, new_n269_,
    new_n270_, new_n271_, new_n273_, new_n274_, new_n275_, new_n276_,
    new_n277_, new_n279_, new_n280_, new_n281_, new_n282_, new_n283_,
    new_n284_, new_n286_, new_n287_, new_n288_, new_n290_, new_n291_,
    new_n292_, new_n293_, new_n294_, new_n295_, new_n296_, new_n297_,
    new_n298_, new_n299_;
  assign new_n87_ = STAR0 & STAR1;
  assign new_n88_ = ~STAR2 & new_n87_;
  assign new_n89_ = IBT1 & ~IBT2;
  assign new_n90_ = ~CAT1 & IBT0;
  assign new_n91_ = ~CAT0 & ~IBT0;
  assign new_n92_ = ~new_n90_ & ~new_n91_;
  assign new_n93_ = new_n89_ & ~new_n92_;
  assign new_n94_ = ~CAT5 & IBT1;
  assign new_n95_ = ~CAT3 & ~IBT1;
  assign new_n96_ = ~new_n94_ & ~new_n95_;
  assign new_n97_ = IBT0 & ~new_n96_;
  assign new_n98_ = ~CAT4 & IBT1;
  assign new_n99_ = ~CAT2 & ~IBT1;
  assign new_n100_ = ~new_n98_ & ~new_n99_;
  assign new_n101_ = ~IBT0 & ~new_n100_;
  assign new_n102_ = ~new_n97_ & ~new_n101_;
  assign new_n103_ = IBT2 & ~new_n102_;
  assign new_n104_ = ~new_n93_ & ~new_n103_;
  assign new_n105_ = OWL_N & WATCH;
  assign new_n106_ = ~new_n104_ & new_n105_;
  assign new_n107_ = FBI & new_n88_;
  assign new_n108_ = new_n106_ & new_n107_;
  assign new_n109_ = LSD & OWL_N;
  assign new_n110_ = ~new_n108_ & ~new_n109_;
  assign new_n111_ = ~STAR3 & ~new_n110_;
  assign new_n112_ = ~new_n107_ & new_n109_;
  assign LSD_P = new_n111_ | new_n112_;
  assign new_n114_ = OWL_N & FBI;
  assign ORWD_F = ~WATCH | new_n104_;
  assign new_n116_ = new_n88_ & ORWD_F;
  assign new_n117_ = new_n114_ & ~new_n116_;
  assign new_n118_ = new_n114_ & ~new_n117_;
  assign new_n119_ = ACCRPY & OWL_N;
  assign ACCRPY_P = new_n118_ | new_n119_;
  assign new_n121_ = ~BULL0 & BULL1;
  assign new_n122_ = ~BULL2 & ~BULL3;
  assign new_n123_ = BULL4 & ~BULL5;
  assign new_n124_ = BULL6 & new_n123_;
  assign new_n125_ = new_n121_ & new_n122_;
  assign new_n126_ = new_n124_ & new_n125_;
  assign new_n127_ = WATCH & new_n126_;
  assign new_n128_ = ~new_n117_ & ~new_n127_;
  assign new_n129_ = STAR3 & new_n88_;
  assign new_n130_ = ~CAT0 & ~CAT1;
  assign new_n131_ = WATCH & new_n130_;
  assign new_n132_ = new_n89_ & new_n131_;
  assign new_n133_ = ORWD_F & ~new_n132_;
  assign new_n134_ = ~new_n129_ & ~new_n133_;
  assign new_n135_ = ~new_n126_ & new_n134_;
  assign new_n136_ = ~new_n128_ & ~new_n135_;
  assign new_n137_ = VERR_N & ~new_n136_;
  assign new_n138_ = ~OWL_N & ~new_n114_;
  assign VERR_F = new_n137_ | new_n138_;
  assign new_n140_ = OWL_N & END;
  assign new_n141_ = ~MMERR & ~VST0;
  assign new_n142_ = new_n140_ & new_n141_;
  assign new_n143_ = RATR & OWL_N;
  assign new_n144_ = ~COMPPAR & new_n140_;
  assign new_n145_ = ~new_n142_ & ~new_n143_;
  assign RATR_P = new_n144_ | ~new_n145_;
  assign new_n147_ = MARSSR & OWL_N;
  assign new_n148_ = new_n105_ & new_n126_;
  assign MARSSR_P = new_n147_ | new_n148_;
  assign new_n150_ = ~VLENESR & KBG_N;
  assign VLENESR_P = OWL_N & ~new_n150_;
  assign new_n152_ = VSUMESR & OWL_N;
  assign new_n153_ = VST1 & new_n140_;
  assign VSUMESR_P = new_n152_ | new_n153_;
  assign new_n155_ = PLUTO0 & OWL_N;
  assign new_n156_ = COMPPAR & ~VST1;
  assign new_n157_ = new_n140_ & ~new_n156_;
  assign new_n158_ = OWL_N & ~KBG_N;
  assign new_n159_ = ~new_n142_ & ~new_n158_;
  assign new_n160_ = ~new_n157_ & new_n159_;
  assign new_n161_ = ~new_n148_ & new_n160_;
  assign new_n162_ = new_n89_ & ~new_n161_;
  assign new_n163_ = ~IBT0 & new_n162_;
  assign PLUTO0_P = new_n155_ | new_n163_;
  assign new_n165_ = PLUTO1 & OWL_N;
  assign new_n166_ = IBT0 & new_n162_;
  assign PLUTO1_P = new_n165_ | new_n166_;
  assign new_n168_ = PLUTO2 & OWL_N;
  assign new_n169_ = IBT2 & ~new_n161_;
  assign new_n170_ = ~IBT0 & ~IBT1;
  assign new_n171_ = new_n169_ & new_n170_;
  assign PLUTO2_P = new_n168_ | new_n171_;
  assign new_n173_ = PLUTO3 & OWL_N;
  assign new_n174_ = IBT0 & ~IBT1;
  assign new_n175_ = new_n169_ & new_n174_;
  assign PLUTO3_P = new_n173_ | new_n175_;
  assign new_n177_ = PLUTO4 & OWL_N;
  assign new_n178_ = ~IBT0 & IBT1;
  assign new_n179_ = new_n169_ & new_n178_;
  assign PLUTO4_P = new_n177_ | new_n179_;
  assign new_n181_ = PLUTO5 & OWL_N;
  assign new_n182_ = IBT0 & IBT1;
  assign new_n183_ = new_n169_ & new_n182_;
  assign PLUTO5_P = new_n181_ | new_n183_;
  assign new_n185_ = ~ICLR & ~END;
  assign new_n186_ = KBG_N & new_n185_;
  assign OWL_F = ~new_n127_ & new_n186_;
  assign new_n188_ = ~ICLR & FBI;
  assign new_n189_ = DEL1 & new_n188_;
  assign new_n190_ = ~ICLR & ~FBI;
  assign new_n191_ = PY & new_n190_;
  assign PY_P = new_n189_ | new_n191_;
  assign END_P = new_n118_ | new_n140_;
  assign new_n194_ = ORWD_N & new_n87_;
  assign new_n195_ = ~ORWD_N & new_n106_;
  assign new_n196_ = ~new_n114_ & ~new_n195_;
  assign new_n197_ = new_n106_ & ~new_n196_;
  assign new_n198_ = ~new_n88_ & new_n114_;
  assign new_n199_ = ~new_n197_ & ~new_n198_;
  assign new_n200_ = ~new_n194_ & ~new_n199_;
  assign new_n201_ = OWL_N & ~new_n87_;
  assign new_n202_ = STAR2 & new_n201_;
  assign new_n203_ = ~new_n114_ & ~new_n202_;
  assign new_n204_ = FBI & ~new_n203_;
  assign new_n205_ = new_n195_ & new_n202_;
  assign new_n206_ = ~new_n204_ & ~new_n205_;
  assign new_n207_ = ~new_n88_ & ~new_n206_;
  assign new_n208_ = FBI & new_n106_;
  assign new_n209_ = ~new_n207_ & ~new_n208_;
  assign FBI_P = new_n200_ | ~new_n209_;
  assign new_n211_ = ~VACC & OWL_N;
  assign new_n212_ = OVACC & new_n211_;
  assign WATCH_P = new_n105_ | new_n212_;
  assign OVACC_P = VACC & ~ICLR;
  assign new_n215_ = new_n117_ & ~new_n134_;
  assign new_n216_ = KBG_N & ~new_n215_;
  assign KBG_F = new_n138_ | new_n216_;
  assign DEL1_P = CAPSD & ~ICLR;
  assign new_n219_ = FBI & DEL1;
  assign new_n220_ = OWL_N & COMPPAR;
  assign new_n221_ = ~new_n219_ & new_n220_;
  assign new_n222_ = DEL1 & ~COMPPAR;
  assign new_n223_ = new_n114_ & new_n222_;
  assign COMPPAR_P = new_n221_ | new_n223_;
  assign new_n225_ = VST1 & new_n188_;
  assign new_n226_ = VST0 & new_n190_;
  assign VST0_P = new_n225_ | new_n226_;
  assign new_n228_ = VST1 & new_n190_;
  assign new_n229_ = PY & new_n188_;
  assign VST1_P = new_n228_ | new_n229_;
  assign new_n231_ = ~ORWD_N & ~ORWD_F;
  assign new_n232_ = OWL_N & ~FBI;
  assign new_n233_ = ~new_n231_ & new_n232_;
  assign new_n234_ = STAR0 & new_n233_;
  assign new_n235_ = ~STAR0 & ~new_n196_;
  assign STAR0_P = new_n234_ | new_n235_;
  assign new_n237_ = ~STAR1 & new_n195_;
  assign new_n238_ = ~new_n87_ & new_n114_;
  assign new_n239_ = ~new_n237_ & ~new_n238_;
  assign new_n240_ = STAR0 & ~new_n239_;
  assign new_n241_ = ~STAR0 & new_n201_;
  assign new_n242_ = ~new_n233_ & ~new_n241_;
  assign new_n243_ = STAR1 & ~new_n242_;
  assign STAR1_P = new_n240_ | new_n243_;
  assign new_n245_ = ~new_n201_ & ~new_n233_;
  assign new_n246_ = STAR2 & ~new_n245_;
  assign new_n247_ = new_n88_ & new_n195_;
  assign new_n248_ = new_n88_ & new_n114_;
  assign new_n249_ = ~new_n247_ & ~new_n248_;
  assign STAR2_P = new_n246_ | ~new_n249_;
  assign new_n251_ = OWL_N & ~STAR2;
  assign new_n252_ = new_n245_ & ~new_n251_;
  assign new_n253_ = STAR3 & ~new_n252_;
  assign new_n254_ = STAR2 & ~STAR3;
  assign new_n255_ = new_n87_ & new_n254_;
  assign new_n256_ = ~new_n196_ & new_n255_;
  assign STAR3_P = new_n253_ | new_n256_;
  assign new_n258_ = ~BULL0 & new_n105_;
  assign new_n259_ = OWL_N & ~WATCH;
  assign new_n260_ = BULL0 & new_n259_;
  assign BULL0_P = new_n258_ | new_n260_;
  assign new_n262_ = WATCH & BULL0;
  assign new_n263_ = BULL1 & new_n262_;
  assign new_n264_ = OWL_N & ~new_n263_;
  assign new_n265_ = BULL1 & new_n264_;
  assign new_n266_ = BULL0 & new_n105_;
  assign new_n267_ = ~BULL1 & new_n266_;
  assign BULL1_P = new_n265_ | new_n267_;
  assign new_n269_ = BULL2 & new_n264_;
  assign new_n270_ = BULL1 & ~BULL2;
  assign new_n271_ = new_n266_ & new_n270_;
  assign BULL2_P = new_n269_ | new_n271_;
  assign new_n273_ = BULL1 & BULL2;
  assign new_n274_ = new_n262_ & new_n273_;
  assign new_n275_ = BULL3 & ~new_n274_;
  assign new_n276_ = ~BULL3 & new_n274_;
  assign new_n277_ = ~new_n275_ & ~new_n276_;
  assign BULL3_P = OWL_N & ~new_n277_;
  assign new_n279_ = OWL_N & BULL3;
  assign new_n280_ = new_n274_ & new_n279_;
  assign new_n281_ = ~BULL4 & new_n280_;
  assign new_n282_ = BULL3 & new_n274_;
  assign new_n283_ = OWL_N & ~new_n282_;
  assign new_n284_ = BULL4 & new_n283_;
  assign BULL4_P = new_n281_ | new_n284_;
  assign new_n286_ = ~new_n283_ & ~BULL4_P;
  assign new_n287_ = BULL5 & ~new_n286_;
  assign new_n288_ = new_n123_ & new_n280_;
  assign BULL5_P = new_n287_ | new_n288_;
  assign new_n290_ = ~BULL6 & new_n280_;
  assign new_n291_ = new_n264_ & new_n282_;
  assign new_n292_ = ~new_n290_ & ~new_n291_;
  assign new_n293_ = BULL4 & BULL5;
  assign new_n294_ = ~new_n292_ & new_n293_;
  assign new_n295_ = BULL2 & BULL3;
  assign new_n296_ = new_n293_ & new_n295_;
  assign new_n297_ = OWL_N & ~new_n296_;
  assign new_n298_ = ~new_n264_ & ~new_n297_;
  assign new_n299_ = BULL6 & ~new_n298_;
  assign BULL6_P = new_n294_ | new_n299_;
  assign SDO = VST0;
endmodule


