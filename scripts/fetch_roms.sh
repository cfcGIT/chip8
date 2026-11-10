#!/usr/bin/env bash
# Downloads the third-party test ROMs into roms/ (git-ignored).
#
# ROMs are not committed: they belong to their authors. Each one is pinned to an
# upstream commit and verified with SHA-256, so the download is reproducible and
# a changed or tampered file is rejected.
#
# Usage: scripts/fetch_roms.sh

set -euo pipefail

TIMENDUS='https://raw.githubusercontent.com/Timendus/chip8-test-suite/742e9eac9f5d44466c1c9d83fcaa8158c6683cb1/bin'
CORAX='https://raw.githubusercontent.com/corax89/chip8-test-rom/6a2cc38be6f5e527221353df024b840a86bc8e5f'
BESTCODER='https://raw.githubusercontent.com/stianeklund/chip8/31bdc1d398185a8f1924728c1f94223e02e24e82/roms'

# name|url|sha256
ROMS=(
    # Timendus chip8-test-suite (milestone 2): https://github.com/Timendus/chip8-test-suite
    "1-chip8-logo.ch8|$TIMENDUS/1-chip8-logo.ch8|15f7fb887ea4cb8e40615bb20b0cfd993ef55ca84c535c8c8f3fafa8bf129724"
    "2-ibm-logo.ch8|$TIMENDUS/2-ibm-logo.ch8|00072a250d2f7ccaa3ecc0182bac73e63c168abab96d7ea2df5eba6a4da49067"
    "3-corax+.ch8|$TIMENDUS/3-corax+.ch8|1c7e14eae14d6d5e1e47693804110354cbc4081defe4e6e5d9167c25ffc7b4b0"
    "4-flags.ch8|$TIMENDUS/4-flags.ch8|f00ddadd37bc878473de0c8f16faecf9985dea39036a3a796d551bc9fec47cfa"
    "5-quirks.ch8|$TIMENDUS/5-quirks.ch8|d839350268a3e73c7a16562b3d23c85aa1b92a567f5f61bd6727b1ea44635679"
    "6-keypad.ch8|$TIMENDUS/6-keypad.ch8|558902b0e406bb97dc808c16d55abf493706598246e3c77aea9d9401063169c9"
    "7-beep.ch8|$TIMENDUS/7-beep.ch8|bb8eb25fbfb7520af47d084b9e222914c308d6eec37c10435efa732894cb41c5"
    "8-scrolling.ch8|$TIMENDUS/8-scrolling.ch8|3f43507c45a949e5b014445853205dd1f36bb532cf25baa22209b7c300c596d7"
    # corax89 opcode test: https://github.com/corax89/chip8-test-rom
    "test_opcode.ch8|$CORAX/test_opcode.ch8|67759cf9f5b27db66f0769ea8fd0b30ba220f46d6f19f8ba4fd4108d986ce0ab"
    # BestCoder test ROM (mirrored in stianeklund/chip8)
    "BC_test.ch8|$BESTCODER/BC_test.ch8|e53cf68f850e51164d6087e0bdcaa3a5709491134108a2a48850705d9d5bf27f"
)

# Resolve roms/ relative to the repo root, so the script works from any directory.
ROM_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/roms"
mkdir -p "$ROM_DIR"

sha256_of() { sha256sum "$1" | cut -d' ' -f1; }

failed=0
for entry in "${ROMS[@]}"; do
    IFS='|' read -r name url sha <<< "$entry"
    dest="$ROM_DIR/$name"

    # Skip files that are already present and intact.
    if [[ -f "$dest" && "$(sha256_of "$dest")" == "$sha" ]]; then
        echo "ok        $name"
        continue
    fi

    # Download to a temp name first so a failed or bad download never leaves a broken ROM behind.
    tmp="$dest.part"
    if ! curl -fsSL --retry 2 -o "$tmp" "$url"; then
        rm -f "$tmp"
        echo "FAILED    $name: download error" >&2
        failed=$((failed + 1))
        continue
    fi

    got="$(sha256_of "$tmp")"
    if [[ "$got" != "$sha" ]]; then
        rm -f "$tmp"
        echo "FAILED    $name: SHA-256 mismatch (got $got)" >&2
        failed=$((failed + 1))
        continue
    fi

    mv -f "$tmp" "$dest"
    echo "fetched   $name"
done

if (( failed > 0 )); then
    echo "$failed ROM(s) failed to download." >&2
    exit 1
fi
echo "All ROMs in $ROM_DIR"
