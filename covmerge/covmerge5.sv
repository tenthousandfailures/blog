covergroup c1_cg (ref bit x);
    
    option.per_instance = 0;
    option.get_inst_coverage = 0;
    type_option.merge_instances = 1;

    x : coverpoint x {
        bins _0 = {1'h0};      
        bins _1 = {1'h1};
    }
    
endgroup

module dut (input bit x, input bit clk);

    c1_cg c1_cg_inst = new(x);

endmodule

module tb ();

    bit clk;
    bit x = 0;

    dut duta(x, clk);
    dut dutb(1'h1, clk);
    
    initial begin
        #10;
        clk = 1;
        #10;
        clk = 0;
        #10;

        $finish();
        
    end       
    
    always @(clk) begin
        x = 0;
        duta.c1_cg_inst.sample();
        dutb.c1_cg_inst.sample();
    end
       
endmodule
