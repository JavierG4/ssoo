#!/bin/bash

# sysinfo - Un script que informa del estado del sistema

TITLE="Información del sistema para $HOSTNAME"
RIGHT_NOW=$(date +"%x %r%Z")
TIME_STAMP="Actualizada el $RIGHT_NOW por $USER"

##### Estilos

TEXT_BOLD=$(tput bold)
TEXT_GREEN=$(tput setaf 2)
TEXT_RESET=$(tput sgr0)
TEXT_ULINE=$(tput sgr 0 1)
##### Funciones

system_info()
{
  echo "${TEXT_ULINE}Versión del sistema${TEXT_RESET}"
  echo
  uname -a

}


show_uptime()
{
  echo "$TEXT_ULINE Tiempo de encendido del sistema$TEXT_RESET"
  echo
  uptime
}


drive_space()
{
  echo "${TEXT_ULINE}El espacio ocupado en las particiones / discos duros del sistema.${TEXT_RESET}"
  echo
  df
}


home_space()
{
  if [ "$USER" != root ]; then
    echo 
    du -hs ~

  else
    echo
    du -hs /home/*
  fi
}

##### Programa principal

cat << _EOF_
$TEXT_BOLD$TITLE$TEXT_RESET

$TEXT_GREEN$TIME_STAMP$TEXT_RESET
_EOF_

system_info
show_uptime
drive_space
home_space