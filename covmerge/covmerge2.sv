covergroup c1_cg (ref bit x);
    
    option.per_instance = 0;
    option.get_inst_coverage = 0;
    type_option.merge_instances = 0;
    
    x : coverpoint x {
        bins _0 = {1'h0};      
        bins _1 = {1'h1};
    }
    
endgroup

module dut (input bit x, input bit clk);

    c1_cg c1_cg_inst = new(x);

endmodule

module tb ();

    bit clk = 0;

    dut duta(1'h0, clk);
    dut dutb(1'h1, clk);
    
    initial begin
        #10;
        clk = 1;
        #10;
        clk = 0;
        #10;
        clk = 1;
        #10;
        
        $finish();
        
    end       
    
    always @(posedge clk) begin
        duta.c1_cg_inst.sample();
        dutb.c1_cg_inst.sample();
    end
       
endmodule
