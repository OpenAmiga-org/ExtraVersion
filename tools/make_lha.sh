#!/bin/bash
# Puts the AmigaOS 4 release together and packs it: dist/ExtraVersion.lha
#
#   make lha        (builds first)
#
# Layout:
#   ExtraVersion.info         drawer icon
#   ExtraVersion.readme       OS4Depot readme
#   ExtraVersion/
#     Install (+ .info)       Installation Utility script
#     ExtraVersion.doc (+ .info)
#     C (+ .info)
#     C/ExtraVersion (+ .info)
#
# The LHA and the readme are copied to $RELEASES (default
# /media/sf_claude_folder/releases) for review.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
LHA="${LHA:-$HOME/linux_archivers/lha/lha}"
RELEASES="${RELEASES:-/media/sf_claude_folder/releases}"
IMAGE=walkero/amigagccondocker:os4-gcc15-3.6.1
STAGE="$ROOT/build/stage"
OUT="$ROOT/dist/ExtraVersion.lha"
BIN="$ROOT/build/os4/ExtraVersion"
SRC="$ROOT/src/extraversion.c"

ver=$(sed -n 's/^#define APP_VERSION[[:space:]]*"\(.*\)"/\1/p' "$SRC")       # 1.3
amigadate=$(sed -n 's/^#define APP_DATE[[:space:]]*"\(.*\)"/\1/p' "$SRC")    # 20.11.2026
[ -n "$ver" ] && [ -n "$amigadate" ] || { echo "no APP_VERSION/APP_DATE in $SRC" >&2; exit 1; }

if ! strings "$BIN" | grep -qx "\$VER: ExtraVersion $ver ($amigadate)"; then
	echo "build/os4/ExtraVersion is not ExtraVersion $ver ($amigadate); run make" >&2
	exit 1
fi
# the readme's History must have this version and date on top (DD.MM.YYYY)
histdate=$(echo "$amigadate" | awk -F. '{printf "%02d.%02d.%04d", $1, $2, $3}')
if ! iconv -f latin1 -t utf8 "$ROOT/package/ExtraVersion.readme" | grep -A2 '^## History' \
		| grep -qxF "### $ver ($histdate)"; then
	echo "package/ExtraVersion.readme: History does not start with \"### $ver ($histdate)\"" >&2
	exit 1
fi

rm -rf "$STAGE"
D="$STAGE/ExtraVersion"
mkdir -p "$D/C" "$ROOT/dist"
cp "$BIN" "$D/C/ExtraVersion"
# without the symbols
sudo docker run --rm -u "$(id -u):$(id -g)" -v "$D/C:/c" "$IMAGE" ppc-amigaos-strip /c/ExtraVersion
cp "$ROOT/package/ExtraVersion/Install" "$D/"
sed -e "s/@VERSION@/$ver/g" -e "s/@DATE@/$amigadate/g" "$ROOT/docs/ExtraVersion.doc" > "$D/ExtraVersion.doc"
sed -e "s/@VERSION@/$ver/g" -e "s/@DATE@/$amigadate/g" "$ROOT/package/ExtraVersion.readme" \
	> "$STAGE/ExtraVersion.readme"

# icons saved by icon.library itself (tools/make_icons.py, package/icons/README.md)
I="$ROOT/package/icons"
cp "$I/ExtraVersion.info" "$STAGE/"
cp "$I/Install.info" "$I/ExtraVersion.doc.info" "$I/C.info" "$D/"
cp "$I/C_ExtraVersion.info" "$D/C/ExtraVersion.info"

rm -f "$OUT"
(cd "$STAGE" && "$LHA" aq2o5 "$OUT" ExtraVersion.info ExtraVersion.readme ExtraVersion)
"$LHA" l "$OUT"
echo "ExtraVersion $ver ($amigadate) -> $OUT"

# the GitHub page's changelog, from the readme
python3 -I "$ROOT/tools/update_readme.py"

if [ -d "$RELEASES" ]; then
	cp "$OUT" "$STAGE/ExtraVersion.readme" "$RELEASES/"
	echo "copied LHA and readme to $RELEASES"
fi
