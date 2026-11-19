#!/usr/bin/env bash
# Запуск ostis-example-app только с C++-агентами из этого проекта.
#
# Отличие от ostis-example-app/scripts/start.sh machine:
#   - не загружается scp-machine, поэтому SCP-агенты не выполняются
#     и не конфликтуют с C++-агентами за один и тот же класс действия;
#   - загружается build/Release/extensions этого проекта.
#
# Использование:
#   ./run-cpp-agent.sh              # запустить машину с C++-агентами
#   ./run-cpp-agent.sh --build      # сначала пересобрать модуль
#   ./run-cpp-agent.sh --kb         # сначала пересобрать базу знаний
#   ./run-cpp-agent.sh --no-web     # не запускать веб-интерфейс
#   ./run-cpp-agent.sh --check      # после старта прогнать оба действия
#                                   # и напечатать результат в терминал
#
# Обычный запуск (SCP-реализация) остаётся как раньше:
#   cd /home/maksim/Documents/labs/tools/ostis-example-app
#   ./scripts/start.sh machine
#   ./scripts/start.sh web

set -euo pipefail

PROBLEM_SOLVER_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
APP_DIR="${OSTIS_APP_DIR:-/home/maksim/Documents/labs/tools/ostis-example-app}"
MODULE_DIR="$PROBLEM_SOLVER_DIR/build/Release/extensions"
MODULE_SO="$MODULE_DIR/libbank-processing-module.so"

BUILD_MODULE=0
BUILD_KB=0
START_WEB=1
RUN_CHECK=0
WEB_PID=""
MACHINE_PID=""

usage() {
    sed -n '2,20p' "${BASH_SOURCE[0]}" | sed 's/^# \{0,1\}//'
}

for arg in "$@"; do
    case "$arg" in
        --build) BUILD_MODULE=1 ;;
        --kb) BUILD_KB=1 ;;
        --no-web) START_WEB=0 ;;
        --check) RUN_CHECK=1 ;;
        -h|--help) usage; exit 0 ;;
        *) echo "Неизвестный аргумент: $arg"; usage; exit 1 ;;
    esac
done

cleanup() {
    if [[ -n "$MACHINE_PID" ]]; then
        kill "$MACHINE_PID" 2>/dev/null || true
        wait "$MACHINE_PID" 2>/dev/null || true
    fi
    if [[ -n "$WEB_PID" ]]; then
        kill "$WEB_PID" 2>/dev/null || true
        wait "$WEB_PID" 2>/dev/null || true
    fi
}
trap cleanup EXIT
trap 'exit 130' INT TERM

port_busy() {
    local port="$1"
    if command -v ss >/dev/null 2>&1; then
        ss -ltn 2>/dev/null | grep -qE "[:.]${port}[[:space:]]"
    else
        (exec 3<>"/dev/tcp/127.0.0.1/${port}") 2>/dev/null
    fi
}

if [[ ! -d "$APP_DIR" ]]; then
    echo "Не найден ostis-example-app: $APP_DIR"
    echo "Укажи путь через OSTIS_APP_DIR=/путь/к/ostis-example-app"
    exit 1
fi

if port_busy 8090; then
    echo "Порт 8090 уже занят — скорее всего, уже запущена другая sc-машина."
    echo "Останови её (Ctrl+C в терминале с ./scripts/start.sh machine) и запусти скрипт снова."
    exit 1
fi

if [[ "$BUILD_MODULE" -eq 1 || ! -f "$MODULE_SO" ]]; then
    echo "== Сборка C++-модуля =="
    cd "$PROBLEM_SOLVER_DIR"
    if [[ ! -d build/Release ]]; then
        conan install . --build=missing
    fi
    cmake --preset conan-release
    cmake --build --preset conan-release
    echo
fi

if [[ "$BUILD_KB" -eq 1 ]]; then
    echo "== Пересборка базы знаний =="
    cd "$APP_DIR"
    ./scripts/start.sh build_kb
    echo
fi

if [[ "$START_WEB" -eq 1 && -d "$APP_DIR/interface/sc-web/.venv" ]]; then
    if port_busy 8080; then
        echo "Веб-интерфейс уже запущен на порту 8080, второй раз не запускаю."
    else
        echo "== Запуск веб-интерфейса =="
        (
            cd "$APP_DIR/interface/sc-web"
            exec .venv/bin/python server/app.py
        ) > "$PROBLEM_SOLVER_DIR/build/web.log" 2>&1 &
        WEB_PID=$!
        echo "Веб-интерфейс: http://localhost:8080 (лог: $PROBLEM_SOLVER_DIR/build/web.log)"
    fi
fi

export LD_LIBRARY_PATH="$APP_DIR/install/sc-machine/lib:$APP_DIR/install/scl-machine/lib:${LD_LIBRARY_PATH:-}"

echo
echo "== Запуск sc-машины с C++-агентами (без scp-machine) =="
echo "Расширения:"
echo "  $APP_DIR/install/sc-machine/lib/extensions"
echo "  $APP_DIR/install/scl-machine/lib/extensions"
echo "  $MODULE_DIR"
echo
echo "Ждём в логе: Initialize BankProcessingModule и Subscribe agent ..."
echo "Остановка: Ctrl+C"
echo

cd "$APP_DIR"
"$APP_DIR/install/sc-machine/bin/sc-machine" \
    -s kb.bin \
    -e "$APP_DIR/install/sc-machine/lib/extensions;$APP_DIR/install/scl-machine/lib/extensions;$MODULE_DIR" \
    -c ostis-example-app.ini &
MACHINE_PID=$!

if [[ "$RUN_CHECK" -eq 1 ]]; then
    for _ in $(seq 1 300); do
        port_busy 8090 && break
        sleep 0.1
    done
    echo
    echo "== Проверка агентов =="
    "$APP_DIR/interface/sc-web/.venv/bin/python" "$PROBLEM_SOLVER_DIR/tools/check_agents.py" || true
    echo
fi

wait "$MACHINE_PID" || true
