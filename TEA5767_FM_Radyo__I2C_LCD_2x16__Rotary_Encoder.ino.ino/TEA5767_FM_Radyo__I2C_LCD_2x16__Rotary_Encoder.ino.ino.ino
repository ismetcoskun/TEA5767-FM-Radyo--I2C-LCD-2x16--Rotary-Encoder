
//************** "TEA5767 FM Radio Module and Arduino" Digital FM Radio Project****************//

//"TEA5767 FM Radio Module", "I2C LCD 2x16","Rotary Encoder", "Arduino (Atmega328P)"
//TA2PWR Ham Radio Call Sign (Ankara-Turkiye)/TR
//Ismet Coskun 
//05.2024


//Çubuk Sinyal Seviyesi (SS) Karakterlerinin çizimi (Ekranın sol üst köşesinde...)

byte sinyalgosterge3  [8] = {B11100,
                             B10100,
                             B11100,
                             B01000,
                             B01000,
                             B00000,
                             B10000
                            };

byte sinyalgosterge6  [8] = {B11100,
                             B10100,
                             B11100,
                             B01000,
                             B01000,
                             B00000,
                             B01000,
                             B11000
                            };
byte sinyalgosterge9  [8] = {B11100,
                             B10100,
                             B11100,
                             B01000,
                             B01000,
                             B00100,
                             B01100,
                             B11100
                            };
byte sinyalgosterge12 [8] = {B11100,
                             B10100,
                             B11100,
                             B01000,
                             B01010,
                             B00110,
                             B01110,
                             B11110
                            };
byte sinyalgosterge15 [8] = {B11100,
                             B10100,
                             B11101,
                             B01011,
                             B00111,
                             B01111,
                             B11111
                            };

//Çağırılan Kütüphaneler
#include <TEA5767.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Enkoderin bağlı olduğu pinlerin tanımlaması
#define dt_pin 8
#define clk_pin 9

const int buton = 6; //Enkoder üzerindki butonun (sw pini) Arduino'nun 6 numaralı pine bağlanmıştır.
float frekans = 87.5;//Başlangıç Frekansı. "Radyo ilk açıldığında başlayacak olan frekans"
int ilk_durum; //Enkoder ilk durum
int son_durum; //Enkoder son durum
int btn_durum; //Enkoder sw pini (buton) durumu (istenildiğinde kullanılabilir)
int ss; //sinyal Seviyesi

TEA5767 radio = TEA5767(); //TEA5767 radyo modülünü tanımlar.
LiquidCrystal_I2C ekran (0x27, 16, 2); //I2C LCD ekranı tanımlar

void setup() {

  Wire.begin();
  radio.setFrequency(87.5);
  Serial.begin (9600);

 //2x16 LCD ekran icin I2C protokolünün başlatılması 
  Wire.begin(); //I2C protokolünü başlatır.
  ekran.init ();//LCD ekranı başlatır
  ekran.backlight(); //LCD ekran arka plan ışığını açar. (ekran.noBacklight(); arka plan ışığını kapatır.)
  ekran.clear(); //LCD ekran temizlenir.

  //Enkoderin pin tanımlaması
  pinMode (dt_pin, INPUT); //dt_pin giriş pini olarak ayarlandı.
  pinMode (clk_pin, INPUT); //clk_pin giriş pini olarak ayarlandı.
  pinMode (buton, INPUT); //enkoder üzerinde bulunan buton giriş pini olarak ayarlandı. (istenildiğinde farklı özellikler için kullanılabilir.)
  son_durum = digitalRead(dt_pin); //Okunan enkoder dt pininin durumunu, son_durum adlı değişkene aktarır. 
  //Bu bilgi daha sonra enkoderin konumu için kullanılacaktır.

  //Çubuk Sinyal Seviyesi (SS) Karakterin belirtilmesi (0-15)
  ekran.createChar(1, sinyalgosterge3);
  ekran.createChar(2, sinyalgosterge6);
  ekran.createChar(3, sinyalgosterge9);
  ekran.createChar(4, sinyalgosterge12);
  ekran.createChar(5, sinyalgosterge15);

  //Kullanılabilir Set'ler
  radio.setFrequency(frekans);  //Set the radio to the frequency value
  radio.setStereoNC(true);     //"true" iken gürültü gidermeyi acar.("false" devre dışı bırakır.)
  radio.setMuted(false);      //"true" iken sessize alır. ("false" devre dışı bırakır.)
  //radio.setSearch();
  radio.setStandby (false); //"true" iken uyku moduna alır. ("false" devre dışı bırakır.)

  //Kullanılabilir Sorgulama Get'leri
  //radio.getFrequency();
  radio.getReady(); //Radyo istasyonun hazır mı değil mi? 1 ya da 0
  radio.isStereo();// Radyo modu Stero mu Mono mu?
  radio.isMuted (); //Radyo Sessizde mi değil mi?
  radio.getSignalLevel(); //Radyo Sinyal Seviyesi 0-15

  //istasyon bulma fonksiyonları (istenildiğinde kullanılabilir.)
  //radio.findStations();
  //radio.nextStation();
  //?radio.previousStation();
  //radio.getStations();

}

void loop() {

  //Enkoder Kontrol Kodu
  ilk_durum = digitalRead(dt_pin);

  if (ilk_durum != son_durum) {

    if (digitalRead(clk_pin) != ilk_durum) {
      frekans = frekans + 0.1; //enkode sağa döndürüldüğünde frekansı 100Khz arttırır.
      frekans = constrain(frekans, 87.5, 108.1);      //Limitlenmis frekans aralığı. Frekans aralğı aşıldığında enkoderin başa dönmesi icin...
      if (frekans > 108.0) {                           
        frekans = 87.5; //
      }
      radio.setFrequency(frekans);

    } else {
      frekans = frekans - 0.1; //enkoder sola döndürüldüğünde frekansı "100Khz" azaltır.
      frekans = constrain(frekans, 87.4, 108.0);      //Limitlenmis frekans aralığı. Frekans aralğı aşıldığında enkoderin başa dönmesi icin...
      if (frekans < 87.5) {
        frekans = 108.0; 
      }
      radio.setFrequency(frekans);
    }
    //Frekans bilgisini seri haberleşmeye de yazdırır.
    Serial.print("Frekans: "); 
    Serial.println(frekans);
    {
      //LCD Ekrana "FM RADYO" yazdırır.
      ekran.setCursor(3, 0);// LCD ekran kürsörünü ayarlar.
      ekran.print("FM RADYO"); //LCD ekrana bu yazıyı yazdırır.

      //LCD Ekrana "Ayarlanan Frekansı" Yazdırır.
      ekran.setCursor(4, 1); //Ekran kürsörünü ayarlar.
      ekran.print(frekans); //Frekans bilgisini ayarlanan ekran kürsörüne yazdırır.

    }

    //Radyo Mevcut Durum
    //"Radyo Mevcut Durum" radyonun hazır olup olmadığını gösterir. Radyo otomatik arama-bulmada kullanılabilir.
    //LCD ekranın sağ alt köşesinde aktif olarak gösterilmektedir.

    ekran.setCursor(13, 1);
    if (radio.getReady()) {
      ekran.print("[1]");
    } else {
      ekran.print("[0] ");
    }

    // Sinyal Seviyesi (SS) karakteri İçin. (ekranın sol üst köşesindeki sinyal göstergesi)  

    ss = radio.getSignalLevel();
    ekran.setCursor(0, 0);

    {
      if (ss >= 15)
      {
        ekran.write(byte(5));
      } else if (ss >= 9 && ss < 15) //sinyal seviyesi(ss) 9-15 arasında ise 5 numaralı çubuk karakteri ekranda gösterilir.
      {
        ekran.write(byte(4)); // 4 numaralı
      }
      else if (ss >= 6 && ss < 9) //sinyal seviyesi(ss) 6-9 arasında ise 3 numaralı çubuk ss karakteri ekranda gösterilir.
      {
        ekran.write(byte(3));
      }
      else if (ss >= 0 && ss < 6) //sinyal seviyesi(ss) 0-6 arasında ise 2 numaralı ss çubuk karakteri ekranda gösterilir.
      {
        ekran.write(byte(2));

      }
    }
    //Alınan Raduo Sinyali Stereo-Mono mu? Ses Durumu
    ekran.setCursor(13, 0); //M ve S için Ekran kürsörünü ayarlar
    if (radio.isStereo()) {
      ekran.print("[S]"); //stereo ise ekranda S yazdırır.
    } else {
      ekran.print("[M] ");//değilse (mono) ise ekranda M yazdırır.

    }

  }
  son_durum = ilk_durum;


}
