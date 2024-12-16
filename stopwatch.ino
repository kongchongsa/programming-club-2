#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // 주소와 디스플레이 크기에 맞게 설정
unsigned long startTime = 0;
unsigned long pausedTime = 0;  // 멈춘 시간을 저장하는 변수 추가
bool isRunning = false;

const int startStopButtonPin = 2;  // 시작/정지 버튼 핀
const int resetButtonPin = 3;      // 리셋 버튼 핀

void setup() {
  lcd.init();                      // LCD 초기화
  lcd.backlight();                 // LCD 배경화면 켜기

  pinMode(startStopButtonPin, INPUT_PULLUP);
  pinMode(resetButtonPin, INPUT_PULLUP);

  lcd.setCursor(0, 0);
  lcd.print("Stopwatch:");
  lcd.setCursor(0, 1);
  lcd.print("Press to start");
}

void loop() {
  if (digitalRead(startStopButtonPin) == LOW) {
    delay(100);  // 디바운싱을 위한 짧은 딜레이

    if (!isRunning) {
      if (pausedTime == 0) {  // 처음 시작하는 경우
        startTime = millis();
      } else {  // 이어서 시작하는 경우
        startTime = millis() - pausedTime;
        pausedTime = 0;  // pausedTime 초기화
      }
      isRunning = true;
      lcd.setCursor(0, 1);
      lcd.print("Running...       ");
    } else {
      pausedTime = millis() - startTime;  // 멈춘 시간 기록
      isRunning = false;
      lcd.setCursor(0, 1);
      lcd.print("Paused. Press to start");
    }
  }

  if (digitalRead(resetButtonPin) == LOW) {
    delay(100);  // 디바운싱을 위한 짧은 딜레이

    isRunning = false;
    pausedTime = 0;  // pausedTime 초기화
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Stopwatch:");
    lcd.setCursor(0, 1);
    lcd.print("Press to start");
  }

  if (!isRunning) {  // 멈췄을 때만 멈춘 시간을 표시
    int milliseconds = pausedTime % 1000;
    int seconds = (pausedTime / 1000) % 60;
    int minutes = (pausedTime / 60000) % 60;

    lcd.setCursor(0, 1);
    lcd.print("Paused: ");
    lcd.print(minutes);
    lcd.print(":");
    if (seconds < 10) {
      lcd.print("0");
    }
    lcd.print(seconds);
    lcd.print(":");
    if (milliseconds < 100) {
      lcd.print("0");
    }
    if (milliseconds < 10) {
      lcd.print("0");
    }
    lcd.print(milliseconds);
  } else {  // 실행 중일 때는 시간 계속 표시
    unsigned long currentTime = millis() - startTime;

    int milliseconds = currentTime % 1000;
    int seconds = (currentTime / 1000) % 60;
    int minutes = (currentTime / 60000) % 60;

    lcd.setCursor(0, 1);
    lcd.print("Time: ");
    lcd.print(minutes);
    lcd.print(":");
    if (seconds < 10) {
      lcd.print("0");
    }
    lcd.print(seconds);
    lcd.print(":");
    if (milliseconds < 100) {
      lcd.print("0");
    }
    if (milliseconds < 10) {
      lcd.print("0");
    }
    lcd.print(milliseconds);
  }
}