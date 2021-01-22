#include <Bounce2.h>      // https://github.com/thomasfredericks/Bounce2
#include <BleGamepad.h>   // https://github.com/lemmingDev/ESP32-BLE-Gamepad

#define BUTTON1_PIN 23
#define BUTTON2_PIN 19
#define BUTTON3_PIN 18
#define BUTTON4_PIN 5

#define DPAD_UP_PIN 22
#define DPAD_DOWN_PIN 21
#define DPAD_LEFT_PIN 32
#define DPAD_RIGHT_PIN 33
//#define LED_PIN 13

BleGamepad bleGamepad;       // Instantiate a BleGamepad object

struct buttonHandler {
  Bounce *debouncer;
  uint64_t button;
  bool isPressed;
};

buttonHandler faceButtons[4];
buttonHandler dPad[4];

void initButton(buttonHandler *handler, int buttonPin, uint64_t button, int intervalMs = 5) {

  pinMode(buttonPin, INPUT_PULLUP);

  handler->debouncer = new Bounce();
  handler->debouncer->attach(buttonPin);      // After setting up the button, setup the Bounce instance :
  handler->debouncer->interval(intervalMs);

  handler->button = button;

  handler->isPressed = false;
}

void setup()
{
  Serial.begin(115200);
  bleGamepad.begin();                // Begin the gamepad

  //Face Buttons
  initButton(&faceButtons[0], BUTTON1_PIN, BUTTON_1);
  initButton(&faceButtons[1], BUTTON2_PIN, BUTTON_2);
  initButton(&faceButtons[2], BUTTON3_PIN, BUTTON_3);
  initButton(&faceButtons[3], BUTTON4_PIN, BUTTON_4);

  //Face Buttons
  initButton(&dPad[0], DPAD_UP_PIN, 0); // We won't send a button when a press is detected
  initButton(&dPad[1], DPAD_DOWN_PIN, 0);
  initButton(&dPad[2], DPAD_LEFT_PIN, 0);
  initButton(&dPad[3], DPAD_RIGHT_PIN, 0);

  //pinMode(LED_PIN, OUTPUT);          // Setup the LED :
}

void processButtonPress(buttonHandler *handler) {
  handler->debouncer->update();
  int value = handler->debouncer->read();
  if (value == LOW)
  {
    if (handler->button > 0)
    { 
      Serial.println("Button Pressed");
      bleGamepad.press(handler->button);
    }

    handler->isPressed = true;
  }
  else
  {
    if (handler->button > 0)
    {
      bleGamepad.release(handler->button);
    }

    handler->isPressed = false;
  }
}

signed char processDPad() {
  if (dPad[0].isPressed) {
    if (dPad[2].isPressed) {
      return DPAD_UP_LEFT;
    } else if (dPad[3].isPressed) {
      return DPAD_UP_RIGHT;
    } else {
      return DPAD_UP;
    }
  } else if (dPad[1].isPressed) {
    if (dPad[2].isPressed) {
      return DPAD_DOWN_LEFT;
    } else if (dPad[3].isPressed) {
      return DPAD_DOWN_RIGHT;
    } else {
      return DPAD_DOWN;
    }
  } else if (dPad[2].isPressed) {
    return DPAD_LEFT;
  } else if (dPad[3].isPressed) {
    return DPAD_RIGHT;
  }

  return DPAD_CENTERED;
}

int i;
void loop()
{
  if (bleGamepad.isConnected())
  {
    //Face Buttons
    for (i = 0; i < 4; i++)
    {
      processButtonPress(&faceButtons[i]);
    }

    //D-Pad
    for (i = 0; i < 4; i++)
    {
      processButtonPress(&dPad[i]);
    }
    bleGamepad.setHat1(processDPad());
  }
}
