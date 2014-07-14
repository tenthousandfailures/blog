#!/bin/sh

rm -rf work
vlib work

# mfcu is needed!
vlog -sv -mfcu -pedanticerrors \
    FlyBehavior.sv \
    FlyWithWings.sv \
    FlyNoWay.sv \
    QuackBehavior.sv \
    Quack.sv \
    MuteQuack.sv \
    Squeak.sv \
    Duck.sv \
    MallardDuck.sv \
    DecoyDuck.sv \
    RubberDuck.sv \
    ModelDuck.sv \
    MiniDuckSimulator.sv

vsim -c tb -do "run -all"
