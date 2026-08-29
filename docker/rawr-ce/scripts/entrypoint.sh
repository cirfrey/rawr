#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="/build/${COMPILER_ID}"
MODULE_DIR="/usr/local/lib/rawr/modules/${COMPILER_ID}"
NATIVE_FILE="/etc/rawr/native.ini"
IFACE_PATH="/workspace/${RAWR_MODULE_IFACE:-src/rawr.cppm}"

log() { printf '[%s] %s\n' "${COMPILER_ID}" "$*"; }

# ── meson setup ──────────────────────────────────────────────────────────────
# --reconfigure is a no-op if nothing relevant changed, so it's safe to always
# run. Named volume means /build/${COMPILER_ID} persists across restarts.
log "meson setup..."
meson setup "${BUILD_DIR}" /workspace \
    --native-file "${NATIVE_FILE}" \
    --buildtype debug \
    --reconfigure \
    2>&1 | sed "s/^/[${COMPILER_ID}] /" || {
    log "meson setup FAILED — check /workspace/meson.build"
    exit 1
}

# ── build function ────────────────────────────────────────────────────────────
build_and_install() {
    log "compiling..."
    if meson compile -C "${BUILD_DIR}" 2>&1 | sed "s/^/[${COMPILER_ID}] /"; then
        /install-bmis.sh
        log "ready"
    else
        log "FAILED — waiting for next save"
    fi
}

# ── initial build ─────────────────────────────────────────────────────────────
build_and_install

# ── watcher ───────────────────────────────────────────────────────────────────
log "watching /workspace..."

POLL_FLAG=""
if [ "${WATCH_POLL:-0}" = "1" ]; then
    POLL_FLAG="--poll 500"
    log "note: polling mode (WATCH_POLL=1) — 500ms reaction time"
fi

exec watchexec \
    --watch /workspace \
    --exts cppm,hpp,pp,inc,h \
    --on-busy-update restart \
    ${POLL_FLAG} \
    -- bash -c "
        meson compile -C '${BUILD_DIR}' 2>&1 | sed 's/^/[${COMPILER_ID}] /' \
        && /install-bmis.sh \
        && echo '[${COMPILER_ID}] ready' \
        || echo '[${COMPILER_ID}] FAILED'
    "
