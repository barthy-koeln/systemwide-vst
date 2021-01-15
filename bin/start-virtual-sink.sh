#!/usr/bin/env bash

NAME="SystemwideIO"

if grep -q "$NAME" <<<"$(pacmd list sink)"; then
  echo "$NAME exists"
  exit 0
fi

pacmd load-module module-null-sink sink_name="$NAME" sink_properties=device.description="$NAME"
INFO=$(pactl list short modules | grep sink_name="$NAME")
ID="${INFO:0:2}"

echo "created $NAME ($ID)"