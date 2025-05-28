# ggrd

real-time face recognizer and streamer in cpp ⚡

## Build

### streamer

To build streamer, you'll need required tools:

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install cmake gcc libopencv-dev libspdlog-dev libjpeg-dev zlib1g zlib1g-dev
# Nix/NixOS
nix develop
```

In project directory:

```bash
mkdir -p streamer/build
cd streamer/build
cmake ..
make
```

Internet connection is needed for `cmake ..` to fetch source code.

### preserver

You'll need `go` to build and run preserver, after that:

In project directory:

```bash
cd preserver
go mod tidy
go run ggrd-preserver
```
