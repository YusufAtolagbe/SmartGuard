/*DETECTS AN INTRUSION FROM PIR SENSOR (using interrupts) THEN SENDS A MESSAGE TO A TELEGRAM BOT (yumidebot) SMARTGUARD.
*/

///INCLUDES
#include <WiFi.h>      //not necessary to include this
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

/////////DEFINES///////////////////////////////////////////////////////
#define BOT_TOKEN "7882867268:AAERfFr5VV5BKfi2Za2j_mpiARQFliw2OM8" 
uint8_t PIR_PIN  =13;  //PIR PIN
uint8_t BUZZER_PIN  =23;    //buzzer


String USER_CHAT_ID = ""; //user chat id for telegrambot
   

////CONSTANNTS AND INITIALIZATIONS////////////////////////
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

volatile uint8_t task_num = 0;
volatile uint16_t interrupt = 0; //debugging


/////ISR///////////////////////
void IRAM_ATTR PIR_ISR() {
  task_num = 1;
  interrupt++;
  }


//////FUNCTIONS/////////////////////////////////////////////////////////
void IntrusionDetected(){      //called during an intrusion detectioN 
    bot.sendMessage(USER_CHAT_ID, "🔔*INTRUSION DETCTED*, GO CHECK IT OUT OR CALL THE POLICE NOW!", "Markdown"); 
    Serial.write("\n Alert sent");
    digitalWrite(BUZZER_PIN, HIGH);
    delay(1000);
    digitalWrite(BUZZER_PIN, LOW);
} //endfunction


///nothing executes
void Nothing(){}


///function pointer
void (*tasks[2])(void); //an array if two function pointers, one will point to nothing the other will point to intrusion detected.

void setup() {
  Serial.begin(115200);
  Serial.write("\nSetup");
  pinMode(PIR_PIN, INPUT_PULLUP);  //initialize pins
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  //WiFiManager for initial Wi-Fi setup
WiFiManager wm;

//NOTE: for the code below, autoconnect tries to connect to any saved network, if it fails after 30 seconds,
        //it creates a temporary access point(yumidesetup.com) that you can connect to.

if (!wm.autoConnect("yumidesetup.com")) {     //if unsuccessful

  Serial.println("Initial connection failed.");  //DEBUGGING 
  Serial.println("Waiting 1 minute for SmartGuard to connect to WIFI..."); //DEBUGGING
  unsigned long start_attempt_time = millis();
 // for 60 seconds keep checking if Wifi is connected
  while((millis()-start_attempt_time) < 60000){
    delay(100);
    if (WiFi.status() == WL_CONNECTED){           //if wifi is connected
      Serial.println("\nWIFI connection successful"); //debugging
      break;
    } //end if    
  } //end while
   
  if (WiFi.status() != WL_CONNECTED){       // if wifi is not connected
     Serial.println("\n Connection unsuccessful after 1 minute, restarting device..."); //debugging
     ESP.restart();
   } //end if
   
} //end if


secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org  
                                                     //this basically verifies that we are talking to telegram
Serial.print("Retrieving time: ");    ///DEBUGGING
configTime(0, 0, "pool.ntp.org"); //get UTC time via NTP //needed for certificte validation (for esp32 to trust the connection).
time_t now = time(nullptr);       //gives us the current time in seconds since 1970
while (now < 24 * 3600)          //this wll evaluate to false coz 24*3600 = 1 day in seconds so if now is less than that,
//then we are not yet in 1970
{
  Serial.print(".");
  delay(100);
  now = time(nullptr);
}
Serial.println(now);
Serial.println("Waiting for user to send /start to the bot...");

tasks[0] = Nothing;  //make it do nothing
tasks[1] = IntrusionDetected;   //attach IntrusionDetcted to it

} //end setup


void loop() {
char string_buffer[128];
delay(100);
static bool start_received = false; //turn_on_off=true;//checks if start has been sent and on/off messages have been sent by user.
int num_new_messages = bot.getUpdates(bot.last_message_received + 1); //gets the latest messages/message since the last 
                                                                    //time it checked.
//last message received returns the id of the last messae received before now, getUpdates checks for the number of new messages 
//received since the specified chatid, and stores the message/s in bot.messages.

if (num_new_messages) {
  for (uint8_t i = 0; i < num_new_messages; i++) {
    String ChatId = bot.messages[i].chat_id.c_str();
    const char* text = bot.messages[i].text.c_str();
    if (strcmp(text, "/start") == 0 && !start_received) {        //if user sends start
      USER_CHAT_ID = ChatId;
      bot.sendMessage(USER_CHAT_ID, "*SMARTGUARD ACTIVE ✅*\nSend /on (lowercase) to enable alerts and /off (lowercase)"
      "to disable them.", "Markdown");
      attachInterrupt(digitalPinToInterrupt(PIR_PIN), PIR_ISR, FALLING); //attach interrupts to PIR sensor
      Serial.write("\nInterrupt assigned to PIR pin");
      start_received = true; // prevent further /start processing
      
    } //end if 
    
    else if ( strcmp(text, "/on") == 0 && ChatId == USER_CHAT_ID ){ //if user sends on
      bot.sendMessage(USER_CHAT_ID, "🛡️ *SmartGuard Armed.* Intrusion detection is now active.", "Markdown");
      attachInterrupt(digitalPinToInterrupt(PIR_PIN), PIR_ISR, FALLING); //attach interrupts to PIR sensor
      Serial.write("\nInterrupt assigned to PIR pin");
    } //end elseif
     
    else if (strcmp(text, "/off") == 0 && ChatId == USER_CHAT_ID ){ //if user sends off
      bot.sendMessage(USER_CHAT_ID, "🔕 *SmartGuard Disarmed.* Intrusion detection is now off.", "Markdown");
      detachInterrupt(digitalPinToInterrupt(PIR_PIN));   //detach interupts from PIR sensor
      Serial.write("\nDetached Interrupts from PIR pin");
      task_num = 0;   //call Nothing
    } //end elseif
  } //end for
}   //end if

tasks[task_num](); //call whichever function is to be called
task_num = 0; //RESET BACK TO ZERO
sprintf(string_buffer, "\nloop\ninterrupt = %d\t task_num = %d", interrupt, task_num);
Serial.write(string_buffer); 


} //end loop
