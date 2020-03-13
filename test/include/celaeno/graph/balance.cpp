//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : balance
// @created     : Friday Mar 13, 2020 18:31:22 -03
//

#pragma once

#include <taygete/graph.hpp>
#include <taygete/graph-reader.hpp>
#include <asterope/graph/dot-parser.hpp>
#include <asterope/graph/to-png.hpp>
#include <celaeno/graph/balance.hpp>
#include <celaeno/graph/views/depth.hpp>

namespace celaeno::graph::balance::test
{

template<typename T1, typename T2>
void run(T1&& circuit, T2&& alias)
{
  // Define output names
  std::string original{std::string{alias}};
  std::string balanced{std::string{alias} + "-balanced"};

  // Use taygete data structure
  taygete::graph::Graph<int64_t> g;

  // Read circuit into a stream
  std::stringstream ss; ss << circuit;

  // Callback for populating the graph
  auto callback = [&g]<typename T>(T&& a, T&& b) -> void
  {
    g.emplace(std::make_pair(std::forward<T>(a),std::forward<T>(b)));
  };

  // Read the graph into g
  taygete::graph::reader::Reader reader(ss, callback);

  // Write graph to .png file
  asterope::graph::to_png::to_png(
    asterope::graph::dot_parser::dot_parser(g.data()),
    original
  );

  // Create a depth view
  auto g_views
  {
    celaeno::graph::views::depth::depth(
      0,
      [&g](auto&& e){ return g.get_successors(std::forward<decltype(e)>(e)); },
      [&g](auto&& e){ return g.get_predecessors(std::forward<decltype(e)>(e)); }
    )
  };
  auto& d_view{g_views.first};

  auto pred = [&g]<typename T>(T&& e)
    { return g.get_predecessors(std::forward<T>(e)); };

  int64_t depth{};
  if( ! d_view.empty() )
  {
    depth = ( d_view.rbegin()->first ) + 1;
  }
  else
  {
    std::cerr << " * Empty map" << std::endl;
    exit(1);
  }

  // Balace the graph
  celaeno::graph::balance::balance(d_view, g, depth, pred);

  // Write the balance graph to .png file
  asterope::graph::to_png::to_png(
    asterope::graph::dot_parser::dot_parser(g.data()),
    balanced
  );
}

TEST_CASE("balance.cpp")
{

SECTION("C17")
{
  static constexpr std::string_view c17
  {
    "module c17 (N1, N2, N3, N6, N7, N22, N23);"
    "input  N1, N2, N3, N6, N7;"
    "output N22, N23;"
    "wire new_N10_, new_N11_, new_N16_, new_N19_;"
    "assign new_N10_ = ~N1 | ~N3;"
    "assign new_N11_ = ~N3 | ~N6;"
    "assign new_N16_ = ~N2 | ~new_N11_;"
    "assign new_N19_ = ~new_N11_ | ~N7;"
    "assign N22 = ~new_N10_ | ~new_N16_;"
    "assign N23 = ~new_N16_ | ~new_N19_;"
    "endmodule"
  };
  run(c17,"c17");
} // SECTION

SECTION("t")
{
  static constexpr std::string_view t
  {
    "module t  ("
    "    pi0, pi1, pi2, pi3, pi4,"
    "    po0, po1  );"
    "  input  pi0, pi1, pi2, pi3, pi4;"
    "  output po0, po1;"
    "  wire n9, n10, n11, n13;"
    "  assign n9 = pi2 & pi3;"
    "  assign n10 = ~n9 & pi1;"
    "  assign n11 = pi0 & pi2;"
    "  assign po0 = n10 | n11;"
    "  assign n13 = ~pi1 & ~pi4;"
    "  assign po1 = ~n9 & ~n13;"
    "endmodule"
  };
  run(t,"t");
} // SECTION

SECTION("1-bit adder AIOG")
{
  static constexpr std::string_view adder_1_bit_aoig
  {
    "module top( x, y, z, H, I);"
    "input x, y, z;"
    "output H, I;"
    "wire A, B, D, E, F;"
    "assign A =  y | z ;"
    "assign B =  y & z ;"
    "assign D =  A & ~B ;"
    "assign E =  x | D ;"
    "assign F =  x & D ;"
    "assign H =  E & ~F ;"
    "assign I =  B | F ;"
    "endmodule"
  };
  run(adder_1_bit_aoig,"1-bit-adder-aoig");
} // SECTION

SECTION("b1-r2")
{
  static constexpr std::string_view b1_r2
  {
    "module b1 (pi0, pi1, pi2, po0, po1, po2, po3  );"
    "  input  pi0, pi1, pi2;"
    "  output po0, po1, po2, po3;"
    "  wire n8, n9, n11, n12;"
    "  assign n8 = pi0 & pi1;"
    "  assign n9 = ~pi0 & ~pi1;"
    "  assign po1 = ~n8 & ~n9;"
    "  assign n11 = ~pi2 & n8;"
    "  assign n12 = pi2 & n9;"
    "  assign po2 = n11 | n12;"
    "  assign po3 = ~pi2;"
    "  assign po0 = pi2;"
    "endmodule"
  };
  run(b1_r2,"b1-r2");
} // SECTION

SECTION("clpl")
{
  static constexpr std::string_view clpl
  {
    "module clpl  ("
    "    pi00, pi01, pi02, pi03, pi04, pi05, pi06, pi07, pi08, pi09, pi10,"
    "    po0, po1, po2, po3, po4  );"
    "  input  pi00, pi01, pi02, pi03, pi04, pi05, pi06, pi07, pi08, pi09,"
    "    pi10;"
    "  output po0, po1, po2, po3, po4;"
    "  wire n18, n20, n22, n24, n26;"
    "  assign n18 = pi00 & pi04;"
    "  assign po2 = n18 | pi02;"
    "  assign n20 = po2 & pi05;"
    "  assign po1 = n20 | pi03;"
    "  assign n22 = po1 & pi06;"
    "  assign po0 = n22 | pi01;"
    "  assign n24 = po0 & pi08;"
    "  assign po4 = n24 | pi07;"
    "  assign n26 = po4 & pi09;"
    "  assign po3 = n26 | pi10;"
    "endmodule"
  };
  run(clpl,"clpl");
} // SECTION

SECTION("newtag")
{
  static constexpr std::string_view newtag
  {
    "module newtag  (" 
    "    pi0, pi1, pi2, pi3, pi4, pi5, pi6, pi7,"
    "    po0  );"
    "  input  pi0, pi1, pi2, pi3, pi4, pi5, pi6, pi7;"
    "  output po0;"
    "  wire new_n10_, new_n11_, new_n12_, new_n13_, new_n14_, new_n15_, new_n16_,"
    "    new_n17_;"
    "  assign new_n10_ = pi6 & pi7;"
    "  assign new_n11_ = ~pi4 & ~pi5;"
    "  assign new_n12_ = new_n10_ & ~new_n11_;"
    "  assign new_n13_ = pi4 & pi5;"
    "  assign new_n14_ = ~new_n12_ & ~new_n13_;"
    "  assign new_n15_ = pi3 & new_n14_;"
    "  assign new_n16_ = pi1 & ~pi2;"
    "  assign new_n17_ = ~pi0 & new_n16_;"
    "  assign po0 = new_n15_ | ~new_n17_;"
    "endmodule"
  };
  run(newtag,"newtag");
} // SECTION

SECTION("xor_5")
{
  static constexpr std::string_view xor5_r
  {
    "module xor5 (" 
    "    x0, x1, x2, x3, x4,"
    "    z0  );"
    "  input  x0, x1, x2, x3, x4;"
    "  output z0;"
    "  wire new_n7_, new_n8_, new_n9_, new_n10_, new_n11_, new_n12_, new_n13_,"
    "    new_n14_, new_n15_, new_n16_, new_n17_, new_n18_, new_n19_, new_n20_;"
    "  assign new_n7_ = x1 & x2;"
    "  assign new_n8_ = ~x1 & ~x2;"
    "  assign new_n9_ = ~new_n7_ & ~new_n8_;"
    "  assign new_n10_ = x0 & ~new_n9_;"
    "  assign new_n11_ = ~x1 & x2;"
    "  assign new_n12_ = x1 & ~x2;"
    "  assign new_n13_ = ~new_n11_ & ~new_n12_;"
    "  assign new_n14_ = ~x0 & ~new_n13_;"
    "  assign new_n15_ = ~new_n10_ & ~new_n14_;"
    "  assign new_n16_ = ~x3 & x4;"
    "  assign new_n17_ = x3 & ~x4;"
    "  assign new_n18_ = ~new_n16_ & ~new_n17_;"
    "  assign new_n19_ = ~new_n15_ & new_n18_;"
    "  assign new_n20_ = new_n15_ & ~new_n18_;"
    "  assign z0 = new_n19_ | new_n20_;"
    "endmodule"
  };
  run(xor5_r,"xor5_r");
} // SECTION

SECTION("c432")
{
  static constexpr std::string_view c432
  {
    "module c432 ("
    "    N1, N4, N8, N11, N14, N17, N21, N24, N27, N30, N34, N37, N40, N43, N47,"
    "    N50, N53, N56, N60, N63, N66, N69, N73, N76, N79, N82, N86, N89, N92,"
    "    N95, N99, N102, N105, N108, N112, N115,"
    "    N223, N329, N370, N421, N430, N431, N432  );"
    "  input  N1, N4, N8, N11, N14, N17, N21, N24, N27, N30, N34, N37, N40,"
    "    N43, N47, N50, N53, N56, N60, N63, N66, N69, N73, N76, N79, N82, N86,"
    "    N89, N92, N95, N99, N102, N105, N108, N112, N115;"
    "  output N223, N329, N370, N421, N430, N431, N432;"
    "  wire new_n44_, new_n45_, new_n46_, new_n47_, new_n48_, new_n49_, new_n50_,"
    "    new_n51_, new_n52_, new_n53_, new_n54_, new_n55_, new_n56_, new_n57_,"
    "    new_n58_, new_n59_, new_n61_, new_n62_, new_n63_, new_n64_, new_n65_,"
    "    new_n66_, new_n67_, new_n68_, new_n69_, new_n70_, new_n71_, new_n72_,"
    "    new_n73_, new_n74_, new_n75_, new_n76_, new_n77_, new_n78_, new_n79_,"
    "    new_n80_, new_n81_, new_n82_, new_n83_, new_n84_, new_n85_, new_n86_,"
    "    new_n87_, new_n88_, new_n89_, new_n90_, new_n91_, new_n92_, new_n93_,"
    "    new_n94_, new_n95_, new_n96_, new_n97_, new_n98_, new_n99_, new_n100_,"
    "    new_n101_, new_n102_, new_n103_, new_n104_, new_n105_, new_n106_,"
    "    new_n107_, new_n108_, new_n109_, new_n110_, new_n111_, new_n112_,"
    "    new_n114_, new_n115_, new_n116_, new_n117_, new_n118_, new_n119_,"
    "    new_n120_, new_n121_, new_n122_, new_n123_, new_n124_, new_n125_,"
    "    new_n126_, new_n127_, new_n128_, new_n129_, new_n130_, new_n131_,"
    "    new_n132_, new_n133_, new_n134_, new_n135_, new_n136_, new_n137_,"
    "    new_n138_, new_n139_, new_n140_, new_n141_, new_n142_, new_n143_,"
    "    new_n144_, new_n145_, new_n146_, new_n147_, new_n148_, new_n149_,"
    "    new_n150_, new_n151_, new_n152_, new_n153_, new_n154_, new_n155_,"
    "    new_n156_, new_n157_, new_n158_, new_n159_, new_n160_, new_n161_,"
    "    new_n162_, new_n163_, new_n164_, new_n165_, new_n166_, new_n167_,"
    "    new_n168_, new_n169_, new_n170_, new_n171_, new_n172_, new_n173_,"
    "    new_n174_, new_n176_, new_n177_, new_n178_, new_n179_, new_n180_,"
    "    new_n181_, new_n182_, new_n183_, new_n184_, new_n185_, new_n186_,"
    "    new_n187_, new_n188_, new_n189_, new_n190_, new_n191_, new_n192_,"
    "    new_n193_, new_n194_, new_n195_, new_n196_, new_n197_, new_n198_,"
    "    new_n199_, new_n200_, new_n201_, new_n202_, new_n203_, new_n204_,"
    "    new_n205_, new_n206_, new_n207_, new_n208_, new_n209_, new_n210_,"
    "    new_n211_, new_n212_, new_n213_, new_n214_, new_n215_, new_n216_,"
    "    new_n217_, new_n218_, new_n219_, new_n220_, new_n221_, new_n222_,"
    "    new_n223_, new_n224_, new_n225_, new_n226_, new_n227_, new_n228_,"
    "    new_n229_, new_n230_, new_n231_, new_n232_, new_n233_, new_n234_,"
    "    new_n235_, new_n236_, new_n238_, new_n239_, new_n241_, new_n242_,"
    "    new_n243_, new_n244_, new_n245_, new_n246_, new_n248_, new_n249_,"
    "    new_n250_, new_n251_;"
    "  assign new_n44_ = ~N1 & N4;"
    "  assign new_n45_ = ~N11 & N17;"
    "  assign new_n46_ = ~N24 & N30;"
    "  assign new_n47_ = ~N37 & N43;"
    "  assign new_n48_ = ~N50 & N56;"
    "  assign new_n49_ = ~N63 & N69;"
    "  assign new_n50_ = ~N76 & N82;"
    "  assign new_n51_ = ~N89 & N95;"
    "  assign new_n52_ = ~N102 & N108;"
    "  assign new_n53_ = ~new_n44_ & ~new_n45_;"
    "  assign new_n54_ = ~new_n46_ & new_n53_;"
    "  assign new_n55_ = ~new_n47_ & new_n54_;"
    "  assign new_n56_ = ~new_n48_ & new_n55_;"
    "  assign new_n57_ = ~new_n49_ & new_n56_;"
    "  assign new_n58_ = ~new_n50_ & new_n57_;"
    "  assign new_n59_ = ~new_n51_ & new_n58_;"
    "  assign N223 = new_n52_ | ~new_n59_;"
    "  assign new_n61_ = ~new_n44_ & ~N223;"
    "  assign new_n62_ = new_n44_ & N223;"
    "  assign new_n63_ = ~new_n61_ & ~new_n62_;"
    "  assign new_n64_ = N4 & ~N8;"
    "  assign new_n65_ = ~new_n63_ & new_n64_;"
    "  assign new_n66_ = ~new_n45_ & ~N223;"
    "  assign new_n67_ = new_n45_ & N223;"
    "  assign new_n68_ = ~new_n66_ & ~new_n67_;"
    "  assign new_n69_ = N17 & ~N21;"
    "  assign new_n70_ = ~new_n68_ & new_n69_;"
    "  assign new_n71_ = ~new_n46_ & ~N223;"
    "  assign new_n72_ = new_n46_ & N223;"
    "  assign new_n73_ = ~new_n71_ & ~new_n72_;"
    "  assign new_n74_ = N30 & ~N34;"
    "  assign new_n75_ = ~new_n73_ & new_n74_;"
    "  assign new_n76_ = ~new_n47_ & ~N223;"
    "  assign new_n77_ = new_n47_ & N223;"
    "  assign new_n78_ = ~new_n76_ & ~new_n77_;"
    "  assign new_n79_ = N43 & ~N47;"
    "  assign new_n80_ = ~new_n78_ & new_n79_;"
    "  assign new_n81_ = ~new_n48_ & ~N223;"
    "  assign new_n82_ = new_n48_ & N223;"
    "  assign new_n83_ = ~new_n81_ & ~new_n82_;"
    "  assign new_n84_ = N56 & ~N60;"
    "  assign new_n85_ = ~new_n83_ & new_n84_;"
    "  assign new_n86_ = ~new_n49_ & ~N223;"
    "  assign new_n87_ = new_n49_ & N223;"
    "  assign new_n88_ = ~new_n86_ & ~new_n87_;"
    "  assign new_n89_ = N69 & ~N73;"
    "  assign new_n90_ = ~new_n88_ & new_n89_;"
    "  assign new_n91_ = ~new_n50_ & ~N223;"
    "  assign new_n92_ = new_n50_ & N223;"
    "  assign new_n93_ = ~new_n91_ & ~new_n92_;"
    "  assign new_n94_ = N82 & ~N86;"
    "  assign new_n95_ = ~new_n93_ & new_n94_;"
    "  assign new_n96_ = ~new_n51_ & ~N223;"
    "  assign new_n97_ = new_n51_ & N223;"
    "  assign new_n98_ = ~new_n96_ & ~new_n97_;"
    "  assign new_n99_ = N95 & ~N99;"
    "  assign new_n100_ = ~new_n98_ & new_n99_;"
    "  assign new_n101_ = ~new_n52_ & ~N223;"
    "  assign new_n102_ = new_n52_ & N223;"
    "  assign new_n103_ = ~new_n101_ & ~new_n102_;"
    "  assign new_n104_ = N108 & ~N112;"
    "  assign new_n105_ = ~new_n103_ & new_n104_;"
    "  assign new_n106_ = ~new_n65_ & ~new_n70_;"
    "  assign new_n107_ = ~new_n75_ & new_n106_;"
    "  assign new_n108_ = ~new_n80_ & new_n107_;"
    "  assign new_n109_ = ~new_n85_ & new_n108_;"
    "  assign new_n110_ = ~new_n90_ & new_n109_;"
    "  assign new_n111_ = ~new_n95_ & new_n110_;"
    "  assign new_n112_ = ~new_n100_ & new_n111_;"
    "  assign N329 = new_n105_ | ~new_n112_;"
    "  assign new_n114_ = ~new_n65_ & ~N329;"
    "  assign new_n115_ = new_n65_ & N329;"
    "  assign new_n116_ = ~new_n114_ & ~new_n115_;"
    "  assign new_n117_ = N4 & ~N14;"
    "  assign new_n118_ = ~new_n63_ & new_n117_;"
    "  assign new_n119_ = ~new_n116_ & new_n118_;"
    "  assign new_n120_ = ~new_n70_ & ~N329;"
    "  assign new_n121_ = new_n70_ & N329;"
    "  assign new_n122_ = ~new_n120_ & ~new_n121_;"
    "  assign new_n123_ = N17 & ~N27;"
    "  assign new_n124_ = ~new_n68_ & new_n123_;"
    "  assign new_n125_ = ~new_n122_ & new_n124_;"
    "  assign new_n126_ = ~new_n75_ & ~N329;"
    "  assign new_n127_ = new_n75_ & N329;"
    "  assign new_n128_ = ~new_n126_ & ~new_n127_;"
    "  assign new_n129_ = N30 & ~N40;"
    "  assign new_n130_ = ~new_n73_ & new_n129_;"
    "  assign new_n131_ = ~new_n128_ & new_n130_;"
    "  assign new_n132_ = ~new_n80_ & ~N329;"
    "  assign new_n133_ = new_n80_ & N329;"
    "  assign new_n134_ = ~new_n132_ & ~new_n133_;"
    "  assign new_n135_ = N43 & ~N53;"
    "  assign new_n136_ = ~new_n78_ & new_n135_;"
    "  assign new_n137_ = ~new_n134_ & new_n136_;"
    "  assign new_n138_ = ~new_n85_ & ~N329;"
    "  assign new_n139_ = new_n85_ & N329;"
    "  assign new_n140_ = ~new_n138_ & ~new_n139_;"
    "  assign new_n141_ = N56 & ~N66;"
    "  assign new_n142_ = ~new_n83_ & new_n141_;"
    "  assign new_n143_ = ~new_n140_ & new_n142_;"
    "  assign new_n144_ = ~new_n90_ & ~N329;"
    "  assign new_n145_ = new_n90_ & N329;"
    "  assign new_n146_ = ~new_n144_ & ~new_n145_;"
    "  assign new_n147_ = N69 & ~N79;"
    "  assign new_n148_ = ~new_n88_ & new_n147_;"
    "  assign new_n149_ = ~new_n146_ & new_n148_;"
    "  assign new_n150_ = ~new_n95_ & ~N329;"
    "  assign new_n151_ = new_n95_ & N329;"
    "  assign new_n152_ = ~new_n150_ & ~new_n151_;"
    "  assign new_n153_ = N82 & ~N92;"
    "  assign new_n154_ = ~new_n93_ & new_n153_;"
    "  assign new_n155_ = ~new_n152_ & new_n154_;"
    "  assign new_n156_ = ~new_n100_ & ~N329;"
    "  assign new_n157_ = new_n100_ & N329;"
    "  assign new_n158_ = ~new_n156_ & ~new_n157_;"
    "  assign new_n159_ = N95 & ~N105;"
    "  assign new_n160_ = ~new_n98_ & new_n159_;"
    "  assign new_n161_ = ~new_n158_ & new_n160_;"
    "  assign new_n162_ = ~new_n105_ & ~N329;"
    "  assign new_n163_ = new_n105_ & N329;"
    "  assign new_n164_ = ~new_n162_ & ~new_n163_;"
    "  assign new_n165_ = N108 & ~N115;"
    "  assign new_n166_ = ~new_n103_ & new_n165_;"
    "  assign new_n167_ = ~new_n164_ & new_n166_;"
    "  assign new_n168_ = ~new_n119_ & ~new_n125_;"
    "  assign new_n169_ = ~new_n131_ & new_n168_;"
    "  assign new_n170_ = ~new_n137_ & new_n169_;"
    "  assign new_n171_ = ~new_n143_ & new_n170_;"
    "  assign new_n172_ = ~new_n149_ & new_n171_;"
    "  assign new_n173_ = ~new_n155_ & new_n172_;"
    "  assign new_n174_ = ~new_n161_ & new_n173_;"
    "  assign N370 = new_n167_ | ~new_n174_;"
    "  assign new_n176_ = N1 & N223;"
    "  assign new_n177_ = N8 & N329;"
    "  assign new_n178_ = N14 & N370;"
    "  assign new_n179_ = N4 & ~new_n176_;"
    "  assign new_n180_ = ~new_n177_ & new_n179_;"
    "  assign new_n181_ = ~new_n178_ & new_n180_;"
    "  assign new_n182_ = N11 & N223;"
    "  assign new_n183_ = N21 & N329;"
    "  assign new_n184_ = N27 & N370;"
    "  assign new_n185_ = ~new_n182_ & ~new_n183_;"
    "  assign new_n186_ = ~new_n184_ & new_n185_;"
    "  assign new_n187_ = N17 & new_n186_;"
    "  assign new_n188_ = N24 & N223;"
    "  assign new_n189_ = N34 & N329;"
    "  assign new_n190_ = N40 & N370;"
    "  assign new_n191_ = ~new_n188_ & ~new_n189_;"
    "  assign new_n192_ = ~new_n190_ & new_n191_;"
    "  assign new_n193_ = N30 & new_n192_;"
    "  assign new_n194_ = N37 & N223;"
    "  assign new_n195_ = N47 & N329;"
    "  assign new_n196_ = N53 & N370;"
    "  assign new_n197_ = ~new_n194_ & ~new_n195_;"
    "  assign new_n198_ = ~new_n196_ & new_n197_;"
    "  assign new_n199_ = N43 & new_n198_;"
    "  assign new_n200_ = N50 & N223;"
    "  assign new_n201_ = N60 & N329;"
    "  assign new_n202_ = N66 & N370;"
    "  assign new_n203_ = ~new_n200_ & ~new_n201_;"
    "  assign new_n204_ = ~new_n202_ & new_n203_;"
    "  assign new_n205_ = N56 & new_n204_;"
    "  assign new_n206_ = N63 & N223;"
    "  assign new_n207_ = N73 & N329;"
    "  assign new_n208_ = N79 & N370;"
    "  assign new_n209_ = ~new_n206_ & ~new_n207_;"
    "  assign new_n210_ = ~new_n208_ & new_n209_;"
    "  assign new_n211_ = N69 & new_n210_;"
    "  assign new_n212_ = N76 & N223;"
    "  assign new_n213_ = N86 & N329;"
    "  assign new_n214_ = N92 & N370;"
    "  assign new_n215_ = ~new_n212_ & ~new_n213_;"
    "  assign new_n216_ = ~new_n214_ & new_n215_;"
    "  assign new_n217_ = N82 & new_n216_;"
    "  assign new_n218_ = N89 & N223;"
    "  assign new_n219_ = N99 & N329;"
    "  assign new_n220_ = N105 & N370;"
    "  assign new_n221_ = ~new_n218_ & ~new_n219_;"
    "  assign new_n222_ = ~new_n220_ & new_n221_;"
    "  assign new_n223_ = N95 & new_n222_;"
    "  assign new_n224_ = N102 & N223;"
    "  assign new_n225_ = N112 & N329;"
    "  assign new_n226_ = N115 & N370;"
    "  assign new_n227_ = ~new_n224_ & ~new_n225_;"
    "  assign new_n228_ = ~new_n226_ & new_n227_;"
    "  assign new_n229_ = N108 & new_n228_;"
    "  assign new_n230_ = ~new_n187_ & ~new_n193_;"
    "  assign new_n231_ = ~new_n199_ & new_n230_;"
    "  assign new_n232_ = ~new_n205_ & new_n231_;"
    "  assign new_n233_ = ~new_n211_ & new_n232_;"
    "  assign new_n234_ = ~new_n217_ & new_n233_;"
    "  assign new_n235_ = ~new_n223_ & new_n234_;"
    "  assign new_n236_ = ~new_n229_ & new_n235_;"
    "  assign N421 = ~new_n181_ & ~new_n236_;"
    "  assign new_n238_ = ~new_n193_ & new_n199_;"
    "  assign new_n239_ = new_n230_ & ~new_n238_;"
    "  assign N430 = new_n205_ | ~new_n239_;"
    "  assign new_n241_ = ~new_n193_ & ~new_n199_;"
    "  assign new_n242_ = new_n211_ & new_n241_;"
    "  assign new_n243_ = ~new_n205_ & new_n242_;"
    "  assign new_n244_ = ~new_n199_ & ~new_n205_;"
    "  assign new_n245_ = new_n217_ & new_n244_;"
    "  assign new_n246_ = new_n230_ & ~new_n243_;"
    "  assign N431 = new_n245_ | ~new_n246_;"
    "  assign new_n248_ = ~new_n217_ & new_n241_;"
    "  assign new_n249_ = new_n223_ & new_n248_;"
    "  assign new_n250_ = ~new_n187_ & ~new_n238_;"
    "  assign new_n251_ = ~new_n243_ & new_n250_;"
    "  assign N432 = new_n249_ | ~new_n251_;"
    "endmodule"
  };
  run(c432,"c432");
} // SECTION

} // TEST_CASE

} // namespace celaeno::graph::balance::test
