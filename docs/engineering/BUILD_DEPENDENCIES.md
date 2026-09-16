# Published dependency pins

The September 15 Linux fresh-build failure was a missing Git object, not an
Arch/Mint package mismatch. Gauche requested GView commit `9922248`, which was
present in the developer checkout but absent from every published remote ref.
CMake could clone the repository and then failed to check out that commit.

Local builds automatically used sibling GView/GLayout trees. Those trees also
contained later UI fixes than Gauche's fallback pins, so a successful local build
did not prove a friend could fetch and build the same dependency versions.

The exact clean local revisions used by Gauche are now published on the GView
and GLayout `gauche-20260915` branches. Gauche pins their full commit IDs, not the
moving branch names:

| Dependency | Commit |
| --- | --- |
| Gubsy | `c4d5408c74e61914f3886099db799919a2637696` |
| GView | `71956a3f3884a7e542b65cadd413637b9b3cff0e` |
| GLayout | `50861b524e58195f937e48984b33c1ca65e9631d` |

Gauche supplies `glayout::graph` before loading GView, keeping the layout version
explicit instead of inheriting GView's older standalone fallback. Normal builds
always fetch the published pins. Sibling checkouts require the explicit CMake
option `GAUCHE_USE_LOCAL_DEPS=ON`, including on machines with old cached sibling
paths. Missing developer paths fail clearly instead of silently mixing versions.

An unmodified checkout can update with `git pull --ff-only` and run
`./scripts/run.sh` again; changing GView APIs in Gauche is unnecessary. If a
workaround changed tracked game files, preserve that checkout and use a fresh
clone to compare against the fixed build.

Validation: a detached Gauche checkout at `031e0f0` with only the CMake fix, an
empty build directory and `GAUCHE_USE_LOCAL_DEPS=OFF` fetched every dependency
from GitHub and built the complete Release `gauche` target successfully on
Ubuntu 24.04. The fetched Git HEADs match all three pins. Compiler/linker inputs
contain no sibling development source paths. A subsequent `./scripts/build.sh`
configure/incremental build also passed. No gameplay/API changes were needed.
