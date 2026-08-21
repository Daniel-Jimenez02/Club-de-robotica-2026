print('Hola usuario que probablemente es Daniel, este es el ordenador de numeros de menor a mayor, de mayor a menor y sacar el promedio de los numeros')
numero_de_numeros = int(input('Cuantos numeros quiere promediar y ordenar?: '))
numeros = []
for i in range(numero_de_numeros):
    numero = int(input(f'Ingrese el numero {i + 1}: '))
    numeros.append(numero)

numeros_mayor_a_menor = sorted(numeros, reverse=True)
numeros_menor_a_mayor = sorted(numeros)
numeros_sumados = sum(numeros)
promedio = numeros_sumados / numero_de_numeros
print('-------------------------------------------')
print('|Los numeros de mayor a menor son: ', numeros_mayor_a_menor)
print('|Los numeros de menor a mayor son: ', numeros_menor_a_mayor)
print('|El promedio de los numeros es: ', promedio)
print('-------------------------------------------')