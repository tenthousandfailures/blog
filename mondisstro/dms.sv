module dut;

    reg a,b,c;

    initial begin : abc
        $monitor("%t %m MON0            a: %b b: %b c: %b", $time, a, b, c);
        #100;
        {a,b} = 2'b01;
        c = 1;
        b <= 1'b0;
        #100;
        {a,b} = 2'b10;
        c = 1'bz;
        #100;
        {a,b} = 2'b11;
        c = 0;
        #1000 $finish;
    end // block: abc

    initial begin : ext
        #1
        $monitor("%t %m MON1            a: %b b: %b c: %b", $time, a, b, c);
        $monitor("%t %m MON2            c: %b b: %b c: %b", $time, a, b, c);
    end

    always_comb begin : str
        $strobe("%t %m STROBE+ALWAYS : a: %b b: %b c: %b", $time, a, b, c);
    end

    always_comb begin : alw
        $display("%t %m ALWAYS :        a: %b b: %b c: %b", $time, a, b, c);
    end

endmodule // dut
