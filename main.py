import serial
import pandas
import time
import struct
import math

serialport = '/dev/ttyACM0'
baud_rate = 115200
arduino = serial.Serial('/dev/ttyACM0', 115200)
time.sleep(2)
Kcstart = 1
arduino.write(struct.pack('>B', Kcstart))
somecount = 0

output_file = with open("/home/taliegemen/Desktop/output.txt", "w+")
ser = serial.Serial(serialport, baud_rate)
while somecount < 900:
    line = ser.readline()
    line = line.decode("utf-8")
    # ser.readline returns a binary, convert to string
    print(line)
    output_file.write(line)
    somecount += 1

colnames = ["noww", "poss"]
data = pandas.read_csv("/home/taliegemen/Desktop/output.txt", names=colnames)
noww = data.noww.tolist()
poss = data.poss.tolist()

for a in range(700):
    if not math.isnan(noww[a]):
        noww[a] = int(noww[a])

for b in range(700):
    if not math.isnan(poss[b]):
        noww[b] = int(noww[b])

yatcount = 0
k = 0
z = 0
deadcount = 0
deadtime = 5

while True:
    testmin = poss[0]
    testmax = poss[-1]
    timemin = noww[0]
    pointtau = ((testmax - testmin) * 0.63) + testmin
    print(pointtau)
    for k in range(700):
        if poss[k] == 100:
            deadtime = noww[k] - timemin

        if poss[(k+1)] > poss[k]:
            timemax = noww[(k+1)]

    for z in range(700):
        if pointtau >= poss[z]:
            atta = noww[z]
            alta = poss[z]

        if poss[z] >= pointtau and yatcount == 0:
            yatta = noww[z]
            uste = poss[z]
            yatcount = 1

    someint = uste - alta
    timetau = (((pointtau - alta) / (uste - alta)) * (yatta - atta)) + atta
    tau = (timetau - timemin) - deadtime
    gain = (((testmax - testmin) * 100) / testmin) / 50
    Kc = (1.35 / gain) * ((tau / deadtime) + 0.185)
    ti = 2.5 * (deadtime) * ((tau + (0.185 * deadtime)) / (tau + (0.611 * deadtime)))
    td = 0.37 * (deadtime) * (tau / (tau + (0.185 * deadtime)))
    print(Kc, ti, td)
    break

arduino = serial.Serial('/dev/ttyACM0', 115200)
time.sleep(2)
Kc = int(round(Kc))
ti = int(round(ti))
td = int(round(td))
arduino.write(struct.pack('>BBB', Kc, ti, td))
