# Semina-AVR-RasspberryPi (2021.12 ~ 2024.02)

## 1. 목적 : AVR(MCU), Raspberry PI(SBC)에 관해 매주 기술 목차에 따른 발표 및 프로젝트 진행
    ### 기술 목차
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
    
## 2. 세미나 개요 :
![contents](https://github.com/user-attachments/assets/dfbb63d8-ba1d-4b39-902c-cedc45ea072a)


## 4. 프로젝트

    ### 자동 블라인드 프로젝트 (개인)
        - ATmega128로 조도센서 센서를 ADC하여 임계값에 따른 Servo모터 제어
        - 낮과 밤에 따라 자동 블라인드
    ### 로봇팔 프로젝트 (개인)
        - ATmega128로 가변저항 5개의 ADC 값에 따라 동작하는 5관절 Manipulator 제어
        - 가변저항의 전압값에 따라 로봇팔의 관절을 구성하는 서보모터 제어
    ### Bluetooth 무선 RC카 프로젝트 (팀)
        - ATmega128의 Bluetootn 모듈을 통해 PC와 연결 후 키보드 명령으로 RC카의 전/후/좌/우 제어값 송수신
        - 3개의 적외선 센서 값을 처리하여 라인을 따라 움직이는 4WD DC모터 제어 알고리즘 개발
        - 초음파 센서를 통한 후방 거리 측정 시스템 개발

