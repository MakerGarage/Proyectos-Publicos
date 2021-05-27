import random
import os

DESORDENAR = 0

#Creo 3 arrays vacios
lista1 = []
lista2 = []
lista3 = []

#Relleno las 3 listas
for i in range (1,334):
    lista1.append(i)
for i in range (334,667):
    lista2.append(i)
for i in range (667,1000):
    lista3.append(i)

if DESORDENAR == 1 :
    #Desordeno las listas
    random.shuffle(lista1)
    random.shuffle(lista2)
    random.shuffle(lista3)

#Modifico los numeros
for i in range (1,112):
    Change = open("Papeleta.svg", "rt")
    data = Change.read()

    data = data.replace('##1', str(lista1[0]).zfill(3) )
    data = data.replace('##2', str(lista2[0]).zfill(3) )
    data = data.replace('##3', str(lista3[0]).zfill(3) )

    lista1.pop(0)
    lista2.pop(0)
    lista3.pop(0)

    data = data.replace('##4', str(lista1[0]).zfill(3) )
    data = data.replace('##5', str(lista2[0]).zfill(3) )
    data = data.replace('##6', str(lista3[0]).zfill(3) )

    lista1.pop(0)
    lista2.pop(0)
    lista3.pop(0)

    data = data.replace('##7', str(lista1[0]).zfill(3) )
    data = data.replace('##8', str(lista2[0]).zfill(3) )
    data = data.replace('##9', str(lista3[0]).zfill(3) )

    lista1.pop(0)
    lista2.pop(0)
    lista3.pop(0)

    Change.close()
    Change = open("SVG/"+str(i).zfill(3)+".svg", "wt")
    Change.write(data)
    Change.close()

    #Creamos el pdf
    os.system("inkscape SVG/"+str(i).zfill(3)+".svg  --export-pdf=PDF/"+str(i).zfill(3)+".pdf")

