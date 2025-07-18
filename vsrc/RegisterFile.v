module RegisterFile #(ADDR_WIDTH = 1, DATA_WIDTH = 1) (
  input clk,
  input rst,
  input [DATA_WIDTH-1:0] wdata,
  input [ADDR_WIDTH-1:0] waddr,
  input [ADDR_WIDTH-1:0] raddr1,
  input [ADDR_WIDTH-1:0] raddr2,
  input wen,
  output [DATA_WIDTH-1:0] rdata1,
  output [DATA_WIDTH-1:0] rdata2
);
  logic [DATA_WIDTH-1:0] rf [2**ADDR_WIDTH-1:0] /* verilator public_flat */;

  always @(posedge clk) begin
    if (wen && waddr != 4'b0) rf[waddr] <= wdata;
  end

  assign rdata1 = rf[raddr1];
  assign rdata2 = rf[raddr2];
  
endmodule