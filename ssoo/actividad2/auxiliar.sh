#!/bin/bash

# Función para matar procesos
kill_processes() {
  local signal=$1
  local user=$2

  # Obtener una lista de procesos del usuario
  pids=$(ps -U "$user" -o pid=)

  # Matar los procesos con la señal especificada
  for pid in $pids; do
    if [ "$pid" -ne $$ ]; then
      kill -$signal "$pid"
    fi
  done
}

# Verificar si se proporcionó la opción -k
if [ "$1" == "-k" ]; then
  # Obtener el nombre de usuario actual
  current_user=$(whoami)

  # Matar los procesos trazadores (tracer) del usuario con la señal KILL
  kill_processes 9 "$current_user"

  # Esperar un momento para permitir que los procesos trazadores terminen
  sleep 2

  # Matar todos los procesos trazados (tracee) del usuario con la señal KILL
  kill_processes 9 "$current_user"
fi