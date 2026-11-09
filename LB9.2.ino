
#include <M5Unified.h>
#include <math.h>

// Центр большого круга
int centerX;
int centerY;

// Смещение центра вверх
const int centerOffsetY = -8;

// Размеры кругов
const int targetRadius = 32;
const int playerRadius = 9;

// Допустимое отклонение от центра
const float centerTolerance = 4.0;

// Координаты маленького круга
float playerX;
float playerY;

// Предыдущие координаты маленького круга
int oldPlayerX;
int oldPlayerY;

// Начальные показания акселерометра
float baseX = 0;
float baseY = 0;

// Чувствительность
const float sensitivity = 100.0;

bool isHit = false;
bool firstDraw = true;

// Получение данных акселерометра
void readSensor()
{
    if (M5.Imu.update())
    {
        auto data = M5.Imu.getImuData();

        float offsetX = (data.accel.x - baseX) * sensitivity;
        float offsetY = (data.accel.y - baseY) * sensitivity;

        offsetX = constrain(offsetX, -50, 50);
        offsetY = constrain(offsetY, -65, 65);

        playerX = centerX + offsetX;
        playerY = centerY - offsetY;

        // Ограничиваем движение границами экрана
        playerX = constrain(
            playerX,
            playerRadius,
            M5.Display.width() - playerRadius
        );

        playerY = constrain(
            playerY,
            playerRadius,
            M5.Display.height() - playerRadius
        );
    }
}

// Отрисовка игры
void drawGame()
{
    float dx = playerX - centerX;
    float dy = playerY - centerY;

    float distance = sqrt(dx * dx + dy * dy);

    isHit = distance <= centerTolerance;

    static bool previousHit = false;

    // Обновляем большой круг только при изменении цвета
    if (firstDraw || isHit != previousHit)
    {
        M5.Display.fillCircle(
            centerX,
            centerY,
            targetRadius,
            isHit ? TFT_GREEN : TFT_RED
        );

        previousHit = isHit;
    }

    // Восстанавливаем старое положение маленького круга
    if (!firstDraw)
    {
        int left = oldPlayerX - playerRadius - 1;
        int top = oldPlayerY - playerRadius - 1;
        int size = (playerRadius + 1) * 2 + 1;

        for (int y = top; y <= top + size; y++)
        {
            for (int x = left; x <= left + size; x++)
            {
                int dxOld = x - centerX;
                int dyOld = y - centerY;

                int oldDistanceSquared =
                    dxOld * dxOld + dyOld * dyOld;

                if (oldDistanceSquared <= targetRadius * targetRadius)
                {
                    M5.Display.drawPixel(
                        x,
                        y,
                        isHit ? TFT_GREEN : TFT_RED
                    );
                }
                else
                {
                    M5.Display.drawPixel(x, y, TFT_BLACK);
                }
            }
        }
    }

    // Центральная отметка
    M5.Display.drawPixel(centerX, centerY, TFT_WHITE);

    // Маленький круг
    M5.Display.fillCircle(
        (int)playerX,
        (int)playerY,
        playerRadius,
        TFT_WHITE
    );

    oldPlayerX = (int)playerX;
    oldPlayerY = (int)playerY;

    firstDraw = false;
}

void setup()
{
    auto cfg = M5.config();
    M5.begin(cfg);

    M5.Display.setRotation(0);

    // Получаем размеры экрана после установки ориентации
    int screenWidth = M5.Display.width();
    int screenHeight = M5.Display.height();

    // Определяем точный центр дисплея с учётом смещения
    centerX = screenWidth / 2;
    centerY = screenHeight / 2 + centerOffsetY;

    Serial.begin(115200);

    M5.Display.fillScreen(TFT_BLACK);
    M5.Display.setTextColor(TFT_WHITE);
    M5.Display.setTextDatum(middle_center);

    M5.Display.drawString(
        "Keep device still...",
        centerX,
        centerY
    );

    // Калибровка акселерометра
    float sumX = 0;
    float sumY = 0;
    int samples = 0;

    for (int i = 0; i < 100; i++)
    {
        if (M5.Imu.update())
        {
            auto data = M5.Imu.getImuData();

            sumX += data.accel.x;
            sumY += data.accel.y;

            samples++;
        }

        delay(20);
    }

    if (samples > 0)
    {
        baseX = sumX / samples;
        baseY = sumY / samples;
    }

    // Начальное положение маленького круга
    playerX = centerX;
    playerY = centerY;

    oldPlayerX = (int)playerX;
    oldPlayerY = (int)playerY;

    // Первоначальная отрисовка
    M5.Display.fillScreen(TFT_BLACK);
    drawGame();

    Serial.println("BMI270 game started!");

    Serial.print("Center X: ");
    Serial.println(centerX);

    Serial.print("Center Y: ");
    Serial.println(centerY);
}

void loop()
{
    M5.update();

    readSensor();
    drawGame();

    delay(20);
}