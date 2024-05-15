set (TARGET_NAME "estimate_aloha_things")
set (SOURCES 
  "estimation.cpp"
  "Subscriber.cpp"
  "Subscriber.h"
  "AlohaSimulator.cpp"
  "AlohaSimulator.h"
)

add_executable (${TARGET_NAME} ${SOURCES})
