-- eLua running on Sedna RISC-V emulator

return {
  cpu = 'SEDNA',
  components = {
    romfs = true,
    wofs = false,
    shell = { advanced = true },
    sercon = { uart = 0, speed = 115200 },
    term = { lines = 25, cols = 80 },
    cints = true,
    linenoise = { shell_lines = 20, lua_lines = 20 },
    mmcfs = { spi = 0, cs_port = 0, cs_pin = 0 },
  },
  modules = {
    generic = { 'all_lua', 'bit', 'cpu', 'pack', 'term', 'tmr', 'uart', 'sedna' },
    platform = { 'riscv' }
  }
}

