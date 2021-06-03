from datetime import date
from datetime import datetime

import json
import requests #pip install requests

import matplotlib
matplotlib.use('Agg') #Se usa para generar la imagen de fondo sin tener que hacer la figura en ventana
import matplotlib.pyplot as plt #pip install matplotlib==3.3.1

import statistics

#Funcion que devuelve un array con los precios
def getPreciosLuz():
    #Día actual
    today = date.today() #aaaa-mm-dd

    #Montamos la mensaje para obtener el json con los precios
    mensaje = "https://apidatos.ree.es/es/datos/mercados/precios-mercados-tiempo-real?start_date="
    mensaje += str(today)
    mensaje += "T00:00&end_date="
    mensaje += str(today)
    mensaje += "T23:59&time_trunc=hour"

    #Obtenemos el response
    response = requests.get(mensaje)

    #Montamos los datos 
    data = json.loads(response.text)

    #Extraemos los precios de cada hora y los guardamos en un array
    precioLuz = []
    for n in range (0,24,1):
        precioMWh = data["included"][0]["attributes"]["values"][n]["value"]
        preciokWh = precioMWh/1000
        precio = round(preciokWh,4)
        precioLuz.append(precio)
    
    return precioLuz

#Funcion que devuelve un mensaje con los precios
def imprimirPreciosLuz(precioLuz):
    #Montamos el mensaje con los precios que vamos a devolver
    mensaje = "El precio para el dia de hoy es:\n\n"

    for n in range(0,24,1):
        if n < 10:
            mensaje += "0"
        mensaje += str(n)
        mensaje += ":00 a "
        if n < 9:
            mensaje += "0"
        if n < 23:
            mensaje += str(n+1)
        else:
            mensaje += "00"

        mensaje += ":00 "
        mensaje += '\U000027A1'
        mensaje += " "
        mensaje += str(precioLuz[n])
        mensaje += " € kWh"
        mensaje += "\n"

    #Buscamos el precio maximo del dia
    mensaje += "\n"
    mensaje += "El precio mas caro es de "
    mensaje += '\U000027A1'
    mensaje += " "
    mensaje += str(max(precioLuz))
    mensaje += " €"

    #Buscamos el precio minimo del dia
    mensaje += "\n"
    mensaje += "El precio mas barato es de "
    mensaje += '\U000027A1'
    mensaje += " "
    mensaje += str(min(precioLuz))
    mensaje += " €"

    #Buscamos el precio medio del dia
    mensaje += "\n"
    mensaje += "El precio medio es de "
    mensaje += '\U000027A1'
    mensaje += " "
    mensaje += str(round(statistics.mean(precioLuz),4))
    mensaje += " €"
    return mensaje
        
#Funcion que crea un grafico de barras con los precios del dia
def crearGraficoBarrasPrecios(precioLuz):
    #Definimos una lista con las horas
    horas = list(range(0,24))

    fig, ax = plt.subplots()
    #Colocamos una etiqueta en el eje Y
    ax.set_ylabel('€ kWh')
    #Colocamos una etiqueta en el eje X
    ax.set_xlabel('Hora')
    #Colocamos un título
    title = "Precio de la luz durante el día "
    title += str(date.today())
    ax.set_title(title)
    #Añadimos la linea discontinua con el valor medio
    valorMedio = round(statistics.mean(precioLuz),4)
    plt.axhline(y=valorMedio, color='k', linestyle='--')
    #Creamos las mascaras para pintar en colores 
    precioHorasValle = []
    horasValle = []
    precioPunta = []
    horasPunta = []
    #Consideramos baratas si el valor de la hora es menor que el valor medio menos el 20%
    for index, item in enumerate(precioLuz):
        if item < valorMedio-valorMedio*0.20:
            precioHorasValle.append(item)
            horasValle.append(index)
        #Consideramos que es cara si el valor de la hora es mayor que el valor medio mas el 20%
        elif item >= valorMedio+valorMedio*0.20:
            precioPunta.append(item)
            horasPunta.append(index)
        

    #Creamos la grafica de barras
    plt.bar(horas, precioLuz,color='b')
    plt.bar(horasValle, precioHorasValle,color='g')
    plt.bar(horasPunta, precioPunta,color='r')

    #Guardamos la grafica
    plt.savefig('assets/PrecioLuz.png')
    #Abrimos la grafica para devolverla
    foto = open('assets/PrecioLuz.png', 'rb')
    return foto