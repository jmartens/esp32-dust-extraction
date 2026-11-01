# Build system and containerised ESP-IDF workflow

The firmware is an ESP-IDF project in `firmware/`; Linux host unit tests live in `firmware/test`.
All builds and host tests can run in the official `espressif/idf` Docker image, so no local ESP-IDF
install is needed and every developer (and CI) uses the same toolchain.

## Commands

| Task | Windows (PowerShell) | Linux / macOS |
| --- | --- | --- |
| Build firmware (esp32) | `./scripts/idf-docker.ps1 build` | `make build` |
| Linux host tests + coverage | `./scripts/idf-docker.ps1 test` | `make test` |
| Remove build volume | `./scripts/idf-docker.ps1 clean` | `make clean` |
| Shell in the container | `./scripts/idf-docker.ps1 shell` | `make shell` |
| menuconfig | `./scripts/idf-docker.ps1 menuconfig` | `make menuconfig` |
| Flash / monitor | see [firmware/FLASHING.md](../../firmware/FLASHING.md) | see FLASHING.md |

Run commands from the repository root. Only Docker is required. `test` runs
`firmware/test/run_coverage.sh` (the script CI runs) and writes reports to `firmware/test/coverage/`.

## Design

* **Pinned image**: the tag lives only in `scripts/idf-version` (currently `v6.1`, matching
  `esp_idf_version` in `.github/workflows/build.yml`). Both wrappers read it. To switch versions,
  edit that file (and the workflow) in one change (Renovate does this: custom regex managers in `renovate.json` track both as `espressif/idf` in one grouped PR; minor/major bumps need approval on the dependency dashboard); `IDF_IMAGE` overrides the full image name.
* **Working directory**: the project root is mounted at `/project` and commands run in
  `/project/firmware` (or `/project/firmware/test`), because the root is not an IDF project.
* **Container-local build output**: builds use `-B /build/<target>` on a named volume
  (`open-extraction-idf-build-<version>`). The host `build/` directory is never used, so a stale
  host-configured `build/` (with Windows paths in its CMake cache) cannot break container builds.
  `clean` deletes the volume.
* **ccache**: the volume `open-extraction-idf-ccache` is mounted at `/ccache` with
  `IDF_CCACHE_ENABLE=1` and `CCACHE_DIR=/ccache`, so it persists between runs (as CI does).
* **Host user** (Makefile): runs with `-u $(id -u):$(id -g)`, `HOME=/tmp` and git
  `safe.directory=*` (via `GIT_CONFIG_*`), so files written to the checkout are not root-owned.
  Named volumes are chowned to that user once per run. The PowerShell wrapper runs as the image
  default user (Docker Desktop maps ownership).
* `firmware/test/run_coverage.sh` honours `BUILD_DIR` (default `build`) so the same script works in
  CI and in the container volume.
* Generated `sdkconfig*`, `dependencies.lock` and `managed_components/` are git-ignored.

## Version updates (Renovate)

* Patch updates of the IDF image open PRs automatically; minor and major updates wait for approval on the Renovate dependency dashboard.
* Rationale: IDF minors can change deprecations, Kconfig/sdkconfig, component APIs and managed-component compatibility, and the build uses `-Werror`, so a toolchain change should be deliberate. CI (build and host tests) still validates every update PR, but does not cover hardware behaviour.
* Both `scripts/idf-version` and `esp_idf_version` in the build workflow are tracked as `espressif/idf` and grouped into one PR so they stay in sync.

## Troubleshooting

* **Stale host `build/`**: running `idf.py` on the host and then `docker run ... idf.py build` against
  the same `build/` fails with host paths in the CMake cache (for example
  `cannot read spec file 'E:/.../build/specs/picolibc.specs'`). Use the wrappers (volume-backed
  build) or delete `build/`.
* **`Cannot allocate memory` during compile**: Docker Desktop has too little memory (observed with
  about 2 GB on a 4-CPU VM). Retry (ccache keeps progress) or raise the memory limit.
* **Git worktrees**: the wrappers work in worktrees, but git inside the container cannot see a
  worktree's `.git` file (it points at a host path); this only produces warnings.

## CI decision

CI keeps `espressif/esp-idf-ci-action`. It already runs the same `espressif/idf:v6.1` image with the
same ccache settings and `run_coverage.sh`, adds GitHub cache integration, and replacing it with the
wrapper would not simplify the workflow. Local and CI builds therefore share image, tag, and test
script; keep the workflow tag in sync with `scripts/idf-version`.

## Docker Sandboxes (`sbx`) for AI agents

Not evaluated hands-on. Docker Sandboxes can isolate AI coding agents using the same image; it is
optional and nothing in this project depends on it.
