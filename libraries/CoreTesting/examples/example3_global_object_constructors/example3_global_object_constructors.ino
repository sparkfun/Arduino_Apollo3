// Author: Owen Lyke
// Created: May 2 2019
// License: MIT. See SparkFun Arduino Apollo3 Project for more information

// This example demonstrates that global C++ object constructors are called prior to entering 'setup()'

// Define the test class
class shape {
  public:
  shape( uint16_t sides);
  uint16_t num_sides;
  void printSides( void );
};

// Constructor is used to initialize the 'num_sides' member
shape::shape( uint16_t sides){
  num_sides = sides;
}

// A standard way to display the 'num_sides' member
void shape::printSides( void ){
  am_util_stdio_printf("Object's num sides = %d\n", num_sides);
}

// Create a global instance of the 'shape' class
// If constructors are not called the memory will be
// at best initialized to zero and at worst entirely 
// random. With global constructors called you can 
/// rest assured that 'rect.num_sides' will be 4
shape rect(4);

void setup() {
  // put your setup code here, to run once:

  am_util_stdio_printf("Global object: 'rect'\n");
  rect.printSides();      // Should show that 'rect.num_sides' == 4

  am_util_stdio_printf("\nChanging 'rect.num_sides' to 13\n");
  rect.num_sides = 13;
  rect.printSides();      // Should show that 'rect.num_sides' == 13

  am_util_stdio_printf("\nLocal object: 'rect_2'\n");
  shape rect_2(7);        // Demonstrates locally constructed objects
  rect_2.printSides();  
}

void loop() {
  // put your main code here, to run repeatedly:

}
