
#include <M5Unified.h>
#include <WiFi.h>
#include <WebServer.h>

// Данные Wi-Fi
const char* ssid = "ASOIU";
const char* password = "kaf.asoiu.48";

// Создание веб-сервера
WebServer server(80);

// Состояние прямоугольников
bool rect1State = false;
bool rect2State = false;

// Цвета
uint16_t grayColor = TFT_DARKGRAY;

// Отображение информации на экране
void drawRectangles()
{
    M5.Display.fillScreen(TFT_BLACK);

    M5.Display.setTextColor(TFT_WHITE);
    M5.Display.setTextDatum(middle_center);

    // IP-адрес сверху
    M5.Display.setTextSize(1);
    M5.Display.drawString(
        WiFi.localIP().toString(),
        67, 18
    );

    // Первый прямоугольник
    M5.Display.fillRect(
        15, 45, 105, 55,
        rect1State ? TFT_RED : grayColor
    );

    // Второй прямоугольник
    M5.Display.fillRect(
        15, 135, 105, 55,
        rect2State ? TFT_GREEN : grayColor
    );
}

// Главная страница сайта

void handleRoot()
{
    String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <title>BAR Control</title>

        <style>
            body {
                font-family: Arial, sans-serif;
                text-align: center;
                background-color: white;
                margin: 0;
                padding: 20px 10px;
            }

            h1 {
                font-size: 36px;
                margin-top: 55px;
                margin-bottom: 45px;
                color: #333;
            }

            p {
                font-size: 20px;
                color: #888;
                margin-top: 0;
                margin-bottom: 15px;
            }

            button {
                width: 210px;
                height: 83px;
                font-size: 36px;
                color: white;
                border: none;
                border-radius: 6px;
                cursor: pointer;
                margin-bottom: 50px;
            }

            .grey {
                background-color: #34495e;
            }

            .red {
                background-color: #e53935;
            }

            .green {
                background-color: #00fa04;
            }
        </style>
    </head>

    <body>

        <h1>M5StickS3 Web Server</h1>

        <p>BAR1: )rawliteral";

    html += rect1State ? "RED" : "GREY";

    html += R"rawliteral(</p>

        <form action="/toggle1" method="GET">
            <button class=")rawliteral";

    html += rect1State ? "grey" : "red";

    html += R"rawliteral(" type="submit">)rawliteral";

    html += rect1State ? "GREY" : "RED";

    html += R"rawliteral(</button>
        </form>

        <p>BAR2: )rawliteral";

    html += rect2State ? "GREEN" : "GREY";

    html += R"rawliteral(</p>

        <form action="/toggle2" method="GET">
            <button class=")rawliteral";

    html += rect2State ? "grey" : "green";

    html += R"rawliteral(" type="submit">)rawliteral";

    html += rect2State ? "GREY" : "GREEN";

    html += R"rawliteral(</button>
        </form>

    </body>
    </html>
    )rawliteral";

    server.send(200, "text/html; charset=utf-8", html);
}


// Переключение первого прямоугольника
void handleToggle1()
{
    rect1State = !rect1State;

    drawRectangles();
    handleRoot();
}

// Переключение второго прямоугольника
void handleToggle2()
{
    rect2State = !rect2State;

    drawRectangles();
    handleRoot();
}

void setup()
{
    // Инициализация устройства
    auto cfg = M5.config();
    M5.begin(cfg);

    // Сброс состояний при каждом запуске
    rect1State = false;
    rect2State = false;

    // Вертикальная ориентация дисплея
    M5.Display.setRotation(0);
    M5.Display.fillScreen(TFT_BLACK);

    M5.Display.setTextColor(TFT_WHITE);
    M5.Display.setTextDatum(middle_center);
    M5.Display.drawString("Connecting Wi-Fi...", 67, 65);

    // Запуск последовательного порта
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("M5StickS3 Web Server");
    Serial.println("Connecting to Wi-Fi...");

    // Подключение к Wi-Fi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Wi-Fi connected!");

    // Вывод IP-адреса
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Отображение прямоугольников и IP
    drawRectangles();

    // Настройка маршрутов
    server.on("/", handleRoot);
    server.on("/toggle1", handleToggle1);
    server.on("/toggle2", handleToggle2);

    // Запуск сервера
    server.begin();

    Serial.println("Web server started!");
    Serial.println("====================");
}

void loop()
{
    server.handleClient();
    M5.update();
}
