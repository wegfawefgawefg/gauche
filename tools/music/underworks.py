"""Original foundry sketch: measured weight, breath and machinery, not wall-to-wall drums."""
from score import Score


def shift_below():
    s = Score("shift_below", "The Shift Below", 92, 36, 4, 610,
              "Industry / 4:4. Low horns, cello ostinato, modal membrane drums and distant metal.")
    s.voices = {0: (60, 83, 72), 1: (42, 77, 41), 2: (43, 77, 57),
                3: (70, 62, 80), 4: (48, 39, 61)}
    harmony = [(["D3", "A3", "C4"], "D2"), (["D3", "F3", "A3"], "D2"),
               (["Bb2", "F3", "A3"], "Bb1"), (["C3", "G3", "Bb3"], "C2"),
               (["G2", "D3", "F3"], "G1"), (["Eb3", "G3", "Bb3"], "Eb2"),
               (["A2", "E3", "G3"], "A1"), (["A2", "E3", "C#4"], "A1")]
    s.sections = [(0, "The lift descends"), (4, "First shift / measured drums"),
                  (12, "Furnace floor / horns"), (20, "Stopped machinery / breath"),
                  (24, "Second shift / counterline"), (32, "The lift returns")]
    for bar in range(36):
        chord, root = harmony[((bar - 4) // 2) % 8] if bar >= 4 else harmony[0]
        intensity = 40 if bar < 4 or 20 <= bar < 24 or bar >= 32 else 62
        s.note(2, bar, 0, root, 3.7, intensity)
        if bar % 2 == 0:
            s.chord(4, bar, 0, chord, 7.5, intensity - 17)
        if 4 <= bar < 32 and not 20 <= bar < 24:
            rhythm = [(0, 0, .65), (1.5, 1, .35), (2, 2, .65), (3.5, 1, .32)]
            for beat, degree, length in rhythm:
                if bar % 4 == 3 and beat == 3.5:
                    continue
                s.note(1, bar, beat, chord[degree], length, 58 if bar < 12 else 69)
            s.hit("war", bar, 0, .20 if bar < 12 else .27, -.22, 54)
            s.hit("frame", bar, 2.5, .10, .30, 105)
            if bar % 2:
                s.hit("war", bar, 3, .11, .15, 76)
            if bar % 4 == 2:
                s.hit("metal", bar, 3.5, .035, .5, 310)
        if bar in (0, 12, 24, 32):
            s.hit("metal", bar, .1, .065, -.45, 145)
        if bar % 4 == 0:
            s.texture(bar, 4, root, .014)
    horn = [(0, "D3", 2.3), (3, "A3", .7), (4, "F3", 1.8),
            (6, "E3", 1), (8, "D3", 2.6), (12, "C3", 2.8)]
    horn_turn = [(0, "G3", 2.4), (3, "F3", .7), (4, "Eb3", 2.3),
                 (8, "E3", 1.7), (10, "G3", 1.4), (12, "C#3", 2.7)]
    s.phrase(3, 0, [(0, "D3", 4), (6, "A2", 2.5), (10, "C3", 2), (13, "D3", 2)], 51)
    s.phrase(0, 8, horn, 63)
    s.phrase(0, 12, horn_turn, 74)
    s.phrase(0, 16, horn, 70)
    s.phrase(1, 20, [(0, "G3", 2.8), (4, "F3", 2), (7, "Eb3", .8),
                     (8, "E3", 2.5), (12, "C#3", 3)], 65)
    s.phrase(0, 24, horn_turn, 72)
    s.phrase(3, 24, [(1, "D3", 1.5), (5, "Bb2", 2), (9, "B2", 1.5), (13, "A2", 2)], 52)
    s.phrase(0, 28, horn, 65)
    s.phrase(3, 32, [(0, "D3", 3), (5, "C3", 2), (9, "A2", 2.5), (13, "D3", 2)], 48)
    for bar in range(4, 32, 4):
        s.expression(0, bar, [(0, 53), (1, 75), (2, 92), (3.5, 69),
                             (6, 81), (9, 89), (11, 66), (14, 76), (15.5, 51)])
    return s
