from telebot import types

#Funcion que codifica el mensaje de bienvenida que se envia al iniciar la conversación con el bot
def mensajeStart(message):
    mensaje = "Bienvenido al bot de MakerGarage, estas son las instrucciones que puedo realizar\n\n"
    mensaje += '\U0001F4B0' + '\U0001F4A1' + " " + '\U0001F4B9'+ "\n"
    mensaje += "/precioLuz para conocer el precio de la luz"+ "\n"
    mensaje += '\U00002753' + "\n"
    mensaje += "/help para mostrar este mensaje de nuevo"
    return mensaje

#Teclado para no tener que teclear los comandos
def tecladoStart(message):
    teclado = types.ReplyKeyboardMarkup()
    itemPrecioLuz = types.KeyboardButton('\U0001F4B0' + '\U0001F4A1' + " " + '\U0001F4B9'+ "\n" + "Precio Luz")
    itemAyuda = types.KeyboardButton('\U00002753' + "\n" + "Help")
    teclado.row(itemPrecioLuz)
    teclado.row(itemAyuda)

    return teclado