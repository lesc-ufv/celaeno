// Benchmark "C17.iscas" written by ABC on Wed Mar 10 23:12:51 2021

module C17.iscas   
    1GAT0 , 2GAT1 , 3GAT2 , 6GAT3 , 7GAT4 ,
    22GAT10 , 23GAT9   ;
  input  1GAT0 , 2GAT1 , 3GAT2 , 6GAT3 , 7GAT4 ;
  output 22GAT10 , 23GAT9 ;
  wire new_n8_, new_n9_, new_n10_, new_n12_;
  assign new_n8_ = 1GAT0  & 3GAT2 ;
  assign new_n9_ = 3GAT2  & 6GAT3 ;
  assign new_n10_ = 2GAT1  & ~new_n9_;
  assign 22GAT10  = new_n8_ | new_n10_;
  assign new_n12_ = 7GAT4  & ~new_n9_;
  assign 23GAT9  = new_n10_ | new_n12_;
endmodule


