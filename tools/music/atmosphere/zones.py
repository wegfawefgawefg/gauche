"""Ice, Industry, and a percussion-led threat study without an orchestral lead."""
from score import Piece


def ice():
    p = Piece("ice_weight", "Ice Has Weight", "ICE / HELD RESONANCE", 102,
              "Rubbed glass held for many seconds, low pressure below it, tiny stone fractures. High detail without a chime melody.",
              "Cold, slow and exposed. Are the glass overtones interesting or too piercing?", 804,room=5.8,wet=.34)
    for at,dur,hz,gain,pan in [(4,24,231.3,.095,-.4),(34,20,227.8,.071,.3),
                              (64,27,173.8,.09,-.2)]:
        p.sound(at,dur,"breath",hz,gain,pan,character="glass")
    for at,dur,hz in [(13,18,46.25),(49,14,46.25),(75,18,43.65)]:
        p.sound(at,dur,"breath",hz,.095,.05,character="pipe")
    for at,hz,gain,pan in [(9,619,.019,.55),(43,487,.026,-.5),(60,711,.014,.4),(94,533,.018,-.4)]:
        p.sound(at,3,"strike",hz,gain,pan,material="stone",softness=.85)
    p.sound(30,12,"friction",321,.026,-.2,material="ice")
    p.sound(58,13,"strike",384.7,.017,.3,material="ice",softness=.94)
    p.mark(0,"Rubbed glass"); p.mark(33,"Pressure without percussion"); p.mark(63,"Lower ice")
    return p


def pipes():
    p = Piece("thaw_pipes", "Thaw in the Pipes", "ICE → INDUSTRY / WATER & METAL", 92,
              "An air-filled pipe, low metal touched rather than struck hard, irregular hollow droplets. Brief activity followed by waiting.",
              "A quieter mechanical direction, with no march or drum kit.", 805,room=4.1,wet=.31)
    for at,dur,hz,gain in [(2,18,61.74,.11),(35,18,61.74,.075),(68,20,55,.105)]:
        p.sound(at,dur,"breath",hz,gain,-.15,character="pipe")
    for at,hz,gain,pan in [(12,219,.036,.4),(41,173,.03,-.3),(76,219,.026,.25)]:
        p.sound(at,13,"strike",hz,gain,pan,material="steel",softness=.92)
    for at,pattern,hz,pan in [(6,[(0,.5),(2.1,1),(2.49,.3)],384,-.5),
                             (28,[(0,.4),(1.63,.7)],429,.45),
                             (55,[(0,1),(.39,.3),(3.07,.5)],361,-.3),
                             (83,[(0,.6),(1.19,.25)],402,.4)]:
        p.knocks(at,pattern,"coconut",hz,.027,pan)
    p.sound(49,7,"friction",183,.026,.35,material="steel")
    p.mark(0,"Air in the pipe"); p.mark(25,"Only drops"); p.mark(67,"A larger cavity")
    return p


def shift():
    p = Piece("last_shift", "After the Last Shift", "INDUSTRY / HOLLOW PULSE", 100,
              "Wide low drum skins, steel cavities, a tired pipe breath and an occasional uneven bell. Rhythms stop before settling into a loop.",
              "More weight and motion. Compare its hollow percussion with the dry Forest knocks.", 806,room=3.3,wet=.25)
    for at,dur,hz,gain in [(4,19,49,.115),(42,16,49,.095),(73,22,46.25,.12)]:
        p.sound(at,dur,"breath",hz,gain,-.05,character="pipe")
    groups = [(8,[0,2.7,3.32,6.1],.095),(29,[0,1.89,4.33],.075),
              (51,[0,2.4,3.03,6.5,8.1],.12),(81,[0,3.05,3.68,7.2],.095)]
    for at, offsets, gain in groups:
        for i, offset in enumerate(offsets):
            p.sound(at+offset,3,"strike",72 if i%3==0 else 108,
                    gain*(1 if i%3==0 else .55),(-.17 if i%2 else .17),material="hide",softness=.85)
        p.sound(at+1.22,9,"strike",157,gain*.27,.48,material="steel",softness=.84)
    for at,hz,pan in [(22,557.3,-.4),(64,553.6,.35),(91,557.3,-.3)]:
        p.sound(at,9,"strike",hz,.017,pan,material="bell",softness=.8)
    p.sound(36,9,"friction",118,.023,-.45,material="steel")
    p.mark(0,"Low skin and air"); p.mark(35,"The machines stop"); p.mark(50,"Heavier broken pulse")
    return p


def pressure():
    p = Piece("pressure", "Do Not Wake the Press", "THREAT / BOSS STUDY", 72,
              "A weight-bearing pulse with missing steps. Skin and struck steel gather, then leave a hole where the next hit should be.",
              "A more urgent option without a heroic tune; should combat music go this far?", 809,room=2.2,wet=.2,target=-25)
    for at,dur,hz,gain in [(1,21,55,.105),(26,17,56.1,.12),(49,18,55,.09)]:
        p.sound(at,dur,"breath",hz,gain,0,character="wire")
    # Designed accumulations, not an invariant four-on-the-floor backing pattern.
    for at, offsets in [(4,[0,2.4,4.8,5.38,8.4]),(19,[0,1.8,4.2,4.76,7.8,9]),
                        (36,[0,2.1,2.67,4.2,6.3,7.41,8.4]),(57,[0,2.8,5.6])]:
        for i, offset in enumerate(offsets):
            p.sound(at+offset,2.8,"strike",66 if i%2==0 else 99,
                    .13 if i%2==0 else .065,(-.2 if i%2 else .2),material="hide",softness=.72)
        p.sound(at+.9,5,"strike",133,.037,.4,material="steel",softness=.8)
    p.sound(49,10,"friction",203,.05,-.4,material="steel")
    p.mark(0,"The first weight"); p.mark(35,"Compressed spacing"); p.mark(48,"Missing blows")
    return p


def pieces():
    return [ice(), pipes(), shift(), pressure()]
