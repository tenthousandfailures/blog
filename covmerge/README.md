See the URL for more
<https://tenthousandfailures.com/blog/>

COMMAND LINES FOR QUESTA
 > qverilog +cover covmerge1.sv -R -coverage -gui -do "run -all"
 > qverilog +cover covmerge2.sv -R -coverage -gui -do "run -all"
 > qverilog +cover covmerge3.sv -R -coverage -gui -do "run -all"
 > qverilog +cover covmerge4.sv -R -coverage -gui -do "run -all"
 > qverilog +cover covmerge5.sv -R -coverage -gui -do "run -all"

covmerge1.sv
covergroup defined outside of dut merge instances

covmerge2.sv
covergroup defined outside of dut no merge instances

covmerge3.sv
covergroup defined inside of dut merge instances

covmerge4.sv
covergroup defined inside of dut no merge instances

covmerge5.sv
covergroup defined outside of dut merge but per_instance is 0

