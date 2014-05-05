See the URL for more
<https://tenthousandfailures.com/blog/>

# COMMAND LINES FOR QUESTA
```shell
 > qverilog +cover union_merge.sv -R -coverage -gui -do "run -all"
 > qverilog +cover weighted_merge.sv -R -coverage -gui -do "run -all"
 > qverilog +cover module_merge.sv -R -coverage -gui -do "run -all"
 > qverilog +cover covmerge4.sv -R -coverage -gui -do "run -all"
 > qverilog +cover efficient_merge.sv -R -coverage -gui -do "run -all"
```

* union_merge.sv

covergroup defined outside of dut merge instances

* weighted_merge.sv

covergroup defined outside of dut no merge instances

* module_merge.sv

covergroup defined inside of dut merge instances

* covmerge4.sv

covergroup defined inside of dut no merge instances

* efficient_merge.sv

covergroup defined outside of dut merge but per_instance is 0

