# Copyright lowRISC contributors.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0

subdir('lib')

foreach device_name, device_lib : sw_lib_arch_core_devices
  aes_serial_elf = executable(
    'aes_serial_' + device_name,
    sources: ['aes_serial.c'],
    name_suffix: 'elf',
    dependencies: [
      device_lib,
      riscv_crt,
      sw_lib_dif_aes,
      sw_lib_mmio,
      sw_lib_runtime_hart,
      sw_lib_runtime_log,
      sw_sca_lib_prng,
      sw_sca_lib_sca,
      sw_sca_lib_simple_serial,
    ],
  )

  aes_serial_embedded = custom_target(
    'aes_serial_' + device_name,
    command: make_embedded_target_command,
    depend_files: [make_embedded_target_depend_files,],
    input: aes_serial_elf,
    output: make_embedded_target_outputs,
    build_by_default: true,
  )

  custom_target(
    'aes_serial_export_' + device_name,
    command: export_target_command,
    depend_files: [export_target_depend_files,],
    input: [aes_serial_elf, aes_serial_embedded],
    output: 'aes_serial_export_' + device_name,
    build_always_stale: true,
    build_by_default: true,
  )
endforeach
