
module c1(i0,i1,i2,o19,o21,o22,o23);

  input i0, i1, i2;

  output o19, o21, o22, o23;

  wire n3, n4,  n5 , n6 , n7 , n8 , n9 , n12
    , n13, n14, n15, n16, n17, n18, n20
    , n21, n22, n23;

  assign n3 = i0;
  assign n4 = i0 & i1;
  assign n5 = n2;
  assign n7 = n3 & n4;
  assign n8 = n4 & n5;
  assign n9 = n5 & n6;
  assign n12 = n8;
  assign n13 = n8 & n9;
  assign n14 = n9;
  assign n15 = n3 & n7;
  assign n16 = n7 & n12;
  assign n17 = n12;
  assign n18 = n13 & n14;
  assign n19 = n15;
  assign n20 = n15 & n16;
  assign o21 = n20 & n17;
  assign o22 = n17 & n18;
  assign o23 = n18 & n14;

endmodule
