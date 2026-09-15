"""Render pass 03 without changing/replacing any earlier audio."""
import argparse
from pathlib import Path
import sys

LEGACY = Path(__file__).resolve().parents[1]/"atmosphere"
sys.path.insert(0,str(LEGACY))
from render import render
from arrangements import sketches
from ice_song import ice_song
from palette import VOICES

OUTPUT = Path(__file__).resolve().parents[3]/"assets/music/contrasts"


def pieces():
    return [ice_song(),*sketches()]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--only")
    parser.add_argument("--masters",type=Path)
    args=parser.parse_args()
    tracks=pieces()
    if args.only and args.only not in [p.slug for p in tracks]:
        parser.error("Unknown track: "+args.only)
    for track in tracks:
        if not args.only or args.only==track.slug:
            # Palette boundary is executable: no shared breath/friction drone in
            # the new Forest, Industry, electronic or title directions.
            if track.slug!="beneath_the_thaw":
                assert all(e["kind"] not in ("breath","friction","glass") for e in track.events)
            render(track,OUTPUT,args.masters,VOICES)


if __name__=="__main__":
    main()
