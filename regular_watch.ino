#include <Wire.h>               // I2C 통신을 위한 라이브러리
#include <LiquidCrystal_I2C.h>  // I2C LCD 제어를 위한 라이브러리
#include <Ds1302.h>             // DS1302 RTC 제어를 위한 라이브러리

// DS1302 핀 설정
#define DS1302_CLK 4
#define DS1302_DAT 5
#define DS1302_RST 6

// DS1302 객체 생성
Ds1302 rtc(DS1302_RST, DS1302_DAT, DS1302_CLK);

// I2C LCD 설정 (주소: 0x27, 크기: 16x2)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // LCD 초기화
  lcd.init();                  // LCD 시작
  lcd.backlight();             // LCD 백라이트 켜기

  // RTC 초기화
  rtc.halt();                  // RTC 작동 시작 (매개변수 없음)

  // RTC 시간 설정 (필요한 경우 주석 해제하여 설정)
  // Ds1302::DateTime dt = {2024, 12, 3, 15, 30, 0}; // 연도, 월, 일, 시, 분, 초
  // rtc.setDateTime(&dt);
}

void loop() {
  // RTC에서 현재 시간 읽기
  Ds1302::DateTime now;       // DateTime 구조체 선언
  rtc.getDateTime(&now);      // RTC 시간 데이터를 구조체에 저장

  // LCD 첫 번째 줄에 시간 표시
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  printTime(now.hour, now.minute, now.second);

  // LCD 두 번째 줄에 날짜 표시
  lcd.setCursor(0, 1);
  lcd.print("Date: ");
  printDate(now.day, now.month, now.year);

  delay(1000);                // 1초 대기 후 갱신
}

// 시간 출력 함수
void printTime(int hour, int minute, int second) {
  if (hour < 10) lcd.print("0");
  lcd.print(hour);
  lcd.print(":");
  if (minute < 10) lcd.print("0");
  lcd.print(minute);
  lcd.print(":");
  if (second < 10) lcd.print("0");
  lcd.print(second);
}

// 날짜 출력 함수
void printDate(int day, int month, int year) {
  if (day < 10) lcd.print("0");
  lcd.print(day);
  lcd.print("/");
  if (month < 10) lcd.print("0");
  lcd.print(month);
  lcd.print("/");
  lcd.print(year);
}