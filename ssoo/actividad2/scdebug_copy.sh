#!/bin/bash

# sysinfo - Un script que informa del estado del sistema
prog=
argumentos_sto=
argumento1=
argumento2=
TITLE="Script 1"
FICHERO="./scdebug/$prog"
UUIDGEN=$(uuidgen)
nattch=
pattch=
vall_aceptado=
v_aceptado=
nattch_lista=()
pattch_lista=()
vall_lista=()
v_lista=()
args=""
kill_aceptado=
prog_solo=
prog_solo_aceptado=
##### Estilos
#PID=$(ps -u | sort -n -k10 -r | grep $prog$ |  tail -n1 | tr -s " " " " | cut -f2 -d" ")
#PID3=$(ps -u | tr -s " " " " | sort -n -k10 -r | grep $prog$ | head -n1 | cut -d " " -f2)
#PID1=$(ps -e -opid,comm | grep $prog | sort -n -r -k1 | head -n1)
#PID2=$(pidof -s $prog)
#PID4=$(ps -eo pid,comm | grep "$prog" | sort -n -r | head -n1 | awk '{print $1}')
#pid=$(ps -eo pid,fname | grep $prog | sort -n -r | head -n1 | tr -s " " " " | cut -d" " -f2)
TEXT_BOLD=$(tput bold)
TEXT_GREEN=$(tput setaf 2)
TEXT_RESET=$(tput sgr0)
TEXT_ULINE=$(tput sgr 0 1)
#Funciones
#error_exit
#{

#}
check_prog() {
  if ! command -v strace &> /dev/null; then
    echo "comando strace, no disponible en en el programa"
    exit 1
  fi
  if ! command -v uuidgen &> /dev/null; then
    echo " comando uuidigen, no disponible en en el programa"
    exit 1
  fi
}

launch_strace_p() { #$1 Pid $2 args_sto $3 Destination
  if [ -n $1 ]; then # Que tenga -p
    if [ -n $2 ]; then # Que tenga argumentos
      strace $2 -p $1 -o $3 2>&1 | tee -a $3
    else
      strace -c -p $1 -o $3 2>&1 | tee -a $3
    fi
  else
    if [ -n $2 ]; then # Que tenga argumentos
      strace $2 -o $3 $prog_solo $args 2>&1 | tee -a $3
    else
      strace -o $3 $prog_solo $args 2>&1 | tee -a $3
    fi
  fi
}


findpid() 
{
  PID4=$(ps -eo pid,comm | grep "$1" | sort -n -r | head -n1 | awk '{print $1}')
}

usage()
{
  echo "scdebug [-h] [-sto arg] [-v | -vall] [-k] [prog [arg …] ] [-nattch progtoattach …] [-pattch pid1 … ]"
}

repositorio_request()
{
if [ -e "./scdebug/" ]; then
  if [  -e "./scdebug/$1" ]; then
    echo ""
  else
    mkdir ./scdebug/$1
  fi
else
  mkdir ./scdebug/
  mkdir ./scdebug/$1
fi
UUIDGEN=$(uuidgen)
}

help1()
{
  echo " Este script requiere que le pongas argumentos del tipo myrpog argv1 argv1"
  echo " Además puedes añadir el -sto seguido de parametros que quieras usar para el strace"
  echo " Si añades -nattch seguido de [-sto arg] vas a hacer un attacth del programa"
}

process_name()
{
  proceso_nombre=$(ps -p $1 -o comm=)
}

func_kill()
{
 for pid in $(ps -u $USER -o pid); do
    if [ -e /proc/"$pid"/status ]; then
      PROCESO_TRAZADO=$(cat /proc/$pid/status | grep TracerPid: | awk '{print $2}' )
      if [ "$PROCESO_TRAZADO" != "0" ]; then
        kill $PROCESO_TRAZADO 2>/dev/null
        if [ "$?" != "0" ]; then
          echo " No pudo ser eliminado $PROCESO_TRAZADO"
        fi        
        kill $pid 2>/dev/null
        if [ "$?" != "0" ]; then
          echo " No pudo ser eliminado $pid"
        fi
      fi
    fi
  done
}

inicio2()
{
echo "----Estos programas estan siendo trazados----"
for pid in $(ps -u $USER -o pid); do
  if [ -e "/proc/$pid/status" ]; then
    tracer_pid=$(awk '/TracerPid/ {print $2}' /proc/$pid/status)
    if [ "$tracer_pid" -ne 0 ]; then
      tracer_name=$(ps -p $tracer_pid -o comm=)
      process_name $pid
      echo "Proceso Trazado - PID: $pid, Nombre: $proceso_nombre, Tracer PID: $tracer_pid, Tracer Nombre: $tracer_name"
    fi
  fi
done
}

check_prog
while [ -n "$1" ]; do
  case "$1" in
    -sto)
      shift
      argumentos_sto=$1
      shift
      ;;
    -h | --help)
      help1
      exit
    ;;
    -u)
      usage
      exit
    ;;
    -nattch)
      nattch='1'
      shift
      while [ -n "$1" ] && [[ $1 != -* ]]; do
        nattch_lista+=($1)
        shift
      done
      ;;
    -pattch)
      pattch="1"
      shift
      while [ -n "$1" ] && [[ $1 != -* ]]; do
        pattch_lista+=($1)
        shift
      done
      ;;
    -k)
      kill_aceptado="1"
      shift
    ;;
    -v)
      v_aceptado="1"
      shift
      while [ -n "$1" ]; do
        v_lista+=($1)
        shift
      done
    ;;
    -vall)
      vall_aceptado="1"
      shift
      while [ -n "$1" ]; do
        vall_lista+=($1)
        shift
      done
    ;;
    *)
      if [[ $1 != -* ]]; then
        prog_solo_aceptado="1"
        prog_solo=$1
        shift
        while [[ $1 != "-pattch" && $1 != "-nattch" ]] && [ -n "$1" ]; do
          #echo "aqui"
          args+=""$1
          shift
        done
      fi
    ;;
    esac
done

if [ -z "$v_aceptado" ] && [ -z "$vall_aceptado" ]; then
  inicio2
fi

if [ -n "$argumentos_sto" ] && [ -n $prog_solo ]; then
  #echo "a"
  repositorio_request $prog_solo
  strace $argumentos_sto -o ./scdebug/$prog_solo/trace_$(uuidgen).txt $prog_solo $args &
fi

if [ -z "$argumentos_sto" ] && [ "$prog_solo_aceptado" = "1" ] && [ -z "$v_aceptado" ] && [ -z "$vall_aceptado" ] && [ -z $kill_aceptado ]; then
  echo c
  repositorio_request $prog_solo
  strace -o ./scdebug/$prog_solo/trace_$(uuidgen).txt $prog_solo $args &
fi

if [ -n "$nattch" ]; then
  if [ -n "$argumentos_sto" ]; then
    for name in "${nattch_lista[@]}"; do
      echo "c"
      repositorio_request $name
      findpid $name
      strace $argumentos_sto -p $PID4 -o ./scdebug/$name/trace_$(uuidgen).txt&
    done
  else
    for name in "${nattch_lista[@]}"; do
      #echo "d"
      repositorio_request $name
      findpid $name
      strace -c -p $PID4 -o ./scdebug/$name/trace_$(uuidgen).txt&
    done
  fi
fi
 
if [ -n "$pattch" ]; then
  if [ -n "$argumentos_sto" ]; then
    for PID in "${pattch_lista[@]}"; do
      #echo "e"
      process_name $PID
      repositorio_request $proceso_nombre
      strace $argumentos_sto -p $PID -o ./scdebug/$proceso_nombre/trace_$(uuidgen).txt&
    done
  else
    for PID in "${pattch_lista[@]}"; do
      echo "r"
      process_name $PID
      repositorio_request $proceso_nombre
      strace -c -p $PID -o ./scdebug/$proceso_nombre/trace_$(uuidgen).txt&
    done
  fi
fi

if [ "$v_aceptado" = "1" ]; then
  for nombre in "${v_lista[@]}"; do
    ruta="./scdebug/$nombre"
    nombre_traza=$(ls -t ./scdebug/$nombre | head -n 1)
    fecha=$(stat -c %y ./scdebug/$nombre/$nombre_traza | awk '{print $1, $2}' | cut -d '.' -f 1)
    echo "=============== COMMAND: $nombre ======================="
    echo "=============== TRACE FILE: $nombre_traza ================="
    echo "=============== TIME: $fecha =============="
    echo " "
  done
fi

if [ "$vall_aceptado" = "1" ]; then
  for nombre in "${vall_lista[@]}"; do
    for file in $(ls -t "./scdebug/$nombre"); do
      ruta="./scdebug/$"
      fecha=$(stat -c %y ./scdebug/$nombre/$file | awk '{print $1, $2}' | cut -d '.' -f 1)
      echo "=============== COMMAND: $nombre ======================="
      echo "=============== TRACE FILE: $file ================="
      echo "=============== TIME: $fecha =============="
      echo " "
    done
  done
fi

if [ "$kill_aceptado" = "1" ]; then
  func_kill
fi