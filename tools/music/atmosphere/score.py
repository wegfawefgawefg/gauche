"""Time in seconds, material, gesture, level and space — deliberately not a song grid."""
from dataclasses import dataclass, field


@dataclass
class Piece:
    slug: str
    title: str
    zone: str
    seconds: float
    description: str
    contrast: str
    seed: int
    loop: bool = True
    room: float = 2.8
    wet: float = .23
    target: float = -26
    events: list = field(default_factory=list)
    markers: list = field(default_factory=list)

    def sound(self, at, duration, kind, hz, gain, pan=0, **options):
        assert at >= 0 and duration > 0 and at < self.seconds
        self.events.append(dict(at=at, duration=duration, kind=kind, hz=hz,
                                gain=gain, pan=pan, **options))

    def mark(self, at, text):
        self.markers.append(dict(seconds=at, name=text))

    def knocks(self, at, pattern, material, hz, gain, pan=0):
        """Explicit asymmetrical little groups, never an endlessly repeated bar."""
        for offset, accent in pattern:
            self.sound(at+offset, 1.8, "strike", hz*(1+.027*offset), gain*accent,
                       pan, material=material, softness=.65)
