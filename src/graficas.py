import matplotlib.pyplot as plt
import math
import numpy as np

def Read_Two_Column_File(file_name):
    with open(file_name, 'r') as data:
        x = []
        y = []
        for line in data:
            p = line.split()
            x.append(float(p[0]))
            y.append(float(p[1]))

    return x, y

x,y = Read_Two_Column_File("rmse_areav2.txt")
x_s, y_s = Read_Two_Column_File("rmse_solidanglev2.txt")
x_a, y_a = Read_Two_Column_File("rmse_areav2_grande.txt")
x_s_a, y_s_a = Read_Two_Column_File("rmse_solidanglev2_grande.txt")
x_t, y_t = Read_Two_Column_File("data_time_areav2.txt")
x_t_s, y_t_s = Read_Two_Column_File("data_time_solidanglev2.txt")
x_t_a, y_t_a = Read_Two_Column_File("data_time_areav2_grande.txt")
x_t_s_a, y_t_s_a = Read_Two_Column_File("data_time_solidanglev2_grande.txt")
x_e, y_e = Read_Two_Column_File("rmse_ellipse.txt")
x_e_s, y_e_s = Read_Two_Column_File("rmse_ellipse_sa.txt")
x_e_a, y_e_a = Read_Two_Column_File("rmse_ellipse_grande.txt")
x_e_s_a, y_e_s_a = Read_Two_Column_File("rmse_ellipse_sa_grande.txt")
x_e_t, y_e_t = Read_Two_Column_File("data_time_ellipse.txt")
x_e_t_s, y_e_t_s = Read_Two_Column_File("data_time_ellipse_sa.txt")
x_e_t_a, y_e_t_a = Read_Two_Column_File("data_time_ellipse_area_grande.txt")
x_e_t_s_a, y_e_t_s_a = Read_Two_Column_File("data_time_ellipse_sa_grande.txt")

# RECTANGULOS

plt.plot(x,y,label="RMSE Área", marker='+')
plt.plot(x_s, y_s, label="RMSE Ángulo Sólido", marker="+")
plt.xlabel("Muestras por píxel")
plt.ylabel("RMSE")
plt.legend()
plt.show()


plt.plot(x_a,y_a,label="RMSE Área", marker='+')
plt.plot(x_s_a, y_s_a, label="RMSE Ángulo Sólido", marker="+")
plt.xlabel("Muestras por píxel")
plt.ylabel("RMSE (con luz más grande)")
plt.legend()
plt.show()

r = [y[i]-y_s[i] for i in range(len(y))]
r_a = [y_a[i]-y_s_a[i] for i in range(len(y_a))]
plt.plot(x, r, marker="+", label= "Diferencia original")
plt.plot(x, r_a, marker='+', label="Diferencia con luz más ancha")
plt.xlabel("Muestras por píxel")
plt.ylabel("Diferencia entre la RMSE del área y del ángulo sólido")
plt.legend()
plt.show()

plt.plot(x_t,y_t,label="Área", marker='+')
plt.plot(x_t_s, y_t_s, label="Ángulo Sólido", marker="+")
plt.xlabel("Muestras por píxel")
plt.ylabel(r"Tiempo ($\mu s$)")
plt.legend()
plt.show()

plt.plot(x_t_a,y_t_a,label="Área con luz grande", marker='+')
plt.plot(x_t_s_a, y_t_s_a, label="Ángulo Sólido con luz grande", marker="+")
plt.xlabel(r"Muestras por píxel")
plt.ylabel(r"Tiempo ($\mu s$)")
plt.legend()
plt.show()
print("La proporción entre tiempos con cien muestras con la luz normal es: " , y_t_s[len(y_t_s)-1]/y_t[len(y_t)-1])
print("La proporción entre tiempos con cien muestras con la luz grande es: " , y_t_s_a[len(y_t_s_a)-1]/y_t_a[len(y_t_a)-1])

# ELIPSES

plt.plot(x_e,y_e,label="RMSE Área", marker='+')
plt.plot(x_e_s, y_e_s, label="RMSE Ángulo Sólido", marker="+")
plt.xlabel("Muestras por píxel")
plt.ylabel("RMSE Elipse")
plt.legend()
plt.show()


plt.plot(x_e_a,y_e_a,label="RMSE Área", marker='+')
plt.plot(x_e_s_a, y_e_s_a, label="RMSE Ángulo Sólido", marker="+")
plt.xlabel("Muestras por píxel")
plt.ylabel("RMSE (con luz más grande)")
plt.legend()
plt.show()

r_e = [y_e[i]-y_e_s[i] for i in range(len(y_e))]
r_e_a = [y_e_a[i]-y_e_s_a[i] for i in range(len(y_e_a))]
plt.plot(x, r_e, marker="+", label= "Diferencia original")
plt.plot(x, r_e_a, marker='+', label="Diferencia con luz más ancha")
plt.xlabel("Muestras por píxel")
plt.ylabel("Diferencia entre la RMSE del área y del ángulo sólido en elipses")
plt.legend()
plt.show()

plt.plot(x_e_t,y_e_t,label="Área", marker='+')
plt.plot(x_e_t_s, y_e_t_s, label="Ángulo Sólido", marker="+")
plt.xlabel("Muestras por píxel")
plt.ylabel(r"Tiempo ($\mu s$) en elipses")
plt.legend()
plt.show()

plt.plot(x_e_t_a, y_e_t_a,label="Área con luz grande", marker='+')
plt.plot(x_e_t_s_a, y_e_t_s_a, label="Ángulo Sólido con luz grande", marker="+")
plt.xlabel(r"Muestras por píxel")
plt.ylabel(r"Tiempo ($\mu s$) en elipses")
plt.legend()
plt.show()
print("La proporción entre tiempos con cien muestras con la luz normal en elipses es: " , y_e_t_s[len(y_e_t_s)-1]/y_e_t[len(y_e_t)-1])
print("La proporción entre tiempos con cien muestras con la luz grande en elipses es: " , y_e_t_s_a[len(y_e_t_s_a)-1]/y_e_t_a[len(y_e_t_a)-1])
