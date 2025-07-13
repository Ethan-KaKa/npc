`include "tao_defines.v"

module tao_decode (
    input clk,
    input rst,

    input [31:0] i_inst,
    input [31:0] i_pc,
    //
    output dec_rs1en,
    output dec_rs2en,
    output dec_rdwen,
    output [`TAO_REG_ADDR-1:0] dec_rs1idx,
    output [`TAO_REG_ADDR-1:0] dec_rs2idx,
    output [`TAO_REG_ADDR-1:0] dec_rdidx,
    output [31:0] dec_imm,

    // 
    output [`TAO_SIZE_BUS-1:0] info_bus
);

//basic info
    //opcode
wire [6:0] opcode = i_inst[6:0];
    //
wire [`TAO_REG_ADDR-1:0] rd = i_inst[10:7];
wire [`TAO_REG_ADDR-1:0] rs1 = i_inst[18:15];
wire [`TAO_REG_ADDR-1:0] rs2 = i_inst[23:20];
wire [2:0] funct3 = i_inst[14:12];
wire [6:0] funct7 = i_inst[31:25];
    //imm
wire [31:0] imm_I = {{20{i_inst[31]}}, i_inst[31:20]};
wire [31:0] imm_S = {{20{i_inst[31]}}, i_inst[31:25], i_inst[11:7]};
wire [31:0] imm_B = {{19{i_inst[31]}}, i_inst[31], i_inst[7], i_inst[30:25], i_inst[11:8], 1'b0};
wire [31:0] imm_U = {i_inst[31:12], 12'b0};
wire [31:0] imm_J = {{11{i_inst[31]}}, i_inst[31], i_inst[19:12], i_inst[20], i_inst[30:21], 1'b0};
    //opcode
wire opcode_1_0_00 = (opcode[1:0] == 2'b00); 
wire opcode_1_0_01 = (opcode[1:0] == 2'b01); 
wire opcode_1_0_10 = (opcode[1:0] == 2'b10); 
wire opcode_1_0_11 = (opcode[1:0] == 2'b11);

wire opcode_4_2_000 = (opcode[4:2] == 3'b000);
wire opcode_4_2_001 = (opcode[4:2] == 3'b001);
wire opcode_4_2_010 = (opcode[4:2] == 3'b010);
wire opcode_4_2_011 = (opcode[4:2] == 3'b011);
wire opcode_4_2_100 = (opcode[4:2] == 3'b100);
wire opcode_4_2_101 = (opcode[4:2] == 3'b101);
wire opcode_4_2_110 = (opcode[4:2] == 3'b110);
wire opcode_4_2_111 = (opcode[4:2] == 3'b111);

wire opcode_6_5_00 = (opcode[6:5] == 2'b00);
wire opcode_6_5_01 = (opcode[6:5] == 2'b01);
wire opcode_6_5_10 = (opcode[6:5] == 2'b10);
wire opcode_6_5_11 = (opcode[6:5] == 2'b11);

    //funct3 / 7
wire funct3_000 = (funct3 == 3'b000);
wire funct3_001 = (funct3 == 3'b001);
wire funct3_010 = (funct3 == 3'b010);
wire funct3_011 = (funct3 == 3'b011);
wire funct3_100 = (funct3 == 3'b100);
wire funct3_101 = (funct3 == 3'b101);
wire funct3_110 = (funct3 == 3'b110);
wire funct3_111 = (funct3 == 3'b111);

wire funct7_0000000 = (funct7 == 7'b0000000);
wire funct7_0100000 = (funct7 == 7'b0100000);
wire funct7_0000001 = (funct7 == 7'b0000001);
wire funct7_0000101 = (funct7 == 7'b0000101);
wire funct7_0001001 = (funct7 == 7'b0001001);
wire funct7_0001101 = (funct7 == 7'b0001101);
wire funct7_0010101 = (funct7 == 7'b0010101);
wire funct7_0100001 = (funct7 == 7'b0100001);
wire funct7_0010001 = (funct7 == 7'b0010001);
wire funct7_0101101 = (funct7 == 7'b0101101);
wire funct7_1111111 = (funct7 == 7'b1111111);
wire funct7_0000100 = (funct7 == 7'b0000100);
wire funct7_0001000 = (funct7 == 7'b0001000);
wire funct7_0001100 = (funct7 == 7'b0001100);
wire funct7_0101100 = (funct7 == 7'b0101100);
wire funct7_0010000 = (funct7 == 7'b0010000);
wire funct7_0010100 = (funct7 == 7'b0010100);
wire funct7_1100000 = (funct7 == 7'b1100000);
wire funct7_1110000 = (funct7 == 7'b1110000);
wire funct7_1010000 = (funct7 == 7'b1010000);
wire funct7_1101000 = (funct7 == 7'b1101000);
wire funct7_1111000 = (funct7 == 7'b1111000);
wire funct7_1010001 = (funct7 == 7'b1010001);
wire funct7_1110001 = (funct7 == 7'b1110001);
wire funct7_1100001 = (funct7 == 7'b1100001);
wire funct7_1101001 = (funct7 == 7'b1101001);



//
    //big type
wire lui    = (opcode_6_5_01 & opcode_4_2_101 & opcode_1_0_11);
wire auipc  = (opcode_6_5_00 & opcode_4_2_101 & opcode_1_0_11);

wire branch = (opcode_6_5_11 & opcode_4_2_000 & opcode_1_0_11);

wire jal    = (opcode_6_5_11 & opcode_4_2_011 & opcode_1_0_11);
wire jalr   = (opcode_6_5_11 & opcode_4_2_001 & opcode_1_0_11);


wire op_imm = (opcode_6_5_00 & opcode_4_2_100 & opcode_1_0_11);

wire ebreak = (
    (opcode_6_5_11 & opcode_4_2_100 & opcode_1_0_11) & 
    (imm_I == 32'b1) &
    (funct3_000));


    //small type
        //op_imm
wire op_imm_addi = (op_imm & funct3_000);


//rs rd
assign dec_rdwen = (lui | auipc | jal | jalr | op_imm );
assign dec_rs1en = (jalr | branch | op_imm );
assign dec_rs2en = (branch);


assign dec_rdidx = rd;
assign dec_rs1idx = rs1;
assign dec_rs2idx = rs2;

//imm的选择
assign dec_imm = (
    (jalr | op_imm_addi) ? imm_I : 
    ((branch) ? imm_B : 
    ((lui | auipc) ? imm_U :
    ((jal) ? imm_J : 32'b0)))
);




//info
    //操作类型
assign info_bus[`TAO_BUS_EBREAK] = ebreak;
assign info_bus[`TAO_BUS_ADDI] = (lui | auipc | jal | jalr | op_imm_addi);

    //操作数选择
assign info_bus[`TAO_BUS_OP1_PC] = (auipc | jal | jalr);
assign info_bus[`TAO_BUS_OP1_RS1] = (op_imm_addi | branch);
assign info_bus[`TAO_BUS_OP1_0] = (lui);

assign info_bus[`TAO_BUS_OP2_4] = (jal | jalr);
assign info_bus[`TAO_BUS_OP2_IMM] = (lui | auipc | op_imm_addi);
assign info_bus[`TAO_BUS_OP2_RS2] = (branch);

    //PC
assign info_bus[`TAO_BUS_DNPC_EN] = (jal | jalr | branch);
assign info_bus[`TAO_BUS_DNPC_JALR] = (jalr);


endmodule