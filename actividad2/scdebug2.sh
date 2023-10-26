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
nattch_procs=()
pattch_procs=()
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
  echo "scdebug [-h] [-sto arg] [-v | -vall] [-nattch progtoattach] [prog [arg1 …]]"
}

repositorio_request()
{
if [ -e "./scdebug/" ]; then
  if [  -e "./scdebug/$1" ]; then
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

func_kill()
{

}

inicio()
{
current_user=$(whoami)
# Usar `ps` para obtener una lista de procesos del usuario actual
# y guardar la salida en un archivo temporal
ps -eo pid,comm,user --sort=start_time > process_list.tmp
# Iterar a través de la lista de procesos
while read -r pid process_name user; do
  # Verificar si el proceso es del usuario actual
  if [ "$user" == "$current_user" ]; then
    # Comprobar si el proceso está siendo trazado
    tracer_pid=$(awk '/TracerPid/ {print $2}' /proc/$pid/status)
    if [ "$tracer_pid" -ne 0 ]; then
      # Obtener el nombre del proceso trazador
      tracer_name=$(ps -p $tracer_pid -o comm=)
      # Imprimir información del proceso trazado y trazador
      echo "Proceso Trazado - PID: $pid, Nombre: $process_name, Tracer PID: $tracer_pid, Tracer Nombre: $tracer_name"
    fi
  fi
done < process_list.tmp

# Eliminar el archivo temporal
rm process_list.tmp
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
      shift
      while [ -n "$1" ]; do
        if [[ $1 != -* ]]; then
          findpid $1
          nattch_procs+=($PID4)
          shift
        fi
      done
      ;;
      -pattch)
        while [ -n "$1" ]; do
          pattch_procs+=($PID)
        done
      ;;
    -k)
      func_kill
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
  latest_file=$(ls -t "./scdebug/$prog"| head -1)
  if [ -n "$latest_file" ]; then
    echo "Contenido del archivo de depuración más reciente: $latest_file"
    cat "$latest_file"
  else
    echo "No se encontraron archivos de depuración en el directorio."
  fi
fi

if [ -n "$vall_aceptado" ]; then
  for file in $(ls -t "./scdebug/$prog"); do
    echo "Contenido del archivo de : $debug_file"
    cat $debug_file
    echo "=============================="
  done
fi