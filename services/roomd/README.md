# Gauche room service

Builds the pinned Gubsy room directory, NAT rendezvous and UDP relay as one
headless binary. No SDL, game assets, GPU or running Gauche simulation required.
Gauche adapts the same protocols in `src/net/rooms` and `src/net/traversal.cpp`.

## Build

```sh
sudo apt update
sudo apt install -y build-essential cmake git ca-certificates
cmake -S services/roomd -B build-roomd -DCMAKE_BUILD_TYPE=Release
cmake --build build-roomd --parallel 2
./build-roomd/gubsy-roomd --help
```

CMake requires 3.24+. Source is fetched at the same Gubsy revision as the game.
For local development, pass `-DGAUCHE_GUBSY_SOURCE_DIR=/absolute/path/to/gubsy`.

## Deployment shape

Start with one Linux IPv4 VPS near a useful route between the players. For the
Fukuoka/Houston group, Los Angeles is an initial candidate, not a measured winner.
Vultr's API on 2026-09-15 lists `vc2-1c-2gb` at $10/month: one shared vCPU,
2 GB RAM, 55 GB disk and 2,048 GB transfer. This is an initial small-group size,
not a benchmarked capacity guarantee. Compile with at most two workers there.

Roles and ports:

| Role | Transport | Port |
| --- | --- | --- |
| Room directory | HTTP behind the public TLS endpoint | TCP 8788 |
| NAT rendezvous | UDP | 8789 |
| Relay fallback | UDP | 8790 |
| Public room API | HTTPS reverse proxy | TCP 443 |

Gauche uses libcurl with certificate verification for the HTTPS room API.
The upstream Gubsy room HTTP client is not used here. HTTP carries room credentials;
keep local development on loopback. UDP punching and relay already authenticate
their protocol packets; the game does not need to open inbound router ports.

For a local service process:

```sh
./build-roomd/gubsy-roomd --host=127.0.0.1 --port=8788 \
  --punch-port=8789 --relay-port=8790 --relay
```

Public deployment needs a service account, systemd restart/log policy, firewall
rules, TLS endpoint and correct advertised public UDP address. Upstream currently
advertises its bind address in `/health`; do not advertise `0.0.0.0` as a usable
remote destination. The deployed service binds its public IPv4 so both UDP addresses are advertised correctly.

The service coordinates direct connections first. If punching succeeds it is
outside the game packet path. If punching fails, the same server relays opaque
datagrams. Relay traffic and snapshot/rejoin bursts determine transfer usage.
Retain limits on allocations, packet sizes, rate and idle expiry. Do not put
API keys, SSH private keys or room authorization secrets in this repository.

## Active playtesting deployment

- URL: **https://45.77.123.14** (Vultr Los Angeles, `gauche-roomd-lax`).
- Ubuntu 24.04; plan `vc2-1c-2gb`, $10/month at creation on 2026-09-15.
- `/health` and `/rooms` are reachable over verified HTTPS. `/debug/*` is not
  proxied; TCP 8788 is blocked externally. UDP 8789/8790 serve punch/relay.
- `gauche-roomd.service` runs as an unprivileged account with automatic restart.
- Nginx terminates TLS. Certbot 5.8 obtained a short-lived IP certificate;
  `gauche-certbot.timer` checks renewal twice daily and reloads Nginx on renewal.
- SSH uses the dedicated local `~/.ssh/gauche_roomd_ed25519` key. The provisioning
  manifest is `~/.config/gauche/roomd-vps.json`; the Vultr API key is stored beside
  it in `vultr-api-key` with permissions 0600. Neither belongs in Git or the game.

To reproduce on a **fresh dedicated Ubuntu VPS**, copy the compiled `gubsy-roomd`
plus these scripts and the unit into one directory on that server, then run:

```sh
sudo bash install-server.sh PUBLIC_IPV4
sudo bash enable-https.sh PUBLIC_IPV4
# Optional owned domain with an A record already pointing to the server:
# sudo bash enable-https.sh PUBLIC_IPV4 rooms.example.com
```

The installer configures the fresh server's firewall and Nginx default site.
It is not intended for a shared machine with unrelated services. For an update,
replace `/opt/gauche-roomd/gubsy-roomd` and restart `gauche-roomd`; do not rerun the
bootstrap scripts merely to update the binary. Service state is in memory, so a
restart removes room registrations and relay allocations.

Validation: public HTTPS health/discovery, debug-route denial, private backend
port denial, and a bounded native Gauche forced-relay join/snapshot/ready/start
check succeeded. This does not constitute an intercontinental gameplay test.
