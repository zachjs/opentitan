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
fusesoc --cores-root . run --target=syn --setup lowrisc:systems:top_earlgrey

# copy all files into directory "syn_out"
\rm -Rf syn_out
mkdir syn_out
cp \
  build/lowrisc_systems_top_earlgrey_0.1/src/*/*.sv \
  build/lowrisc_systems_top_earlgrey_0.1/src/*/*/*.sv \
  build/lowrisc_systems_top_earlgrey_0.1/src/*/*/*/*.sv \
  syn_out

# remove some unneeded modules
\rm syn_out/pins_if.sv

# not synthesizable
\rm syn_out/prim_pad_wrapper.sv
\rm syn_out/prim_generic_pad_wrapper.sv

# troublesome and not used?
\rm syn_out/tlul_assert.sv
\rm syn_out/tlul_assert_multiple.sv

# match filename to module name
mv syn_out/ibex_register_file{_ff,}.sv

# only meant to be included within a module
mv syn_out/prim_util_memload.sv{,h}
sed -i.bak -e "s/prim_util_memload\.sv/prim_util_memload.svh/" syn_out/*.sv
rm syn_out/*.bak  # some sed implementations require backup extensions

#-------------------------------------------------------------------------
# convert all RTL files to Verilog
#-------------------------------------------------------------------------
printf "\n\nsv2v:\n"
sv2v -DSYNTHESIS syn_out/*.sv > syn_out/opentitan.v
modules=`cat syn_out/opentitan.v | grep "^module" | sed -e "s/^module //" | sed -e "s/ (//"`
for module in $modules; do
  cat syn_out/opentitan.v | sed -n "/^module $module /,/^endmodule/p" > syn_out/$module.v
done
rm syn_out/opentitan.v

#-------------------------------------------------------------------------
# run LEC (generarted Verilog vs. original SystemVerilog)
#-------------------------------------------------------------------------
printf "\n\nLEC RESULTS:\n"
cd ./hw/formal
for file in `ls ../../syn_out/*.v`; do
  module=`basename -s .v $file`
  ./lec_sv2v ../../syn_out $module > /dev/null 2>&1

  # summarize results
  result=`grep "Compare Results" lec_${module}.log`
  if [ $? -ne 0 ]; then
    result="CRASH"
  else
    result=`echo $result | awk '{ print $4 }'`
  fi
  printf "%-35s %s\n" $module $result
done
cd ../..

#-------------------------------------------------------------------------
# run yosys
#-------------------------------------------------------------------------
printf "\n\nYosys:\n"

# for now, read in each Verilog file into Yosys and only output errors
# and warnings
yosys -QTqp "
read_verilog syn_out/*.v;
hierarchy -check -top top_earlgrey;
synth_ice40;
write_blif out.blif;
"

# TODOs:
#  - add full yosys synthesis for all modules
#  - add final LEC check (RTL-versus-netlist)
