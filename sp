#!/bin/bash

ROOT="$(cd "$(dirname "$0")" && pwd)"
BUILD="$ROOT/build"
PYTHON="$ROOT/.venv/bin/python"
PID_DIR="$ROOT/.smartproxy"

case "$1" in

    compile)
        mkdir -p "$BUILD"
        cd "$BUILD"
        cmake ..
        make
        ;;

    run)
        mkdir -p "$PID_DIR"

        "$PYTHON" "$ROOT/backends/mock_server.py" 1 > "$PID_DIR/backend1.log" 2>&1 &
        echo $! > "$PID_DIR/backend1.pid"

        "$PYTHON" "$ROOT/backends/mock_server.py" 2 > "$PID_DIR/backend2.log" 2>&1 &
        echo $! > "$PID_DIR/backend2.pid"

        "$PYTHON" "$ROOT/backends/mock_server.py" 3 > "$PID_DIR/backend3.log" 2>&1 &
        echo $! > "$PID_DIR/backend3.pid"

        "$BUILD/smartproxy" > "$PID_DIR/smartproxy.log" 2>&1 &
        echo $! > "$PID_DIR/smartproxy.pid"

        echo "smartproxy is running."
        echo "Backends: 5001, 5002, 5003"
        echo "Proxy:    8080"
        ;;

    stop)
        if [ -d "$PID_DIR" ]; then
            for pid_file in "$PID_DIR"/*.pid; do
                if [ -f "$pid_file" ]; then
                    kill "$(cat "$pid_file")" 2>/dev/null
                    rm "$pid_file"
                fi
            done
        fi

        echo "smartproxy stopped."
        ;;

    *)
        echo "Usage: ./sp {compile|run|stop}"
        ;;

esac