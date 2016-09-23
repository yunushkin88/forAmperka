



// подключение основной бибиотеки 
#include "Energia.h"




#ifndef __CC3200R1M1RGC__
#include <SPI.h>                // не подключать SPI (так как это не модуль) если CC3200 LaunchPad
#endif
#include <WiFi.h>


// определяем структуры и классы


// определяем переменные и константы
char wifi_name[] = "AmperkaNet"; // имя Wi-Fi сети
char wifi_password[] = "ILoveAmperka"; // пароль

WiFiServer myServer(80); //создание экземпляра класса сервер и инициализауия на 80 порт
uint8_t oldCountClients = 0;
uint8_t countClients = 0;


// Добавляем код инициализации 
void setup()
{
    Serial.begin(115200);
    delay(500);
    
    Serial.println("***  WiFi Web-Server for Amperka with Love ***");
    
    // Старт WiFi и создание сети с именем AmperkaNet
    //  wifi_password это пароль ILoveAmperka.
    Serial.print("Запуск AP... ");
    WiFi.beginNetwork(wifi_name, wifi_password);
    while (WiFi.localIP() == INADDR_NONE)
    {
        // печатаем точки пока идет настройка точки доступа
        Serial.print('.');
        delay(300);
    }
    Serial.println("НАСТРОЙКА_ЗАВЕРШЕНА");
    
    Serial.print("LAN name = ");
    Serial.println(wifi_name);
    Serial.print("WPA password = ");
    Serial.println(wifi_password);
    
    
    pinMode(RED_LED, OUTPUT);      // Установка ножки красного светодиода на выход и выключение светодиода
    digitalWrite(RED_LED, LOW);
    pinMode(GREEN_LED, OUTPUT);      // Установка ножки зеленого светодиода на выход и выключение светодиода
    digitalWrite(GREEN_LED, LOW);
    pinMode(YELLOW_LED, OUTPUT);      // Установка ножки оранжевого светодиода на выход и выключение светодиода
    digitalWrite(YELLOW_LED, LOW);
    
    IPAddress ip = WiFi.localIP();
    Serial.print("Webserver IP address = ");
    Serial.println(ip);
    
    Serial.print("Web-server port = ");
    myServer.begin();                           // Старт Web-сервера на порту 80
    Serial.println("80");
    Serial.println();
}

// Бесконечный главный цикл
void loop()
{
    
    countClients = WiFi.getTotalDevices();
    
    // проверка подключился или нет клиент в данный момент
    if (countClients != oldCountClients)
    {
        if (countClients > oldCountClients)
        {  // Client connect
            //            digitalWrite(RED_LED, !digitalRead(RED_LED));
            Serial.println("Client connected to AP");
            for (uint8_t k = 0; k < countClients; k++)
            {
                Serial.print("Client #");
                Serial.print(k);
                Serial.print(" at IP address = ");
                Serial.print(WiFi.deviceIpAddress(k));
                Serial.print(", MAC = ");
                Serial.println(WiFi.deviceMacAddress(k));
                Serial.println("CC3200 in AP mode only accepts one client.");
            }
        }
        else
        {  //Клиент отключился
            //            digitalWrite(RED_LED, !digitalRead(RED_LED));
            Serial.println("Клиент отключился от сети.");
            Serial.println();
        }
        oldCountClients = countClients;
    }
    
    WiFiClient myClient = myServer.available();
    
    if (myClient)
    {                             // если клиент подключился,
        Serial.println(". Клиент подключился к серверу");          
        char buffer[150] = {0};                 // создание буффера для входных данных
        int8_t i = 0;
        while (myClient.connected())
        {            // цикл пока есть клиент 
            if (myClient.available())
            {             // если есть байты для чтения от клиента,
                char c = myClient.read();             // прочитать байт, 
                Serial.write(c);                    // отправить на монитор последовательного порта
                if (c == '\n') {                    // если байт является символом новой строки
                    
                    // если текущая строка пустая, то есть два символа новой строки в строке.
                    // то это конец запроса клиента HTTP, чтобы отправить ответ:
                    if (strlen(buffer) == 0)
                    {
                        // HTTP заголовки всегда начинаются с кода ответа (например, HTTP / 1.1 200 OK)
                        // и типа содержимого, так что если клиент знает, что будет, то пустая строка:
                        myClient.println("HTTP/1.1 200 OK");
                        myClient.println("Content-type:text/html");
                        myClient.println();
                        
                        // содержание ответа HTTP следует за заголовком:
                        myClient.println("<html><head><title>WiFi Web-Server for Amperka</title></head><body align=center>");
                        myClient.println("<h1 align=center><font color=\"red\">WiFi Web-Server for Amperka</font></h1>");
                        myClient.print("Red LED <button onclick=\"location.href='/RH'\">HIGH</button>");
                        myClient.println(" <button onclick=\"location.href='/RL'\">LOW</button><br>");
                        myClient.print("Green LED <button onclick=\"location.href='/GH'\">HIGH</button>");
                        myClient.println(" <button onclick=\"location.href='/GL'\">LOW</button><br>");
                        myClient.print("Yellow LED <button onclick=\"location.href='/YH'\">HIGH</button>");
                        myClient.println(" <button onclick=\"location.href='/YL'\">LOW</button><br>");
                        
                        // Ответ HTTP заканчивается другой пустой строкой:
                        myClient.println();
                        // выход из цикла:
                        break;
                    }
                    else
                    {      // если вы получили новую строку, а затем очистить буфер:
                        memset(buffer, 0, 150);
                        i = 0;
                    }
                }
                else if (c != '\r')
                {    // если получили что-нибудь еще, кроме возврата каретки,
                    buffer[i++] = c;      // добавить в конец текущей строки
                }
                
                Serial.println();
                String text = buffer;
                // Проверка запроса клиента "GET /H" или "GET /L":
                if (text.endsWith("GET /RH"))
                {
                    digitalWrite(RED_LED, HIGH);               // GET /H включение светодиода
                    Serial.println(". RED_LED on");
                }
                if (text.endsWith("GET /RL"))
                {
                    digitalWrite(RED_LED, LOW);                // GET /L выключение светодиода
                    Serial.println(". RED_LED off");
                }
                if (text.endsWith("GET /GH"))
                {
                    digitalWrite(GREEN_LED, HIGH);               // GET /H  включение светодиода
                    Serial.println(". GREEN_LED on");
                }
                if (text.endsWith("GET /GL"))
                {
                    digitalWrite(GREEN_LED, LOW);                // GET /L выключение светодиода
                    Serial.println(". GREEN_LED off");
                }
                if (text.endsWith("GET /YH"))
                {
                    digitalWrite(YELLOW_LED, HIGH);               // GET /H включение светодиода
                    Serial.println(". YELLOW_LED on");
                }
                if (text.endsWith("GET /YL"))
                {
                    digitalWrite(YELLOW_LED, LOW);                // GET /L выключение светодиода
                    Serial.println(". YELLOW_LED off");
                }
            }
        }
        
        // закрыть соединение:
        myClient.stop();
        Serial.println(". Клиент отключился от сервера.");
        Serial.println();
    }
}

