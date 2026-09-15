"""Three different Forest answers: breath, dry wood, and something unsettling."""
from score import Piece


def root_lung():
    p = Piece("root_lung", "Root Lung", "FOREST / BREATH", 96,
              "Long, low breaths caught inside a wooden body. A few hollow knocks; the middle empties out.",
              "Warmest option. Is the quiet horn-like body useful when it never becomes a tune?", 801)
    for at, dur, hz, gain, pan in [(3,20,73.42,.19,-.2), (28,16,73.42,.13,.2),
                                  (57,24,65.4,.17,-.1), (79,13,73.42,.11,.15)]:
        p.sound(at,dur,"breath",hz,gain,pan,character="root")
    p.sound(16,9,"friction",143,.027,.5,material="wood")
    p.sound(68,12,"friction",196,.034,-.4,material="wood")
    p.knocks(8,[(0,1),(.43,.32)],"coconut",162,.046,-.48)
    p.knocks(32,[(0,.7),(1.1,.25)],"wood",211,.032,.5)
    p.knocks(50,[(0,.45)],"wood",131,.025,-.3)
    p.knocks(76,[(0,1),(.31,.4),(2.6,.22)],"coconut",148,.036,.4)
    p.mark(0,"Breath and hollow wood"); p.mark(43,"An almost empty clearing"); p.mark(57,"Low return")
    return p


def knots():
    p = Piece("knots", "Knots in the Bark", "FOREST / DRY MATERIAL", 88,
              "Close coconut shells, rubbing wood and distant stone. No continuous bass bed; long gaps separate small rhythms.",
              "Drier and more physical. Does this feel like a place, or too much like isolated sound effects?", 802,
              room=1.5,wet=.14)
    for at, pattern, hz, gain, pan in [
        (3,[(0,1),(.29,.32),(1.31,.65)],244,.09,-.35),
        (15,[(0,.7),(.58,.4),(2.06,1),(2.24,.28)],183,.064,.4),
        (38,[(0,1),(.81,.3)],221,.06,-.4),
        (55,[(0,.8),(.27,.4),(1.72,.55),(3.01,.25)],157,.07,.2),
        (78,[(0,.65),(1.17,.35)],244,.058,-.25)]:
        p.knocks(at,pattern,"coconut",hz,gain,pan)
    for at,hz,pan in [(10,126,.5),(44,181,-.35),(69,153,.25)]:
        p.sound(at,5,"friction",hz,.048,pan,material="wood")
    p.sound(27,10,"breath",82.41,.072,-.1,character="root")
    p.sound(62,12,"breath",82.41,.069,.1,character="root")
    p.sound(22,2,"strike",520,.018,-.5,material="stone",softness=.7)
    p.sound(73,2,"strike",431,.022,.45,material="stone",softness=.8)
    p.mark(0,"Close, hollow objects"); p.mark(26,"A brief resonance"); p.mark(52,"Broken rhythm")
    return p


def footbridge():
    p = Piece("footbridge", "Something under the Footbridge", "FOREST / HORROR", 94,
              "A strained wire underneath the wood, uneven scraping, a very low breathing cavity. No jump-scare hit.",
              "More uncomfortable. Keep this tension, or reserve it for haunted places?", 803,room=3.6,wet=.25)
    for at,dur,hz,gain,pan in [(5,19,57.8,.11,-.2),(31,23,59.1,.08,.2),(67,20,57.8,.12,0)]:
        p.sound(at,dur,"breath",hz,gain,pan,character="pipe")
    for at,dur,hz,gain,pan in [(13,16,147.2,.045,.45),(44,14,151.7,.038,-.4),
                              (73,17,146.1,.041,.3)]:
        p.sound(at,dur,"breath",hz,gain,pan,character="wire")
    for at,hz,pan in [(3,107,-.5),(28,193,.4),(59,107,-.3),(84,169,.45)]:
        p.knocks(at,[(0,.8),(.71,.22)],"wood",hz,.035,pan)
    p.sound(37,8,"friction",287,.028,-.5,material="stone")
    p.sound(62,5,"friction",112,.04,.45,material="wood")
    p.mark(0,"Below the boards"); p.mark(31,"Wire pressure"); p.mark(56,"Wait before the return")
    return p


def pieces():
    return [root_lung(), knots(), footbridge()]
