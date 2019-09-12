/*
 * nbt.h
 *
 *  Created on: Jun 10, 2018
 *      Author: Itamar Eliakim
 */
#ifndef NBT_H_
#define NBT_H_

typedef struct{
  unsigned long timeout;
  unsigned long previousMillis;

}nbt_t;

void NBT_init(nbt_t * nbt, unsigned long interval);
bool NBT_handler(nbt_t * nbt);


#endif /* NBT_H_ */
