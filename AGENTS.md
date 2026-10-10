# Medal Of Honor

Resolve `[XPORT_ROOT]` from `xport-project.json` and read `[XPORT_ROOT]/AGENTS.md`. Keep only stable game-specific facts and task-routing links here; put detailed evidence under `status`. Never create, edit, move, delete or regenerate `README.md`.

## Project facts

- Native short name: `MOH`; language: C
- Solution: `src/platform/win/MOH.sln`; Debug executable: `bin/MOH_debug.exe`; Release executable: `bin/MOH.exe`; working directory: `bin`; intermediates: `_build`
- Runtime data: `bin/DATA`; Red Book output when applicable: `bin/MUSIC`
- Accepted original: `SLUS_009.74`, SHA-256 `19215344b0031358a6f628cbee0bd124f23275d4621aa16ada8f959cfa0792e3`; entry `0x8001DFD4`, runtime GP `0x80035D1C`
- Implementation language is C under the startup scaffold contract; this is not a claim that all original source was C
- Cold boot BIOS: `scph5502.bin`, SHA-256 `1faaa18fa820a0225e488d9f086296b8e6c46df739666093987ff7d8fd352c09`; entry RA `0xBFC03D60` verified in `status/audits/startup-entry-state.json`
- Entrypoint implementation is WIP; its original callsites are audited in `status/audits/startup-entrypoint-translation.json`. Direct callees remain TODO; no startup stubs were added
- `moh_guest_sp`, `moh_guest_fp`, `moh_guest_gp` and `moh_guest_ra` retain audited startup register state for deferred translated callees
- Main normal noreturn contract: `status/audits/startup-main-return-contract.json`; independently called block `0x8001E07C` remains for coverage
- GPU graph-type binding remains unresolved (`0u`); no address is inferred and no GPU wrapper is called by the translated entry
- Runtime roles: `user` on port `2432`, `lockstep` on port `2433`; `DuckStation.bat` launches the manual user role
- Additional emulator audit data is isolated under `tools/duckstation/data/entry-audit-v1`; its owned process was closed
- Native linking with translated callees and the final Release build are deferred to coverage
