from precioLuz import *
from bienvenida import * 

import telebot #pip install PyTelegramBotAPI
TOKEN = "" # Ponemos nuestro Token generado con el @BotFather

#Creamos la instancia del bot
bot = telebot.TeleBot(TOKEN)  #Creamos nuestra instancia "bot" a partir de ese TOKEN

#Esta función se ejecuta cuando enviemos el comando start o help
@bot.message_handler(regexp='/start|/help|\U00002753')
def send_welcome(message):
    bot.send_message(message.chat.id,mensajeStart(message),reply_markup=tecladoStart(message))


#Esta función se ejecuta cuando enviemos el comando precio
@bot.message_handler(regexp='/precioLuz|\U0001F4B0' + '\U0001F4A1' + " " + '\U0001F4B9') 
def send_precioLuz(message):
    precios = getPreciosLuz()
    mensaje = imprimirPreciosLuz(precios)
    foto = crearGraficoBarrasPrecios(precios)
    bot.send_message(message.chat.id,mensaje)
    bot.send_photo(message.chat.id,foto)

bot.polling()
