"""Rebuild all 66 native-pixel replacements for the former imagegen sprites.

Usage: python3 tools/art/underworks.py (requires Pillow).
The four domain scripts can also run individually. PNGs are committed; normal
game builds do not run Python or require Pillow. Each drawing starts on a blank
RGBA canvas; no external art, random input, resizing or imagegen is involved.
"""
import underworks_workers
import underworks_creatures
import underworks_machines
import underworks_equipment
