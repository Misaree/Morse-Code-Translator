#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

//basic strings for initialisation
String morseAlphabet[26] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..",
  "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."
};
String morseNumbers[10] = {
  "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----."
};
String morseSpecial[10] = {
  "--..--", "-.-.--", "..--..", ".--.-.", ".-.-.", "-...-", "---...", "-.-.-.", "-....-", "-..-."
};
String specialChars[10] = {
  ",", "!", "?", "@", "+", "=", ":", ";", "-", "/"
};

String inputText = ""; //to store the incoming text or Morse code
String decodedMessage = ""; //to store the decoded message

//basic setup
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Choose Mode:");
  lcd.setCursor(0, 1);
  lcd.print("D or E");
  Serial.println("Choose Mode: 'D' for Decoding, 'E' for Encoding");
}

void loop() {
  static char mode = '\0'; //variable to store the mode
  static String inputText = ""; //variable to store incoming text
  static bool modeSelected = false; //flag to check if mode has been selected

  // If mode is not set, prompt the user to choose a mode
  if (mode == '\0') {
    if (Serial.available() > 0) {//serial monitor is on
      mode = Serial.read(); // Read the mode character
      while (Serial.available() > 0) {//clear the buffer after reading the mode
        Serial.read(); 
      }
      if (mode == 'D' || mode == 'E') {
        Serial.println(mode == 'D' ? "Decoder Mode" : "Encoder Mode");
        modeSelected = true; //set the flag true if a mode is chosen
      } 
      else {
        Serial.println("Invalid Mode. Choose 'D' for Decoding or 'E' for Encoding.");
        mode = '\0'; //reset the mode after entering an invalid input
      }
    }
  }

  //If the mode has been selected, prompt for the text once
  if (modeSelected) {
    Serial.println("Enter Text:");
    modeSelected = false; // Reset the flag
  }

  // Handle Morse code decoding
  if (mode == 'D') {
    while (Serial.available() > 0) {
      char inChar = (char)Serial.read();
      if (inChar == '\n') {
        if (inputText.length() > 0) { // Check if there's something to decode
          lcd.clear();
          decodeMorse(inputText);
          lcd.clear();
          lcd.print("Decoded Message:");
          lcd.setCursor(0, 1);
          lcd.print(decodedMessage);
          Serial.println("Decoded Message: " + decodedMessage);
          inputText = ""; // Clear for next input
          mode = '\0'; //reset mode for reselection
          Serial.println("Choose Mode: 'D' for Decoding, 'E' for Encoding");
        }
      } 
      else if (inChar != '\r') {
        inputText += inChar; //build Morse input
      }
    }
  }

  //handle text to Morse code encoding
  if (mode == 'E') {
    while (Serial.available() > 0) {
      char inChar = (char)Serial.read();
      if (inChar == '\n') 
      {
        if (inputText.length() > 0) 
        { //check if there's something to encode
          lcd.clear();
          String morseCode = encodeText(inputText);
          lcd.clear();
          lcd.print("Encoded Morse:");
          lcd.setCursor(0, 1);
          lcd.print(morseCode);
          Serial.println("Encoded Morse: " + morseCode);
          inputText = "";//clear for next input
          mode = '\0'; //reset mode to allow re-selection
          Serial.println("Choose Mode: 'D' for Decoding, 'E' for Encoding");
        }
      } else if (inChar != '\r') 
      {
        inputText += inChar; //build text input
      }
    }
  }
}



void decodeMorse(String input) {
  String morseCharacter = "";
  decodedMessage = ""; //clear previous message
  for (int i = 0; i < input.length(); i++) 
  {
    if (input[i] == ' ' || i == input.length() - 1)
    //if the loop encounters a space (' ') or reaches the end of the morseCode string
    //then it means the current Morse code character has ended 
    {
      if (i == input.length() - 1) 
      {
        morseCharacter += input[i];
      }
      if (morseCharacter == "") 
      {
        continue;
      }
      char decodedChar = decodeCharacter(morseCharacter);
      //this function would take one character at a time to convert to decoded character
      if (decodedChar != '\0') 
      {
        decodedMessage += decodedChar;
      }
      morseCharacter = "";
    } 
    else 
    {
      morseCharacter += input[i];
    }
  }
}

char decodeCharacter(String morseCharacter) 
{
  for (int i = 0; i < 26; i++) 
  {
    if (morseCharacter == morseAlphabet[i]) 
    {
      return char('A' + i);
    }
  }
  for (int i = 0; i < 10; i++) 
  {
    if (morseCharacter == morseNumbers[i]) 
    {
      return char('0' + i);
    }
  }
  for (int i = 0; i < 10; i++) 
  {
    if (morseCharacter == morseSpecial[i]) 
    {
      return specialChars[i].charAt(0);
    }
  }
  Serial.println("Invalid Input!");
  return '\0';
}

String encodeText(String text) 
{
  String encoded = "";
  for (int i = 0; i < text.length(); i++) 
  {
    char c = text.charAt(i);
    c = toupper(c); // Convert character to uppercase because Morse code arrays are in uppercase
    if (c >= 'A' && c <= 'Z') 
    {
      encoded += morseAlphabet[c - 'A'] + " ";//if c=B (66)-> morseAlphabet[B-A]=[66-65]=[1]="-...""
    } 
    else if (isdigit(c)) 
    {
      encoded += morseNumbers[c - '0'] + " ";
    } 
    else 
    {
      // Look for the character in the specialChars array
      for (int j = 0; j < 10; j++) 
      {
        if (c == specialChars[j].charAt(0)) 
        {
          encoded += morseSpecial[j] + " ";
          break; // Stop the loop once we've found the character
        }
      }
    }
  }
  encoded.trim(); // Trim the trailing space
  return encoded; // Now we return the modified 'encoded' string
}
