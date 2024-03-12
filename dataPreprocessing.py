import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import datetime

# Defino una función que me determina la distancia entre dos fechas teniendo el cuenta la diferencia de horas y
# devolviendome la distancia en dias con posibilidad fraccionaria. (Es decir es posible, por ejemplo, 25.37 dias).
def get_days_between(datePast, dateFuture):
   difference = dateFuture - datePast
   return difference.total_seconds() / datetime.timedelta(days=1).total_seconds()

#Cargo los datos.
data = pd.read_csv('Exp_Octubre.csv', sep=';')

# Realizando una exploración de los datos se hallaron algunos datos faltantes, sin embargo dado que representaban
# menos del 1% de las filas totales fueron descartados. De esta manera solucionamos posibles errores de faltante de datos
# para este problema puntual
data.drop(data[(data.bid == "\\N") |  (data.ask =="\\N") | (data.underBid == "\\N") | (data.underAsk == "\\N")].index, inplace = True)

# Dado que no voy a necesitar la descripcion, ni el valor strike ya que lo voy a agregar aparte en el programa en C++,
# tampoco me interesa el kind dado que ya se que es una opcion call voy a tirar esas columnas.
data.drop(columns = ['description', 'strike', 'kind'],axis = 1, inplace=True)

# Para facilitar el tratamiento de los datos, se transforman las columnas en floats dado que eran strings.
data.loc[:, 'bid'] = data.bid.apply(lambda x: x.replace(',','.')).astype(float)
data.loc[:, 'ask'] = data.ask.apply(lambda x: x.replace(',','.')).astype(float)
data.loc[:, 'underBid'] = data.underBid.apply(lambda x: x.replace(',','.')).astype(float)
data.loc[:, 'underAsk'] = data.underAsk.apply(lambda x: x.replace(',','.')).astype(float)

# Ahora voy a tomar los valores bid y ask del activo y del subyacente y realizo un promedio en una nueva columna,
# luego descarto las columnas espúreas.
data['price'] = (data.bid + data.ask)/2
data['underPrice'] = (data.underBid + data.underAsk)/2
data.drop(columns = ['bid', 'ask', 'underAsk', 'underBid'], axis = 1, inplace = True)

# Transformo las fechas a tipo datetime lo cual me facilita encontrar el time to maturity
data.created_at = pd.to_datetime(data.created_at)
#Copio y guardo las fechas para usar en los gráficos.
dates_df = data.created_at.copy()
dates_df.to_csv('dates.csv', index = False)
# 20/10/2023 17:30. Por lo consultado por mail, el día de cierre sería este, buscando por internet encontré que el
# horario suele ser ese también.

#Lo transformo en tipo datetime para poder encontrar la diferencia con las demás fechas
closing_date_str = '10/20/2023 17:30'
close_date = datetime.datetime.strptime(closing_date_str, '%m/%d/%Y %H:%M')

# Aplico a cada uno de los elementos de fechas la función definida arriba para encontrar el time to maturity en días.
data['timeToMaturity'] = data.created_at.apply(lambda x: get_days_between(x, close_date))
# Borro la columna de fecha ya que solo me importa el time to maturity.
data = data.drop(['created_at'], axis = 1)

# Debido a que en Black-Scholes se utiliza en general el tiempo en años, hago la conversión.
data.loc[:,'timeToMaturity'] = data.timeToMaturity.apply(lambda x: x/365)

'''
Guardo la información procesada en un nuevo .csv. Ha quedado organizada con el precio de la opción promedio,
el precio del subyacente promedio, y el timeToMaturity considerado en años

price   underPrice  timeToMaturity

'''
print('Sus datos han sido procesados correctamente.')
data.to_csv('processedData.csv', sep = ';', index = False)