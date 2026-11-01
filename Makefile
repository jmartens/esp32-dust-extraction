# Containerised ESP-IDF workflow (Linux/macOS). Windows: scripts/idf-docker.ps1.
# The IDF image tag is pinned in scripts/idf-version (single source of truth).
IDF_VERSION  := $(shell cat scripts/idf-version)
IDF_IMAGE    ?= espressif/idf:$(IDF_VERSION)
IDF_TARGET   ?= esp32
IDF_PORT     ?=
IDF_DEVICE   ?=
BUILD_VOLUME := open-extraction-idf-build-$(IDF_VERSION)
CCACHE_VOLUME := open-extraction-idf-ccache

# Run as the host user so files written to the bind mount are not root-owned.
HOST_UID := $(shell id -u)
HOST_GID := $(shell id -g)

DOCKER_RUN = docker run --rm $(if $(TTY),-it,) \
	-u $(HOST_UID):$(HOST_GID) -e HOME=/tmp -e GIT_CONFIG_COUNT=1 -e GIT_CONFIG_KEY_0=safe.directory -e GIT_CONFIG_VALUE_0='*' \
	-e IDF_CCACHE_ENABLE=1 -e CCACHE_DIR=/ccache \
	-v "$(CURDIR):/project" \
	-v $(BUILD_VOLUME):/build -v $(CCACHE_VOLUME):/ccache \
	$(if $(IDF_DEVICE),--device $(IDF_DEVICE),) \
	-w /project/firmware

.PHONY: help volumes build test clean shell menuconfig flash monitor

help:
	@echo "Targets: build test clean shell menuconfig flash monitor"
	@echo "Image: $(IDF_IMAGE)  (edit scripts/idf-version to change)"

# Named volumes are created root-owned; hand them to the host user once.
volumes:
	@for v in $(BUILD_VOLUME) $(CCACHE_VOLUME); do \
		docker volume create $$v >/dev/null; \
	done
	@docker run --rm -u 0 -v $(BUILD_VOLUME):/build -v $(CCACHE_VOLUME):/ccache \
		$(IDF_IMAGE) chown $(HOST_UID):$(HOST_GID) /build /ccache

build: volumes
	$(DOCKER_RUN) $(IDF_IMAGE) idf.py -B /build/$(IDF_TARGET) set-target $(IDF_TARGET)
	$(DOCKER_RUN) $(IDF_IMAGE) idf.py -B /build/$(IDF_TARGET) build

# Same script CI runs; BUILD_DIR keeps host build output in the volume.
test: volumes
	# PIP_TARGET: the IDF venv is root-owned, so non-root installs gcovr elsewhere.
	$(DOCKER_RUN) -e BUILD_DIR=/build/host-test -e PIP_TARGET=/tmp/pydeps -e PYTHONPATH=/tmp/pydeps -w /project/firmware/test \
		$(IDF_IMAGE) bash ./run_coverage.sh

clean:
	-docker volume rm $(BUILD_VOLUME)

shell: volumes
	$(MAKE) TTY=1 --no-print-directory _shell

menuconfig: volumes
	$(MAKE) TTY=1 --no-print-directory _menuconfig

flash: volumes
	@test -n "$(IDF_DEVICE)$(IDF_PORT)" || { echo "Set IDF_DEVICE=/dev/ttyUSB0 (Linux) or IDF_PORT=rfc2217://host.docker.internal:4000?ign_set_control"; exit 1; }
	$(DOCKER_RUN) $(IDF_IMAGE) idf.py -B /build/$(IDF_TARGET) -p $(or $(IDF_PORT),$(IDF_DEVICE)) flash

monitor: volumes
	$(MAKE) TTY=1 --no-print-directory _monitor

.PHONY: _shell _menuconfig _monitor
_shell:
	$(DOCKER_RUN) $(IDF_IMAGE) bash
_menuconfig:
	$(DOCKER_RUN) $(IDF_IMAGE) idf.py -B /build/$(IDF_TARGET) menuconfig
_monitor:
	$(DOCKER_RUN) $(IDF_IMAGE) idf.py -B /build/$(IDF_TARGET) -p $(or $(IDF_PORT),$(IDF_DEVICE)) monitor
