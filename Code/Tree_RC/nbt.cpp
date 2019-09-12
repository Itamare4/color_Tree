/*
 * nbt.c
 *
 *  Created on: Jun 10, 2018
 *      Author: Itamar Eliakim
 */
#include "nbt.h"
#include <Arduino.h>

//NBT - Non Blocking Timer
void NBT_init(nbt_t * nbt, unsigned long interval)
{
  nbt->timeout = interval;
  nbt->previousMillis = millis();
}

bool NBT_handler(nbt_t * nbt)
{
  if(millis()-nbt->previousMillis>nbt->timeout){
    nbt->previousMillis = millis();
    return true;
  }

  return false;
}
