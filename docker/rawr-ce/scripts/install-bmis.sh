#!/usr/bin/env bash
set -euo pipefail

COMPILER_ID="${COMPILER_ID:?}"
COMPILER_FAMILY="${COMPILER_FAMILY:?}"
CXX="${CXX:?}"
MODULE_DIR="/usr/local/lib/rawr/modules/${COMPILER_ID}"
MANIFEST="/workspace/modules.manifest"

if [ ! -f "${MANIFEST}" ]; then
    echo "[${COMPILER_ID}] install-bmis: modules.manifest not found, skipping"
    exit 0
fi

mkdir -p "${MODULE_DIR}"

case "${COMPILER_FAMILY}" in
    gcc)
        # Clear mapper; will be rebuilt entry by entry.
        > "${MODULE_DIR}/rawr.map"

        while IFS=' ' read -r iface_rel module_name || [ -n "${iface_rel}" ]; do
            # Skip blank lines and comments.
            [[ "${iface_rel}" =~ ^#|^[[:space:]]*$ ]] && continue

            echo "[${COMPILER_ID}] BMI: ${module_name}"
            ${CXX} -std=c++20 ${CXXFLAGS:-} \
                -fmodule-only \
                -fmodules-cache-path="${MODULE_DIR}" \
                "/workspace/${iface_rel}"
            # GCC derives the .gcm name from the module name declared inside
            # the file, not from the filename. It writes to cache-path directly.
            # Add this module to the mapper for external consumers.
            echo "${module_name} ${MODULE_DIR}/${module_name}.gcm" \
                >> "${MODULE_DIR}/rawr.map"
        done < "${MANIFEST}"

        echo "[${COMPILER_ID}] GCC mapper: ${MODULE_DIR}/rawr.map"
        echo "[${COMPILER_ID}] Consume with: --module-mapper=${MODULE_DIR}/rawr.map"
        ;;

    clang)
        while IFS=' ' read -r iface_rel module_name || [ -n "${iface_rel}" ]; do
            [[ "${iface_rel}" =~ ^#|^[[:space:]]*$ ]] && continue

            echo "[${COMPILER_ID}] BMI: ${module_name}"
            ${CXX} -std=c++20 \
                --precompile \
                -fprebuilt-module-path="${MODULE_DIR}" \
                "/workspace/${iface_rel}" \
                -o "${MODULE_DIR}/${module_name}.pcm"
            # -fprebuilt-module-path is passed to each step so that when
            # compiling rawr.abi.cppm, Clang can find rawr.abi.sysv.pcm
            # already in the dir.
        done < "${MANIFEST}"

        echo "[${COMPILER_ID}] Clang BMIs: ${MODULE_DIR}/"
        echo "[${COMPILER_ID}] Consume with: -fprebuilt-module-path=${MODULE_DIR}"
        ;;
esac
