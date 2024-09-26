# Semina-AVR-RasspberryPi (2021.12 ~ 2024.02)

## 1. ���� : AVR(MCU), Raspberry PI(SBC)�� ���� ���� ��� ������ ���� ��ǥ �� ������Ʈ ����
    ### ��� ����
        -  ATmega128
            1. C language
            2. Controlling GPIO pins to interface with LEDs and switches
            3. Controlling 7-segments using an AVR with dynamic multiplexing and interrupts
            4. To interface an LM35 temperature sensor with an AVR using SAR ADC and send the data via UART (RS232)
            5. A CDS sensor's light intensity is measured via the SAR ADC, and the Timer/Counter generates a PWM signal to control the servo motor's position accordingly.
            6. A sonar sensor measures distance, and using PWM signals to control the speed and direction of a DC motor based on the measured distance.
            7. The MPU6050 gyro sensor communicates via I2C, and its data is processed with a complementary filter, while a PID and state feedback control algorithm regulate the DC motor's movement based on the sensor readings
            
        - RasspberryPi
            1. Porting the Raspbian OS enables control of GPIO pins to interface with LEDs and switches for hardware interaction.
            2. An ADC-UART communication program is developed to enable data exchange between a Raspberry Pi and a PC's terminal program (PuTTY) using the RS-232 standard.
            3. useing threads to concurrently read ADC values from an MCP-3208 chip via SPI (utilizing a 12-bit SAR ADC for variable resistor voltage readings) and control a servo motor based on these readings.
            4. useing threads to concurrently read ADC values from a NAU-7802 chip via I2C (utilizing a 12-bit Sigma-Delta ADC for temperature and humidity sensor readings) and control a servo motor based on these readings.
            5. Socket communication between two Raspberry Pi devices, with the server and client using threads to handle simultaneous data transmission and reception; this enables real-time motor control commands to be sent from the client to the server, which then processes these commands to control the motor based on the exchanged data.
    
## 2. ���̳� ���� :

   
    

## 4. ������Ʈ

    ### �ڵ� ����ε� ������Ʈ (����)
        - ATmega128�� �������� ������ ADC�Ͽ� �Ӱ谪�� ���� Servo���� ����
        - ���� �㿡 ���� �ڵ� ����ε�
    ### �κ��� ������Ʈ (����)
        - ATmega128�� �������� 5���� ADC ���� ���� �����ϴ� 5���� Manipulator ����
        - ���������� ���а��� ���� �κ����� ������ �����ϴ� �������� ����
    ### Bluetooth ���� RCī ������Ʈ (��)
        - ATmega128�� Bluetootn ����� ���� PC�� ���� �� Ű���� ������� RCī�� ��/��/��/�� ��� �ۼ���
        - 3���� ���ܼ� ���� ���� ó���Ͽ� ������ ���� �����̴� 4WD DC���� ���� �˰��� ����
        - ������ ������ ���� �Ĺ� �Ÿ� ���� �ý��� ����

