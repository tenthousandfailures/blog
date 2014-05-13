module dut (input bit x, input bit clk);

    covergroup c1_cg (ref bit x);
        
        option.per_instance = 1;
        type_option.merge_instances = 1;
        
        x : coverpoint x {
            bins _0 = {1'h0};      
            bins _1 = {1'h1};
        }
        
    endgroup
        
    c1_cg c1_cg_inst = new(x);
    
    always @(posedge clk) begin
        c1_cg_inst.sample();
    end
    
endmodule

module tb ();

    bit clk = 0;

    dut duta(1'h0, clk);
    dut dutb(1'h1, clk);
    
    initial begin
        #10; clk = 1;
        #10; clk = 0;
        #10; clk = 1;
        #10;
        $finish();
    end       
       
endmodule
