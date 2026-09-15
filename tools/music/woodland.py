"""Two original woodland arrangements. Motifs are authored, not sampled."""
from score import Score


def hollow_bough():
    s = Score("hollow_bough", "Under the Hollow Bough", 72, 40, 3, 2401,
              "Forest / 6:8. Cello and bassoon, an answering flute, harp and soft frame drum.")
    s.voices = {0: (42, 82, 46), 1: (70, 72, 79), 2: (73, 68, 73),
                3: (46, 63, 34), 4: (48, 42, 63), 5: (43, 71, 58)}
    # Dorian opening; the middle turns through F major and Bb before finding D again.
    opening = [(["D3", "A3", "C4", "E4"], "D2"),
               (["G3", "B3", "D4", "E4"], "G2"),
               (["C3", "G3", "B3", "D4"], "C2"),
               (["A2", "E3", "G3", "C4"], "A1"),
               (["D3", "F3", "A3", "E4"], "D2"),
               (["G3", "B3", "D4", "A4"], "G2"),
               (["E3", "G3", "B3", "D4"], "E2"),
               (["A2", "E3", "G3", "B3"], "A1")]
    middle = [(["F3", "A3", "C4", "E4"], "F2"),
              (["C3", "G3", "B3", "E4"], "E2"),
              (["Bb2", "F3", "A3", "D4"], "Bb1"),
              (["G3", "Bb3", "D4", "F4"], "G2"),
              (["D3", "F3", "A3", "C4"], "D2"),
              (["E3", "G3", "B3", "D4"], "E2"),
              (["A2", "E3", "G3", "D4"], "A1"),
              (["A2", "E3", "G3", "C#4"], "A1")]
    s.sections = [(0, "Roots / low strings"), (8, "A small clearing / flute answer"),
                  (16, "The old house / harmonic turn"), (24, "Returning path / fuller pulse"),
                  (32, "Under the leaves / thinning to the beginning")]
    for bar in range(40):
        chord, root = (middle if 16 <= bar < 24 else opening)[bar % 8]
        arc = [46, 58, 51, 64, 43][bar // 8]
        s.note(5, bar, 0, root, 2.7, arc - 7)
        if bar % 2 == 0:
            s.chord(4, bar, .05, chord, 2.6, arc - 12)
            s.expression(4, bar, [(0, 45), (.5, 66), (1.8, 75), (2.7, 49)])
        if 4 <= bar < 36:
            for beat, index in [(0, 0), (1, 2), (1.5, 1), (2.5, 3)]:
                if bar % 4 == 3 and beat > 1:
                    continue
                s.note(3, bar, beat, chord[index], .55, arc - 8)
        if 8 <= bar < 32 and bar % 4 != 3:
            s.hit("frame", bar, 0, .10 if bar < 24 else .15, -.2, 92)
            s.hit("brush", bar, 1.5, .035, .3, 500)
        if bar % 8 == 0:
            s.texture(bar, 8, root, .010)
    cello_a = [(0, "D3", 1.4), (1.5, "F3", .6), (2.5, "E3", .45),
               (3, "D3", 1), (4.5, "A3", 1.1), (6, "G3", 1.8),
               (8, "E3", .7), (9, "C3", 1.4), (10.5, "D3", .8)]
    cello_b = [(0, "F3", 1), (1.5, "A3", 1.25), (3, "B3", .65),
               (4, "A3", .7), (5, "G3", .7), (6, "E3", 2), (9, "C#3", 2.3)]
    flute_a = [(0, "A4", .7), (1, "D5", 1.1), (2.5, "E5", .4),
               (3, "D5", 1.3), (4.5, "B4", .7), (6, "G4", 1.8),
               (8.5, "E5", .35), (9, "D5", 1.4), (10.5, "C5", .8)]
    flute_b = [(0, "A4", .75), (1, "F5", 1.25), (3, "E5", .7),
               (4, "D5", .8), (5.5, "B4", .4), (6, "G4", 1.8), (9, "E4", 1.8)]
    for bar, melody in [(0, cello_a), (4, cello_b), (24, cello_a), (28, cello_b)]:
        s.phrase(0, bar, melody, 68 if bar < 8 else 73)
    s.phrase(2, 8, flute_a, 62)
    s.phrase(2, 12, flute_b, 59)
    s.phrase(1, 16, [(0, "F3", 2), (3, "G3", .8), (4, "E3", 1.6),
                     (6, "D3", 1.6), (8, "F3", .7), (9, "G3", 2)], 65)
    s.phrase(0, 20, [(0, "A3", 1.6), (2, "F3", .7), (3, "G3", 2.2),
                     (6, "E3", 1.6), (8, "D3", .7), (9, "C#3", 2.4)], 71)
    s.phrase(2, 26, [(0, "D5", 1.8), (3, "C5", 1.5), (6, "A4", 1.4)], 51)
    s.phrase(1, 32, cello_a, 54)
    s.phrase(0, 36, [(0, "F3", 1.8), (3, "G3", 2.2), (6, "E3", 1.9),
                     (9, "A2", 2.3)], 53)
    return s


def fern_lanterns():
    s = Score("fern_lanterns", "Lanterns among Ferns", 84, 32, 4, 381,
              "Forest / nocturnal 4:4. Clarinet, bowed low strings and sparse glass harmonics.")
    s.voices = {0: (71, 78, 77), 1: (42, 80, 47), 2: (48, 43, 65),
                3: (45, 66, 35), 4: (11, 48, 82), 5: (43, 68, 59)}
    chords = [(["E3", "G3", "B3", "F#4"], "E2"),
              (["C3", "G3", "B3", "D4"], "C2"),
              (["A2", "E3", "B3", "C4"], "A1"),
              (["B2", "F#3", "A3", "D#4"], "B1"),
              (["G3", "B3", "D4", "A4"], "G2"),
              (["D3", "A3", "C4", "F#4"], "D2"),
              (["C3", "E3", "G3", "B3"], "C2"),
              (["B2", "F#3", "A3", "D#4"], "B1")]
    s.sections = [(0, "Dusk / clarinet"), (8, "Footpaths / pizzicato pulse"),
                  (16, "Lantern light / cello answer"), (24, "Night settles / reprise")]
    for bar in range(32):
        chord, root = chords[(bar // 2) % 8]
        arc = [46, 59, 65, 43][bar // 8]
        s.note(5, bar, 0, root, 3.65, arc - 8)
        s.chord(2, bar, .1, chord, 3.5, arc - 14)
        s.expression(2, bar, [(0, 41), (.9, 65), (2.4, 76), (3.7, 43)])
        if 8 <= bar < 28:
            for beat, index in [(0, 0), (1.5, 2), (2.5, 1)]:
                s.note(3, bar, beat, chord[index], .45, arc - 2)
        if bar % 4 in (1, 3):
            s.note(4, bar, 2.5, chord[-1], 1.1, 38)
        if 12 <= bar < 24:
            s.hit("brush", bar, 1, .027, -.3)
            s.hit("frame", bar, 2.5, .08, .2, 78)
        if bar % 4 == 0:
            s.texture(bar, 4, root, .012)
    motif = [(0, "B3", 1.1), (1.5, "E4", 1.6), (3.5, "F#4", .4),
             (4, "G4", 1.3), (6, "F#4", .7), (7, "E4", .7),
             (8, "D4", 1.7), (10, "B3", 1.4), (12, "G3", 1.8), (14, "B3", 1.3)]
    answer = [(0, "C4", 1.5), (2, "E4", 1.4), (4, "B3", 2),
              (7, "G3", .75), (8, "F#3", 1.5), (10, "A3", 1.5),
              (12, "D#4", 2.5)]
    s.phrase(0, 0, motif, 62)
    s.phrase(1, 4, answer, 66)
    s.phrase(0, 8, motif, 69)
    s.phrase(0, 12, answer, 61)
    s.phrase(1, 16, [(0, "G3", 2), (3, "B3", .8), (4, "D4", 2),
                     (7, "A3", .8), (8, "F#3", 1.8), (10, "A3", 1.5),
                     (12, "C4", 1.5), (14, "A3", 1.3)], 76)
    s.phrase(0, 20, [(0, "G4", 2), (3, "E4", .8), (4, "D4", 2.6),
                     (8, "D#4", 1.5), (10, "F#4", 1.5), (12, "B3", 2.6)], 68)
    s.phrase(0, 24, motif, 53)
    s.phrase(1, 28, answer, 52)
    return s
