#!/bin/bash

echo -n "Введите начальное количество палочек: "
read N
while [ $N -gt 0 ]
do
 echo -n "Тяните палочки от 1 до 4: " 
 read K
 if [ $K -ge 1 -a $K -le 4 ]
   then
     if [ $K -le $N ]
       then
         N=$(($N-$K))
         echo "Осталось "$N" палочек"
         if [ $N == 0 ] 
           then
             echo "Палочки кончились, Вы проиграли"
           else
             L=4
             if [ $L -gt $N ]
                then
                  L=$((N))
             fi
             M=$(( ( RANDOM % L )  + 1 ))
             N=$(($N-$M))
             echo "Я вытянул "$M" палочек"
             echo "Осталось "$N" палочек"
             if [ $N == 0 ]
             	then
             		echo "Я проиграл"
             fi
          fi
       else
         echo "Осталось только "$N" палочек"
     fi
   else
     echo "Введенное число не сдержится в диапазоне от 1 до 4"
 fi
done
