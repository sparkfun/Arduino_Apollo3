#ifndef _AP3_POST_VARIANT_H_
#define _AP3_POST_VARIANT_H_

// Some fail-unsafe defines for SD card library compilation. 
// These ideally would be defined in a variant header, however
// if they are not these definitions may suffice
#ifndef SS
#define SS 50
#endif  // SS
#ifndef MOSI
#define MOSI 50
#endif  // SS
#ifndef MISO
#define MISO 50
#endif  // SS
#ifndef CLK
#define CLK 50
#endif  // SS




#endif // _AP3_POST_VARIANT_H_