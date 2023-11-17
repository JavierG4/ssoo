#variables
ARGS_STO=
NATTCH=
UUID=$(uuidgen)
PROG=
PID=
PROG_ARGV1=
PROG_ARGV2=
KILL=
PROCESO_TRAZADO=
pid= 
PROG_NATTCH=
PATTCH=
PROG_PATTCH=
nombre_proceso=

PID_INFO() {
  echo "---------------------------------------------------------------------------------------------";
  echo "      ESTOS SON LOS PID DE LOS PROGRAMAS QUE ESTAN SIENDO TRACEADOS";
  echo "---------------------------------------------------------------------------------------------";
  for pid in $(ps -u $USER -o pid); do
    if [ -e /proc/"$pid"/status ]; then
      PROCESO_TRAZADO=$(cat /proc/"$pid"/status | grep TracerPid: | awk '{print $2}')
      if [ "$PROCESO_TRAZADO" != "0" ]; then
        ps -o pid,comm --no-header $PROCESO_TRAZADO | tr -s ' ' ' '
        echo " "
      fi
    fi
  done
}

KILL() {
  for pid in $(ps -u $USER -o pid); do
    if [ -e /proc/"$pid"/status ]; then
      PROCESO_TRAZADO=$(cat 2> /dev/null /proc/$pid/status | grep TracerPid: | awk '{print $2}' )
      if [ "$PROCESO_TRAZADO" != "0" ]; then
        kill $PROCESO_TRAZADO 2>/dev/null
        kill $pid 2>/dev/null  
      fi
    fi
  done
}

pattach() {
  for pid in "${PROG_PATTCH[@]}"; do
    nombre_proceso=$(ps -p $pid -o comm=)
    strace -c -p $pid -o ./scdebug/$nombre_proceso/trace_$.txt &
  done
}


while [ -n "$1" ]; do
  case $1 in
    -sto )
      shift
      ARGS_STO=$1
      ;;
    -nattch )
      NATTCH="1"
      shift
      while [ -n "$1" ] && [[ $1 != -* ]]; do
        PROG_NATTCH+=($1)
        shift
      done
      ;;
    -pattch )
      PATTCH="1"
      shift
      while [ -n "$1" ] && [[ $1 != -* ]]; do
        PROG_PATTCH+=($1)
      done
      ;;
    -k )
      KILL
      ;;
    -h ) 
      usage
      exit
      ;;
    * )
      PROG=$1
      if [ -e "./scdebug/" ]; then
        if [ ! -e "./scdebug/$PROG/" ]; then
          mkdir "./scdebug/$PROG/"
        fi
      else 
        mkdir "./scdebug/"
        mkdir "./scdebug/$PROG/"
      fi
      PID=$(ps -eo pid,comm | grep "$PROG" | sort -n -r | head -n1 | awk '{print $1}')
      if [ "$NATTCH" = "1" ] ; then
        if [ -n "$ARGS_STO" ]; then
          for name in "${PROG_NATTCH[@]}"; do
            PID=$(ps -eo pid,comm | grep "$name" | sort -n -r | head -n1 | awk '{print $1}')
            if [ -e "./scdebug/" ]; then
              if [ ! -e "./scdebug/$name/" ]; then
                mkdir "./scdebug/$name/"
              fi
            fi
            strace -c -p $PID -o ./scdebug/$name/trace_$(uuidgen).txt &
          done
        else
          for name in "${PROG_NATTCH[@]}"; do
            PID=$(ps -e -o pid,comm | grep "$name" | sort -n -r | head -n1 | awk '{print $1}')
            if [ -e "./scdebug/" ]; then
              if [ ! -e "./scdebug/$name/" ]; then
                mkdir "./scdebug/$name/"
              fi
            fi
            strace -c -p $PID -o ./scdebug/$name/trace_$(uuidgen).txt&
          done
        fi
      else
        strace $ARGS_STO -o "./scdebug/$PROG/trace_$UUID.txt" $PROG &
      fi
      ;;
    esac
    shift
  done
  if [ -n $PATTCH != "" ];then 
    pattach
  fi
  PID_INFO
  exit