void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.printf("abs(-5.14): %f\n", abs(-5.14));




  Serial.printf("constrain(1, 3, 5): %f\n", constrain(1, 3, 5));

  Serial.printf("max(4,2.4): %f\n", max(4,2.4));
  Serial.printf("min(4,2.4): %f\n", min(4,2.4));
  Serial.printf("sq(9.3): %f\n", sq(9.3));
  Serial.printf("sqrt(9.3): %f\n", sqrt(9.3));

  Serial.printf("cos((float)3.1415/3): %f\n", cos((float)3.1415/3));
  Serial.printf("sin((float)3.1415/3): %f\n", sin((float)3.1415/3));
  Serial.printf("tan((float)3.1415/3): %f\n", tan((float)3.1415/3));

  Serial.printf("isAplha('a'): %d\n", isAlpha('a'));
  Serial.printf("isAplha('1'): %d\n", isAlpha('1'));
  Serial.printf("isAlphaNumeric('a'): %d\n", isAlphaNumeric('a'));
  Serial.printf("isAlphaNumeric('1'): %d\n", isAlphaNumeric('1'));
  Serial.printf("isAlphaNumeric(' '): %d\n", isAlphaNumeric(' '));
  Serial.printf("isAscii('a'): %d\n", isAscii('a'));
  Serial.printf("isAscii(0xFF): %d\n", isAscii(0xFF));
  Serial.printf("isControl(0x0D): %d\n", isControl(0x0D));
  Serial.printf("isControl('a'): %d\n", isControl('a'));
  Serial.printf("isDigit('a'): %d\n", isDigit('a'));
  Serial.printf("isDigit('1'): %d\n", isDigit('1'));
  Serial.printf("isGraph(0x7F): %d\n", isGraph(0x7F));
  Serial.printf("isGraph('a'): %d\n", isGraph('a'));
  Serial.printf("isHexadecimalDigit('F'): %d\n", isHexadecimalDigit('F'));
  Serial.printf("isHexadecimalDigit('G'): %d\n", isHexadecimalDigit('G'));
  Serial.printf("isHexadecimalDigit('0'): %d\n", isHexadecimalDigit('0'));
  Serial.printf("isHexadecimalDigit('x'): %d\n", isHexadecimalDigit('x'));
  Serial.printf("isLowerCase('a'): %d\n", isLowerCase('a'));
  Serial.printf("isLowerCase('A'): %d\n", isLowerCase('A'));
  Serial.printf("isLowerCase('1'): %d\n", isLowerCase('1'));
  Serial.printf("isLowerCase(0x03): %d\n", isLowerCase(0x03));

//  bit(3)
//  bit(5)
//  bit(30)
//  bit(31)
//
//  bitClear(0xFF, 3)
//  bitClear(0xFF, 31)
//
//  bitRead(0xAA,3)
//  bitRead(0xAA,4)
  
  
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
