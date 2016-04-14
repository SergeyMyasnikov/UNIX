#!/bin/bash

echo -n "Введите x0: "
read x0
echo -n "Введите y0: "
read y0
a=1
b=2
if [ $(($x0*$a+$b)) == $y0 ] 
   then
     echo "Принадлежит"
   else
     echo "Не принадлежит"
fi
