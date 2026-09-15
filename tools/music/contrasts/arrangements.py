"""Different musical identities, with separate dominant instruments and structures."""
from score import Piece


def forest():
    p = Piece("seed_and_string","Seed and String","FOREST / PREPARED STRINGS & WOOD",84,
              "Low prepared strings, dry gourd contacts and seed-pod rattles. Small lopsided figures wander between clearings; no wind or pipe bed.",
              "A more tactile, grounded Forest direction. Is this kind of rhythm useful without becoming too tuneful?",902,
              room=1.0,wet=.09,target=-26)
    phrases = [
        (1,[(0,73.42,1),(2.35,146.83,.42),(4.12,110,.62),(8.7,82.41,.56)]),
        (18,[(0,73.42,.75),(1.6,220,.2),(5.8,110,.45)]),
        (35,[(0,65.41,.85),(3.9,130.81,.43),(5.1,98,.6),(9.2,146.83,.24)]),
        (57,[(0,73.42,.8),(2.6,110,.55),(7.5,146.83,.27)]),
        (72,[(0,82.41,.5),(3.7,73.42,.4)])]
    for start,notes in phrases:
        for index,(at,hz,gain) in enumerate(notes):
            p.sound(start+at,5,"prepared",hz,.14*gain,(-.18 if index%2 else .18),pin=.22,felt=.5)
        p.knocks(start+.9,[(0,.8),(.43,.22),(2.63,.45)],"coconut",181,.039,-.4)
        p.sound(start+5.4,1.1,"rattle",1200,.012,.45,density=19)
    for at,hz,pan in [(12,146.83,.4),(28,110,-.4),(47,130.81,.3),(66,146.83,-.3)]:
        p.sound(at,6,"tape",hz,.021,pan,reverse=.2)
    p.mark(0,"Low prepared-string figure"); p.mark(32,"A different clearing"); p.mark(56,"Loose return")
    return p


def industry():
    p = Piece("counterweight","Counterweight","INDUSTRY / SKIN, SCRAP & ROUGH BASS",88,
              "Heavy low skin, dry rack scrapes and a rough oscillator bass. A broken machine rhythm builds, shuts off, and returns with a different spacing.",
              "Much more rhythm-led than the pipe studies. Does it have the right weight without crowding combat?",903,
              room=.9,wet=.12,target=-25.5)
    beat=60/82
    for start,count in [(1,3),(26,2),(58,3)]:
        for bar in range(count):
            base=start+bar*beat*7
            for offset,accent in [(0,1),(2,.48),(3.5,.7),(5,.65)]:
                p.sound(base+offset*beat,2,"strike",62 if offset==0 else 103,
                        .1*accent,-.08 if offset==0 else .18,material="hide",softness=.75)
            for offset,accent in [(1,.6),(2.75,.4),(4.5,.8),(6,.35)]:
                p.sound(base+offset*beat,1.7,"strike",337+bar*11,.025*accent,-.4,
                        material="steel",softness=.93)
            p.sound(base+.12,1.8,"bass",49 if bar!=1 else 46.25,.06,0,grit=.55)
            p.sound(base+4*beat,1.3,"scrape",490,.015,.4,teeth=27+bar*3)
        p.sound(start+count*beat*7+1,6,"strike",116,.026,.25,material="steel",softness=.92)
    # The shutdown is a real change of musical behavior, not another low swell.
    p.sound(44,3.5,"scrape",280,.022,-.3,teeth=6)
    p.sound(49,6,"strike",154,.021,.35,material="steel",softness=.98)
    p.sound(53,1.5,"rattle",760,.009,-.3,density=9)
    p.mark(0,"Seven-step machinery"); p.mark(42,"Shutdown / loose parts"); p.mark(58,"Restart")
    return p


def fourth():
    p = Piece("watchtower_relay","Watchtower Relay","FOURTH ZONE? / DRY ELECTRONICS",78,
              "Small electronic relays, clipped low pulses and short radio-like interference. Hard edges, little room sound; no pipe or acoustic drone.",
              "A provisional military/robot palette only—the fourth biome is still undecided.",904,
              room=.5,wet=.035,target=-26)
    for start,spacing in [(1,.47),(20,.61),(46,.47),(62,.7)]:
        for index in range(13 if start==46 else 9):
            if index in (3,7,10):
                continue
            at=start+index*spacing
            p.sound(at,.8,"relay",247 if index%3 else 370,.021 if index%2 else .033,
                    -.4 if index%2 else .4,bend=-.08 if index%3 else .17)
            if index%4==0:
                p.sound(at+.05,1.2,"bass",41.2,.062,0,grit=.12)
        p.sound(start+6.3,2.4,"radio",891,.012,-.25 if start<40 else .25)
    p.sound(35,3,"radio",1317,.017,.25)
    p.sound(40,1,"relay",164.81,.02,-.25,bend=-.34)
    p.mark(0,"Dry relays"); p.mark(32,"Broken transmission"); p.mark(46,"Irregular response")
    return p


def title():
    p = Piece("crooked_door","The Crooked Door","TITLE / WARPED PLUCKED MEMORY",92,
              "Prepared-string scraps stretch, reverse and recur in the quiet. Low plucks give the fragments a center, without a sustained background bed.",
              "An intimate, odd title alternative. Compare this texture with the direct Forest strings.",905,
              room=2.0,wet=.22,target=-26)
    for at,dur,hz,gain,pan in [(1,13,146.83,.062,-.3),(14,10,220,.033,.3),
                              (30,15,130.81,.057,.25),(48,12,196,.031,-.3),
                              (64,15,146.83,.056,-.25),(78,9,110,.04,.3)]:
        p.sound(at,dur,"tape",hz,gain,pan,reverse=.58)
    for at,hz,gain in [(4,73.42,.11),(22,110,.038),(33,65.41,.09),
                       (55,98,.045),(67,73.42,.085),(82,82.41,.04)]:
        p.sound(at,7,"prepared",hz,gain,0,pin=.04,felt=.85)
    p.sound(27,3,"rattle",940,.007,-.4,density=5)
    p.sound(60,2,"rattle",1100,.007,.4,density=7)
    p.mark(0,"Memory in fragments"); p.mark(30,"Changed register"); p.mark(64,"Recognition without a tune")
    return p


def sketches():
    return [forest(),industry(),fourth(),title()]
