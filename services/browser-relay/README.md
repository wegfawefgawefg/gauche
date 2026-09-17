# Browser multiplayer bridge

See [browser build and deployment](../../docs/engineering/BROWSER_BUILD.md).
This service only adapts browser WebSockets to the existing authenticated
rendezvous/relay ports. Cloudflare Pages hosts the game and website.

Deploy `relay.py`, `install.py`, and `teeming-browser-relay.service` together to
the dedicated room VPS, then run `python3 install.py` as root. It uses Ubuntu's
maintained `python3-aiohttp` package. Changes to relay code need a bridge restart;
the room directory and native relay do not need restarting.

Allowed origins currently include the production Pages URL and localhost port
8787. New custom domains need to be added to both `ORIGINS` and Nginx's
`teeming_origin` map. Do not turn this into an arbitrary host/port UDP proxy.
