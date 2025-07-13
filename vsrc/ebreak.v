module ebreak(
    input clk,
    input rst,
    input ebreak_en
);

    import "DPI-C" function void ebreak_funct(input int ebreak);

    always @(posedge clk) begin
        if (!rst)
            ebreak_funct({{31{1'b0}}, ebreak_en});
    end
    
endmodule