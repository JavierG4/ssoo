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
vall_aceptado=
v_aceptado=
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
findpid() 
{
  PID4=$(ps -eo pid,comm | grep "$1" | sort -n -r | head -n1 | awk '{print $1}')
}

usage()
{
  echo scdebug [-h] [-sto arg] [-v | -vall] [-nattch progtoattach] [prog [arg1 …]]
}

repositorio_request()
{
if [ -e ./scdebug/ ]; then
  if [  -e ./scdebug/$1 ]; then
    echo $TEXT_BOLD"Directorio ya creado"$TEXT_RESET
  else
    mkdir ./scdebug/$1
  fi
else
  mkdir ./scdebug/
  mkdir ./scdebug/$1
fi
}
help1()
{
  echo " Este script requiere que le pongas argumentos del tipo myrpog argv1 argv1"
  echo " Además puedes añadir el -sto seguido de parametros que quieras usar para el strace"
  echo " Si añades -nattch seguido de [-sto arg] vas a hacer un attacth del programa"
}
while [ -n "$1" ]; do
  case "$1" in
    -sto)
      shift
      argumentos_sto= $1
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
      nattch="1"
      shift 
      prog= $1      
      shift
      ;;
    -v)
      v_aceptado="1"
      shift
      prog=$1
    ;;
    -vall)
      vall_aceptado="1"
      shift
      prog=$1
    ;;
    *)
      if [ -z "$prog" ] then
        prog= $1
        shift
      fi
      else
        usage
        exit 1
    ;;
    esac
done

if [ -n "$argumentos_sto" ]; then
  repositorio_request $prog
  strace $argumentos_sto -o ./scdebug/$prog/trace_$UUIDGEN.txt $prog
fi

if [ -n "$nattch" ]; then
  repositorio_request
  findpid $prog
  strace -c -p $PID4 -o ./scdebug/$prog/trace_$UUIDGEN.txt
fi

if [ -n "$v_aceptado" ]; then
  latest_file=$(ls -t "./scdebug/$prog/trace_$UUIDGEN.txt"/*.log | head -1)
  if [ -n "$latest_file" ]; then
    echo "Contenido del archivo de depuración más reciente: $latest_file"
    cat "$latest_file"
  else
    echo "No se encontraron archivos de depuración en el directorio."
  fi
fi

if [ -n "$vall_aceptado" ]; then
  for file in $(ls -t "./scdebug/$prog/trace_$UUIDGEN.txt"/*.log); do
    echo "Contenido del archivo de : $debug_file"
    cat $debug_file
    echo "==============================\n"
  done
fi