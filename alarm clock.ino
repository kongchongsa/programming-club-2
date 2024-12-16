#include <Wire.h>               // I2C 통신을 위한 라이브러리
#include <LiquidCrystal_I2C.h>  // I2C LCD 제어를 위한 라이브러리
#include <Ds1302.h>             // DS1302 RTC 제어를 위한 라이브러리

// DS1302 핀 설정 (아두이노 핀 번호와 연결)
#define DS1302_CLK 4            // DS1302의 CLK 핀 (아두이노 핀 4)
#define DS1302_DAT 5            // DS1302의 DAT 핀 (아두이노 핀 5)
#define DS1302_RST 6            // DS1302의 RST 핀 (아두이노 핀 6)

// DS1302 RTC 객체 생성
Ds1302 rtc(DS1302_RST, DS1302_DAT, DS1302_CLK);

// I2C LCD 설정 (주소: 0x27, 크기: 16x2)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// 버튼 핀 설정
#define BUTTON_SET 2            // 알람 시간 설정 버튼
#define BUTTON_TOGGLE 3         // 알람 활성/비활성 토글 버튼

// 부저 핀
#define BUZZER_PIN 7            // 알람 작동 시 소리를 내는 부저 핀

// 알람 시간 설정 변수
int alarmMinute = 0;            // 알람 분
int alarmHour = 0;              // 알람 시
bool alarmEnabled = false;      // 알람 활성화 상태 (true: 활성화, false: 비활성화)

// LED 핀 설정
#define LED_PIN 13              // 알람 작동 시 켜지는 LED 핀

// 디바운스 변수 (버튼의 잘못된 입력 방지)
unsigned long lastDebounceTime = 0; // 마지막 버튼 입력 시간
unsigned long debounceDelay = 50;   // 디바운스 지연 시간 (50ms)
bool lastButtonStateSet = HIGH;     // 이전 BUTTON_SET 버튼 상태
bool lastButtonStateToggle = HIGH; // 이전 BUTTON_TOGGLE 버튼 상태

// ** 함수 선언 추가 **
void printTime(int hour, int minute, int second = -1); // 프로토타입 선언

void setup() {
  // 핀 모드 설정
  pinMode(BUTTON_SET, INPUT_PULLUP);     // BUTTON_SET 핀을 내부 풀업 저항과 함께 입력으로 설정
  pinMode(BUTTON_TOGGLE, INPUT_PULLUP);  // BUTTON_TOGGLE 핀을 내부 풀업 저항과 함께 입력으로 설정
  pinMode(LED_PIN, OUTPUT);              // LED 핀을 출력으로 설정
  pinMode(BUZZER_PIN, OUTPUT);           // BUZZER 핀을 출력으로 설정

  // 초기 LED 및 부저 상태 설정
  digitalWrite(LED_PIN, LOW);            // LED 끄기
  digitalWrite(BUZZER_PIN, LOW);         // 부저 끄기

  // LCD 초기화
  lcd.init();                            // LCD 시작
  lcd.backlight();                       // LCD 백라이트 켜기

  // RTC 초기화
  rtc.halt();                            // RTC 활성화 (매개변수 없음)
}

void loop() {
  // RTC에서 현재 시간 읽기
  Ds1302::DateTime now;                  // RTC 시간 저장용 DateTime 구조체 생성
  rtc.getDateTime(&now);                 // RTC 시간 데이터를 구조체에 저장

  // LCD 첫 번째 줄에 현재 시간 표시
  lcd.setCursor(0, 0);                   // LCD 첫 번째 줄 첫 번째 칸으로 커서 이동
  lcd.print("Time: ");                   // "Time: " 문자열 출력
  printTime(now.hour, now.minute, now.second); // 현재 시간 출력 (HH:MM:SS 형식)

  // LCD 두 번째 줄에 알람 정보 표시
  lcd.setCursor(0, 1);                   // LCD 두 번째 줄 첫 번째 칸으로 커서 이동
  lcd.print("Alarm: ");                  // "Alarm: " 문자열 출력
  if (alarmEnabled) {                    // 알람이 활성화된 경우
    printTime(alarmHour, alarmMinute);   // 알람 시간 출력 (HH:MM 형식)
    lcd.print(" ON ");                   // 알람 활성화 표시
  } else {
    lcd.print("--:-- OFF");              // 알람 비활성화 표시
  }

  // 버튼 입력 처리
  handleButtonInput();

  // 알람 조건 확인 및 작동
  if (alarmEnabled && now.hour == alarmHour && now.minute == alarmMinute) {
    digitalWrite(LED_PIN, HIGH);         // LED 켜기
    tone(BUZZER_PIN, 1000);              // 부저 작동 (1kHz 주파수)
  } else {
    digitalWrite(LED_PIN, LOW);          // LED 끄기
    noTone(BUZZER_PIN);                  // 부저 끄기
  }

  delay(1000);                           // 1초 대기 후 반복
}

// 버튼 입력 처리 함수
void handleButtonInput() {
  bool currentSetState = digitalRead(BUTTON_SET);       // BUTTON_SET 버튼 현재 상태 읽기
  bool currentToggleState = digitalRead(BUTTON_TOGGLE); // BUTTON_TOGGLE 버튼 현재 상태 읽기

  // BUTTON_SET 디바운스 처리 및 알람 시간 설정
  if (currentSetState == LOW && lastButtonStateSet == HIGH && millis() - lastDebounceTime > debounceDelay) {
    alarmMinute++;                       // 버튼을 누를 때마다 알람 분 1 증가
    if (alarmMinute >= 60) {             // 알람 분이 60 이상이면
      alarmMinute = 0;                   // 분을 0으로 초기화하고
      alarmHour = (alarmHour + 1) % 24;  // 시를 1 증가 (24시간 형식 유지)
    }
    lastDebounceTime = millis();         // 마지막 버튼 입력 시간 갱신
  }
  lastButtonStateSet = currentSetState;  // 버튼 상태 갱신

  // BUTTON_TOGGLE 디바운스 처리 및 알람 활성화/비활성화
  if (currentToggleState == LOW && lastButtonStateToggle == HIGH && millis() - lastDebounceTime > debounceDelay) {
    alarmEnabled = !alarmEnabled;        // 버튼을 누를 때마다 알람 활성화 상태 전환
    lastDebounceTime = millis();         // 마지막 버튼 입력 시간 갱신
  }
  lastButtonStateToggle = currentToggleState; // 버튼 상태 갱신
}

// 시간 표시 함수
void printTime(int hour, int minute, int second = -1) {
  // 시간을 HH:MM(:SS) 형식으로 출력
  if (hour < 10) lcd.print("0");
  lcd.print(hour);
  lcd.print(":");
  if (minute < 10) lcd.print("0");
  lcd.print(minute);

  // 초(second) 값이 주어진 경우 출력
  if (second >= 0) {
    lcd.print(":");
    if (second < 10) lcd.print("0");
    lcd.print(second);
  }
}