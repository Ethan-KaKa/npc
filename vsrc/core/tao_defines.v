//
    //normal
    //info_bus_size
`define TAO_SIZE_BUS 32
    //reg
`define TAO_REG_ADDR 4
`define TAO_REG_DATA 32




//info_bus


    //进行的操作类型
`define TAO_BUS_EBREAK 0
`define TAO_BUS_ADDI 1


    //操作数的选择
`define TAO_BUS_OP1_PC 3
`define TAO_BUS_OP1_RS1 4
`define TAO_BUS_OP1_0 5

`define TAO_BUS_OP2_4 6
`define TAO_BUS_OP2_IMM 7
`define TAO_BUS_OP2_RS2 8

    //PC跳转
`define TAO_BUS_DNPC_EN 9
`define TAO_BUS_DNPC_JALR 10