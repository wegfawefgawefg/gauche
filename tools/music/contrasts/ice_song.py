"""One developed home for the old pipe palette, rather than a whole soundtrack."""
from score import Piece


def ice_song():
    p = Piece("beneath_the_thaw","Beneath the Thaw", "ICE / FULL ARRANGEMENT CANDIDATE",210,
              "One pipe-based Ice song: the cavity opens, a brittle walking pulse gathers, pressure rises, then exposed glass takes over before the low return.",
              "Does the pulse, harmonic detail and middle-section contrast give the pipe material enough development?",901,
              room=4.6,wet=.3,target=-26)
    # A: space and weight. A quiet three-pitch upper figure will reappear later;
    # it is not continually harmonized or answered by another lead instrument.
    for at,dur,hz,gain in [(2,25,46.25,.11),(27,19,46.25,.078),
                           (50,23,55,.085),(78,22,61.74,.105),
                           (146,24,46.25,.092),(173,25,46.25,.073)]:
        p.sound(at,dur,"breath",hz,gain,-.07,character="pipe")
    for at,dur,hz,gain,pan in [(10,12,277.18,.025,-.3),(25,10,246.94,.019,.35),
                              (37,9,369.99,.013,.15),(65,12,329.63,.022,-.3),
                              (92,11,311.13,.022,.3),(117,17,277.18,.028,-.25),
                              (126,15,278.02,.009,.3),(138,9,184.99,.027,.25),
                              (157,12,277.18,.02,-.3),(174,11,246.94,.018,.3),
                              (190,13,369.99,.015,.05)]:
        p.sound(at,dur,"glass",hz,gain,pan,rough=.013)
    # B: sparse frozen walking. Each occurrence changes omissions and accents.
    pulse_sets=[(44,[0,1.4,3.5,4.2,7.7,9.8,11.2],.047),
                (59,[0,2.1,2.8,6.3,8.4],.035),
                (78,[0,1.4,2.8,3.5,5.6,7,9.1,10.5],.052),
                (96,[0,.7,2.1,4.9],.035),
                (153,[0,1.4,4.9,7,9.8],.036),
                (174,[0,2.1,2.8,6.3],.026)]
    for start, offsets,gain in pulse_sets:
        for index,offset in enumerate(offsets):
            p.sound(start+offset,2.5,"strike",139 if index%3 else 185,
                    gain*(.55 if index%2 else 1),-.2 if index%2 else .25,
                    material="stone",softness=.82)
        p.sound(start+.45,6,"prepared",92.5,gain*.7,0,pin=.08,felt=.82)
    # C: a pressure peak, then a middle with no low pipe at all.
    p.sound(90,13,"scrape",480,.012,-.35,teeth=8)
    p.sound(106,9,"strike",231.7,.018,.35,material="steel",softness=.98)
    p.sound(113,9,"tape",185,.023,.35,reverse=.85)
    p.sound(135,7,"tape",123.47,.021,-.4,reverse=.75)
    for at,hz,gain,pan in [(6,639,.011,.4),(31,509,.013,-.4),(72,731,.009,.3),
                          (103,588,.012,-.35),(144,627,.008,.4),(185,509,.008,-.4)]:
        p.sound(at,3,"strike",hz,gain,pan,material="ice",softness=.95)
    for at,name in [(0,"A · cavity and first upper figure"),(43,"B · brittle walking pulse"),
                    (78,"C · pressure gathers"),(111,"D · exposed glass, pipe falls away"),
                    (146,"E · changed return"),(189,"F · thinning into the loop")]:
        p.mark(at,name)
    return p
