#!/usr/bin/env python3
"""Patch a TTF/OTF font to match the client's tuned vertical layout metrics.

The client engine (ioFTFaceImpl) positions UI text using the font's
`max(hhea.ascender, head.yMax)` as the baseline ascent and the bbox height
as the line box height. The UI has been tuned for years around the original
Korean font (NanumGothic, korea.ttf):

    head.yMin = -223, head.yMax = 858
    hhea.ascender = 800, hhea.descender = -300

Fonts with inflated global bboxes (e.g. pan-CJK fonts such as Noto Sans CJK,
whose head.yMax reaches 1806) shift all UI text downward. This script
rewrites those metrics so any font lays out identically to korea.ttf.

NOTE: Since the engine-side normalization (fixed 0.858/1.081 ratios in
ioFTFaceImpl::UpdateGlyphGlobalInfo), patching is no longer REQUIRED, but
keeping font metrics aligned is still good hygiene for other tools.

Usage:
    python patch-font-metrics.py <font.ttf> [<font2.ttf> ...]   # patch in place
    python patch-font-metrics.py --check <font.ttf>             # inspect only

Requires: fontTools (pip install fonttools)

IMPORTANT: recalcBBoxes MUST stay False. With the default True, fontTools
recalculates head.yMin/yMax from the CFF TopDict FontBBox on save and
silently reverts this patch (observed with Noto Sans CJK KR).
"""

import sys

from fontTools.ttLib import TTFont

HEAD_Y_MIN = -223
HEAD_Y_MAX = 858
HHEA_ASCENDER = 800
HHEA_DESCENDER = -300
TYPO_ASCENDER = 800
TYPO_DESCENDER = -300


def describe(path):
    font = TTFont(path, lazy=False)
    head = font["head"]
    hhea = font["hhea"]
    upm = head.unitsPerEm
    max_bearing = max(hhea.ascender, head.yMax)
    box_height = head.yMax - head.yMin
    print(f"  upm={upm} head.yMin={head.yMin} head.yMax={head.yMax} "
          f"hhea.asc={hhea.ascender} hhea.dsc={hhea.descender}")
    for size in (20, 24):
        bearing_px = int(max_bearing * size / upm + 0.99999)
        height_px = int(box_height * size / upm + 0.99999)
        print(f"  @{size}px: MaxBearingY={bearing_px}px  buffer={height_px}px"
              f"  (engine target: {int(0.858 * size + 0.99999)}px / {int(1.081 * size + 0.99999)}px)")
    font.close()


def patch(path):
    font = TTFont(path, lazy=False)
    head = font["head"]
    hhea = font["hhea"]

    head.yMin = HEAD_Y_MIN
    head.yMax = HEAD_Y_MAX
    hhea.ascender = HHEA_ASCENDER
    hhea.descender = HHEA_DESCENDER

    if "OS/2" in font:
        os2 = font["OS/2"]
        os2.sTypoAscender = TYPO_ASCENDER
        os2.sTypoDescender = TYPO_DESCENDER

    font.recalcBBoxes = False
    font.recalcTimestamp = False
    font.save(path)
    font.close()

    check = TTFont(path)
    ok = (check["head"].yMin == HEAD_Y_MIN
          and check["head"].yMax == HEAD_Y_MAX
          and check["hhea"].ascender == HHEA_ASCENDER)
    check.close()
    print(f"  patched -> {HEAD_Y_MIN}/{HEAD_Y_MAX}, asc={HHEA_ASCENDER}: "
          f"{'OK' if ok else 'FAILED (values not persisted!)'}")
    return ok


def main(argv):
    if len(argv) < 2:
        print(__doc__)
        return 1

    check_only = "--check" in argv
    paths = [a for a in argv[1:] if not a.startswith("--")]
    all_ok = True

    for path in paths:
        print(f"=== {path} ===")
        if check_only:
            describe(path)
        else:
            all_ok = patch(path)

    return 0 if all_ok else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv))
