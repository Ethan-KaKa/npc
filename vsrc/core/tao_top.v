`include "tao_defines.v"

module tao_top (
    input clk,
    input rst,
    input [31:0] inst,
    output [`TAO_SIZE_BUS-1:0] npc,
    output [`TAO_SIZE_BUS-1:0] snpc,
    output [`TAO_SIZE_BUS-1:0] dnpc
);

wire [31:0] i_pc;


wire dec_rs1en;
wire dec_rs2en;
wire dec_rdwen;
wire [`TAO_REG_ADDR-1:0] dec_rs1idx;
wire [`TAO_REG_ADDR-1:0] dec_rs2idx;
wire [`TAO_REG_ADDR-1:0] dec_rdidx;
wire [31:0] dec_imm;
wire [`TAO_SIZE_BUS-1:0] info_bus;

tao_decode core_tao_decode (
    .clk(clk),
    .rst(rst),
    .i_inst(inst),
    .i_pc(i_pc),
    .dec_rs1en(dec_rs1en),
    .dec_rs2en(dec_rs2en),
    .dec_rdwen(dec_rdwen),
    .dec_rs1idx(dec_rs1idx),
    .dec_rs2idx(dec_rs2idx),
    .dec_rdidx(dec_rdidx),
    .dec_imm(dec_imm),
    .info_bus(info_bus)
);



tao_exu core_tao_exu (
    .clk(clk),
    .rst(rst),
    .i_pc(i_pc),
    .exu_rs1en(dec_rs1en),
    .exu_rs2en(dec_rs2en),
    .exu_rdwen(dec_rdwen),
    .exu_rs1idx(dec_rs1idx),
    .exu_rs2idx(dec_rs2idx),
    .exu_rdidx(dec_rdidx),
    .exu_imm(dec_imm),
    .info_bus(info_bus),
    .npc(i_pc),
    .dnpc(dnpc),
    .snpc(snpc)
);

assign npc = i_pc;


endmodule
