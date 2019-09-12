#include <Adafruit_NeoPixel.h>
#include <RCSwitch.h>

#ifdef __AVR__
 #include <avr/power.h>
#endif

#define STRIP_ONE_PIN        10
#define STRIP_TWO_PIN        8
#define STRIP_THREE_PIN      6
#define STRIP_FOUR_PIN       12

#define NUMPIXELS 30
#define DELAYVAL 500
#define NR_KNOWN_KEYS   6

#define MAXIMUM_VALUE 200
#define DIFF_TOLERANCE 30
#define ADD_N_SUBTRACT 15

Adafruit_NeoPixel strip_one(NUMPIXELS, STRIP_ONE_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_two(NUMPIXELS, STRIP_TWO_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_three(NUMPIXELS, STRIP_THREE_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_four(NUMPIXELS, STRIP_FOUR_PIN, NEO_GRB + NEO_KHZ800);

RCSwitch mySwitch = RCSwitch();

struct readRGB 
{ 
   int r;
   int g;
   int b;
   int led_num;
}read_rgb;

byte strip_one_led[NR_KNOWN_KEYS][3] =  {
    {0, 1, 100},
    {2, 3, 4},
    {5, 6, 7},
    {8, 9, 100},
    {10, 11, 12},
    {13, 14, 15},
};

byte strip_two_led[NR_KNOWN_KEYS][3] =  {
    {0, 1, 2},
    {3, 4, 5},
    {6, 7, 8},
    {9, 10, 11},
    {12, 13, 14},
    {15, 16, 17},
};

byte strip_three_led[NR_KNOWN_KEYS][3] =  {
    {0, 1, 2},
    {3, 4, 5},
    {6, 7, 8},
    {9, 10, 11},
    {12, 13, 14},
    {15, 16, 17},
};

byte strip_four_led[NR_KNOWN_KEYS][3] =  {
    {0, 1, 100},
    {2, 3, 4},
    {5, 6, 7},
    {8, 9, 100},
    {10, 11, 12},
    {13, 14, 15},
};

struct readRGB normalizeNumber(struct readRGB input){
  int loc = 0;
  int rgb_input[3] = {input.r, input.g, input.b};
  int maximum = rgb_input[0];
  
  for (int i = 1; i < 3; i++) {
    if (rgb_input[i] > maximum) {
       maximum  = rgb_input[i];
       loc = i;
    }
  }

  struct readRGB output;
  float scale_factor = MAXIMUM_VALUE/rgb_input[loc];
  output.r = scale_factor * input.r;
  output.g = scale_factor * input.g;
  output.b = scale_factor * input.b;
  output.led_num = input.led_num;
  return output;
}

struct readRGB verifyLargeDiff(struct readRGB input){
  struct readRGB output;
  if (abs(input.r-input.g)<DIFF_TOLERANCE){
    if (input.r>input.g) {
      output.r = input.r + ADD_N_SUBTRACT;
      output.g = input.g + ADD_N_SUBTRACT;
    }
    else{
      output.r = input.r - ADD_N_SUBTRACT;
      output.g = output.g + ADD_N_SUBTRACT;      
    }
  }
  if (abs(input.r-input.b)<DIFF_TOLERANCE){
    if (input.r>input.b) {
      output.r = input.r + ADD_N_SUBTRACT;
      output.b = input.b - ADD_N_SUBTRACT;
    }
    else{
      output.r = input.r - ADD_N_SUBTRACT;
      output.b = input.b + ADD_N_SUBTRACT;      
    }
  }
  if (abs(input.b-input.g)<DIFF_TOLERANCE){
    if (input.b>input.g) {
      output.b = input.b + ADD_N_SUBTRACT;
      output.g = input.g - ADD_N_SUBTRACT;
    }
    else{
      output.b = input.b - ADD_N_SUBTRACT;
      output.g = input.g + ADD_N_SUBTRACT;      
    }
  }

  output.led_num = input.led_num;
  return output;
}

void printReadRGB(struct readRGB input){
  Serial.print("R: ");Serial.print(input.r);Serial.print("\t");
  Serial.print("G: ");Serial.print(input.g);Serial.print("\t");
  Serial.print("B: ");Serial.print(input.b);Serial.print("\t");
  Serial.print("LED num: ");Serial.print(input.led_num);Serial.println("\t");
}

void setup() {
  Serial.begin(9600);
  mySwitch.enableReceive(0);
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif
  strip_one.begin();
  strip_two.begin();
  strip_three.begin();
  strip_four.begin();
  Serial.println("Start Tree Side");

  for(int i=0; i<=20; i++) {
    strip_one.setPixelColor(i, strip_one.Color(255, 0, 0));
    strip_two.setPixelColor(i, strip_one.Color(0, 255, 0));
    strip_three.setPixelColor(i, strip_one.Color(0, 0, 255));
    strip_four.setPixelColor(i, strip_one.Color(255, 0, 255));
  }
  strip_one.show();
  strip_two.show();
  strip_three.show();
  strip_four.show();  
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
//  read_rgb.r = 52;
//  read_rgb.g = 60;
//  read_rgb.b = 52;
//  read_rgb.led_num = 1;
//  printReadRGB(read_rgb);
//  readRGB diff = verifyLargeDiff(read_rgb);
//  readRGB output = normalizeNumber(diff);
//  printReadRGB(output);
  Serial.println("####################################");
  
  if (mySwitch.available()) {
    long int recived_data = mySwitch.getReceivedValue();
    int len1 = mySwitch.getReceivedBitlength();
    Serial.println(recived_data);
    read_rgb.r = (recived_data >> 0) & 0xff;
    read_rgb.g = (recived_data >> 8) & 0xff;
    read_rgb.b = (recived_data >> 16) & 0xff;
    read_rgb.led_num = (recived_data >> 24) & 0xff;
    mySwitch.resetAvailable();

    readRGB diff = verifyLargeDiff(read_rgb);
    readRGB read_rgb = normalizeNumber(diff);
    
    if (read_rgb.led_num<6){
      Serial.println("Strip One");
      read_rgb.led_num = read_rgb.led_num;
      int start_led = strip_one_led[read_rgb.led_num][0];
      int end_led = (strip_one_led[read_rgb.led_num][2]==100) ? strip_one_led[read_rgb.led_num][1] : strip_one_led[read_rgb.led_num][2];   //This is a hack because some of the hearts we have only two WS2812B instead of three
      Serial.print("Start LED: ");Serial.print(start_led);Serial.print("\t");
      Serial.print("End LED: ");Serial.print(end_led);Serial.println("");
      for(int i=start_led; i<=end_led; i++) {
        strip_one.setPixelColor(i, strip_one.Color(read_rgb.r, read_rgb.g, read_rgb.b));
        strip_one.show();
      }
    }
    else if (read_rgb.led_num<12){
      Serial.println("Strip Two");
      read_rgb.led_num = read_rgb.led_num - 6;
      int start_led = strip_two_led[read_rgb.led_num][0];
      int end_led = (strip_two_led[read_rgb.led_num][2]==100) ? strip_two_led[read_rgb.led_num][1] : strip_two_led[read_rgb.led_num][2];   //This is a hack because some of the hearts we have only two WS2812B instead of three
      Serial.print("Start LED: ");Serial.print(start_led);Serial.print("\t");
      Serial.print("End LED: ");Serial.print(end_led);Serial.println("");
      for(int i=start_led; i<=end_led; i++) {
        strip_two.setPixelColor(i, strip_two.Color(read_rgb.r, read_rgb.g, read_rgb.b));
        strip_two.show();
      }
    }
    else if (read_rgb.led_num<18){
      Serial.println("Strip Three");
      read_rgb.led_num = read_rgb.led_num - 12;
      int start_led = strip_three_led[read_rgb.led_num][0];
      int end_led = (strip_three_led[read_rgb.led_num][2]==100) ? strip_three_led[read_rgb.led_num][1] : strip_three_led[read_rgb.led_num][2];   //This is a hack because some of the hearts we have only two WS2812B instead of three
      Serial.print("Start LED: ");Serial.print(start_led);Serial.print("\t");
      Serial.print("End LED: ");Serial.print(end_led);Serial.println("");
      for(int i=start_led; i<=end_led; i++) {
        strip_three.setPixelColor(i, strip_three.Color(read_rgb.r, read_rgb.g, read_rgb.b));
        strip_three.show();
      }
    }
    else if (read_rgb.led_num<24){
      Serial.println("Strip Four");
      read_rgb.led_num = read_rgb.led_num - 18;
      int start_led = strip_four_led[read_rgb.led_num][0];
      int end_led = (strip_four_led[read_rgb.led_num][2]==100) ? strip_four_led[read_rgb.led_num][1] : strip_four_led[read_rgb.led_num][2];   //This is a hack because some of the hearts we have only two WS2812B instead of three
      Serial.print("Start LED: ");Serial.print(start_led);Serial.print("\t");
      Serial.print("End LED: ");Serial.print(end_led);Serial.println("");
      for(int i=start_led; i<=end_led; i++) {
        strip_four.setPixelColor(i, strip_four.Color(read_rgb.r, read_rgb.g, read_rgb.b));
        strip_four.show();
      }
    }
  }
}
