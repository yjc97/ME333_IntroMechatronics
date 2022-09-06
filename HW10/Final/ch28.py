# chapter 28 in python

# sudo apt-get install python3-pip
# python3 -m pip install pyserial
# sudo apt-get install python3-matplotlib

import matplotlib.pyplot as plt 
from statistics import mean 

def read_plot_matrix():
    n_str = ser.read_until(b'\n');  # get the number of data points to receive
    n_int = int(n_str) # turn it into an int
    print('Data length = ' + str(n_int))
    ref = []
    data = []
    data_received = 0
    while data_received < n_int:
        dat_str = ser.read_until(b'\n');  # get the data as a string, ints seperated by spaces
        dat_f = list(map(float,dat_str.split())) # now the data is a list
        #print(dat_f)
        ref.append(dat_f[0])
        data.append(dat_f[1])
        data_received = data_received + 1
    meanzip = zip(ref,data)
    meanlist = []
    for i,j in meanzip:
        meanlist.append(abs(i-j))
    score = mean(meanlist)
    t = range(len(ref)) # index array
    plt.plot(t,ref,'r*-',t,data,'b*-')
    plt.title('Score = ' + str(score))
    plt.ylabel('value')
    plt.xlabel('index')
    plt.show()

from genref import genRef

import serial
ser = serial.Serial('COM3',230400,rtscts=1)
print('Opening port: ')
print(ser.name)

has_quit = False
# menu loop
while not has_quit:
    print('PIC32 MOTOR DRIVER INTERFACE')
    # display the menu options; this list will grow
    print('\t a:read current sensor (ADC counts)   \t b:read current sensor (mA) ') # '\t' is a tab
    print('\t c:read encoder (counts)   \t d:read encoder (degrees)  \t e:reset encoder')
    print('\t f:set PWM (-100 to 100)   \t g:set current gains  \t h:get current gains')
    print('\t i:set position gains   \t j:get position gains  \t k:test current control')
    print('\t l:go to angle (degrees)   \t m:load step trajectory  \t n:load cubic trajectory')
    print('\t o:execute trajectory   \t p:unpower the motor  \t q:quit client  \t r:get mode')
    # read the user's choice
    selection = input('\nENTER COMMAND: ')
    selection_endline = selection+'\n'
     
    # send the command to the PIC32
    ser.write(selection_endline.encode()); # .encode() turns the string into a char array
    
    # take the appropriate action
    # there is no switch() in python, using if elif instead
    if (selection == 'a'):
       print('Requesting current ADC:')
       n_str = ser.read_until(b'\n')
       n_int = int(n_str)
       print('Current ADC = ' +str(n_int)+'\n')

    elif(selection =='b'):
       print('Requesting current mA:')
       n_str = ser.read_until(b'\n')
       f_int = float(n_str)
       print('Current mA = ' +str(f_int)+'\n')

    elif(selection =='c'):
       print('Requesting encoder counts:')
       n_str = ser.read_until(b'\n')
       n_int = int(n_str)
       print('Encoder counts = ' +str(n_int)+'\n')

    elif(selection =='d'):
       print('Requesting encoder angle:')
       n_str = ser.read_until(b'\n')
       f_int = float(n_str)
       print('Encoder angle = ' +str(f_int)+'\n')

    elif(selection == 'e'):
       print('Requesting reset encoder ')

    elif(selection == 'f'):
       pwm_str =input('Enter pwm -100 to 100: ')
       pwm_int = int(pwm_str)
       print('pwm = ' +str(pwm_int))
       ser.write((str(pwm_int)+'\n').encode())

    elif(selection == 'g'):
        kp_str = input('Enter current kp: ')
        kp_float = float(kp_str)
        print('kp = ' + str(kp_float))
        ki_str = input('Enter current ki: ')
        ki_float = float(ki_str)
        print('ki = ' + str(ki_float))
        ser.write((str(kp_float)+" "+str(ki_float)+'\n').encode())

    elif(selection == 'h'):
        print('Requesting kp:')
        k_str = ser.read_until(b'\n')
        x = k_str.split()
        kp_float = float(x[0])
        print('kp = ' +str(kp_float)+'\n')
        print('Requesting ki:')
        ki_float = float(x[1])
        print('ki = ' +str(ki_float)+'\n')

    elif(selection == 'i'):
        kp_str = input('Enter postion kp :')
        kp_float = float(kp_str)
        print('kp = ' + str(kp_float))
        ki_str = input('Enter position ki: ')
        ki_float = float(ki_str)
        print('ki = ' + str(ki_float))
        kd_str = input('Enter position kd: ')
        kd_float = float(kd_str)
        print('kd = ' + str(kd_float))
        ser.write((str(kp_float)+' '+str(ki_float)+' '+str(kd_float)+'\n').encode())

    elif(selection == 'j'):
        print('Requesting kp:')
        k_str = ser.read_until(b'\n')
        x = k_str.split()
        kp_float = float(x[0])
        print('kp = ' +str(kp_float)+'\n')
        print('Requesting ki:')
        ki_float = float(x[1])
        print('ki = ' +str(ki_float)+'\n')
        print('Requesting kd:')
        kd_float = float(x[2])
        print('kd = ' +str(kd_float)+'\n')
   
    elif (selection == 'k'):
       read_plot_matrix()
   
    elif (selection == 'm'):
        ref = genRef('step')
        #print(len(ref))
        t = range(len(ref))
        plt.plot(t,ref,'r*-')
        plt.ylabel('ange in degrees')
        plt.xlabel('index')
        plt.show()
        # send 
        ser.write((str(len(ref))+'\n').encode())
        for i in ref:
            ser.write((str(i)+'\n').encode())

    elif (selection == 'n'):
        ref = genRef('cubic')
        #print(len(ref))
        t = range(len(ref))
        plt.plot(t,ref,'r*-')
        plt.ylabel('ange in degrees')
        plt.xlabel('index')
        plt.show()
        # send 
        ser.write((str(len(ref))+'\n').encode())
        for i in ref:
            ser.write((str(i)+'\n').encode())

    elif (selection == 'o'):
        read_plot_matrix()

    elif(selection == 'p'):
        print('Unpowering motor ')

    elif(selection == 'r'):
        print('Requesting mode: ')
        n_str = ser.read_until(b'\n')
        n_int = int(n_str)
        print('Encoder counts = ' +str(n_int)+'\n')

    elif (selection == 'q'):
       print('Exiting client')
       has_quit = True; # exit client
       # be sure to close the port
       ser.close()
    else:
        print('Invalid Selection ' + selection_endline)



