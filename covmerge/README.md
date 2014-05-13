See the URL for more
<https://tenthousandfailures.com/blog/>

Source code for posts on the tenthousandfailures.com blog and for the paper "Merging System Verilog Covergroups by Example".

Licensed under the GNU General Public License v2 see LICENSE.txt for more details.

Written by Eldon Nelson

## Command Lines for Mentor Questa to Run
```shell
 > qverilog +cover union_merge.sv -R -coverage -gui -do "run -all"
 > qverilog +cover weighted_merge.sv -R -coverage -gui -do "run -all"
 > qverilog +cover module_merge.sv -R -coverage -gui -do "run -all"
 > qverilog +cover covmerge4.sv -R -coverage -gui -do "run -all"
 > qverilog +cover efficient_merge.sv -R -coverage -gui -do "run -all"
```

## union_merge.sv
covergroup defined outside of dut union merge with instances

## weighted_merge.sv
covergroup defined outside of dut weighted average merge with instances

## module_merge.sv
covergroup defined inside of dut instance coverage only try merge

## covmerge4.sv
covergroup defined inside of dut instance coverage only no merge

## efficient_merge.sv
covergroup defined outside of dut union merge with no instances

