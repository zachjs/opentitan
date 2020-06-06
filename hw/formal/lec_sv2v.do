// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

// dofile for LEC script lec_sv2v

//-----------------------------------------------------------------
// read in golden (SystemVerilog) and revised (Verilog)
//-----------------------------------------------------------------

// black box all instantiated modules (so only top-module is used)
set undefined cell black_box

// rename for comparison
set naming rule -mdportflatten
set naming rule -instance "%s" "%s_%d" "%s"
set naming rule -variable "%s" "%s" "%s"
set naming rule -mdportflatten -mdport_bitblast
set naming rule -parameter ""
set naming rule -instance_array "%s_%d"
set naming rule -enable_unnamed_blk_naming

// read golden design
read design -golden -sv12 \
  $LEC_DIR/prim_assert.sv \
  $LEC_DIR/top_pkg.sv \
  $LEC_DIR/tlul_pkg.sv \
  $LEC_DIR/prim_generic_*.sv \
  $LEC_DIR/*_pkg.sv \
  $LEC_DIR/$LEC_TOP.sv

// read revised design
read design -revised -verilog \
  $LEC_DIR/prim_generic_*.v \
  $LEC_DIR/$LEC_TOP.v

// force comparison of desired module
set root module $LEC_TOP -golden
set root module $LEC_TOP -revised

//-----------------------------------------------------------------
// pre-LEC reports
//-----------------------------------------------------------------
report design data
report black box
report module

//-----------------------------------------------------------------
// flatten
//-----------------------------------------------------------------
set flatten model -seq_constant
set flatten model -latch_fold
set flatten model -pin
set flatten model -latch_transparent
set flatten model -nomap
set flatten model -seq_merge
set flatten model -nodff_to_dlat_zero
set flatten model -seq_constant -noseq_constant_feedback
set flatten model -gated_clock
set flatten model -seq_redundant
set flatten model -all_seq_merge
set flatten model -nodff_to_dlat_feedback

//-----------------------------------------------------------------
// compare
//-----------------------------------------------------------------
set system mode lec
map key points
set parallel option -threads 4
analyze datapath -merge -verbose -effort ultra
add compare point -all
set compare effort ultra
compare -threads 4
analyze abort -compare

//-----------------------------------------------------------------
// reports
//-----------------------------------------------------------------
report compare data -class nonequivalent -class abort -class notcompared
report verification -verbose
report statistics
usage

exit -force
