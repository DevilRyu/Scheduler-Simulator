from matplotlib import pyplot as plt
import matplotlib
matplotlib.use('Agg')

def graficador(filename, ejey):

    x = []
    y1 = []
    y2 = []
    y3 = []
    y4 = []

    f = open("result/"+filename+".dat", "r")

    for i in range(5):
        if i == 0:
            data = f.readline().split(" \n")[0].split(" ")
            for dato in data:
                x.append(float(dato))
        if i == 1:
            data = f.readline().split(" \n")[0].split(" ")
            for dato in data:
                y1.append(float(dato))
        if i == 2:
            data = f.readline().split(" \n")[0].split(" ")
            for dato in data:
                y2.append(float(dato))
        if i == 3:
            data = f.readline().split(" \n")[0].split(" ")
            for dato in data:
                y3.append(float(dato))
        if i == 4:
            data = f.readline().split(" \n")[0].split(" ")
            for dato in data:
                y4.append(float(dato))
    f.close()

    # Creamos el gráfico

    plt.plot(x, y1, x, y2, x, y3, x, y4)
    plt.xlabel("Burst Time")
    plt.ylabel(ejey)
    plt.legend(['FCFS', 'RR1', 'RR4', 'SJF'])
    plt.savefig("result/"+filename+".jpeg", format="jpeg",bbox_inches='tight')
    plt.close()


graficador("schedwaits", "Tiempo de Espera Promedio")
graficador("schednturns", "Tiempo de Respuesta Normalizado Promedio")
graficador("schedturns", "Tiempos de Respuesta Promedio")
