# Release icons

These `.info` files were saved by AmigaOS 4's icon.library (native format,
64x64 true colour pictures, normal and selected). AmigaOS 4 does not read
PNG icons, so they can't be written on the host.

Their pictures are in `pictures/`: `<icon>.png` (normal) and
`<icon>.sel.png` (selected), 64x64 RGBA, in the OS 4.1 style, made by the
OpenAmiga icon tools (motif: a version tag with a second tag and a "+"
behind it). The drawers use the stock `def_drawer` picture as their base,
as most Amiga programs do. The selected pictures are made like the stock
OS 4.1 ones (each colour channel 84 darker, gold glow), so they are used
as they are.

| Icon | Picture | In the archive |
|------|---------|----------------|
| `ExtraVersion` | drawer with the version tags | `ExtraVersion.info` |
| `Install` | drive with a green arrow | `ExtraVersion/Install.info` |
| `ExtraVersion.doc` | text page | `ExtraVersion/ExtraVersion.doc.info` |
| `C` | drawer with a shell window | `ExtraVersion/C.info` |
| `C_ExtraVersion` | shell window with the version tags | `ExtraVersion/C/ExtraVersion.info` |

To change an icon:

1. Change its pictures in `pictures/`.
2. `python3 -I tools/make_icons.py build/icons` writes `*.argb`,
   `mkicons.script` and `icons-preview.png` (a second argument takes the
   pictures from another drawer; the icons' types, tools, tool types,
   stacks and windows are in its ICONS table).
3. Build `tools/os4/mkicon.c` with `ppc-amigaos-gcc` and copy it, the
   `.argb` files and the script to a drawer in the OS4 guest.
4. Run `Execute mkicons.script` in that drawer; the icons appear in `out/`.
5. Copy them back here. `C_ExtraVersion.info` becomes
   `C/ExtraVersion.info` in the archive (tools/make_lha.sh does this).
