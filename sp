#!/bin/bash

case "$1" in
    compile)
        mkdir -p build
        cd build
        cmake ..
        make
        ;;

    clean)
        rm -rf build
        ;;

    rebuild)
        rm -rf build
        mkdir build
        cd build
        cmake ..
        make
        ;;

    *)
        echo "Usage: ./sp {compile|clean|rebuild}"
        ;;
esac