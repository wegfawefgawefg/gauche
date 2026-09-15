"""Two title directions and compact cues. Material gestures, not victory fanfares."""
from score import Piece


def roots_title():
    p = Piece("door_roots", "The Door in the Roots", "TITLE / WARM & UNEASY", 108,
              "One low breath, a hollow wooden answer, then room to wait. The gesture returns in a different register without growing into a melody.",
              "The more welcoming title option. A small identity, with unease around its edges.", 810,room=3.4,wet=.24)
    for at,dur,hz,gain,pan in [(3,22,73.42,.13,-.12),(17,14,110,.055,.15),
                              (43,23,73.42,.105,.1),(76,25,65.41,.12,-.1)]:
        p.sound(at,dur,"breath",hz,gain,pan,character="root")
    for at,hz,gain,pan in [(11,220,.04,-.35),(48,196,.032,.3),(90,220,.03,-.3)]:
        p.knocks(at,[(0,1),(.73,.28)],"coconut",hz,gain,pan)
    p.sound(27,14,"friction",148,.025,-.4,material="wood")
    p.sound(60,17,"breath",147.6,.023,.4,character="wire")
    p.sound(68,10,"strike",438.7,.012,.3,material="bell",softness=.97)
    p.sound(94,9,"friction",131,.026,.3,material="wood")
    p.mark(0,"A door-shaped breath"); p.mark(36,"Wait at the threshold"); p.mark(76,"A darker return")
    return p


def machine_title():
    p = Piece("machine_moss", "A Machine Dreams of Moss", "TITLE / STRANGER", 98,
              "Air moves through an impossible pipe; soft wooden contact and bent metal slowly trade places. Almost no rhythm.",
              "The less pastoral title. Does this feel distinctively Gauche, or too abstract?", 811,room=4.5,wet=.3)
    for at,dur,hz,gain,pan in [(2,24,61.74,.11,-.2),(37,23,62.1,.085,.2),(69,23,55,.11,0)]:
        p.sound(at,dur,"breath",hz,gain,pan,character="pipe")
    for at,dur,hz,gain,pan in [(16,18,184.2,.042,.4),(47,17,181.7,.03,-.35),
                              (78,15,183.9,.032,.3)]:
        p.sound(at,dur,"breath",hz,gain,pan,character="glass")
    p.sound(8,12,"strike",123.8,.035,.3,material="steel",softness=.98)
    p.sound(57,12,"strike",123.8,.029,-.3,material="steel",softness=.96)
    p.sound(30,6,"friction",174,.027,-.35,material="wood")
    p.sound(64,8,"friction",131,.025,.4,material="wood")
    p.knocks(42,[(0,.65),(.61,.25)],"wood",247,.033,-.4)
    p.knocks(86,[(0,.5)],"coconut",196,.027,.3)
    p.mark(0,"Metal with breath inside"); p.mark(30,"Wood intrudes"); p.mark(66,"Lower chamber")
    return p


def cue(slug,title,seconds,description,seed):
    return Piece(slug,title,"SHORT CUE",seconds,description,
                 "An isolated candidate; game timing and mix are still to be chosen.",seed,
                 loop=False,room=1.7,wet=.22,target=-27)


def cues():
    mark = cue("title_mark","Title mark — a hollow seal",10,
               "A short title/logo gesture, using air and a low wooden seal rather than an orchestral sting.",820)
    mark.sound(.1,3,"strike",146.8,.11,-.1,material="coconut",softness=.83)
    mark.sound(.5,7,"breath",73.42,.09,.1,character="root")
    mark.sound(2.8,5,"strike",294.1,.018,.3,material="steel",softness=.96)

    entry = cue("level_entry","Level entry — through the threshold",8,
                "Two soft wooden contacts and a breath that opens, then leaves the soundscape clear.",821)
    entry.knocks(.15,[(0,1),(.48,.4)],"wood",196,.075,-.15)
    entry.sound(.4,5,"breath",98,.065,.1,character="root")

    clear = cue("level_clear","Level clear — unclenching",10,
                "A held tension releases into a warmer cavity. Relief without a bright success arpeggio.",822)
    clear.sound(.05,3.2,"breath",147.8,.035,-.3,character="wire")
    clear.sound(1.1,6.6,"breath",73.42,.095,.08,character="root")
    clear.sound(.3,6,"strike",220,.028,.3,material="steel",softness=.93)

    win = cue("run_won","Run won — daylight through wood",15,
              "A larger exhale, a low hollow impact and a slowly appearing upper resonance. More warmth than triumph.",823)
    win.sound(.1,3,"strike",82.4,.09,0,material="hide",softness=.95)
    win.sound(.3,10,"breath",82.41,.115,-.15,character="root")
    win.sound(3.5,8.5,"breath",123.47,.043,.2,character="root")
    win.sound(5.4,6,"strike",329.7,.019,.4,material="ice",softness=.99)

    lose = cue("run_lost","Run lost — the cavity closes",13,
               "Wood stops, a rough low wire hangs in the air, and the remaining breath dies away. No comedy sad trombone.",824)
    lose.sound(.1,2,"strike",109,.06,-.2,material="wood",softness=.8)
    lose.sound(.35,8,"breath",52.4,.09,0,character="pipe")
    lose.sound(.7,7,"breath",107.7,.026,.25,character="wire")

    shop = cue("shop_arrives","Shop arrives — wheels and a crooked bell",9,
               "Hollow cart-wheel contacts and a slightly off metal bell. Small and tactile, with time for the merchant voice.",825)
    shop.knocks(.1,[(0,.5),(.31,.65),(.87,1),(1.02,.25)],"coconut",201,.07,-.25)
    shop.sound(1.4,6,"strike",557.3,.028,.25,material="bell",softness=.89)
    shop.sound(1.43,5.7,"strike",559.1,.01,-.2,material="bell",softness=.97)
    return [mark,entry,clear,win,lose,shop]


def pieces():
    return [roots_title(),machine_title(),*cues()]
