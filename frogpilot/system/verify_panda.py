#!/usr/bin/env python3
from time import sleep

from panda import Panda

BODY_CAN_IDS = {0x750, 0x752, 0x660, 0x285}

def hw_name(hw_type):
  names = {
    Panda.HW_TYPE_WHITE_PANDA: 'white',
    Panda.HW_TYPE_GREY_PANDA: 'grey',
    Panda.HW_TYPE_BLACK_PANDA: 'black',
    Panda.HW_TYPE_UNO: 'uno',
    Panda.HW_TYPE_DOS: 'dos',
    Panda.HW_TYPE_RED_PANDA: 'red',
    Panda.HW_TYPE_RED_PANDA_V2: 'red v2',
    Panda.HW_TYPE_TRES: 'tres',
    Panda.HW_TYPE_CUATRO: 'cuatro',
  }
  return names.get(hw_type, 'unknown')

def expected_body_bus(hw_type, flipped):
  if hw_type in Panda.H7_DEVICES:
    return 1 if flipped else 0
  if hw_type in (Panda.HW_TYPE_UNO, Panda.HW_TYPE_DOS, Panda.HW_TYPE_BLACK_PANDA):
    return 1
  return None

def scan_can(panda, secs=1):
  seen = set()
  end = panda.get_microsecond_timer() + secs * 1_000_000
  while panda.get_microsecond_timer() < end:
    for addr, _, _, bus in panda.can_recv():
      if addr in BODY_CAN_IDS:
        seen.add(bus)
  return sorted(seen)

def main():
  serials = Panda.list()
  print(f'Found {len(serials)} panda(serial): {serials}\n')

  for serial in serials:
    with Panda(serial, claim=False) as panda:
      hw_type = panda.get_type()
      if isinstance(hw_type, bytearray):
        hw_type = bytes(hw_type)
      flipped = panda.health()['car_harness_status'] == Panda.HARNESS_STATUS_FLIPPED
      body_bus = expected_body_bus(hw_type, flipped)

      print(f'Panda {serial[:8]}…')
      print(f'  hw: {hw_name(hw_type)}, mcu: {"H7" if hw_type in Panda.H7_DEVICES else "F4"}')
      print(f'  harness flipped: {flipped}')
      print(f'  expected Toyota body bus: {body_bus}')
      print('  sniffing CAN for one second…')
      buses = scan_can(panda)
      print(f'  body-CAN traffic seen on: {buses}\n')

if __name__ == '__main__':
  main()
