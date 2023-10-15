#!/bin/bash

# sysinfo - Un script que informa del estado del sistema
prog=
argumentos=
argumento1=
argumento2=
TITLE="Script 1"
FICHERO="./scdebug/$prog"
UUIDGEN=$(uuidgen)
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
  PID4=$(ps -eo pid,comm | grep "$prog" | sort -n -r | head -n1 | awk '{print $1}')
}

usage()
{
  echo scdebug [-h] [-sto arg] [-v | -vall] [-nattch progtoattach] [prog [arg1 …]]
}

repositorio_request()
{
if [ -e ./scdebug/ ]; then
  if [  -e ./scdebug/$prog ]; then
    echo $TEXT_BOLD"Directorio ya creado"$TEXT_RESET
  else
    mkdir ./scdebug/$prog
  fi
else
  mkdir ./scdebug/
  mkdir ./scdebug/$prog
fi
}
help1()
{
  echo " Este script requiere que le pongas argumentos del tipo myrpog argv1 argv1"
  echo " Además puedes añadir el -sto seguido de parametros que quieras usar para el strace"
  echo " Si añades -nattch seguido de [-sto arg] vas a hacer un attacth del programa"
}
# scdebug [-h] [-sto arg] [-v | -vall] [-nattch progtoattach] [prog [arg1 …]]
  case $1 in
    -sto)
      if [[ $3 != -nattch ]]; then
        argumentos=$2
        prog=$3
        repositorio_request
        strace $argumentos -o ./scdebug/$prog/trace_$UUIDGEN.txt $prog&
      else
        argumentos=$2
        prog=$4
        repositorio_request
        findpid
        strace -c -p $PID4 -o ./scdebug/$prog/trace_$UUIDGEN.txt
      fi
      ;;
    -h | --help)
      help1
      exit
    ;;
    -u)
      usage
    ;;
    *)
      prog=$1
      repositorio_request
      case $# in
        2)
          strace -o ./scdebug/$prog/trace_"$UUIDGEN".txt "$prog"
        ;;
        3)
          argumento1=$2
          strace -o ./scdebug/"$prog"/trace_$UUIDGEN.txt "$prog" "$argumento1"
          ;;
        4)
          argumento1=$2
          argumento2=$3
          strace -o ./scdebug/$prog/trace_$UUIDGEN.txt $prog $argumento1 $argumento2
        ;;
        esac

  esac