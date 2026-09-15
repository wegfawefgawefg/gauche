"""Small offline score notation; beats, named notes, articulations and phrases."""
from dataclasses import dataclass, field
import random


def pitch(name):
    if isinstance(name, int):
        return name
    letter, octave = name[:-1], int(name[-1])
    semitone = {"C": 0, "D": 2, "E": 4, "F": 5, "G": 7, "A": 9, "B": 11}[letter[0]]
    semitone += letter.count("#") - letter.count("b")
    return 12 * (octave + 1) + semitone


@dataclass
class Score:
    slug: str
    title: str
    bpm: float
    bars: int
    meter: float
    seed: int
    description: str
    # channel -> GM program (zero based), volume, pan
    voices: dict = field(default_factory=dict)
    events: list = field(default_factory=list)
    percussion: list = field(default_factory=list)
    textures: list = field(default_factory=list)
    sections: list = field(default_factory=list)

    def __post_init__(self):
        self.rng = random.Random(self.seed)

    @property
    def duration(self):
        return self.bars * self.meter * 60 / self.bpm

    def at(self, bar, beat=0):
        return (bar * self.meter + beat) * 60 / self.bpm

    def note(self, voice, bar, beat, name, length, velocity=60, loosen=.014):
        start = max(0, self.at(bar, beat) + self.rng.uniform(-loosen, loosen))
        end = start + length * 60 / self.bpm
        key = pitch(name)
        velocity = max(1, min(112, velocity + self.rng.randrange(-3, 4)))
        self.events.extend([(start, 1, voice, key, velocity), (end, 0, voice, key, 0)])

    def chord(self, voice, bar, beat, notes, length, velocity=48, spread=.035):
        for index, key in enumerate(notes):
            self.note(voice, bar, beat + index * spread, key, length, velocity)

    def phrase(self, voice, bar, notes, velocity=62, transpose=0):
        """Each entry is (offset in beats, pitch, duration); None creates no note."""
        for beat, key, length in notes:
            if key is not None:
                self.note(voice, bar, beat, pitch(key) + transpose, length, velocity)

    def expression(self, voice, bar, values):
        for beat, value in values:
            self.events.append((self.at(bar, beat), 2, voice, 11, value))

    def hit(self, kind, bar, beat, strength=.15, pan=0, tone=75):
        self.percussion.append((kind, self.at(bar, beat), strength, pan, tone))

    def texture(self, bar, bars, root, strength=.01):
        self.textures.append((self.at(bar), bars * self.meter * 60 / self.bpm,
                              pitch(root), strength))
