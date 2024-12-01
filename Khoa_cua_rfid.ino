#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN 10  // Chân chọn (SDA) của RFID
#define RST_PIN 9  // Chân RST của RFID
#define SERVO_PIN 2  // Chân kết nối với servo

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Khởi tạo đối tượng RFID
Servo servo;  // Khởi tạo đối tượng servo
#define coi 3
int ledGreen = 8;  // Chân kết nối với đèn LED xanh
int ledRed = 7;    // Chân kết nối với đèn LED đỏ
#define baodong 4
bool doorLocked = true;
int wrongAttempts = 0;

// UID của các thẻ RFID được chấp nhận
byte cardUID1[] = {0xD3, 0xA4, 0xD3, 0x1B}; // UID của thẻ 1
//byte cardUID2[] = {0x63, 0x7E, 0xF6, 0xF6}; // UID của thẻ 2
#define chay A0
void setup() {
  Serial.begin(9600);
  Serial.println("OK");
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("OK 1");
  servo.attach(SERVO_PIN);
  servo.write(0);  // Ban đầu servo đóng cửa
   pinMode(coi, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(baodong, OUTPUT);
  digitalWrite(baodong, HIGH);
  digitalWrite(ledGreen, LOW);
  digitalWrite(ledRed, LOW);
  pinMode(chay, INPUT_PULLUP);
}

void loop() {
  if(digitalRead(chay) == 0){
    unlockDoor();
  }
  else if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Đọc UID thẻ RFID
    Serial.println("OK");
    byte cardUID[mfrc522.uid.size];
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      cardUID[i] = mfrc522.uid.uidByte[i];
    }
    
    // Kiểm tra UID thẻ
    if (compareUID(cardUID, cardUID1)) {
      if (doorLocked) {
        unlockDoor();
        wrongAttempts = 0;
      }
      Serial.println("THE DUNG");
      digitalWrite(ledGreen, HIGH);
      delay(5000);
      lockDoor();
      digitalWrite(ledGreen, LOW);
    } else {
      Serial.print("THE SAI LAN: ");
      wrongAttempts++;
      Serial.println(wrongAttempts);
      digitalWrite(ledRed, HIGH);
      delay(1000);
      digitalWrite(ledRed, LOW);
      
      if (wrongAttempts >= 3) {
        // Kêu còi khi sai thẻ quá 3 lần
        Serial.println("SAI QUA 3 LAN");
        digitalWrite(coi, HIGH);
        digitalWrite(baodong, LOW);
        delay(10000);  // Dừng 2 giây
        digitalWrite(coi, LOW);
        digitalWrite(baodong, HIGH);
        wrongAttempts = 0;  // Đặt lại số lần sai
      }
    }
    
    mfrc522.PICC_HaltA();
  }
  else lockDoor();
}

void unlockDoor() {
  servo.write(90);  // Mở cửa
  doorLocked = false;
}

void lockDoor() {
  servo.write(0);  // Đóng cửa
  doorLocked = true;
}

bool compareUID(byte* UID1, byte* UID2) {
  for (byte i = 0; i < 4; i++) {
    if (UID1[i] != UID2[i]) {
      return false;
    }
  }
  return true;
}
