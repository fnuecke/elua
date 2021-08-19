-- Configuration file for the Sedna RISC-V VM Platform

local libfdt = sf( "src/platform/%s/libfdt", platform )

-- specific_files = "start.S platform.c stubs.c systimer.c console.c platform_int.c mod_riscv.c"
specific_files = "start.S platform.c platform_int.c lib/sbi/uart8250.c libfdt/fdt.c libfdt/fdt_ro.c libfdt/fdt_addresses.c"
local ldscript = "riscv.ld"

-- Prepend with path
specific_files = utils.prepend_path( specific_files, sf( "src/platform/%s", platform ) )
ldscript = sf( "src/platform/%s/%s", platform, ldscript )

local includes = sf( "src/platform/%s/include", platform )

-- Standard GCC flags
addcf{ '-ffunction-sections', '-fdata-sections', '-fno-strict-aliasing', '-Wall', '-I'..includes, '-I'..libfdt }
addlf{ '-nostartfiles', '-nostdlib', '-T', ldscript, '-Wl,--gc-sections', '-Wl,--allow-multiple-definition' }
addlib{ 'c', 'gcc', 'm' }

-- We define our own types, avoids conflicts with all the stuff SBI defines.
addcf{ '-DOPENSBI_EXTERNAL_SBI_TYPES=type.h' }

-- We use the 'sim' flag to enable dynamic memory size support.
addcf{ '-DELUA_BOARD_SIM' }

-- Set architecture flags.
local target_flags = { '-march=rv32gc', '-mabi=ilp32d' }

addcf{ target_flags, '-fno-builtin', '-fno-stack-protector' }
addlf{ target_flags, '-Wl,-static' }
addaf{ target_flags }

-- Toolset data
tools.sedna = {}

-- Array of file names that will be checked against the 'prog' target; their absence will force a rebuild
tools.sedna.prog_flist = { output .. ".bin" }

-- We use 'gcc' as the assembler
toolset.asm = toolset.compile