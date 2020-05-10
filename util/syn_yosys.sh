#!/bin/bash

# Copyright lowRISC contributors.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0

# This script converts all SystemVerilog RTL files to Verilog and then
# runs Yosys.
#
# The following tools are required:
#  - sv2v: SystemVerilog-to-Verilog converter from github.com/zachjs/sv2v
#  - yosys: synthesis tool from github.com/YosysHQ/yosys
#
# Usage:
#   syn_yosys.sh 2>&1 | tee syn.std

#-------------------------------------------------------------------------
# use fusesoc to generate files and file list
#-------------------------------------------------------------------------
\rm -Rf build
fusesoc --cores-root . run --target=sim --setup lowrisc:systems:top_earlgrey_verilator

# copy all files into directory "syn_out"
\rm -Rf syn_out
mkdir syn_out
cp \
  build/lowrisc_systems_top_earlgrey_verilator_0.1/src/*/*.sv \
  build/lowrisc_systems_top_earlgrey_verilator_0.1/src/*/*/*.sv \
  build/lowrisc_systems_top_earlgrey_verilator_0.1/src/*/*/*/*.sv \
  syn_out

# remove some unneeded modules
\rm syn_out/top_earlgrey_verilator.sv
\rm syn_out/ibex_tracer.sv
\rm syn_out/dmidpi.sv
\rm syn_out/gpiodpi.sv
\rm syn_out/jtagdpi.sv
\rm syn_out/spidpi.sv
\rm syn_out/uartdpi.sv
\rm syn_out/usbdpi.sv
\rm syn_out/pins_if.sv


#-------------------------------------------------------------------------
# convert all RTL files to Verilog
#-------------------------------------------------------------------------
cd syn_out
sv2v -DSYNTHESIS -DSV2V *.sv > opentitan.v

# modules=`cat $ve | grep "^module" | grep -v "^module top" | sed -e "s/module \([a-zA-Z0-0_]\{1,\}\).*/\1/"`

#####-------------------------------------------------------------------------
##### run LEC (generarted Verilog vs. original SystemVerilog)
#####-------------------------------------------------------------------------
####printf "\n\nLEC RESULTS:\n"
####cd ../../hw/formal
####for file in ../../util/syn_out/*.v; do
####  module=`basename -s .v $file`
####  lec_sv2v ../../util/syn_out $module > /dev/null 2>&1
####
####  # summarize results
####  result=`grep "Compare Results" lec_${module}.log`
####  if [ $? -ne 0 ]; then
####    result="CRASH"
####  else
####    result=`echo $result | awk '{ print $4 }'`
####  fi
####  printf "%-25s %s\n" $module $result
####done
####cd -

#-------------------------------------------------------------------------
# run yosys
#-------------------------------------------------------------------------
printf "\n\nYosys:\n"

# for now, read in each Verilog file into Yosys and only output errors
# and warnings
for file in *.v; do
  yosys -QTqp "
read_verilog syn_out/*.v \
hierarchy -check -top top_earlgrey \
synth_ice40 \
write_blif out.blif \
"
done

# TODOs:
#  - add full yosys synthesis for all modules
#  - add final LEC check (RTL-versus-netlist)
