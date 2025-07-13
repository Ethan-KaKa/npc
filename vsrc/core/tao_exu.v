`include "tao_defines.v"

module tao_exu (
    input clk,
    input rst,

    input [31:0] i_pc,
    input exu_rs1en,
    input exu_rs2en,
    input exu_rdwen,
    input [`TAO_REG_ADDR-1:0] exu_rs1idx,
    input [`TAO_REG_ADDR-1:0] exu_rs2idx,
    input [`TAO_REG_ADDR-1:0] exu_rdidx,
    inout [31:0] exu_imm,

    input [31:0] info_bus,

    output [`TAO_SIZE_BUS-1:0] npc,
    output [`TAO_SIZE_BUS-1:0] snpc,
    output [`TAO_SIZE_BUS-1:0] dnpc
);

//更新PC
assign snpc = i_pc + 4;
assign dnpc = info_bus[`TAO_BUS_DNPC_JALR] ? ((src1 + exu_imm) & ~1) : (i_pc + exu_imm);
wire [31:0] next_pc = info_bus[`TAO_BUS_DNPC_EN] ? dnpc : snpc;
gnrl_dffr #(.WIDTH(32), .RESET_VAL(32'h80000000)) pc_active_dffr (
    .clk(clk),
    .rst(rst),
    .din(next_pc),
    .dout(npc),
    .wen(1)
);


//read and write back
wire [31:0] src1;
wire [31:0] src2;
wire [31:0] alu_result;
RegisterFile #(.ADDR_WIDTH(`TAO_REG_ADDR), .DATA_WIDTH(`TAO_REG_DATA)) Reg_16 (
    .clk(clk), 
    .rst(rst),
    .wdata(alu_result),
    .waddr(exu_rdidx),
    .raddr1(exu_rs1idx),
    .raddr2(exu_rs2idx),
    .wen(exu_rdwen),
    .rdata1(src1),
    .rdata2(src2)
);

//操作数选择
wire [31:0] alu_op1 = (
    info_bus[`TAO_BUS_OP1_PC] ? i_pc :
    info_bus[`TAO_BUS_OP1_RS1] ? src1 :
    info_bus[`TAO_BUS_OP1_0] ? 32'b0 : 32'b0
);
wire [31:0] alu_op2 = (
    info_bus[`TAO_BUS_OP2_4] ? 4 :
    info_bus[`TAO_BUS_OP2_IMM] ? exu_imm :
    info_bus[`TAO_BUS_OP2_RS2] ? src2 : 32'b0
);


//执行
    //ADDI
wire [31:0] alu_add = alu_op1 + alu_op2;


    //ebreak
ebreak top_ebreak (
    .clk(clk),
    .rst(rst),
    .ebreak_en(info_bus[`TAO_BUS_EBREAK])
);

    //选择result
assign alu_result = info_bus[`TAO_BUS_ADDI] ? alu_add : 32'b0; 




endmodule